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
	{"nwt", WebServer::static_callback_nwt, sizeof(WebServer::per_session_data__nwt*), 10},
	{NULL, NULL, 0, 0}
};

WebServer::WebServer() : taskRunning(ATOMIC_FLAG_INIT) {
	memset(&info, 0, sizeof(info));
	info.port = 8080;
	info.iface = NULL;
	info.gid = -1;
	info.uid = -1;
	info.protocols = protocols;
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

		fprintf(stderr, "    %s = %s\n", (char *)c, buf);
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

int WebServer::static_callback_nwt(libwebsocket_context* context, libwebsocket* wsi, libwebsocket_callback_reasons reason, void* user, void* in, size_t len) {
	instance->callback_nwt(context, wsi, reason, user, in, len);
}

int WebServer::callback_nwt(libwebsocket_context* context, libwebsocket* wsi, libwebsocket_callback_reasons reason, void* user, void* in, size_t len) {
	per_session_data__nwt* pss = nullptr;
	if (reason != LWS_CALLBACK_PROTOCOL_INIT) {
		pss = *((per_session_data__nwt**)user);
	}
	std::string inBuf = std::string((const char*)in, len);
	switch (reason) {
		case LWS_CALLBACK_ESTABLISHED:
			*((per_session_data__nwt**)user) = new per_session_data__nwt();
			break;
		case LWS_CALLBACK_CLOSED:
			delete pss;
			break;
		case LWS_CALLBACK_FILTER_PROTOCOL_CONNECTION:
			dump_handshake_info(wsi);
			break;
		case LWS_CALLBACK_RECEIVE:
			if (inBuf.size() > 0) {
				if (inBuf[0] == 0x00) {
					// Keep-alive, ignore
				} else if (inBuf[0] == 0x01) {
					if (inBuf[1] == 0x00 && inBuf[2] == 0x02) {
					} else {
						pss->outBuf += "\x02\x00\x02";
					}
				}
			printf("< %s\n", inBuf.c_str());
			if (inBuf == "hello\n") {
				pss->outBuf = "Hello, world!";
			}
			}
			return 1;
		case LWS_CALLBACK_SERVER_WRITEABLE:
			if (pss->outBuf.size()) {
				int m = libwebsocket_write(wsi, (uint8_t*)pss->outBuf.c_str(), pss->outBuf.size(), LWS_WRITE_TEXT);
				if (m < (int)pss->outBuf.size()) {
					lwsl_err("ERROR %d writing to nwt socket\n", pss->outBuf.size());
					pss->outBuf = "";
					return -1;
				}
				pss->outBuf = "";
			}
			break;
		default:
			break;
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
	context = libwebsocket_create_context(&info);
	if (context == NULL) {
		lwsl_err("libwebsocket init failed\n");
		return;
	}
	auto prev = std::chrono::steady_clock::now();
	while (taskRunning.test_and_set()) {
		libwebsocket_service(context, 50);
		auto now = std::chrono::steady_clock::now();
		if (std::chrono::duration_cast<std::chrono::milliseconds>(now - prev).count() > 50) {
			// Trigger writeable every 50ms
			libwebsocket_callback_on_writable_all_protocol(&protocols[Protocols::PREFERENCES]);
		}
	}
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

