#include "lib.h"

#define DEFAULT_BUFLEN 1024

struct Session {
    // Completion Routine Callback Model은 Overlapped IO를 사용
    
    // Overlapped에서 CONTAINING_RECORD로 온전하게 Session 구조체를 참조할 수 있도록
    // 반드시 Session 구조체의 첫번째 멤버로 WSAOVERLAPPED 구조체를 두어야 한다
    WSAOVERLAPPED overlapped = {};
    SOCKET sock = INVALID_SOCKET;
    char buf[DEFAULT_BUFLEN] = {};
    WSABUF wsabuf = {};
    int recvbytes = 0;
    int sendbytes = 0;

    Session() {}
    Session(SOCKET sock) : sock(sock) {}
};

// Completion Routine Callback을 RecvCallback, SendCallback으로 구현
void CALLBACK RecvCallback(DWORD error, DWORD recvbytes, LPWSAOVERLAPPED overlapped, DWORD flags);
void CALLBACK SendCallback(DWORD error, DWORD sendbytes, LPWSAOVERLAPPED overlapped, DWORD flags);

int main() {
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    // WSASocket을 사용하여 비동기 소켓 생성
    SOCKET servsock = WSASocket(
        AF_INET, SOCK_STREAM, 0,
        NULL, 0, WSA_FLAG_OVERLAPPED
    );
    if (servsock == INVALID_SOCKET) {
        cout << "socket() error" << endl;
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

    vector<Session*> sessions;
    sessions.reserve(100);

    // Event를 사용하지 않고 단일 루프로 구현

    while (true) {
        SOCKADDR_IN cliaddr;
        int addrlen = sizeof(cliaddr);
        SOCKET clisock = accept(servsock, (SOCKADDR*)&cliaddr, &addrlen);

        if (clisock != INVALID_SOCKET) {
            // 이벤트를 쓰지 않기 때문에, 논블로킹 모드로 수동 전환
            u_long on = 1;
            if (ioctlsocket(clisock, FIONBIO, &on) == SOCKET_ERROR) {
                cout << "ioctlsocket() error" << endl;
                break;
            }

            // CR 모델에서는 Session을 동적 할당하여 Danling Pointer 문제를 해결
            Session* newsession = new Session( clisock );
            newsession->wsabuf.buf = newsession->buf;
            newsession->wsabuf.len = DEFAULT_BUFLEN;
            newsession->overlapped = {};
            sessions.push_back(newsession);

            cout << "Client Connected" << endl;

            DWORD flags = 0;

            WSARecv(
                newsession->sock, &newsession->wsabuf, 1,
                (LPDWORD)&newsession->recvbytes, &flags,
                &newsession->overlapped, RecvCallback
                // 마지막인자를 NULL에서 RecvCallback으로 변경하면
                // Event 방식에서 CR Callback 방식으로 변경
            );
        }

        // CR 모델의 특징으로, 처리가 끝난 시점을 오직 Alertable Wait로만 판단하기 때문에,
        // SleepEx 함수를 사용하여 Alertable Wait(Sleep과 비슷하지만 Ex를 꼭 써야함)을 수행 
        SleepEx(50, TRUE); // 50ms 동안 대기
    }

    closesocket(servsock);

    WSACleanup();
    return 0;
}

// RecvCallback은 WSARecv의 CR로 사용해서 WSASend까지 수행
void CALLBACK RecvCallback(
    DWORD error, DWORD recvbytes, LPWSAOVERLAPPED overlapped, DWORD flags
) {
    // Overlapped에서 CONTAINING_RECORD를 사용해서 Session 포인터로 변환
    Session* session = CONTAINING_RECORD(overlapped, Session, overlapped);
    if (error || recvbytes == 0) {
        cout << "Client Disconnected" << endl;
        closesocket(session->sock);
        delete session;
        return;
    }

    session->sendbytes = recvbytes;

    if (
        WSASend(
            session->sock, &session->wsabuf, 1, 
            (LPDWORD)&session->sendbytes, 0, 
            overlapped, SendCallback
        ) == SOCKET_ERROR
    ) {
        // WSA_IO_PENDING 상태면, Alertable Wait를 한 번 수행해봄
        // 아니면, 에러 발생
        if (WSAGetLastError() != WSA_IO_PENDING) {
            cout << "WSASend() error"<< endl;
            closesocket(session->sock);
            delete session;
            return;
        }

        SleepEx(50, TRUE);
    }
}

// SendCallback은 WSASend의 CR로 사용해서 WSARecv까지 수행
void CALLBACK SendCallback(
    DWORD error, DWORD sendbytes, LPWSAOVERLAPPED overlapped, DWORD flags
) {
    // Overlapped에서 CONTAINING_RECORD를 사용해서 Session 포인터로 변환
    Session* session = CONTAINING_RECORD(overlapped, Session, overlapped);
    if (error) {
        cout << "send() error" << endl;
        closesocket(session->sock);
        delete session;
        return;
    }

    session->recvbytes = DEFAULT_BUFLEN;
    session->wsabuf.buf = session->buf;
    session->wsabuf.len = DEFAULT_BUFLEN;

    if (
        WSARecv(
            session->sock, &session->wsabuf, 1,
            (LPDWORD)&session->recvbytes, &flags,
            overlapped, RecvCallback
        ) == SOCKET_ERROR
    ) {
        // WSA_IO_PENDING 상태면, Alertable Wait를 한 번 수행해봄
        // 아니면, 에러 발생
        if (WSAGetLastError() != WSA_IO_PENDING) {
            cout << "WSARecv() error" << endl;
            closesocket(session->sock);
            delete session;
            return;
        } 
        
        SleepEx(50, TRUE);
    }
}
// Completion Routine Callback Model은 Event Model과 달리,
// 이벤트를 전혀 사용하지 않고, 단일 루프로 구현할 수 있어
// 이벤트 기반의 고질적 문제인 64개 제한을 해결할 수 있다

// 하지만, 콜백 체이닝 과정에서 각 비동기 소켓의 입출력이 끝난 시점을 알 수 없어,
// APC Queue(쓰레드 당 하나씩 있는 작업 큐)에 작업을 넣어주고,
// Alertable Wait를 수행해서 처리가 끝난 시점을 알아내야 한다

// 이는 Sleep을 사용한거나 마찬가지이기 때문에, 성능 하락에 영향을 줄 수 있다
