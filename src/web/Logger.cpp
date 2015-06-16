/*----------------------------------------------------------------------------*/
/* Copyright (c) Creighton 2015. All Rights Reserved.                         */
/* Open Source Software - May be modified and shared but must                 */
/* be accompanied by the license file in the root source directory            */
/*----------------------------------------------------------------------------*/

#include "ghLib/web/Server.h"

namespace ghLib {
namespace web {

int WebServer::static_callback_logger(libwebsocket_context* context, libwebsocket* wsi, libwebsocket_callback_reasons reason, void* user, void* in, size_t len) {
	instance->callback_logger(context, wsi, reason, user, in, len);
}

int WebServer::callback_logger(libwebsocket_context* context, libwebsocket* wsi, libwebsocket_callback_reasons reason, void* user, void* in, size_t len) {
	auto log = logger->GetSubLogger("Logger");
	per_session_data__logger* pss = nullptr;
	if (reason != LWS_CALLBACK_PROTOCOL_INIT) {
		pss = *((per_session_data__logger**)user);
	}
	std::string inBuf = std::string((const char*)in, len);
	switch (reason) {
		case LWS_CALLBACK_ESTABLISHED:
			*((per_session_data__logger**)user) = new per_session_data__logger();
			break;
		case LWS_CALLBACK_CLOSED:
			delete pss;
			break;
		case LWS_CALLBACK_FILTER_PROTOCOL_CONNECTION:
			dump_handshake_info(wsi);
			break;
		case LWS_CALLBACK_RECEIVE:
			if (inBuf.size() > 0) {
				log->Debug(ghLib::Format("< %s", inBuf.c_str()));
				if (inBuf[0] == 'G') {
					auto logName = inBuf.substr(1);
					log->Trace(ghLib::Format("Getting entries from log: \"%s\"", logName.c_str()));
					std::stringstream ss;
					ss << *ghLib::Logger::GetLogger(logName);
					pss->outBuf = ss.str();
				}
			}
			return 1;
		case LWS_CALLBACK_SERVER_WRITEABLE:
			if (pss->outBuf.size()) {
				int m = libwebsocket_write(wsi, (uint8_t*)pss->outBuf.c_str(), pss->outBuf.size(), LWS_WRITE_TEXT);
				log->Debug(ghLib::Format("> %s", pss->outBuf.c_str()));
				if (m < (int)pss->outBuf.size()) {
					log->Error(ghLib::Format("%d writing to nwt socket", pss->outBuf.size()));
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

}
}

