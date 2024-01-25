#include "lib.h"

int main() {
    // UDP 소켓 생성
    int clisock = socket(AF_INET, SOCK_DGRAM, 0);
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

    // UDP는 connect()를 사용하지 않음
    // 즉, 비연결(Unconnected) 소켓이라고 함
    while (true) {
        char buf[1024];
        cout << "Input: "; cin >> buf;

        // UDP는 send() 대신 sendto()를 사용함
        // sendto(클라 소켓, 보낼 데이터, 데이터 길이, 옵션, 서버 주소, 서버 주소 길이)
        int sendlen = sendto(clisock, buf, strlen(buf) + 1, 0, (sockaddr*)&servAddr, sizeof(servAddr));
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

    // 소켓 종료
    close(clisock);
    return 0;
}