// #include "lib.h"

// #define DEFAULT_BUFLEN 1024

// struct Session {
//     SOCKET sock = INVALID_SOCKET;
//     char buf[DEFAULT_BUFLEN] = {};
//     WSABUF wsabuf = {};
//     int recvbytes = 0;
//     int sendbytes = 0;
//     WSAOVERLAPPED overlapped = {};
// };

// void CALLBACK RecvCallback(DWORD error, DWORD recvbytes, LPWSAOVERLAPPED overlapped, DWORD flags);
// void CALLBACK SendCallback(DWORD error, DWORD sendbytes, LPWSAOVERLAPPED overlapped, DWORD flags);

// int main() {
//     WSADATA wsaData;
//     WSAStartup(MAKEWORD(2, 2), &wsaData);

//     SOCKET servsock = WSASocket(
//         AF_INET, SOCK_STREAM, 0,
//         NULL, 0, WSA_FLAG_OVERLAPPED
//     );
//     if (servsock == INVALID_SOCKET) {
//         cout << "socket() error" << endl;
//         return 1;
//     }

//     SOCKADDR_IN servaddr;
//     memset(&servaddr, 0, sizeof(servaddr));
//     servaddr.sin_family = AF_INET;
//     servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
//     servaddr.sin_port = htons(12345);

//     if (bind(servsock, (SOCKADDR*)&servaddr, sizeof(servaddr)) == SOCKET_ERROR) {
//         cout << "bind() error" << endl;
//         return 1;
//     }

//     if (listen(servsock, SOMAXCONN) == SOCKET_ERROR) {
//         cout << "listen() error" << endl;
//         return 1;
//     }

//     vector<Session*> sessions;
//     sessions.reserve(100);

//     WSAEVENT servEvent = WSACreateEvent();
//     if (WSAEventSelect(servsock, servEvent, FD_ACCEPT | FD_CLOSE) == SOCKET_ERROR) {
//         cout << "WSAEventSelect() error" << endl;
//         return 1;
//     }

//     while (true) {
//         if (WSAWaitForMultipleEvents(1, &servEvent, FALSE, WSA_INFINITE, FALSE) == WSA_WAIT_FAILED) {
//             cout << "WSAWaitForMultipleEvents() error" << endl;
//             break;
//         }

//         WSANETWORKEVENTS netEvents;
//         if (WSAEnumNetworkEvents(servsock, servEvent, &netEvents) == SOCKET_ERROR) {
//             cout << "WSAEnumNetworkEvents() error" << endl;
//             break;
//         }

//         if (netEvents.lNetworkEvents & FD_ACCEPT) {
//             if (netEvents.iErrorCode[FD_ACCEPT_BIT] != 0) {
//                 cout << "FD_ACCEPT error" << endl;
//                 break;
//             }

//             SOCKADDR_IN cliaddr;
//             int addrlen = sizeof(cliaddr);
//             SOCKET clisock = accept(servsock, (SOCKADDR*)&cliaddr, &addrlen);
//             while (clisock == INVALID_SOCKET) {
//                 if (WSAGetLastError() != WSAEWOULDBLOCK) {
//                     cout << "accept() error" << endl;
//                     break;
//                 }
//                 continue;
//             }

//             // 이벤트를 쓰지 않기 때문에, 논블로킹 모드로 수동 전환
//             u_long on = 1;
//             if (ioctlsocket(clisock, FIONBIO, &on) == SOCKET_ERROR) {
//                 cout << "ioctlsocket() error" << endl;
//                 break;
//             }

//             Session* newsession = new Session{ clisock };
//             sessions.push_back(newsession);
//             newsession->wsabuf.buf = newsession->buf;
//             newsession->wsabuf.len = DEFAULT_BUFLEN;
//             newsession->overlapped = {};

//             cout << "Client connected" << endl;

//             DWORD flags = 0;
//             if (
//                 WSARecv(
//                     clisock, &sessions.back()->wsabuf, 1,
//                     (LPDWORD)&sessions.back()->recvbytes, &flags,
//                     &sessions.back()->overlapped, RecvCallback
//                 ) == SOCKET_ERROR
//             ) {
//                 if (WSAGetLastError() != WSA_IO_PENDING) {
//                     cout << "WSARecv() error" << endl;
//                     break;
//                 }

//                 SleepEx(INFINITE, TRUE);
//             }
//         }

//         if (netEvents.lNetworkEvents & FD_CLOSE) {
//             if (netEvents.iErrorCode[FD_CLOSE_BIT] != 0) {
//                 cout << "FD_CLOSE error" << endl;
//                 break;
//             }
//         }
//     }

//     closesocket(servsock);

//     WSACleanup();
//     return 0;
// }

// void CALLBACK RecvCallback(
//     DWORD error, DWORD recvbytes, LPWSAOVERLAPPED overlapped, DWORD flags
// ) {
//     Session* session = (Session*)overlapped;
//     if (error || recvbytes == 0) {
//         cout << "recv() error" << endl;
//         closesocket(session->sock);
//         return;
//     }

//     session->sendbytes = recvbytes;

//     session->wsabuf.buf = session->buf;
//     session->wsabuf.len = session->recvbytes;
//     session->overlapped = {};

//     if (
//         WSASend(
//             session->sock, &session->wsabuf, 1,
//             (LPDWORD)&session->sendbytes, flags,
//             &session->overlapped, SendCallback
//         ) == SOCKET_ERROR
//     ) {
//         if (WSAGetLastError() != WSA_IO_PENDING) {
//             cout << "WSASend() error"
//                 << WSAGetLastError() << endl;
//             closesocket(session->sock);
//             return;
//         }

//         SleepEx(INFINITE, TRUE);
//     }
// }

// void CALLBACK SendCallback(
//     DWORD error, DWORD sendbytes, LPWSAOVERLAPPED overlapped, DWORD flags
// ) {
//     Session* session = (Session*)overlapped;

//     if (error) {
//         cout << "send() error" << endl;
//         closesocket(session->sock);
//         return;
//     }

//     session->recvbytes = 0;

//     session->wsabuf.buf = session->buf;
//     session->wsabuf.len = DEFAULT_BUFLEN;
//     session->overlapped = {};

//     if (
//         WSARecv(
//             session->sock, &session->wsabuf, 1,
//             (LPDWORD)&session->recvbytes, &flags,
//             &session->overlapped, RecvCallback
//         ) == SOCKET_ERROR
//     ) {
//         if (WSAGetLastError() != WSA_IO_PENDING) {
//             cout << "WSARecv() error" << endl;
//             closesocket(session->sock);
//             return;
//         }

//         SleepEx(INFINITE, TRUE);
//     }
// }