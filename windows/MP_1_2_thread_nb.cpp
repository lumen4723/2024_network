#include "lib.h"

void handleClient(SOCKET clisock);

int main() {
    WSAData wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    SOCKET servsock = socket(AF_INET, SOCK_STREAM, 0);
    if (servsock == INVALID_SOCKET) {
        cout << "socket() error" << endl;
        return 0;
    }

    u_long on = 1;
    if (ioctlsocket(servsock, FIONBIO, &on) == SOCKET_ERROR) {
        cout << "ioctlsocket() error" << endl;
        return 0;
    }

    SOCKADDR_IN servaddr;
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(12345);

    if (bind(servsock, (SOCKADDR*)&servaddr, sizeof(servaddr)) == SOCKET_ERROR) {
        cout << "bind() error" << endl;
        return 0;
    }

    if (listen(servsock, SOMAXCONN) == SOCKET_ERROR) {
        cout << "listen() error" << endl;
        return 0;
    }

    // 클라이언트와 통신할 스레드들을 관리하기 위한 컨테이너
    // std::thread는 C++11에서 추가된 표준 쓰레드 라이브러리
    vector<thread> threads;

    // 핸들 함수를 제외하고 나머지는 크게 다르지 않다
    while (true) {
        SOCKET clisock = accept(servsock, NULL, NULL);
        if (clisock == INVALID_SOCKET) {
            if (WSAGetLastError() == WSAEWOULDBLOCK) {
                continue;
            }
            else {
                cout << "accept() error" << endl;
                return 0;
            }
        }

        cout << "Client Connected" << endl;

        threads.push_back(thread(handleClient, clisock));
    }
    // Accept()와 recv(), send()도 쓰레드로 만들어서 논블로킹이 가능하지만,
    // 그만큼 복잡한 locking과 쓰레드 풀 관리가 필요하다

    for (auto& thread : threads) {
        if (thread.joinable()) {
            thread.join();
        }
    }

    closesocket(servsock);

    WSACleanup();
    return 0;
}

void handleClient(SOCKET clisock) {
    // echo 서버의 기능을 수행하는 코드(non-blocking)
    char buf[1024] = "";

    while (true) {
        int recvlen;
        while (true) {
            recvlen = recv(clisock, buf, sizeof(buf), 0);
            if (recvlen == SOCKET_ERROR) {
                if (WSAGetLastError() == WSAEWOULDBLOCK) {
                    continue;
                }
                else {
                    cout << "recv() error" << endl;
                    closesocket(clisock);
                    return;
                }
            }
            else {
                break;
            }
        }

        if (recvlen == 0) {
            closesocket(clisock);
            cout << "Client Disconnected" << endl;
            return;
        }

        int sendlen;
        while (true) {
            sendlen = send(clisock, buf, strlen(buf) + 1, 0);
            if (sendlen == SOCKET_ERROR) {
                if (WSAGetLastError() == WSAEWOULDBLOCK) {
                    continue;
                }
                else {
                    cout << "send() error" << endl;
                    closesocket(clisock);
                    return;
                }
            }
            else {
                break;
            }
        }

        if (sendlen == 0) {
            closesocket(clisock);
            cout << "Client Disconnected" << endl;
            return;
        }
    }
}