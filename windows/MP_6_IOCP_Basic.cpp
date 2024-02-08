#include "lib.h"

#define DEFAULT_BUFLEN 1024

struct Session {
    WSAOVERLAPPED overlapped = {};
    SOCKET sock = INVALID_SOCKET;
    char buf[1024] = {};
    int recvbytes = 0;
};

int main() {
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    SOCKET servsock = WSASocket(
        AF_INET, SOCK_STREAM, 0,
        NULL, 0, WSA_FLAG_OVERLAPPED
    );
    if (servsock == INVALID_SOCKET) {
        cout << "WSASocket() error" << endl;
        return 1;
    }

    SOCKADDR_IN servaddr;
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(12345);

    if (bind(servsock, (SOCKADDR*)&servaddr, sizeof(servaddr)) == SOCKET_ERROR) {
        cout << "bind() error" << endl;
        return 1;
    }

    if (listen(servsock, SOMAXCONN) == SOCKET_ERROR) {
        cout << "listen() error" << endl;
        return 1;
    }

    HANDLE iocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
    CreateIoCompletionPort((HANDLE)servsock, iocp, 0, 0);

    vector<Session*> sessions;

    while (true) {
        SOCKADDR_IN clientaddr;
        int addrlen = sizeof(clientaddr);

        SOCKET clisock = accept(servsock, NULL, NULL);
        if (clisock == INVALID_SOCKET) {
            cout << "accept() error" << endl;
            return 1;
        }

        Session* session = new Session();
        session->sock = clisock;
        sessions.push_back(session);

        cout << "Client Connected" << endl;

        CreateIoCompletionPort((HANDLE)session->sock, iocp, (ULONG_PTR)session, 0);



        WSABUF wsabuf_R = {};
        wsabuf_R.buf = session->buf;
        wsabuf_R.len = DEFAULT_BUFLEN;

        DWORD flags = 0;

        WSARecv(
            clisock, &wsabuf_R, 1,
            (LPDWORD)&session->recvbytes, &flags,
            &session->overlapped, NULL
        );

        DWORD bytesTransfered;
        ULONG_PTR compKey;
        LPOVERLAPPED lpOverlapped;

        GetQueuedCompletionStatus(
            iocp, &bytesTransfered,
            &compKey, &lpOverlapped, INFINITE
        );

        Session* complSession = (Session*)compKey;
        if (bytesTransfered == 0) {
            cout << "Client Disconnected" << endl;
            closesocket(complSession->sock);
            delete complSession;

            continue;
        }

        WSABUF wsabuf_S = {};
        wsabuf_S.buf = complSession->buf;
        wsabuf_S.len = DEFAULT_BUFLEN;

        WSASend(
            complSession->sock, &wsabuf_S, 1,
            (LPDWORD)&complSession->recvbytes, 0,
            lpOverlapped, NULL
        );



    }

    for (auto session : sessions) {
        closesocket(session->sock);
        delete session;
    }

    closesocket(servsock);
    WSACleanup();
    return 0;
}
