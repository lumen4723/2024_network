#include "lib.h"

#define DEFAULT_BUFLEN 1024

struct Session {
    int sock = INVALID_SOCKET;
    char buf[DEFAULT_BUFLEN];
    int recvbytes = 0;
    int sendbytes = 0;
};

int main() {
    int servsock = socket(AF_INET, SOCK_STREAM, 0);
    if (servsock == INVALID_SOCKET) {
        cout << "socket() error" << endl;
        return 1;
    }

    int flags = fcntl(servsock, F_GETFL, 0);
    fcntl(servsock, F_SETFL, flags | O_NONBLOCK);

    sockaddr_in servaddr;
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(12345);

    if (bind(servsock, (sockaddr*)&servaddr, sizeof(servaddr)) == SOCKET_ERROR) {
        cout << "bind() error" << endl;
        return 1;
    }

    if (listen(servsock, SOMAXCONN) == SOCKET_ERROR) {
        cout << "listen() error" << endl;
        return 1;
    }

    vector<Session> sessions;
    sessions.reserve(100);

    sessions.push_back(Session{servsock});
    
    // pollfd 구조체 리스트를 사용하여 select() 함수를 대체
    vector<pollfd> pfds;
    pfds.reserve(100);
    
    // 서버 소켓을 pfds에 추가, POLLIN을 설정
    pfds.push_back({servsock, POLLIN, 0});

    while (true) {
        // select() 함수를 대체
        // poll은 pfds에 있는 소켓들을 관찰하여 이벤트가 발생하면 revents에 이벤트를 설정
        if (poll(pfds.data(), pfds.size(), INFINITE) == SOCKET_ERROR) {
            cout << "poll() error" << endl;
            break;
        }

        // pfds를 순회하여 revents가 설정된 소켓을 찾아서 처리
        for (int i = 0; i < pfds.size(); i++) {
            // 해당 소켓이 서버 소켓이고 revents가 POLLIN이면 accept() 호출 
            if (pfds[i].fd == servsock && pfds[i].revents & POLLIN) {
                sockaddr_in cliaddr;
                socklen_t addrlen = sizeof(cliaddr);

                int clisock = accept(servsock, (sockaddr*)&cliaddr, &addrlen);
                if (clisock == INVALID_SOCKET) {
                    cout << "accept() error" << endl;
                    continue;
                }

                int flags = fcntl(clisock, F_GETFL, 0);
                fcntl(clisock, F_SETFL, flags | O_NONBLOCK);

                sessions.push_back(Session{clisock});
                // 클라이언트 소켓을 pfds에 추가, POLLIN과 POLLOUT을 설정
                pfds.push_back({clisock, POLLIN | POLLOUT, 0});

                cout << "Client Connected" << endl;

                continue;
            }

            // 해당 소켓이 클라이언트 소켓이고 revents가 POLLIN이면 recv() 호출
            if (pfds[i].revents & POLLIN) {
                sessions[i].recvbytes = recv(sessions[i].sock, sessions[i].buf, DEFAULT_BUFLEN, 0);
                if (sessions[i].recvbytes == SOCKET_ERROR || sessions[i].recvbytes == 0) {
                    cout << "Client Disconnected" << endl;
                    close(sessions[i].sock);
                    sessions.erase(sessions.begin() + i);
                    pfds.erase(pfds.begin() + i--);

                    continue;
                }
            }
            
            // 해당 소켓이 클라이언트 소켓이고 revents가 POLLOUT이면 send() 호출
            if (pfds[i].revents & POLLOUT) {
                int sendlen = send(sessions[i].sock, sessions[i].buf, sessions[i].recvbytes, 0);
                if (sendlen == SOCKET_ERROR) {
                    cout << "send() error" << endl;
                    close(sessions[i].sock);
                    sessions.erase(sessions.begin() + i);
                    pfds.erase(pfds.begin() + i--);

                    continue;
                }

                sessions[i].sendbytes += sendlen;
                
                if (sessions[i].sendbytes == sessions[i].recvbytes) {
                    sessions[i].sendbytes = 0;
                    sessions[i].recvbytes = 0;
                }
            }
        }
    }

    close(servsock);
    return 0;
} // poll()은 윈도우의 WSAEventSelect()와 유사하게 동작한다
// 하지만, poll()은 select(), WSAEventSelect()와 달리 64개 이상의 소켓을 동시 관찰할 수 있다
// 추가로 poll()은 UNIX 계열(리눅스, 맥)에서만 사용 가능하다