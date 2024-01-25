#include "lib.h"

int main () {
    WSAData wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    SOCKET servsock = socket(AF_INET, SOCK_STREAM, 0);
    if (servsock == INVALID_SOCKET) {
        cout << "socket() error" << endl;
        return 0;
    }

    SOCKADDR_IN servAddr;
    memset(&servAddr, 0, sizeof(servAddr));
    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servAddr.sin_port = htons(12345);

    if (bind(servsock, (SOCKADDR*)&servAddr, sizeof(servAddr)) == SOCKET_ERROR) {
        cout << "bind() error" << endl;
        return 0;
    }

    if (listen(servsock, SOMAXCONN) == SOCKET_ERROR) {
        cout << "listen() error" << endl;
        return 0;
    }

    while (true) {
        SOCKET clisock = accept(servsock, NULL, NULL);
        if (clisock == INVALID_SOCKET) {
            cout << "accept() error" << endl;
            return 0;
        }

        cout << "client connected" << endl;
            
        char buf[1000];

        while (true) {
            this_thread::sleep_for(1s);

            int recvlen = recv(clisock, buf, sizeof(buf), 0);
            if (recvlen == SOCKET_ERROR) {
                cout << "recv() error" << endl;
                return 0;
            }
            else if (recvlen == 0) {
                break;
            }

            // null을 무시하고 출력
            cout.write(buf, recvlen);
            cout << endl;
        }

        closesocket(clisock);
        cout << "client disconnected" << endl;
    }

    closesocket(servsock);
    // 윈속 종료
    WSACleanup();
    return 0;
}