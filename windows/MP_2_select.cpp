#include "lib.h"

#define DEFAULT_BUFLEN 1024

// Session struct 또는 Channel struct
struct Session {
    SOCKET sock = INVALID_SOCKET;
    char buf[DEFAULT_BUFLEN];
    int recvbytes = 0; // 세션으로 부터 받은 바이트 수
    int sendbytes = 0; // 세션으로 부터 보낸 바이트 수
};

int main() {
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    SOCKET servsock = socket(AF_INET, SOCK_STREAM, 0);
    if (servsock == INVALID_SOCKET) {
        cout << "socket() error" << endl;
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

    // Session list, 관찰할 세션들의 목록
    vector<Session> sessions;
    sessions.reserve(100);

    // file descriptor : 특정 파일(소켓)에 접근하기 위한 추상화 된 인덱스
    // fd_set: 파일(소켓) 디스크립터를 관리하기 위한 구조체 (비트마스크로 구현되어 있음)
    // FD_ZERO: fd_set을 초기화
    // FD_SET: fd_set에 파일(소켓)을 추가
    // FD_CLR: fd_set에 파일(소켓)을 제거
    // FD_ISSET: fd_set에 특정 파일(소켓)이 들어있는지 확인

    // fd 세트는 입 출력 두 종류로 나눔
    fd_set readfds; // 최대 64개 까지 추가 가능
    fd_set writefds; // 최대 64개 까지 추가 가능

    while (true) {
        // fd_set 초기화
        FD_ZERO(&readfds);
        FD_ZERO(&writefds);

        // 서버 소켓을 readfds에 추가
        // Select 후보군에 서버 소켓을 추가 (Listen 감지용)
        FD_SET(servsock, &readfds);

        // 세션 목록을 순회하며 소켓을 readfds와 writefds에 추가 (recv, send 감지용)
        for (Session& session : sessions) {
            
            // 해당 소켓이 이미 클라이언트가 연결되어 있을 경우 자동 후보군에 추가
            if (session.sock != INVALID_SOCKET) {
                FD_SET(session.sock, &readfds); // recv 후보군에 추가
                FD_SET(session.sock, &writefds); // send 후보군에 추가
            }
        }

        // select 함수 호출
        // select 함수는 readfds, writefds, exceptfds를 관찰하여 변화가 생길 때까지 대기
        // 변화가 생기면 변화가 생긴 파일(소켓)들을 제외한 나머지 파일(소켓)들은 fd에서 제거

        // 변화가 생긴 파일(소켓)들만 다시 각 readfds, writefds, exceptfds에 유지됨
        int ret = select(0, &readfds, &writefds, NULL, NULL);
        if (ret == SOCKET_ERROR) {
            cout << "select() error" << endl;
            return 1;
        }

        // Select 이후 FD_ISSET이 True는 변화가 생긴 파일(소켓)들만 잡아냈다는 뜻이다
        if (FD_ISSET(servsock, &readfds)) {

            SOCKADDR_IN cliaddr;
            int addrlen = sizeof(cliaddr);
            
            SOCKET clisock = accept(servsock, (SOCKADDR*)&cliaddr, &addrlen);
            if (clisock == INVALID_SOCKET) {
                cout << "accept() error" << endl;
                return 1;
            }

            // clisock도 논블록으로 설정
            u_long on = 1;
            if (ioctlsocket(clisock, FIONBIO, &on) == SOCKET_ERROR) {
                cout << "ioctlsocket() error" << endl;
                return 1;
            }

            sessions.push_back(Session{clisock});
            // 변화가 있을 때만 Accept를 하고 세션에 추가 (굉장히 효율적임)

            cout << "Client connected" << endl;
        }

        // 세션 목록을 순회
        for (Session& session : sessions) {
            // 해당 세션의 readfds가 변경되었는지 확인
            if (FD_ISSET(session.sock, &readfds)) {
                session.recvbytes = recv(session.sock, session.buf, DEFAULT_BUFLEN, 0);
                if (session.recvbytes == SOCKET_ERROR) {
                    cout << "recv() error" << endl;
                    return 1;
                }
                // 파일이 변경되었는데 받은 바이트가 0이면 클라이언트가 연결을 끊은 것
                else if (session.recvbytes == 0) {
                    cout << "Client disconnected" << endl;
                    closesocket(session.sock);
                    session.sock = INVALID_SOCKET;

                    continue;
                }

            }

            // 해당 세션의 writefds가 변경되었는지 확인
            if (FD_ISSET(session.sock, &writefds)) {
                int sendlen = send(session.sock, session.buf, session.recvbytes, 0);
                if (sendlen == SOCKET_ERROR) {
                    cout << "send() error" << endl;
                    return 1;
                }

                session.sendbytes += sendlen;

                // 혹시나 값이 너무 커지는 것을 방지하기 위해
                // 에코서버에서 순간 입력과 출력이 같아지면 초기화
                if (session.sendbytes == session.recvbytes) {
                    session.sendbytes = 0;
                    session.recvbytes = 0;
                }
            }
        }
    }

    closesocket(servsock);

    WSACleanup();
    return 0;
} // 불필요한 루프를 돌지 않아도 모두 논블로킹으로 처리 가능하고 비동기적으로 처리 가능
// 하지만, 한 루프에서 최대 64개의 파일(소켓)만 관찰 가능
