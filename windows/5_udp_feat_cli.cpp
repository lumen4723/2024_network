#include "lib.h"

int main() {
    WSAData wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    SOCKET clisock = socket(AF_INET, SOCK_DGRAM, 0);
    if (clisock == INVALID_SOCKET) {
        cout << "socket() error" << endl;
        return 0;
    }

    char ip[16];
    cout << "Input server IP: "; cin >> ip;

    SOCKADDR_IN servAddr;
    memset(&servAddr, 0, sizeof(servAddr));
    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.s_addr = inet_addr(ip);
    servAddr.sin_port = htons(12345);

    char buf[100] = "";
    
    while (true) {
        cout << "Input: "; cin >> buf;

        for(int i = 0; i < 10; i++) {
            int sendlen = sendto(clisock, buf, sizeof(buf), 0, (SOCKADDR*)&servAddr, sizeof(servAddr));
            if (sendlen == SOCKET_ERROR) {
                buf[sendlen] = '\0';
            }
        }
    }

    closesocket(clisock);

    WSACleanup();
    return 0;
}