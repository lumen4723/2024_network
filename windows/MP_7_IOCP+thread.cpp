#include "lib.h"

#define DEFAULT_BUFLEN 1024
#define MAX_THREAD 8 // 쓰레드 풀의 최대 쓰레드 개수(권장: CPU 코어 개수)

struct Session {
    SOCKET sock = INVALID_SOCKET;
    char buf[1024] = {};
    WSAOVERLAPPED readOverLapped = {};
    WSAOVERLAPPED writeOverLapped = {};
};

// 쓰레드 풀의 종료 여부를 나타내는 플래그
atomic<bool> TPoolRunning = true;

// IOCP Worker Thread로 완료된 IO를 쓰레드 방식으로 처리
void WorkerThread(HANDLE iocpHd);

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

    // 기본 IOCP 핸들 생성
    HANDLE iocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);

    vector<Session*> sessions;
    sessions.reserve(100);

    // Worker Thread 풀 생성
    for (int i = 0; i < MAX_THREAD; i++) {

        // CreateThread 는 std::thread와 같은 기능을 제공하는 함수
        // 하지만, std::thread보다 windows API 특화로 조금 더 좋은 성능을 기대할 수 있음
        HANDLE hThread = CreateThread(
            NULL, 0,
            (LPTHREAD_START_ROUTINE)WorkerThread,
            iocp, 0, NULL
        );

        // 쓰레드 핸들은 CloseHandle로 닫아주어야 함
        // 쓰레드 핸들을 닫아도 쓰레드 자체는 계속 동작함
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

        Session* session = new Session({ clisock });
        sessions.push_back(session);

        cout << "Client Connected" << endl;

        // 클라이언트 소켓을 IOCP에 등록(=클라이언트 소켓에 대한 완료된 작업은 IOCP Queue에 들어감)
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
        delete session;
    }

    // 쓰레드 풀을 종료
    TPoolRunning = false;
    CloseHandle(iocp);

    closesocket(servsock);
    WSACleanup();
    return 0;
} // 다중 쓰레드 IOCP서버는 다중 코어를 사용하기 때문에, Basic보다 더 빠른 속도를 기대할 수 있음


void WorkerThread(HANDLE iocpHd) {
    DWORD bytesTransfered;
    Session* session;
    LPOVERLAPPED lpOverlapped;
    WSABUF wsabuf_S = {}, wsabuf_R = {};

    // Atomic 하게 TPoolRunning 전역 변수를 체크하여 종료 여부를 판단
    while (TPoolRunning) {
        bool ret = GetQueuedCompletionStatus(
            iocpHd, &bytesTransfered,
            (ULONG_PTR*)&session, &lpOverlapped, INFINITE
        );
        if (!ret || bytesTransfered == 0) {
            cout << "Client Disconnected" << endl;
            closesocket(session->sock);
            delete session;

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
} // 쓰레드 메인 코드는 Basic과 동일
