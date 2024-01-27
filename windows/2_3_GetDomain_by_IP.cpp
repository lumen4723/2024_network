#include "lib.h"

int main() {
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    char ip[16] = "";
    cout << "Input IP: "; cin >> ip;

    sockaddr_in sockaddr;
    sockaddr.sin_family = AF_INET;
    sockaddr.sin_addr.s_addr = inet_addr(ip);

    // IP to DNS
    // NI_MAXHOST: getnameinfo() 함수가 반환하는 호스트 이름의 최대 길이
    char hostname[NI_MAXHOST];

    // getnameinfo(소켓 주소, 소켓 주소 길이, 호스트 이름 버퍼, 버퍼 길이, 서비스 이름 버퍼, 버퍼 길이, 플래그)
    int result = getnameinfo(
        (struct sockaddr*)&sockaddr,
        sizeof(sockaddr),
        hostname,
        NI_MAXHOST,
        NULL, 0, 0
    );
    if (result != 0) {
        cout << "getnameinfo() failed" << endl;
        return 1;
    }

    // IP 주소를 도메인 이름으로 변환한 결과를 출력하는 예제
    cout << "Hostname: " << hostname << endl;

    WSACleanup();
    return 0;
}