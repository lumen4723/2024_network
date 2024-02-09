#include "lib.h"

#define DEFAULT_BUFLEN 1024
#define MAX_THREAD 8

struct Session {
    SOCKET sock = INVALID_SOCKET;
    char buf[1024] = {};
    WSAOVERLAPPED readOverLapped = {};
    WSAOVERLAPPED writeOverLapped = {};

    Session() {}
    Session(SOCKET sock) : sock(sock) {}
};

atomic<bool> TPoolRunning = true;

void WorkerThread(HANDLE iocpHd);

MemoryPool* MemPool = new MemoryPool(sizeof(Session), 1000);

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

    u_long on = 1;
    if (ioctlsocket(servsock, FIONBIO, &on) == SOCKET_ERROR) {
        cout << "ioctlsocket() error" << endl;
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

    vector<Session*> sessions;
    sessions.reserve(100);

    for (int i = 0; i < MAX_THREAD; i++) {
        HANDLE hThread = CreateThread(
            NULL, 0,
            (LPTHREAD_START_ROUTINE)WorkerThread,
            iocp, 0, NULL
        );

        CloseHandle(hThread);
    }

    while (true) {
        SOCKADDR_IN clientaddr;
        int addrlen = sizeof(clientaddr);

        SOCKET clisock = accept(servsock, NULL, NULL);
        if (clisock == INVALID_SOCKET) {
            if (WSAGetLastError() != WSAEWOULDBLOCK) {
                cout << "accept() error" << endl;
                return 1;
            }

            continue;
        }

        u_long on = 1;
        if (ioctlsocket(clisock, FIONBIO, &on) == SOCKET_ERROR) {
            cout << "ioctlsocket() error" << endl;
            return 1;
        }

        // Session* session = new Session({ clisock }); // 문제의 코드
        // new는 메모리를 동적(런타임 할당)으로 할당하고, 생성자를 호출함
        // 하지만 메모리 동적 할당 작업은 상당히 느리고, 메모리 누수가 발생할 여지가 생김
        Session* session = MemPool_new<Session>(*MemPool, clisock); // 해결책
        
        // C스타일로 하려면, 제네릭을 주기위해 void* alloc로 만든 다음 
        // 포인터 캐스팅해야 함
        // Session* session = (Session*)MemPool_alloc(sizeof(Session));
        // session->sock = clisock;


        sessions.push_back(session);

        cout << "Client Connected" << endl;

        CreateIoCompletionPort((HANDLE)session->sock, iocp, (ULONG_PTR)session, 0);

        WSABUF wsabuf_R = {};
        wsabuf_R.buf = session->buf;
        wsabuf_R.len = DEFAULT_BUFLEN;

        DWORD flags = 0;

        WSARecv(
            clisock, &wsabuf_R, 1,
            NULL, &flags, &session->readOverLapped, NULL
        );
    }

    for (auto session : sessions) {
        closesocket(session->sock);
        // delete session; // 문제의 코드
        // new로 할당한 메모리는 누수 방지를 위해 delete로 해제해야 함
        // 하지만, delete로 해제 역시 성능 문제가 있음 
        MemPool_delete(*MemPool, session); // 해결책
    }

    TPoolRunning = false;
    CloseHandle(iocp);

    closesocket(servsock);
    WSACleanup();
    return 0;
}


void WorkerThread(HANDLE iocpHd) {
    DWORD bytesTransfered;
    Session* session;
    LPOVERLAPPED lpOverlapped;
    WSABUF wsabuf_S = {}, wsabuf_R = {};

    while (TPoolRunning) {
        bool ret = GetQueuedCompletionStatus(
            iocpHd, &bytesTransfered,
            (ULONG_PTR*)&session, &lpOverlapped, INFINITE
        );
        if (!ret || bytesTransfered == 0) {
            cout << "Client Disconnected" << endl;
            closesocket(session->sock);
            // delete session; // 문제의 코드
            // new로 할당한 메모리는 누수 방지를 위해 delete로 해제해야 함
            // 하지만, delete로 해제 역시 성능 문제가 있음
            MemPool_delete(*MemPool, session); // 해결책

            continue;
        }

        if (lpOverlapped == &session->readOverLapped) {
            wsabuf_S.buf = session->buf;
            wsabuf_S.len = bytesTransfered;

            WSASend(
                session->sock, &wsabuf_S, 1,
                NULL, 0, &session->writeOverLapped, NULL
            );
        }
        else if (lpOverlapped == &session->writeOverLapped) {
            wsabuf_R.buf = session->buf;
            wsabuf_R.len = DEFAULT_BUFLEN;

            DWORD flags = 0;

            WSARecv(
                session->sock, &wsabuf_R, 1,
                NULL, &flags, &session->readOverLapped, NULL
            );
        }
    }
}
