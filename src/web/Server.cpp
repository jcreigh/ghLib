/*----------------------------------------------------------------------------*/
/* Copyright (c) Creighton 2015. All Rights Reserved.                         */
/* Open Source Software - May be modified and shared but must                 */
/* be accompanied by the license file in the root source directory            */
/*----------------------------------------------------------------------------*/

#include "ghLib/web/Server.h"

namespace ghLib {
namespace web {

WebServer* WebServer::instance = nullptr;

WebServer *WebServer::GetInstance() {
	if (instance == nullptr) {
		instance = new WebServer();
	}
	return instance;
}

struct per_session_data__http {
	int fd;
};

libwebsocket_protocols WebServer::protocols[] = {
	{"http-only", WebServer::static_callback_http, sizeof(per_session_data__http), 0},
	{"nwtproxy", WebServer::static_callback_nwtproxy, sizeof(WebServer::per_session_data__nwtproxy*), 10},
	{"time", WebServer::static_callback_time, sizeof(WebServer::per_session_data__time*), 15},
	{"ping", WebServer::static_callback_ping, sizeof(WebServer::per_session_data__ping*), 15},
	{"logger", WebServer::static_callback_logger, sizeof(WebServer::per_session_data__logger*), 15},
	{NULL, NULL, 0, 0}
};

WebServer::WebServer() : taskRunning(ATOMIC_FLAG_INIT) {
	memset(&info, 0, sizeof(info));
	info.port = 8080; // TODO: Make configurable?
	info.iface = NULL;
	info.gid = -1;
	info.uid = -1;
	info.protocols = protocols;
	logger = ghLib::Logger::GetLogger("Debug.Web");
	logger->Info("Constructed WebServer");
}

void WebServer::dump_handshake_info(libwebsocket* wsi) {
	int n = 0;
	char buf[256];
	const unsigned char *c;

	do {
		c = lws_token_to_string((lws_token_indexes)n);
		if (!c) {
			n++;
			continue;
		}

		if (!lws_hdr_total_length(wsi, (lws_token_indexes)n)) {
			n++;
			continue;
		}

		lws_hdr_copy(wsi, buf, sizeof buf, (lws_token_indexes)n);

		logger->Debug(ghLib::Format("    %s = %s", (char *)c, buf));
		n++;
	} while (c);

}

int WebServer::static_callback_http(libwebsocket_context* context, libwebsocket* wsi, libwebsocket_callback_reasons reason, void* user, void* in, size_t len) {
	instance->callback_http(context, wsi, reason, user, in, len);
}

int WebServer::callback_http(libwebsocket_context* context, libwebsocket* wsi, libwebsocket_callback_reasons reason, void* user, void* in, size_t len) {
	switch (reason) {
		case LWS_CALLBACK_HTTP:
			dump_handshake_info(wsi);
			if (len < 1) {
				libwebsockets_return_http_status(context, wsi, HTTP_STATUS_BAD_REQUEST, NULL);
				goto try_to_reuse;
			}
			if (lws_hdr_total_length(wsi, WSI_TOKEN_POST_URI)) {
				return 0;
			}
			libwebsockets_return_http_status(context, wsi, 200, "Hello, world!");
			return 1;
			break;
		default:
			break;
	}
	return 0;
try_to_reuse:
	if (lws_http_transaction_completed(wsi)) {
		return -1;
	}
	return 0;
}


void WebServer::SetPort(int newPort) {
	bool isRunning = Running();
	SetEnabled(false, true);
	info.port = newPort;
	SetEnabled(isRunning);
}

void WebServer::Task() {
	logger->Trace("Creating libwebsocket context");
	context = libwebsocket_create_context(&info);
	if (context == NULL) {
		logger->Error("libwebsocket init failed");
		return;
	}
	auto prev = std::chrono::steady_clock::now();
	auto prevPing = std::chrono::steady_clock::now();
	while (taskRunning.test_and_set()) {
		libwebsocket_service(context, 50);
		auto now = std::chrono::steady_clock::now();
		if (std::chrono::duration_cast<std::chrono::milliseconds>(now - prev).count() > 50) {
			// Trigger writeable every 50ms
			libwebsocket_callback_on_writable_all_protocol(&protocols[Protocols::PREFERENCES]);
			libwebsocket_callback_on_writable_all_protocol(&protocols[Protocols::TIME]);
			libwebsocket_callback_on_writable_all_protocol(&protocols[Protocols::LOGGER]);
			prev = std::chrono::steady_clock::now();
		}
		if (std::chrono::duration_cast<std::chrono::milliseconds>(now - prevPing).count() > 2) {
			libwebsocket_callback_on_writable_all_protocol(&protocols[Protocols::PING]); // Ping requires replying a lot faster
			prevPing = std::chrono::steady_clock::now();
		}
	}
	logger->Trace("Destroying libwebsocket context");
	libwebsocket_context_destroy(context);
}

void WebServer::SetEnabled(bool enabled, bool block /* = false */) { // Enable or disable the WebServer task
	if (enabled) {
		if (!taskRunning.test_and_set()) { // Test if started, and if not, start it
			Start();
		}
	} else {
		taskRunning.clear();
		if (block) {
			while (Running()) { // Wait for task to end
				std::this_thread::sleep_for(std::chrono::milliseconds(20));
			}
		}
	}
}

}
}

