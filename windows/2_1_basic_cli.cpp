#include "lib.h"

int main () {
    WSAData wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    SOCKET clisock = socket(AF_INET, SOCK_STREAM, 0);
    if (clisock == INVALID_SOCKET) {
        cout << "socket() error" << endl;
        return 0;
    }

    char ip[16] = "";
    cout << "Input server IP: "; cin >> ip;

    SOCKADDR_IN servAddr;
    memset(&servAddr, 0, sizeof(servAddr));
    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.s_addr = inet_addr(ip);
    servAddr.sin_port = htons(12345);

    if (connect(clisock, (SOCKADDR*)&servAddr, sizeof(servAddr)) == SOCKET_ERROR) {
        cout << "connect() error" << endl;
        return 0;
    }

    char buf[1024] = "";
    
    while (true) {
        cout << "Input: "; cin >> buf;

        int sendlen = send(clisock, buf, strlen(buf) + 1, 0);
        if (sendlen == SOCKET_ERROR) {
            cout << "send() error" << endl;
            return 0;
        }

        int recvlen = recv(clisock, buf, sizeof(buf), 0);
        if (recvlen == SOCKET_ERROR) {
            cout << "recv() error" << endl;
            return 0;
        }
        buf[recvlen] = '\0';

        cout << "Echo: " << buf << endl;
    }

    // 소켓 종료
    closesocket(clisock);

    // 윈속 종료
    WSACleanup();
    return 0;
}