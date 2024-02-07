#include "lib.h"

#define DEFAULT_BUFLEN 1024

struct Session {
    SOCKET sock = INVALID_SOCKET;
    char buf[DEFAULT_BUFLEN] = {};
    // WSABUF 구조체는 WSARecv, WSASend 함수에서 사용
    WSABUF wsabuf = {};
    int recvbytes = 0;
    int sendbytes = 0;
    // WSAOVERLAPPED 구조체는 비동기 소켓 함수에서 사용
    WSAOVERLAPPED overlapped = {};
};

int main() {
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    // WSASocket을 사용하여 비동기 소켓 생성
    // WSA_FLAG_OVERLAPPED 플래그를 사용하여 Overlapped IO를 사용
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

    vector<Session> sessions;
    sessions.reserve(100);

    // servsock에 대한 이벤트 객체 생성
    WSAEVENT servEvent = WSACreateEvent();

    // servsock + cli_OverLapped에 대한 이벤트 객체 생성
    vector<WSAEVENT> wsaEvents;
    wsaEvents.push_back(servEvent);
    sessions.push_back(Session{ servsock });

    // Accept, Close 를 감시
    if (WSAEventSelect(servsock, servEvent, FD_ACCEPT | FD_CLOSE) == SOCKET_ERROR) {
        cout << "WSAEventSelect() error" << endl;
        return 1;
    }

    while (true) {
        int ret = WSAWaitForMultipleEvents(
            wsaEvents.size(), &wsaEvents[0],
            FALSE, WSA_INFINITE, FALSE
        );
        if (ret == WSA_WAIT_FAILED) {
            cout << "WSAWaitForMultipleEvents() error" << endl;
            return 1;
        }

        int index = ret - WSA_WAIT_EVENT_0;

        WSANETWORKEVENTS networkEvents;
        if (
            WSAEnumNetworkEvents(
                sessions[index].sock, wsaEvents[index], &networkEvents
            ) == SOCKET_ERROR
        ) {
            cout << "WSAEnumNetworkEvents() error"
                << WSAGetLastError() << endl;
            return 1;
        }


        // FD_ACCEPT
        if (networkEvents.lNetworkEvents & FD_ACCEPT) {
            if (networkEvents.iErrorCode[FD_ACCEPT_BIT] != 0) {
                cout << "FD_ACCEPT failed" << endl;
                return 1;
            }

            SOCKADDR_IN cliaddr;
            int addrlen = sizeof(cliaddr);

            SOCKET clisock = accept(servsock, (SOCKADDR*)&cliaddr, &addrlen);
            if (clisock == INVALID_SOCKET) {
                if (WSAGetLastError() == WSAEWOULDBLOCK) {
                    continue;
                }
                cout << "accept() error" << endl;
                return 1;
            }

            Session session = Session{ clisock };
            // OverLapped IO를 사용하기 위해 이벤트 객체 생성
            WSAEVENT OLEvent = WSACreateEvent();
            session.overlapped.hEvent = OLEvent;
            sessions.push_back(session);
            
            // OL이벤트는 감지용으로도 사용
            wsaEvents.push_back(OLEvent);
            if (WSAEventSelect(clisock, OLEvent, FD_READ | FD_CLOSE) == SOCKET_ERROR) {
                cout << "WSAEventSelect() error" << endl;
                return 1;
            }

            cout << "Client Connected" << endl;
        }

        if (networkEvents.lNetworkEvents & FD_READ) {
            if (networkEvents.iErrorCode[FD_READ_BIT] != 0) {
                cout << "FD_READ failed" << endl;
                return 1;
            }

            Session& session = sessions[index];
            session.wsabuf.buf = session.buf;
            session.wsabuf.len = DEFAULT_BUFLEN;

            DWORD flags = 0;
            if (
                WSARecv( // WSARecv를 실행하면서 Overlapped IO를 사용
                    session.sock, &session.wsabuf, 1,
                    (LPDWORD)&session.recvbytes, &flags,
                    &session.overlapped, NULL
                ) == SOCKET_ERROR
            ) {
                if (WSAGetLastError() != WSA_IO_PENDING) {
                    cout << "WSARecv() error" << endl;
                    return 1;
                }
            }

            // WSAGetOverlappedResult는 WSARecv가 완료되면 TRUE를 반환
            // 아직 진행 중이면, FALSE를 반환하면 WSA_IO_PENDING 상태
            if (
                WSAGetOverlappedResult(
                    session.sock, &session.overlapped,
                    (LPDWORD)&session.recvbytes, FALSE, &flags
                )
                && WSASend( // WSASend를 실행하면서 Overlapped IO를 사용
                    session.sock, &session.wsabuf, 1,
                    (LPDWORD)&session.sendbytes, flags,
                    &session.overlapped, NULL
                ) == SOCKET_ERROR
            ) {
                if (WSAGetLastError() != WSA_IO_PENDING) {
                    cout << "WSASend() error" << endl;
                    return 1;
                }
            } // WSAGetOverlappedResult는 WSASend가 완료되면 TRUE를 반환
            // 하지만, 체이닝 할 게 없으므로, 다음 조건문으로 넘어감
        }

        if (networkEvents.lNetworkEvents & FD_CLOSE) {
            if (
                networkEvents.iErrorCode[FD_CLOSE_BIT] != 0
                && networkEvents.iErrorCode[FD_CLOSE_BIT] != WSAECONNABORTED
            ) {
                cout << "FD_CLOSE failed" << endl;
                return 1;
            }

            cout << "Client Disconnected" << endl;
            closesocket(sessions[index].sock);
            WSACloseEvent(sessions[index].overlapped.hEvent);
            sessions.erase(sessions.begin() + index);
            wsaEvents.erase(wsaEvents.begin() + index);
        }

    }
    WSACloseEvent(servEvent);

    closesocket(servsock);

    WSACleanup();
    return 0;
} // Overlapped IO + Event Model은 Non-Blocking + Asynchronous 라서 빠르고 효율적이지만,
// 여전히 하나의 이벤트에는 64개의 클라이언트만 감시할 수 있음