#include "lib.h"

int main () {
    int clisock = socket(AF_INET, SOCK_STREAM, 0);
    if (clisock == INVALID_SOCKET) {
        cout << "socket() error" << endl;
        return 0;
    }

    char ip[16];
    cout << "Input server IP: "; cin >> ip;

    sockaddr_in servAddr;
    memset(&servAddr, 0, sizeof(servAddr));
    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.s_addr = inet_addr(ip);
    servAddr.sin_port = htons(12345);

    if (connect(clisock, (sockaddr*)&servAddr, sizeof(servAddr)) == SOCKET_ERROR) {
        cout << "connect() error" << endl;
        return 0;
    }

    char buf[100] = "";
    
    while (true) {
        cout << "Input: "; cin >> buf;

        for(int i = 0; i < 10; i++) {
            int sendlen = send(clisock, buf, sizeof(buf), 0);
            if (sendlen == SOCKET_ERROR) {
                buf[sendlen] = '\0';
            }
        }
    }

    // 소켓 종료
    close(clisock);

    return 0;
}