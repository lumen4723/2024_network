#include "lib.h"

int main () {
    int clisock = socket(AF_INET, SOCK_STREAM, 0);
    if (clisock == INVALID_SOCKET) {
        cout << "socket() error" << endl;
        return 0;
    }

    string server;
    cout << "Input server IP or domain: "; cin >> server;

    // DNS to IP 
    struct addrinfo hints, *res;
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    // getaddrinfo(서버 주소, 포트, 힌트, 결과)
    int status = getaddrinfo(server.c_str(), "http", &hints, &res);
    if (status != 0) {
        cout << "getaddrinfo() error" << endl;
        return 0;
    }

    char ip[16] = "";
    
    // IP -> char[16]
    inet_ntop(res->ai_family, &((sockaddr_in*)res->ai_addr)->sin_addr, ip, sizeof ip);

    // 도메인 이름을 IP주소로 변환한 결과를 출력하는 예제
    cout << "IP: " << ip << endl;

    sockaddr_in servAddr;
    memset(&servAddr, 0, sizeof(servAddr));
    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.s_addr = inet_addr(ip);
    servAddr.sin_port = htons(12345);

    if (connect(clisock, (sockaddr*)&servAddr, sizeof(servAddr)) == SOCKET_ERROR) {
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
    close(clisock);

    return 0;
}