/*----------------------------------------------------------------------------*/
/* Copyright (c) Creighton 2015. All Rights Reserved.                         */
/* Open Source Software - May be modified and shared but must                 */
/* be accompanied by the license file in the root source directory            */
/*----------------------------------------------------------------------------*/

#ifndef GHLIB_WEB_SERVER_H_
#define GHLIB_WEB_SERVER_H_

#include <atomic>
#include <chrono>
#include <string.h>
#include "libwebsockets.h"
#include "ghLib/Runnable.h"
#include "ghLib/Logger.h"

namespace ghLib {
namespace web {

class WebServer : public Runnable {
public:
	static WebServer *GetInstance();
	static int static_callback_http(libwebsocket_context* context, libwebsocket* wsi, libwebsocket_callback_reasons reason, void* user, void* in, size_t len);
	static int static_callback_nwtproxy(libwebsocket_context* context, libwebsocket* wsi, libwebsocket_callback_reasons reason, void* user, void* in, size_t len);
	static int static_callback_time(libwebsocket_context* context, libwebsocket* wsi, libwebsocket_callback_reasons reason, void* user, void* in, size_t len);
	static int static_callback_ping(libwebsocket_context* context, libwebsocket* wsi, libwebsocket_callback_reasons reason, void* user, void* in, size_t len);
	static int static_callback_logger(libwebsocket_context* context, libwebsocket* wsi, libwebsocket_callback_reasons reason, void* user, void* in, size_t len);
	void SetEnabled(bool enabled, bool block = false);
	void SetPort(int newPort);

private:
	static WebServer* instance;
	enum Protocols {
		HTTP = 0,
		PREFERENCES,
		TIME,
		PING,
		LOGGER,
		COUNT
	};
	struct per_session_data__nwtproxy {
		std::string outBuf;
		int socket;
	};
	struct per_session_data__time {
		std::string outBuf;
	};
	struct per_session_data__ping {
		std::string outBuf;
	};
	struct per_session_data__logger {
		std::string inBuf;
		std::string outBuf;
		ghLib::Logger::View* view;
	};


	int port;
	static libwebsocket_protocols protocols[];
	libwebsocket_context* context;
	std::atomic_flag taskRunning;
	lws_context_creation_info info;
	ghLib::Logger* logger;

	WebServer();
	void Task();
	int callback_http(libwebsocket_context* context, libwebsocket* wsi, libwebsocket_callback_reasons reason, void* user, void* in, size_t len);
	int callback_nwtproxy(libwebsocket_context* context, libwebsocket* wsi, libwebsocket_callback_reasons reason, void* user, void* in, size_t len);
	int callback_time(libwebsocket_context* context, libwebsocket* wsi, libwebsocket_callback_reasons reason, void* user, void* in, size_t len);
	int callback_ping(libwebsocket_context* context, libwebsocket* wsi, libwebsocket_callback_reasons reason, void* user, void* in, size_t len);
	int callback_logger(libwebsocket_context* context, libwebsocket* wsi, libwebsocket_callback_reasons reason, void* user, void* in, size_t len);
	void dump_handshake_info(libwebsocket* wsi);


};

}
}

#endif /* GHLIB_WEB_SERVER_H_ */
