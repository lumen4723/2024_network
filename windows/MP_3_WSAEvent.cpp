#include "lib.h"

#define DEFAULT_BUFLEN 1024

struct Session {
    SOCKET sock = INVALID_SOCKET;
    char buf[DEFAULT_BUFLEN];
    int recvbytes = 0;
    int sendbytes = 0;
};

int main() {
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    SOCKET servsock = socket(AF_INET, SOCK_STREAM, 0);
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

    // WSAEventSelect
    // 소켓과 관련된 네트워크 이벤트를 이벤트 객체를 통해 감지

    // WSACreateEvent (이벤트 객체 생성)
    // WSACloseEvent (이벤트 객체 제거)
    // WSAWaitForMultipleEvents (이벤트 신호 상태를 확인)
    // WSAEnumNetworkEvents (구체적인 이벤트 확인)

    vector<WSAEVENT> wsaEvents;
    WSAEVENT servEvent = WSACreateEvent();

    // Warning
    // WSAEventSelect를 사용하면 자동으로 해당 소켓이 반드시 논블로킹 모드로 전환
    // accept() 함수의 리턴값이 servsock과 동일한 타입이므로
    // clisock의 FD_READ, FD_WRITE, FD_CLOSE 는 다시 등록해줘야함

    // 드물게 WSAWOULDBLOCK 에러가 발생할 수 있음, 예외처리 필요

    // 이벤트 발생시 적절한 소켓함수를 호출해야함
    // 아니면, 다음 번에는 동일 네트워크의 이벤트가 발생하지 않음

    wsaEvents.push_back(servEvent);
    sessions.push_back(Session{ servsock });

    // 서버소켓은 Accept, Close 이벤트만 감지, 그리고 논블록으로 설정
    if (WSAEventSelect(servsock, servEvent, FD_ACCEPT | FD_CLOSE) == SOCKET_ERROR) {
        cout << "WSAEventSelect() error" << endl;
        return 1;
    }

    while (true) {
        // WSAWaitForMultipleEvents: 이벤트 신호 상태를 확인
        // 리턴 결과는 이벤트 객체의 첫 번째 인덱스
        int ret = WSAWaitForMultipleEvents(
            wsaEvents.size(),
            &wsaEvents[0],
            FALSE, WSA_INFINITE, FALSE
        );
        if (ret == WSA_WAIT_FAILED) {
            cout << "WSAWaitForMultipleEvents() error" << endl;
            return 1;
        }

        int index = ret - WSA_WAIT_EVENT_0;

        // 소켓 이벤트 초기화
        // WSAResetEvent(wsaEvents[index]); // 해당 내용은 아래에서 같이 처리됨
        
        WSANETWORKEVENTS networkEvents;
        if (WSAEnumNetworkEvents(sessions[index].sock, wsaEvents[index], &networkEvents) == SOCKET_ERROR) {
            cout << "WSAEnumNetworkEvents() error" << endl;
            return 1;
        }

        // 해당 소켓의 Accept 이벤트가 발생했을 경우
        if (networkEvents.lNetworkEvents & FD_ACCEPT) {
            if (networkEvents.iErrorCode[FD_ACCEPT_BIT] != 0) {
                cout << "FD_ACCEPT error" << endl;
                return 1;
            }

            SOCKADDR_IN cliaddr;
            int addrlen = sizeof(cliaddr);

            SOCKET clisock = accept(servsock, (SOCKADDR*)&cliaddr, &addrlen);
            if (clisock == INVALID_SOCKET) {
                if (WSAGetLastError() != WSAEWOULDBLOCK) {
                    cout << "accept() error" << endl;
                    return 1;
                }
                // WSAEWOULDBLOCK이면 다음 조건문으로 넘어감
            }
            else {
                WSAEVENT cliEvent = WSACreateEvent();
                wsaEvents.push_back(cliEvent);
                sessions.push_back(Session{clisock});

                // 클라소켓은 Read, Write, Close 이벤트만 감지, 그리고 논블록으로 설정
                if (WSAEventSelect(clisock, cliEvent, FD_READ | FD_WRITE | FD_CLOSE) == SOCKET_ERROR) {
                    cout << "WSAEventSelect() error" << endl;
                    return 1;
                }

                cout << "Client Connected" << endl;
            }
        }

        // wsaEvent는 소켓의 특정 이벤트까지도 감지할 수 있음
        // 하지만, recv, send는 여전히 동기적 처리방식을 사용하기 때문에,
        // 분할처리를 할 경우 recv에서 무한루프에 빠질 수 있음
        if (networkEvents.lNetworkEvents & (FD_READ | FD_WRITE)) {
            // error check
            if (
                (networkEvents.lNetworkEvents & FD_READ)
                && (networkEvents.iErrorCode[FD_READ_BIT] != 0)
            ) {
                cout << "FD_READ failed" << endl;
                return 0;
            }
            if (
                (networkEvents.lNetworkEvents & FD_WRITE)
                && (networkEvents.iErrorCode[FD_WRITE_BIT] != 0)
            ) {
                cout << "FD_WRITE failed" << endl;
                return 0;
            }

            // 이전 처리가 모두 끝날 때만(recvbytes == 0) recv를 받음
            // 이유는 recv가 동기 처리이므로 무한루프에 빠질 수 있음
            if (sessions[index].recvbytes == 0) {
                int recvlen = recv(
                    sessions[index].sock,
                    sessions[index].buf,
                    sizeof(sessions[index].buf),
                    0
                );
                if (recvlen == SOCKET_ERROR) {
                    if (WSAGetLastError() != WSAEWOULDBLOCK) {
                        cout << "recv() error" << endl;
                        return 0;
                    }
                }
                else {
                    sessions[index].recvbytes = recvlen;
                }
            }

            // 처리해야 할 내용이 있으면 우선 처리
            if (sessions[index].recvbytes > sessions[index].sendbytes) {
                int sendlen = send(
                    sessions[index].sock,
                    sessions[index].buf + sessions[index].sendbytes,
                    sessions[index].recvbytes - sessions[index].sendbytes,
                    0
                );
                if (sendlen == SOCKET_ERROR) {
                    if (WSAGetLastError() != WSAEWOULDBLOCK) {
                        cout << "send() error" << endl;
                        return 0;
                    }
                }
                else {
                    sessions[index].sendbytes += sendlen;

                    // 모든 처리가 끝나면 두 값을 초기화
                    if (sessions[index].recvbytes == sessions[index].sendbytes) {
                        sessions[index].recvbytes = 0;
                        sessions[index].sendbytes = 0;
                    }
                }
            }
        }

        // 해당 소켓의 Close 이벤트가 발생했을 경우
        if (networkEvents.lNetworkEvents & FD_CLOSE) {
            if (
                networkEvents.iErrorCode[FD_CLOSE_BIT] != 0
                && networkEvents.iErrorCode[FD_CLOSE_BIT] != WSAECONNABORTED // 클라이언트가 강제종료
            ) {
                cout << "FD_CLOSE error"
                    << networkEvents.iErrorCode[FD_CLOSE_BIT] << endl;
                return 1;
            }

            cout << "Client Disconnected" << endl;
            closesocket(sessions[index].sock);
            WSACloseEvent(wsaEvents[index]);
            wsaEvents.erase(wsaEvents.begin() + index);
            sessions.erase(sessions.begin() + index);
        }
    }

    closesocket(servsock);

    WSACleanup();
    return 0;
} // WSAEventSelect방식은 select와 동일하게 논블록 + 비동기 방식으로 동작하지만,
// select보다 더 세부적이고 효율적이다