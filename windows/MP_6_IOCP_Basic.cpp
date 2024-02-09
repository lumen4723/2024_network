#include "lib.h"

#define DEFAULT_BUFLEN 1024

// Overlapped Model (Completion Routine Callback Model)
// 비동기 입출력 함수가 완료되면, 쓰레드마다 있는 APC Queue(=Alertable Wait Queue)에 콜백을 넣어줌
// 이후, Alertable Wait 상태에 있는 쓰레드는 APC Queue를 비움 (콜백을 사용해서)

// 하지만, APC Queue는 쓰레드마다 하나씩 있기 때문에, 쓰레드가 많을수록 성능이 떨어짐
// + Alertable Wait 상태에 진입하는데 비용도 다소 발생(Sleep + Context Switching)

// I/O Completion Port(IOCP) Model
// 비동기 입출력 함수가 완료되면, I/O Completion Port(=Global Queue)에 결과를 넣어줌
// I/O Completion Port(=Global Queue)는 전역적으로 하나만 있음
// Alertable Wait상태를 사용하는 것이 아니라 CP 결과 처리를 GetQueuedCompletionStatus 함수로 처리

// 쓰레드가 많아도 CP 큐는 하나이기도 하고
// GetQueuedCompletionStatus 함수는 Alertable Wait를 사용하지 않기 때문에 쓰레드와 궁합이 좋음

// 추가로, 쓰레드 풀(최대 CPU개수 만큼)을 사용 + 메모리 풀(동적 할당을 최소화)을 사용하여 성능을 극대화

struct Session {
    // recvbytes, sendbytes는 안쓰이므로 삭제
    // 대신 WSAOVERLAPPED 구조체를 두개 사용하여
    // Read, Write에 대한 Overlapped IO를 구현
    SOCKET sock = INVALID_SOCKET;
    char buf[1024] = {};
    WSAOVERLAPPED readOverLapped = {};
    WSAOVERLAPPED writeOverLapped = {};
};

int main() {
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    // IOCP도 WSASocket을 사용하여 비동기 소켓 생성
    SOCKET servsock = WSASocket(
        AF_INET, SOCK_STREAM, 0,
        NULL, 0, WSA_FLAG_OVERLAPPED
    );
    if (servsock == INVALID_SOCKET) {
        cout << "WSASocket() error" << endl;
        return 1;
    }

    // 논블로킹 모드로 설정
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

    // CreateIoCompletionPort를 사용하여 IOCP 생성 (INVALID_HANDLE_VALUE는 NULL과 같음)
    // 여기서 HANDLE은 IOCP Queue를 가리키는 포인터
    HANDLE iocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);

    vector<Session*> sessions;
    sessions.reserve(100);

    while (true) {
        SOCKADDR_IN clientaddr;
        int addrlen = sizeof(clientaddr);

        SOCKET clisock = accept(servsock, (SOCKADDR*)&clientaddr, &addrlen);
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

        // IOCP도 Session을 동적 할당하여 Danling Pointer 문제를 해결
        Session* session = new Session({ clisock });
        sessions.push_back(session);

        cout << "Client Connected" << endl;

        // 클라이언트 소켓을 IOCP에 등록(=클라이언트 소켓에 대한 완료된 작업은 IOCP Queue에 들어감)
        CreateIoCompletionPort((HANDLE)session->sock, iocp, (ULONG_PTR)session, 0);


        // WSABUF를 Recv, Send에 각각 하나씩 사용
        WSABUF wsabuf_R = {}, wsabuf_S = {};
        wsabuf_R.buf = session->buf;
        wsabuf_R.len = DEFAULT_BUFLEN;

        DWORD flags = 0;

        WSARecv(
            clisock, &wsabuf_R, 1,
            NULL, &flags, &session->readOverLapped, NULL
        );



        DWORD bytesTransfered;
        LPOVERLAPPED lpOverlapped;

        // GetQueuedCompletionStatus를 사용하여 IOCP Queue에서 완료된 작업을 가져옴
        bool ret = GetQueuedCompletionStatus(
            iocp, &bytesTransfered,
            (ULONG_PTR*)&session, &lpOverlapped, INFINITE
        );
        if (!ret || bytesTransfered == 0) { // 에러 발생(강제종료) 또는 클라이언트 종료시 클라 소켓 종료
            cout << "Client Disconnected" << endl;
            closesocket(session->sock);
            delete session;

            continue;
        }

        // GetQueuedCompletionStatus에서 받은 Overlapped는
        // 두 가지 Overlapped 중 하나일 것이므로
        // 각 케이스에 맞게 처리
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
        // 둘은 순환이 되면서 IOCP Queue에 계속해서 작업을 넣어줌


    }

    for (auto session : sessions) {
        closesocket(session->sock);
        delete session;
    }

    CloseHandle(iocp);

    closesocket(servsock);
    WSACleanup();
    return 0;
}
