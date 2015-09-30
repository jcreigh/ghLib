/*----------------------------------------------------------------------------*/
/* Copyright (c) Creighton 2015. All Rights Reserved.                         */
/* Open Source Software - May be modified and shared but must                 */
/* be accompanied by the license file in the root source directory            */
/*----------------------------------------------------------------------------*/

#include "ghLib/web/Server.h"
#include <sys/socket.h>
#include <netdb.h>
#include <fcntl.h>

namespace ghLib {
namespace web {

int WebServer::static_callback_nwtproxy(libwebsocket_context* context, libwebsocket* wsi, libwebsocket_callback_reasons reason, void* user, void* in, size_t len) {
	instance->callback_nwtproxy(context, wsi, reason, user, in, len);
}

int WebServer::callback_nwtproxy(libwebsocket_context* context, libwebsocket* wsi, libwebsocket_callback_reasons reason, void* user, void* in, size_t len) {
	auto log = logger->GetSubLogger("NWTProxy");
	per_session_data__nwtproxy* pss = nullptr;
	if (reason != LWS_CALLBACK_PROTOCOL_INIT) {
		pss = *((per_session_data__nwtproxy**)user);
	}
	std::string inBuf = std::string((const char*)in, len);
	switch (reason) {
		case LWS_CALLBACK_ESTABLISHED:
			{
			*((per_session_data__nwtproxy**)user) = new per_session_data__nwtproxy();
			pss = *((per_session_data__nwtproxy**)user);
			log->Trace(ghLib::Format("[%p] Connection established", pss));

			pss->socket = socket(AF_INET, SOCK_STREAM, 0);

			sockaddr_in serverAddr;
			hostent* server = gethostbyname("127.0.0.1");

			memset(&serverAddr, 0, sizeof(serverAddr));
			serverAddr.sin_family = AF_INET;
			serverAddr.sin_port = htons(1735); // Default network tables port. TODO: Make this configurable?

			if (connect(pss->socket, (sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
				log->Error("Error connecting to network table"); // TODO: Do something
				close(pss->socket);
				pss->socket = 0;
			} else {

			int flags;
#if defined(O_NONBLOCK)
			if (-1 == (flags = fcntl(pss->socket, F_GETFL, 0)))
				flags = 0;
			fcntl(pss->socket, F_SETFL, flags | O_NONBLOCK);
#else
			flags = 1;
			ioctl(pss->socket, FIOBIO, &flags);
#endif

			}
			}

			break;
		case LWS_CALLBACK_CLOSED:
			log->Trace(ghLib::Format("[%p] Closed", pss));
			close(pss->socket);
			delete pss;
			break;
		case LWS_CALLBACK_FILTER_PROTOCOL_CONNECTION:
			log->Trace(ghLib::Format("[%p] Filter protocol connection", pss));
			dump_handshake_info(wsi);
			break;
		case LWS_CALLBACK_RECEIVE:
			log->Trace(ghLib::Format("<<< %s <<<", ghLib::FilterASCII(inBuf).c_str()));
			if (inBuf.size() > 0) {
				if (send(pss->socket, inBuf.c_str(), inBuf.size(), 0) == -1) {
					pss->outBuf = ghLib::Format("Error writing to socket: %d", errno);
					log->Error(pss->outBuf);
					pss->outBuf += "\n";
				}
			}
			return 1;
		case LWS_CALLBACK_SERVER_WRITEABLE:
			{
				if (pss->socket) {
					char incomingBuffer[1024];
					ssize_t recvAmount = recv(pss->socket, incomingBuffer, 1024, 0);
					if (recvAmount < 0 && errno != EAGAIN) {
						pss->outBuf = ghLib::Format("Error reading from socket: (%s) %d", strerror(errno), errno);
						log->Error(pss->outBuf);;
						pss->outBuf += "\n";
					} else if (recvAmount > 0) {
						pss->outBuf = std::string(incomingBuffer, recvAmount);
					}
				}

				if (pss->outBuf.size()) {
					int m = libwebsocket_write(wsi, (uint8_t*)pss->outBuf.c_str(), pss->outBuf.size(), LWS_WRITE_BINARY);
					log->Trace(ghLib::Format(">>> %s >>>", ghLib::FilterASCII(pss->outBuf).c_str()));
					if (m < (int)pss->outBuf.size()) {
						log->Error(ghLib::Format("%d writing to nwtproxy socket", pss->outBuf.size()));
						pss->outBuf = "";
						return -1;
					}
					pss->outBuf = "";
				}
			}
			break;
		default:
			break;
	}
	return 0;
}

}
}

