#include "lib.h"

int main () {
    int clisock = socket(AF_INET, SOCK_STREAM, 0);
    if (clisock == INVALID_SOCKET) {
        cout << "socket() error" << endl;
        return 0;
    }

    char ip[16];
    cout << "Input server IP: "; cin >> ip;

    struct sockaddr_in servAddr;
    memset(&servAddr, 0, sizeof(servAddr));
    servAddr.sin_family = AF_INET; // IPv4
    servAddr.sin_addr.s_addr = inet_addr(ip); // 접속할 서버 IP
    servAddr.sin_port = htons(12345); // port(htons == host to network short)

    // 서버에 연결 요청
    // connect(클라 소켓, 서버 주소 정보 구조체, 서버 주소 정보 구조체 크기)
    if (connect(clisock, (sockaddr*)&servAddr, sizeof(servAddr)) == SOCKET_ERROR) {
        cout << "connect() error" << endl;
        return 0;
    }

    // 데이터 송신
    char buf[1024];
    cout << "Input: "; cin >> buf;

    // send(클라 소켓, 버퍼, 버퍼 크기, 플래그)
    // 플래그: 0(기본), MSG_OOB(긴급 데이터)
    int sendlen = send(clisock, buf, strlen(buf) + 1, 0);
    if (sendlen == SOCKET_ERROR) {
        cout << "send() error" << endl;
        return 0;
    }

    // 데이터 수신
    // recv(클라 소켓, 버퍼, 버퍼 크기, 플래그)
    // 플래그: 0(기본), MSG_PEEK(버퍼에서 읽지 않고 확인만), MSG_OOB(긴급 데이터)
    int recvlen = recv(clisock, buf, sizeof(buf), 0);
    if (recvlen == SOCKET_ERROR) {
        cout << "recv() error" << endl;
        return 0;
    }
    buf[recvlen] = '\0';

    cout << "Echo: " << buf << endl;

    // 소켓 종료
    close(clisock);

    return 0;
}