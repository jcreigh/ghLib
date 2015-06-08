#include <atomic>
#include <chrono>
#include <string.h>
#include "libwebsockets.h"
#include "ghLib/Runnable.h"

namespace ghLib {
namespace web {

class WebServer : public Runnable {
public:
	static WebServer *GetInstance();
	static int static_callback_http(libwebsocket_context* context, libwebsocket* wsi, libwebsocket_callback_reasons reason, void* user, void* in, size_t len);
	static int static_callback_nwt(libwebsocket_context* context, libwebsocket* wsi, libwebsocket_callback_reasons reason, void* user, void* in, size_t len);
	void SetEnabled(bool enabled, bool block = false);
	void SetPort(int newPort);

private:
	static WebServer* instance;
	enum Protocols {
		HTTP = 0,
		PREFERENCES,
		COUNT
	};
	class per_session_data__nwt {
		public:
		std::string outBuf;
		per_session_data__nwt() {
			printf("Initializing pss nwt - %p\n", this);
			//outBuf = new std::string();
		}
		~per_session_data__nwt() {
			printf("Destroying pss nwt - %p\n", this);
		}
	};

	int port;
	static libwebsocket_protocols protocols[];
	libwebsocket_context* context;
	std::atomic_flag taskRunning;
	lws_context_creation_info info;

	WebServer();
	void Task();
	int callback_http(libwebsocket_context* context, libwebsocket* wsi, libwebsocket_callback_reasons reason, void* user, void* in, size_t len);
	int callback_nwt(libwebsocket_context* context, libwebsocket* wsi, libwebsocket_callback_reasons reason, void* user, void* in, size_t len);
	void dump_handshake_info(libwebsocket* wsi);


};

}
}
