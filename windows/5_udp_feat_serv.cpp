#include "lib.h"

int main() {
    WSAData wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    SOCKET servsock = socket(AF_INET, SOCK_DGRAM, 0);
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

    while (true) {
        SOCKADDR_IN cliAddr;
        int cliAddrLen = sizeof(cliAddr);
        memset(&cliAddr, 0, sizeof(cliAddr));

        this_thread::sleep_for(1s);

        char buf[1000];

        int recvlen = recvfrom(servsock, buf, sizeof(buf), 0, (SOCKADDR*)&cliAddr, &cliAddrLen);
        if (recvlen == SOCKET_ERROR) {
            cout << "recvfrom() error" << endl;
            return 0;
        }
        else if (recvlen == 0) {
            break;
        }

        // null을 무시하고 출력
        cout.write(buf, recvlen);
        cout << endl;
    }

    // 소켓 종료
    closesocket(servsock);

    // 윈속 종료
    WSACleanup();
    return 0;
}