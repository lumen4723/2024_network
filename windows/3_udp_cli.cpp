#include "lib.h"

int main() {
    WSAData wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    // UDP 소켓 생성
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

    char buf[1024] = "";
    // UDP는 connect()를 사용하지 않음
    // 그래서, 비연결(Unconnected) 소켓이라고 함
    while (true) {
        cout << "Input: "; cin >> buf;

        // UDP는 send() 대신 sendto()를 사용함
        // sendto(클라 소켓, 보낼 데이터, 데이터 길이, 옵션, 서버 주소, 서버 주소 길이)
        int sendlen = sendto(clisock, buf, strlen(buf) + 1, 0, (SOCKADDR*)&servAddr, sizeof(servAddr));
        if (sendlen == SOCKET_ERROR) {
            cout << "sendto() error" << endl;
            return 0;
        }

        // UDP는 recv() 대신 recvfrom()을 사용함
        // recvfrom(클라 소켓, 받을 버퍼, 버퍼 길이, 옵션, 서버 주소, 서버 주소 길이)
        int recvlen = recvfrom(clisock, buf, sizeof(buf), 0, NULL, NULL);
        if (recvlen == SOCKET_ERROR) {
            cout << "recvfrom() error" << endl;
            return 0;
        }
        buf[recvlen] = '\0';

        cout << "Echo: " << buf << endl;
    }

    closesocket(clisock);

    WSACleanup();
    return 0;
}