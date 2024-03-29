#include "lib.h"

#define DEFAULT_BUFLEN 1024
#define MAX_EVENTS 8
// 이벤트 리스트의 최대 이벤트 개수 == 최대 쓰레드 개수(권장: CPU 코어 개수)

struct Session {
    int sock = INVALID_SOCKET;
    char buf[DEFAULT_BUFLEN];
    int recvbytes = 0;
    int sendbytes = 0;
};

// 쓰레드 방식에서는 자유로운 동시성을 위해 전역 변수로 선언
vector<Session*> sessions;
epoll_event epEvents[MAX_EVENTS];
mutex mtx; // 동시성 제어를 위한 뮤텍스

// 쓰레드에서 동작할 함수
void WorkerThread(int i);

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

    // vector<Session*> sessions; // 전역 변수로 변경
    sessions.reserve(100);

    sessions.push_back(new Session{servsock});

    int epollfd = epoll_create1(0);
    if (epollfd == SOCKET_ERROR) {
        cout << "epoll_create1() error" << endl;
        return 1;
    }

    // epoll_event epEvents[MAX_EVENTS]; // 전역 변수로 변경
    epoll_event epEvent;

    epEvent.events = EPOLLIN | EPOLLET; // ET 모드로 설정
    epEvent.data.fd = servsock;
    if (epoll_ctl(epollfd, EPOLL_CTL_ADD, servsock, &epEvent) == SOCKET_ERROR) {
        cout << "epoll_ctl() error" << endl;
        return 1;
    }

    while (true) {
        // 여기서 epfds의 최대치는 MAX_EVENTS == 최대 CPU 코어 개수
        int epfds = epoll_wait(epollfd, epEvents, MAX_EVENTS, INFINITE);
        if (epfds == SOCKET_ERROR) {
            cout << "epoll_wait() error" << endl;
            break;
        }

        // 쓰레드를 복수로 생성하기 위한 리스트
        // 쓰레드 풀링을 사용하지 않고, 단순히 쓰레드를 생성하여 사용함
        // 쓰레드 풀링을 사용하려면 작업 큐를 만들어서 쓰레드들이 작업을 가져가도록 해야 함(like IOCP)
        // 이번 예제에서는 간단하게 쓰레드를 생성 방식을 사용
        vector<thread> threads;
        threads.reserve(epfds); // 최대 이벤트 개수만큼 미리 예약

        // 살아남은 소켓에 대한 이벤트 처리
        for (int i = 0; i < epfds; i++) {
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

                epEvent.events = EPOLLIN | EPOLLOUT | EPOLLET; // ET 모드로 설정
                epEvent.data.ptr = newSession;
                if (epoll_ctl(epollfd, EPOLL_CTL_ADD, clisock, &epEvent) == SOCKET_ERROR) {
                    cout << "epoll_ctl() error" << endl;
                    return 1;
                }

                { // 뮤텍스 블록: 범위 기반 락 사용, 세션의 C,U,D를 위한 락
                    lock_guard<mutex> lock(mtx);
                    sessions.push_back(newSession); // 세션의 Create
                } // 뮤텍스의 락이 풀리면서 자동으로 unlock하는 시점

                cout << "Client Connected" << endl;

                continue;
            }

            // 클라이언트 소켓 처리를 위한 쓰레드를 복수로 생성
            // 쓰레드 풀링을 사용하지 않고, 단순히 쓰레드를 생성하여 비효율적임
            threads.push_back(thread(WorkerThread, i));
        }

        // 쓰레드가 모두 종료될 때까지 대기
        for (thread& t : threads) {
            t.join();
        }
    }

    close(servsock);
    close(epollfd);
    return 0;
}

void WorkerThread(int i) {
    Session* session = (Session*)epEvents[i].data.ptr;

    if (epEvents[i].events & EPOLLIN) {
        while (true) {
            int recvlen = recv(session->sock, session->buf, DEFAULT_BUFLEN, 0);
            if (recvlen == SOCKET_ERROR && errno == EAGAIN) {
                break;
            }
            else if (recvlen > 0) {
                { // 값 수정이므로 뮤텍스 블록 사용
                    lock_guard<mutex> lock(mtx);
                    session->recvbytes += recvlen; // 세션의 Update
                }
                if (session->recvbytes == DEFAULT_BUFLEN) {
                    break;
                }
            }
            else {
                cout << "Client Disconnected" << endl;

                close(session->sock);
                { // 값 삭제이므로 뮤텍스 블록 사용
                    lock_guard<mutex> lock(mtx);
                    sessions.erase( // 세션의 Delete
                        remove(sessions.begin(), sessions.end(), session),
                        sessions.end()
                    );
                }
                delete session;
                return;
            }
        }
    }

    if (epEvents[i].events & EPOLLOUT) {
        while (true) {
            int sendlen = send(session->sock, session->buf, session->recvbytes, 0);
            if (sendlen == SOCKET_ERROR) {
                if (errno == EAGAIN) {
                    break;
                }

                cout << "send() error" << endl;

                close(session->sock);
                { // 값 삭제이므로 뮤텍스 블록 사용
                    lock_guard<mutex> lock(mtx);
                    sessions.erase( // 세션의 Delete
                        remove(sessions.begin(), sessions.end(), session),
                        sessions.end()
                    );
                }
                delete session;
                return;
            }

            { // 값 수정이므로 뮤텍스 블록 사용
                lock_guard<mutex> lock(mtx);
                session->sendbytes += sendlen; // 세션의 Update

                if (session->sendbytes == session->recvbytes) {
                    session->sendbytes = 0; // 세션의 Update
                    session->recvbytes = 0; // 세션의 Update

                    break;
                }
            }
        }
    }
}
