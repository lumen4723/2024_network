#include "lib.h"

int main () {
    int servsock = socket(AF_INET, SOCK_STREAM, 0);
    if (servsock == INVALID_SOCKET) {
        cout << "socket() error" << endl;
        return 0;
    }

    sockaddr_in servAddr;
    memset(&servAddr, 0, sizeof(servAddr));
    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servAddr.sin_port = htons(12345);

    if (bind(servsock, (sockaddr*)&servAddr, sizeof(servAddr)) == SOCKET_ERROR) {
        cout << "bind() error" << endl;
        return 0;
    }

    if (listen(servsock, SOMAXCONN) == SOCKET_ERROR) {
        cout << "listen() error" << endl;
        return 0;
    }

    while (true) {
        int clisock = accept(servsock, NULL, NULL);
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

        close(clisock);
        cout << "client disconnected" << endl;
    }

    close(servsock);
    return 0;
}