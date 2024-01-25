#include "lib.h"

int main () {
    // 소켓 생성
    // Address Family: AF_INET(IPv4), AF_INET6(IPv6), AF_UNIX(UNIX)
    // Type: SOCK_STREAM(TCP), SOCK_DGRAM(UDP)
    // Protocol: 0(auto), IPPROTO_TCP, IPPROTO_UDP
    // socket(주소 체계, 소켓 타입, 프로토콜)
    int servsock = socket(AF_INET, SOCK_STREAM, 0);
    if (servsock == INVALID_SOCKET) {
        cerr << "socket() error" << endl;
        return 0;
    }

    // 주소 정보 구조체
    struct sockaddr_in servAddr;
    memset(&servAddr, 0, sizeof(servAddr));
    servAddr.sin_family = AF_INET; // IPv4
    servAddr.sin_addr.s_addr = htonl(INADDR_ANY); // 들어오는 모든 IP
    servAddr.sin_port = htons(12345); // port(htons == host to network short)

    // 서버 소켓 주소 정보 할당
    // bind(서버 소켓, 주소 정보 구조체, 주소 정보 구조체 크기)
    if (bind(servsock, (sockaddr*)&servAddr, sizeof(servAddr)) == SOCKET_ERROR) {
        cerr << "bind() error" << endl;
        return 0;
    }

    // 연결 요청 대기열 크기 설정
    // listen(서버 소켓, 대기열 크기)
    // SOMAXCONN: 가능한 최대 대기열 크기
    if (listen(servsock, SOMAXCONN) == SOCKET_ERROR) {
        cout << "listen() error" << endl;
        return 0;
    }

    // 클라이언트 소켓 생성
    // accept(서버 소켓, 클라 주소 정보 구조체, 클라 주소 정보 구조체 크기)
    // 클라 주소 정보가 NULL 이면, 클라 주소 정보를 알 필요가 없다는 의미
    int clisock = accept(servsock, NULL, NULL);
    if (clisock == INVALID_SOCKET) {
        cout << "accept() error" << endl;
        return 0;
    }

    // 데이터 수신
    char buf[1024];

    // recv(클라 소켓, 버퍼, 버퍼 크기, 플래그)
    // 플래그: 0(기본), MSG_PEEK(버퍼에서 읽지 않고 확인만), MSG_OOB(긴급 데이터)
    int recvlen = recv(clisock, buf, sizeof(buf), 0);
    if (recvlen == SOCKET_ERROR) {
        cout << "recv() error" << endl;
        return 0;
    }
    buf[recvlen] = '\0';

    // 수신 데이터 출력
    // echo는 수신 데이터를 그대로 송신
    cout << "recv: " << buf << endl;

    // 데이터 송신
    // send(클라 소켓, 버퍼, 버퍼 크기, 플래그)
    int sendlen = send(clisock, buf, recvlen, 0);
    if (sendlen == SOCKET_ERROR) {
        cout << "send() error" << endl;
        return 0;
    }

    // 소켓 종료
    close(clisock);

    close(servsock);
    return 0;
}