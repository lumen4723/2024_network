#include "lib.h"

void handleClient(SOCKET clisock) {
    // echo 서버의 기능을 수행하는 코드
    char buf[1024] = "";

    while (true) {
        int recvlen = recv(clisock, buf, sizeof(buf), 0);
        if (recvlen == SOCKET_ERROR) {
            cout << "recv() error" << endl;
            return;
        }
        else if (recvlen == 0) {
            break;
        }
        buf[recvlen] = '\0';

        cout << "recv: " << buf << endl;

        int sendlen = send(clisock, buf, recvlen, 0);
        if (sendlen == SOCKET_ERROR) {
            cout << "send() error" << endl;
            return;
        }
    }

    closesocket(clisock);
    cout << "Client Disconnected" << endl;
}

int main () {
    WSAData wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    SOCKET servsock = socket(AF_INET, SOCK_STREAM, 0);
    if (servsock == INVALID_SOCKET) {
        cout << "socket() error" << endl;
        return 0;
    }

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

    // 클라이언트와 통신할 스레드들을 관리하기 위한 컨테이너
    vector<thread> threads;

    // 무한 루프로 클라이언트 연결을 계속 기다림
    while (true) {
        SOCKET clisock = accept(servsock, NULL, NULL);
        if (clisock == INVALID_SOCKET) {
            cout << "accept() error" << endl;
            return 0;
        }

        cout << "Client Connected" << endl;

        // 클라이언트와 통신할 스레드 생성
        threads.push_back(thread(handleClient, clisock));
        // 쓰레드의 개수 == 클라이언트의 개수 이므로 코어보다 많은 쓰레드를 생성하기 쉬워짐
        // 이는 Context Switching 이 많아져서 성능이 저하될 수 있음
        // 그래서, 가급적 적은(코어의 개수와 비슷한) 쓰레드를 쓰는 것이 좋음
        // 또는, 쓰레드 풀(thread pool == 쓰레드를 미리 생성해두고 재사용하는 것)을 사용하는 것이 좋음
    }

    // 모든 스레드가 종료될 때까지 기다림
    for (auto& t : threads) {
        // joinable()은 해당 스레드가 종료되었는지 확인하는 함수
        // join()은 해당 스레드가 종료될 때까지 기다리는 함수 
        if (t.joinable()) { t.join(); }
    }

    closesocket(servsock);
    
    WSACleanup();
    return 0;
}