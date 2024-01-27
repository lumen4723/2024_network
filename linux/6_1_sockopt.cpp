#include "lib.h"

using namespace std;

int main() {
    int servsock = socket(AF_INET, SOCK_STREAM, 0);
    if (servsock == INVALID_SOCKET) {
        cout << "socket() error" << endl;
        return 0;
    }

    // 소켓 옵션 설정
    
    // SOL_SOCKET: 소켓 옵션 레벨, IPPROTO_TCP: TCP 옵션 레벨, IPPROTO_UDP: UDP 옵션 레벨
    // SO_KEEPALIVE: keepalive 패킷을 주기적으로 보내서 상대방이 살아있는지 확인(TCP 전용)
    // optval: 옵션의 값 (true: 사용, false: 사용하지 않음)
    bool optval = true;
    // setsockopt(세팅할 소켓, 프로토콜 레벨, 옵션 이름, 옵션 값, 옵션 값의 크기)
    setsockopt(
        servsock,
        SOL_SOCKET,
        SO_KEEPALIVE,
        (const char*)&optval,
        sizeof(optval)
    );

    optval = true;
    // SO_REUSEADDR: TIME_WAIT 상태의 소켓을 재사용할 수 있게 해줌
    // bind already in use 에러를 방지할 수 있음
    setsockopt(
        servsock,
        SOL_SOCKET,
        SO_REUSEADDR,
        (const char*)&optval,
        sizeof(optval)
    );

    optval = true;
    // IPPROTO_TCP: IP 프로토콜 관련 옵션
    // TCP_NODELAY: Nagle 알고리즘 작동 여부, 0이면 작동, 1이면 작동하지 않음
    // Nagle 알고리즘: 데이터를 모아서 한 번에 전송하는 알고리즘 (전송/응답이 순차적임)
    // 결과적으로 효율 좋게, 순차적으로 전송할 수 있음
    // 장점: 네트워크 트래픽을 줄일 수 있음
    // 단점: 지연시간이 길어질 수 있음
    setsockopt(
        servsock,
        IPPROTO_TCP,
        TCP_NODELAY,
        (const char*)&optval,
        sizeof(optval)
    );

    // SO_RCVTIMEO: 수신 타임아웃
    // 수신 타임아웃: 수신할 데이터가 없을 때 대기하는 시간
    // 수신 타임아웃이 지나면 recv()는 -1을 반환하고 WSAGetLastError()는 WSAETIMEDOUT을 반환
    // timeval: 타임아웃 시간
    timeval timeout;
    timeout.tv_sec = 5; // 초 단위
    timeout.tv_usec = 0; // 마이크로초 단위
    setsockopt(
        servsock,
        SOL_SOCKET,
        SO_RCVTIMEO,
        (const char*)&timeout,
        sizeof(timeout)
    );

    // SO_SNDTIMEO: 송신 타임아웃
    // 송신 타임아웃: 송신할 데이터가 없을 때 대기하는 시간
    // 송신 타임아웃이 지나면 send()는 -1을 반환하고 WSAGetLastError()는 WSAETIMEDOUT을 반환
    // timeval: 타임아웃 시간
    timeout.tv_sec = 5; // 초 단위
    timeout.tv_usec = 0; // 마이크로초 단위
    setsockopt(
        servsock,
        SOL_SOCKET,
        SO_SNDTIMEO,
        (const char*)&timeout,
        sizeof(timeout)
    );



    // 소켓에 링거 옵션을 설정

    // SO_LINGER: 소켓을 닫을 때 남아있는 데이터를 어떻게 처리할지 결정
    linger ling;
    ling.l_onoff = 1; // 1이면 linger 사용, 0이면 사용하지 않음
    ling.l_linger = 0; // 지연시간
    setsockopt(
        servsock,
        SOL_SOCKET,
        SO_LINGER,
        (const char*)&ling,
        sizeof(ling)
    );
    // SO_DONTLINGER: 소켓을 닫을 때 남아있는 데이터를 모두 버림



    // 소켓에 버퍼 옵션을 설정

    // getsockopt를 통해 버퍼의 크기를 확인할 수 있음
    // SO_SNDBUF: 송신 버퍼의 크기
    int sndbuf;
    socklen_t optionlen = sizeof(sndbuf);
    // getsockopt(확인할 소켓, 프로토콜 레벨, 옵션 이름, 옵션 값, 옵션 값의 크기)
    getsockopt(
        servsock,
        SOL_SOCKET,
        SO_SNDBUF,
        (char*)&sndbuf,
        &optionlen
    );
    cout << "sndbuf: " << sndbuf << endl;

    // setsockopt를 통해 버퍼의 크기를 변경할 수 있음
    sndbuf = 1024;
    // setsockopt(세팅할 소켓, 프로토콜 레벨, 옵션 이름, 옵션 값, 옵션 값의 크기)
    setsockopt(
        servsock,
        SOL_SOCKET,
        SO_SNDBUF,
        (const char*)&sndbuf,
        sizeof(sndbuf)
    );

    // SO_RCVBUF: 수신 버퍼의 크기
    int rcvbuf;
    optionlen = sizeof(rcvbuf);
    getsockopt(
        servsock,
        SOL_SOCKET,
        SO_RCVBUF,
        (char*)&rcvbuf,
        &optionlen
    );
    cout << "rcvbuf: " << rcvbuf << endl;

    rcvbuf = 1024;
    setsockopt(
        servsock,
        SOL_SOCKET,
        SO_RCVBUF,
        (const char*)&rcvbuf,
        sizeof(rcvbuf)
    );



    // Half-close: 데이터의 송수신을 중단하고,
    // 송신만 가능하거나 수신만 가능하게 만드는 것

    // SHUT_WR: send()를 더 이상 사용하지 않겠다는 의미
    // SHUT_RD: recv()를 더 이상 사용하지 않겠다는 의미
    // SHUT_RDWR: 둘 다 사용하지 않겠다는 의미
    shutdown(servsock, SHUT_WR);

    // 단순히 소켓을 닫는 것과의 차이점
    // 소켓을 닫으면 EOF를 의미하는 FIN 패킷을 상대방에게 전송하면서 소켓을 닫기 때문에,
    // 온전히 받았다는 의미나 최종 데이터를 더 이상 받지 못하는 상황이 생길 수 있음
    // closesocket(servsock);

    // 반면, shutdown()은 소켓을 닫지 않고,
    // 송신만 불가능하거나 수신만 불가능하게 만들기 때문에,
    // 상대방에게 EOF는 전송 가능 하지만 FIN 패킷을 전송하지 않음
    // shutdown(servsock, SHUT_WR);

    return 0;
}