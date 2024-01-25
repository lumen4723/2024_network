#include "lib.h"

int main() {
    // UDP 소켓 생성
    int servsock = socket(AF_INET, SOCK_DGRAM, 0);
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

    while (true) {
        sockaddr_in cliAddr;
        uint cliAddrLen = sizeof(cliAddr);
        memset(&cliAddr, 0, sizeof(cliAddr));

        char buf[1024];

        // UDP는 recv() 대신 recvfrom()을 사용함
        // recvfrom(서버 소켓, 받을 버퍼, 버퍼 길이, 옵션, 클라 주소, 클라 주소 길이)
        int recvlen = recvfrom(servsock, buf, sizeof(buf), 0, (sockaddr*)&cliAddr, &cliAddrLen);
        if (recvlen == SOCKET_ERROR) {
            cout << "recvfrom() error" << endl;
            return 0;
        }
        buf[recvlen] = '\0';

        cout << "recv: " << buf << endl;

        // UDP는 send() 대신 sendto()를 사용함
        // sendto(클라 소켓, 보낼 데이터, 데이터 길이, 옵션, 클라 주소, 클라 주소 길이)
        int sendlen = sendto(servsock, buf, strlen(buf) + 1, 0, (sockaddr*)&cliAddr, sizeof(cliAddr));
        if (sendlen == SOCKET_ERROR) {
            cout << "sendto() error" << endl;
            return 0;
        }
    }

    // 소켓 종료
    close(servsock);
    return 0;
}