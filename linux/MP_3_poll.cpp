#include "lib.h"

#define DEFAULT_BUFLEN 1024

struct Session {
    int sock = INVALID_SOCKET;
    char buf[DEFAULT_BUFLEN];
    int recvbytes = 0;
    int sendbytes = 0;
    pollfd pfd;
};

int main() {
    int servsock = socket(AF_INET, SOCK_STREAM, 0);
    if (servsock == INVALID_SOCKET) {
        cout << "socket() error" << endl;
        return 1;
    }

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

    sessions.push_back(Session{sock: servsock, pfd: {servsock, POLLIN, 0}});

    while (true) {
        vector<pollfd> pfds; // 매번 pfds 배열을 새로 생성
        for (Session& session : sessions) {
            pfds.push_back(session.pfd); // sessions에서 pollfd 정보를 가져옴
        }

        if (poll(pfds.data(), pfds.size(), -1) == SOCKET_ERROR) {
            cout << "poll() error" << endl;
            break;
        }

        for (size_t i = 0; i < sessions.size(); i++) {
            if (pfds[i].revents & POLLIN) {
                if (sessions[i].sock == servsock) {
                    sockaddr_in cliaddr;
                    socklen_t addrlen = sizeof(cliaddr);

                    int clisock = accept(servsock, (sockaddr*)&cliaddr, &addrlen);
                    if (clisock == INVALID_SOCKET) {
                        cout << "accept() error" << endl;
                        continue;
                    }

                    sessions.push_back(Session{sock: clisock, pfd: {clisock, POLLIN, 0}});

                    cout << "Client Connected" << endl;

                    continue;
                }

                int recvlen = recv(sessions[i].sock, sessions[i].buf, DEFAULT_BUFLEN, 0);
                if (recvlen == SOCKET_ERROR || recvlen == 0) {
                    cout << "Client Disconnected" << endl;
                    close(sessions[i].sock);
                    sessions.erase(sessions.begin() + i);
                    i--; // 인덱스 조정

                    continue;
                }

                sessions[i].recvbytes = recvlen;
            }
        }

        for (size_t i = 0; i < sessions.size(); i++) {
            if (sessions[i].recvbytes == 0) {
                continue;
            }

            int sendlen = send(sessions[i].sock, sessions[i].buf, sessions[i].recvbytes, 0);
            if (sendlen == SOCKET_ERROR) {
                cout << "send() error" << endl;
                close(sessions[i].sock);
                sessions.erase(sessions.begin() + i);
                i--; // 인덱스 조정

                continue;
            }

            sessions[i].sendbytes += sendlen;
            
            if (sessions[i].sendbytes == sessions[i].recvbytes) {
                sessions[i].sendbytes = 0;
                sessions[i].recvbytes = 0;
            }
        }
    }

    close(servsock);
    return 0;
}