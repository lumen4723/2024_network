#include "lib.h"

int main() {
    WSAData wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    // Blocking Socket
    // accept (접속한 클라이언트가 있으면)
    // -> connect (클라이언트가 접속에 성공하면)
    // -> send/sendto (클라이언트/서버가 데이터를 송신버퍼에 복사하면)
    // -> recv/recvfrom (클라이언트/서버가 데이터를 수신버퍼에서 복사해옴)

    // Non-blocking Socket
    SOCKET clisock = socket(AF_INET, SOCK_STREAM, 0);
    if (clisock == INVALID_SOCKET) {
        cout << "socket() error" << endl;
        return 0;
    }

    // 논블로킹 소켓으로 만들기
    u_long on = 1;
    if (ioctlsocket(clisock, FIONBIO, &on) == SOCKET_ERROR) {
        cout << "ioctlsocket() error" << endl;
        return 0;
    }

    char ip[16] = "";
    cout << "Input server IP: "; cin >> ip;

    SOCKADDR_IN servAddr;
    memset(&servAddr, 0, sizeof(servAddr));
    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.s_addr = inet_addr(ip);
    servAddr.sin_port = htons(12345);

    // 논블로킹 소켓은 connect()도 루프를 돌면서 될 때까지 계속 시도해야함
    while (true) {
        if (connect(clisock, (SOCKADDR*)&servAddr, sizeof(servAddr)) == SOCKET_ERROR) {
            // 논블로킹 소켓은 connect()에서 한번 더 체크해줘야함
            if (WSAGetLastError() == WSAEWOULDBLOCK) {
                continue; // 소켓에러가 논블로킹의 특성으로 인한 에러라면 무시
            }
            // 하지만 중복 connect() 요청은 막아야함
            else if (WSAGetLastError() == WSAEISCONN || WSAGetLastError() == WSAEALREADY) {
                break; // 이미 연결된 상태라면 무시
            }
            // 둘 다 아니면 에러
            else {
                cout << "connect() error" << endl;
                return 0;
            }
        }
    }
    
    char buf[1024] = "";

    while (true) {
        cout << "Input: "; cin >> buf;

        // 논블로킹 소켓은 send()도 루프를 돌면서 될 때까지 계속 시도해야함
        while (true) {
            if (send(clisock, buf, strlen(buf) + 1, 0) == SOCKET_ERROR) {
                // 논블로킹 소켓은 send()에서 한번 더 체크해줘야함
                if (WSAGetLastError() == WSAEWOULDBLOCK) {
                    continue;
                }
                else {
                    cout << "send() error" << endl;
                    return 0;
                }
            }
            else {
                break;
            }
        }

        int recvlen;
        // 논블로킹 소켓은 recv()도 루프를 돌면서 될 때까지 계속 시도해야함
        while (true) {
            recvlen = recv(clisock, buf, sizeof(buf), 0);
            if (recvlen == SOCKET_ERROR) {
                // 논블로킹 소켓은 recv()에서 한번 더 체크해줘야함
                if (WSAGetLastError() == WSAEWOULDBLOCK) {
                    continue;
                }
                else {
                    cout << "recv() error" << endl;
                    return 0;
                }
            }
            else {
                break;
            }
        }

        if (recvlen == 0) {
            break;
        }

        buf[recvlen] = '\0';

        cout << "Echo: " << buf << endl;
    }

    closesocket(clisock);

    WSACleanup();
    return 0;
} // 논블로킹 + 동기 방식은 상당히 많은 루프를 필요로 하기 때문에 비효율적이다