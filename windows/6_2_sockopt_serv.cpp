#include "lib.h"

using namespace std;

int main() {
    WSAData wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    SOCKET servsock = socket(AF_INET, SOCK_STREAM, 0);
    if (servsock == INVALID_SOCKET) {
        cout << "socket() error" << endl;
        return 0;
    }

    bool optval = true;
    // SO_KEEPALIVE: keepalive 패킷을 주기적으로 보내서 상대방이 살아있는지 확인(TCP 전용)
    setsockopt(
        servsock,
        SOL_SOCKET,
        SO_KEEPALIVE,
        (const char*)&optval,
        sizeof(optval)
    );

    // SO_REUSEADDR: TIME_WAIT 상태의 소켓을 재사용할 수 있게 해줌
    setsockopt(
        servsock,
        SOL_SOCKET,
        SO_REUSEADDR,
        (const char*)&optval,
        sizeof(optval)
    );

    // TCP_NODELAY: Nagle 알고리즘 작동 여부, 0이면 작동, 1이면 작동하지 않음
    setsockopt(
        servsock,
        IPPROTO_TCP,
        TCP_NODELAY,
        (const char*)&optval,
        sizeof(optval)
    );

    // // SO_RCVTIMEO: 수신 타임아웃
    // timeval recvtimeout;
    // recvtimeout.tv_sec = 5; recvtimeout.tv_usec = 0; // 정확히 5초
    // setsockopt(
    //     servsock,
    //     SOL_SOCKET,
    //     SO_RCVTIMEO,
    //     (const char*)&recvtimeout,
    //     sizeof(recvtimeout)
    // );

    // SO_SNDTIMEO: 송신 타임아웃
    timeval sendtimeout;
    sendtimeout.tv_sec = 5; sendtimeout.tv_usec = 0; // 정확히 5초
    setsockopt(
        servsock,
        SOL_SOCKET,
        SO_SNDTIMEO,
        (const char*)&sendtimeout,
        sizeof(sendtimeout)
    );

    // SO_LINGER: 소켓을 닫을 때 남아있는 데이터를 어떻게 처리할지 결정
    LINGER linger;
    linger.l_onoff = 1; // 1이면 linger 사용, 0이면 사용하지 않음
    linger.l_linger = 0; // 지연시간
    setsockopt(
        servsock,
        SOL_SOCKET,
        SO_LINGER,
        (const char*)&linger,
        sizeof(linger)
    );

    // SO_SNDBUF: 송신 버퍼의 크기
    int sndbuf = 4096;
    setsockopt(
        servsock,
        SOL_SOCKET,
        SO_SNDBUF,
        (const char*)&sndbuf,
        sizeof(sndbuf)
    );

    // SO_RCVBUF: 수신 버퍼의 크기
    int rcvbuf = 4096;
    setsockopt(
        servsock,
        SOL_SOCKET,
        SO_RCVBUF,
        (const char*)&rcvbuf,
        sizeof(rcvbuf)
    );

    SOCKADDR_IN servAddr;
    memset(&servAddr, 0, sizeof(servAddr));
    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servAddr.sin_port = htons(12345);

    if (bind(servsock, (SOCKADDR*)&servAddr, sizeof(servAddr)) == SOCKET_ERROR) {
        cout << "bind() error" << endl;
        return 0;
    }

    if (listen(servsock, SOMAXCONN) == SOCKET_ERROR) {
        cout << "listen() error" << endl;
        return 0;
    }

    while (true) {
        SOCKET clisock = accept(servsock, NULL, NULL);
        if (clisock == INVALID_SOCKET) {
            cout << "accept() error" << endl;
            return 0;
        }

        cout << "client connected" << endl;

        char buf[1024];

        while (true) {
            int recvlen = recv(clisock, buf, sizeof(buf), 0);
            if (recvlen == SOCKET_ERROR) {
                cout << "recv() error" << endl;
                return 0;
            }
            else if (recvlen == 0) {
                break;
            }
            buf[recvlen] = '\0';

            cout << "recv: " << buf << endl;

            int sendlen = send(clisock, buf, sizeof(buf), 0);
            if (sendlen == SOCKET_ERROR) {
                cout << "send() error" << endl;
                return 0;
            }
        }

        closesocket(clisock);
        cout << "client disconnected" << endl;
    }

    closesocket(servsock);
    
    WSACleanup();
    return 0;
}