#include "lib.h"

int main () {
    // 윈속 초기화
    // 관련 정보가 wsaData에 저장됨
    WSAData wsaData;
    // WSAStartup(버전, 버전 정보가 저장될 주소);
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    // 소켓 생성
    // Address Family: AF_INET(IPv4), AF_INET6(IPv6), AF_UNIX(UNIX)
    // Type: SOCK_STREAM(TCP), SOCK_DGRAM(UDP)
    // Protocol: 0(auto), IPPROTO_TCP, IPPROTO_UDP
    // socket(주소 체계, 소켓 타입, 프로토콜)
    SOCKET servsock = socket(AF_INET, SOCK_STREAM, 0);
    if (servsock == INVALID_SOCKET) {
        cout << "socket() error" << endl;
        return 0;
    }

    // 주소 정보 구조체
    SOCKADDR_IN servAddr;
    memset(&servAddr, 0, sizeof(servAddr));
    servAddr.sin_family = AF_INET; // IPv4
    servAddr.sin_addr.s_addr = htonl(INADDR_ANY); // 들어오는 모든 IP
    servAddr.sin_port = htons(12345); // port(htons == host to network short)
    // network 는 big endian, host(CPU) 는 little endian 이므로 변환해줘야 함

    // 서버 소켓 주소 정보 할당
    // bind(서버 소켓, 주소 정보 구조체, 주소 정보 구조체 크기)
    if (bind(servsock, (SOCKADDR*)&servAddr, sizeof(servAddr)) == SOCKET_ERROR) {
        cout << "bind() error" << endl;
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
    SOCKET clisock = accept(servsock, NULL, NULL);
    if (clisock == INVALID_SOCKET) {
        cout << "accept() error" << endl;
        return 0;
    }

    cout << "client connected" << endl;

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
    closesocket(clisock);
    cout << "client disconnected" << endl;

    closesocket(servsock);
    // 윈속 종료
    WSACleanup();
    return 0;
}