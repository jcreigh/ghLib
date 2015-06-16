/*----------------------------------------------------------------------------*/
/* Copyright (c) Creighton 2015. All Rights Reserved.                         */
/* Open Source Software - May be modified and shared but must                 */
/* be accompanied by the license file in the root source directory            */
/*----------------------------------------------------------------------------*/

#include "ghLib/web/Server.h"

namespace ghLib {
namespace web {

int WebServer::static_callback_nwt(libwebsocket_context* context, libwebsocket* wsi, libwebsocket_callback_reasons reason, void* user, void* in, size_t len) {
	instance->callback_nwt(context, wsi, reason, user, in, len);
}

int WebServer::callback_nwt(libwebsocket_context* context, libwebsocket* wsi, libwebsocket_callback_reasons reason, void* user, void* in, size_t len) {
	auto log = logger->GetSubLogger("NWT");
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
			log->Debug(ghLib::Format("< %s\n", inBuf.c_str()));
			if (inBuf.size() > 0) {
				if (inBuf[0] == 0x00) {
					// Keep-alive, ignore
				} else if (inBuf[0] == 0x01) {
					if (inBuf[1] == 0x00 && inBuf[2] == 0x02) {
					} else {
						pss->outBuf += "\x02\x00\x02";
					}
				}
			if (inBuf == "hello\n") {
				pss->outBuf = "Hello, world!";
			}
			}
			return 1;
		case LWS_CALLBACK_SERVER_WRITEABLE:
			if (pss->outBuf.size()) {
				int m = libwebsocket_write(wsi, (uint8_t*)pss->outBuf.c_str(), pss->outBuf.size(), LWS_WRITE_TEXT);
				log->Debug(ghLib::Format("> %s\n", pss->outBuf.c_str()));
				if (m < (int)pss->outBuf.size()) {
					log->Error(ghLib::Format("%d writing to nwt socket\n", pss->outBuf.size()));
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

