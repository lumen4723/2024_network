#include "lib.h"

int main () {
    WSAData wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    SOCKET clisock = socket(AF_INET, SOCK_STREAM, 0);
    if (clisock == INVALID_SOCKET) {
        cout << "socket() error" << endl;
        return 0;
    }

    string server;
    cout << "Input server IP or domain: "; cin >> server;

    // DNS to IP
    // gethostbyname() 함수를 이용하여 도메인 이름을 IP 주소로 변환
    struct hostent *host = gethostbyname(server.c_str());
    if (host == NULL) {
        cout << "gethostbyname() error" << endl;
        return 0;
    }

    char ip[16] = "";
    inet_ntop(AF_INET, host->h_addr, ip, sizeof(ip));

    // 도메인 이름을 IP 주소로 변환한 결과를 출력하는 예제
    cout << "IP: " << ip << endl;

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