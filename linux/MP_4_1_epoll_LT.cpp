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

    // 포인터를 사용한 이유는 epoll_wait() 함수에서 효율적으로 처리하기 위함
    sessions.push_back(new Session{servsock});

    // 기존의 epoll_create()는 인자에 0이외의 값만 사용 가능
    // int epollfd = epoll_create(NOT ZERO);

    // epoll_create1()은 인자에 EPOLL_CLOEXEC을 사용하여
    // 프로세스 종료 시 epollfd를 자동으로 닫아주는 플래그를 설정
    // int epollfd = epoll_create1(EPOLL_CLOEXEC);
    int epollfd = epoll_create1(0);
    if (epollfd == SOCKET_ERROR) {
        cout << "epoll_create1() error" << endl;
        return 1;
    }

    // epoll_event 리스트와 epoll_event 구조체를 사용
    epoll_event epEvents[MAX_EVENTS], epEvent;

    // 서버 소켓은 EPOLLIN 이벤트를 감지하도록 설정
    epEvent.events = EPOLLIN;
    epEvent.data.fd = servsock;
    // 서버 소켓 정보가 들어간 epoll_event 구조체를 epollfd에 추가
    if (epoll_ctl(epollfd, EPOLL_CTL_ADD, servsock, &epEvent) == SOCKET_ERROR) {
        cout << "epoll_ctl() error" << endl;
        return 1;
    }

    while (true) {
        // epoll_wait() 함수를 사용하여 모든 이벤트를 감지하고 변경된 소켓만 남김
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

                // 포인터를 사용한 이유는 epoll_wait() 함수에서 효율적으로 처리하기 위함
                Session* newSession = new Session{clisock};

                // 클라이언트 소켓은 EPOLLIN과 EPOLLOUT 이벤트를 감지하도록 설정
                epEvent.events = EPOLLIN | EPOLLOUT;
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

            // 포인터를 사용해서 불 필요한 이중 반복문 대신 이벤트 주소를 직접 참조
            Session* session = (Session*)epEvents[i].data.ptr;

            // 상태가 변경된 소켓이 클라이언트 소켓이면
            // EPOLLIN 또는 EPOLLOUT 이벤트를 감지하므로 recv() 또는 send() 호출
            if (epEvents[i].events & EPOLLIN) {
                session->recvbytes = recv(session->sock, session->buf, DEFAULT_BUFLEN, 0);
                if (session->recvbytes == 0 || session->recvbytes == SOCKET_ERROR) {
                    cout << "Client Disconnected" << endl;
                    close(session->sock);
                    // erase-remove idiom을 사용하여 sessions에서 session을 제거
                    // erase-remove idiom: vector에서 특정 값을 제거할 때 사용하는 국룰 방식
                    sessions.erase(
                        remove(sessions.begin(), sessions.end(), session),
                        sessions.end()
                    );
                    delete session;

                    continue;
                }
            }

            if (epEvents[i].events & EPOLLOUT) {
                int sendlen = send(session->sock, session->buf, session->recvbytes, 0);
                if (sendlen == SOCKET_ERROR) {
                    cout << "send() error" << endl;
                    close(session->sock);
                    // erase-remove idiom을 사용하여 sessions에서 session을 제거
                    sessions.erase(
                        remove(sessions.begin(), sessions.end(), session),
                        sessions.end()
                    );
                    delete session;

                    continue;
                }

                session->sendbytes += sendlen;

                if (session->sendbytes == session->recvbytes) {
                    session->sendbytes = 0;
                    session->recvbytes = 0;
                }
            }
        }
    }

    close(servsock);
    close(epollfd);
    return 0;
} // epoll LT(Level Trigger, 레벨 트리거) 모드는 이벤트가 발생할 때까지 계속해서 이벤트를 감지하므로 무한 루프가 필요 없음
// 따라서 while (true) 루프를 사용하지 않고 for (epfds) 루프를 사용하여 epoll_wait() 함수의 반환값을 사용
// 고전적인 poll()과 유사하나 epoll은 이벤트가 발생한 소켓만 살아남기 때문에 조금 더 효율적임
// 하지만, epoll LT보다 더 효율적인 방식이 있음
