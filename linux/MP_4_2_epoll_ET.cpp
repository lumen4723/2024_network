#include "lib.h"

#define DEFAULT_BUFLEN 1024
#define MAX_EVENTS 100 // epoll의 한 번에 처리할 이벤트 수

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

    vector<Session*> sessions;
    sessions.reserve(100);

    sessions.push_back(new Session{servsock});

    // 동일하게 epoll_create1() 함수를 사용하여 epollfd를 생성
    int epollfd = epoll_create1(0);
    if (epollfd == SOCKET_ERROR) {
        cout << "epoll_create1() error" << endl;
        return 1;
    }

    // epoll_event 리스트와 epoll_event 구조체를 사용
    epoll_event epEvents[MAX_EVENTS], epEvent;

    // 서버 소켓은 EPOLLIN 이벤트를 감지하도록 설정하고
    // EPOLLET 플래그를 추가하여 ET(Edge Trigger, 엣지 트리거) 모드로 설정
    epEvent.events = EPOLLIN | EPOLLET;
    epEvent.data.fd = servsock;
    // 서버 소켓 정보가 들어간 epoll_event 구조체를 epollfd에 추가
    if (epoll_ctl(epollfd, EPOLL_CTL_ADD, servsock, &epEvent) == SOCKET_ERROR) {
        cout << "epoll_ctl() error" << endl;
        return 1;
    }

    while (true) {
        // 동일하게 epoll_wait() 함수를 사용하여 이벤트를 감지
        int epfds = epoll_wait(epollfd, epEvents, MAX_EVENTS, INFINITE);
        if (epfds == SOCKET_ERROR) {
            cout << "epoll_wait() error" << endl;
            break;
        }

        // 살아남은 소켓 = 상태가 변경된 소켓이므로 이벤트들을 처리
        for (int i = 0; i < epfds; i++) {
            // 상태가 변경된 소켓이 서버 소켓이면 무조건 EPOLLIN이므로 accept() 호출
            if (epEvents[i].data.fd == servsock) {
                sockaddr_in cliaddr;
                socklen_t addrlen = sizeof(cliaddr);

                int clisock = accept(servsock, (sockaddr*)&cliaddr, &addrlen);
                if (clisock == INVALID_SOCKET) {
                    cout << "accept() error" << endl;
                    break;
                }

                int flags = fcntl(clisock, F_GETFL, 0);
                fcntl(clisock, F_SETFL, flags | O_NONBLOCK);

                Session* newSession = new Session{clisock};

                // 클라이언트 소켓은 EPOLLIN | EPOLLOUT 이벤트를 감지하도록 설정
                // EPOLLET 플래그를 추가하여 ET(Edge Trigger, 엣지 트리거) 모드로 설정
                epEvent.events = EPOLLIN | EPOLLOUT | EPOLLET;
                epEvent.data.ptr = newSession;
                // 클라이언트 소켓 정보가 들어간 epoll_event 구조체를 epollfd에 추가
                if (epoll_ctl(epollfd, EPOLL_CTL_ADD, clisock, &epEvent) == SOCKET_ERROR) {
                    cout << "epoll_ctl() error" << endl;
                    return 1;
                }

                sessions.push_back(newSession);

                cout << "Client Connected" << endl;

                continue;
            }

            // 동일하게 포인터를 사용해서 효율적으로 처리
            Session* session = (Session*)epEvents[i].data.ptr;
            bool isClosed = false; // 소켓이 닫히면 두 번 break를 하기 위한 플래그

            if (epEvents[i].events & EPOLLIN) {
                while (true) { // ET 모드에서는 반복문을 사용하여 모든 데이터를 읽어야 함
                    int recvlen = recv(session->sock, session->buf, DEFAULT_BUFLEN, 0);
                    if (recvlen == SOCKET_ERROR && errno == EAGAIN) {
                        // ET 모드에서는 EAGAIN 에러가 발생하면 모든 데이터를 읽은 것
                        // EAGAIN: 리소스가 사용 가능(=온전하게 읽음)할 때까지 대기하라는 의미
                        break;
                    }
                    else if (recvlen > 0) {
                        session->recvbytes += recvlen;
                        if (session->recvbytes == DEFAULT_BUFLEN) {
                            break;
                        }
                    }
                    else {
                        cout << "Client Disconnected" << endl;
                        close(session->sock);
                        // 동일하게 erase-remove idiom을 사용하여 sessions에서 session을 제거
                        sessions.erase(
                            remove(sessions.begin(), sessions.end(), session),
                            sessions.end()
                        );
                        delete session;

                        // 소켓이 닫혔으므로 두 번 break를 하기 위한 플래그 설정
                        isClosed = true;

                        break;
                    }
                }

                if (isClosed) { continue; }
            }

            if (epEvents[i].events & EPOLLOUT) {
                while (true) {
                    int sendlen = send(session->sock, session->buf, session->recvbytes, 0);
                    if (sendlen == SOCKET_ERROR) {
                        if (errno == EAGAIN) {
                            // ET 모드에서는 EAGAIN 에러가 발생하면 모든 데이터를 보낸 것
                            break;
                        }

                        cout << "send() error" << endl;
                        close(session->sock);
                        // 동일하게 erase-remove idiom을 사용하여 sessions에서 session을 제거
                        sessions.erase(
                            remove(sessions.begin(), sessions.end(), session),
                            sessions.end()
                        );
                        delete session;

                        break;
                    }

                    session->sendbytes += sendlen;

                    if (session->sendbytes == session->recvbytes) {
                        session->sendbytes = 0;
                        session->recvbytes = 0;

                        break;
                    }
                }
            }
        }
    }

    close(servsock);
    close(epollfd);
    return 0;
} // epoll ET(Edge Trigger, 엣지 트리거) 모드는 이벤트가 완료된 시점에 단 한 번만 이벤트를 감지하므로
// 이벤트가 완료될 때까지 계속해서 이벤트를 감지하도록 무한 루프를 사용해야 함
// 따라서 while (true) 루프를 사용하여 epoll_wait() 함수의 반환값을 사용

// epoll ET방식은 Overlapped I/O Event 처럼 이벤트가 완료된 시점에 단 한 번만 이벤트를 감지하므로
// epoll LT(Level Trigger, 레벨 트리거) 방식보다 더 효율적임

// 하지만, epoll ET방식은 Overlapped I/O Event보다 64개 이상의 소켓을 동시처리 가능하고,
// Overlapped I/O Completion Routine에 비해 Alertable wait비용이 없어서 더 효율적임