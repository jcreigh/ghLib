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
			pss = *((per_session_data__logger**)user);
			pss->view = new ghLib::Logger::View(ghLib::Logger::Level::TRACE);
			log->Info(ghLib::Format("[%p] Connection established", pss));
			break;
		case LWS_CALLBACK_CLOSED:
			log->Info(ghLib::Format("[%p] Closed", pss));
			delete pss->view;
			delete pss;
			break;
		case LWS_CALLBACK_FILTER_PROTOCOL_CONNECTION:
			log->Trace(ghLib::Format("Filter protocol connection"));
			dump_handshake_info(wsi);
			break;
		case LWS_CALLBACK_RECEIVE:
			if (inBuf.size() > 0) {
				pss->inBuf += inBuf;
			}
			return 1;
		case LWS_CALLBACK_SERVER_WRITEABLE:
			{
			auto pos = (pss->inBuf).find("\n");
			if (pos != std::string::npos) {
				auto buf = (pss->inBuf).substr(0, pos);
				pss->inBuf = (pss->inBuf).substr(pos + 1);
				auto tokens = ghLib::Tokenize(buf, ',', '\\');
				if (tokens[0] == "G") {
					int start = 0;
					int count = 10;
					if (tokens.size() > 1) {
						start = tokens[1].size() ? std::stoi(tokens[1]) : start;
						if (tokens.size() > 2) {
							count = tokens[2].size() ? std::stoi(tokens[2]) : count;
						}
					}
					log->Debug(ghLib::Format("[%p] Getting %d entries starting at %d", pss, count, start));
					auto entries = pss->view->GetEntries(start, count);
					std::string outBuf = ghLib::Format("{\"start\": %d, \"count\": %d, \"entries\": [", start, (int)entries.size());

					for (int i = 0; i < (int)entries.size(); i++) {
						auto e = entries[i];
						outBuf += ghLib::Format("{\"level\": \"%s\", \"text\": \"%s\", \"time\": %d, \"logger\": \"%s\"}",
								ghLib::Logger::levelNames[e.GetLevel()].c_str(),
								ghLib::EscapeString(e.GetText()).c_str(),
								(std::chrono::duration_cast<std::chrono::milliseconds>(e.GetTime().time_since_epoch()).count()),
								ghLib::EscapeString(e.GetLogger()->GetName()).c_str()
								);
						if ((i + 1) < (int)entries.size()) {
							outBuf += ",";
						}
					}
					outBuf += "]}";
					pss->outBuf += outBuf;
				} else if (tokens[0] == "S") {
					auto logName = tokens[1];
					auto levelInput = tokens[2];
					bool valid = false;
					auto level = ghLib::Logger::levelMap.find(levelInput);
					if (level != ghLib::Logger::levelMap.end()) {
						log->Info(ghLib::Format("Setting log \"%s\" verbosity to %s", logName.c_str(), levelInput.c_str()));
						pss->view->SetVerbosity(logName, level->second);
						pss->outBuf += "OK\n";
					} else {
						log->Error(ghLib::Format("Can't set log \"%s\" verbosity. Invalid level: \"%s\"", logName.c_str(), levelInput.c_str()));
						pss->outBuf += "FAIL\n";
					}
				}
					// enum Level { TRACE, DEBUG, INFO, WARN, ERROR, FATAL, DISABLED };

			}
			if (pss->outBuf.size()) {
				int m = libwebsocket_write(wsi, (uint8_t*)pss->outBuf.c_str(), pss->outBuf.size(), LWS_WRITE_TEXT);
				log->Debug(ghLib::Format(">>> %s >>>", ghLib::FilterASCII(pss->outBuf).c_str()));
				if (m < (int)pss->outBuf.size()) {
					log->Error(ghLib::Format("[%p] %d writing to logger socket", pss, pss->outBuf.size()));
					pss->outBuf = "";
					return -1;
				}
				pss->outBuf = "";
			}
			break;
			}
		default:
			break;
	}
	return 0;
}

}
}

