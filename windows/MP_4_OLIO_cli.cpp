#include "lib.h"

int main() {
    WSAData wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    SOCKET clisock = WSASocket(
        AF_INET, SOCK_STREAM, 0,
        NULL, 0, WSA_FLAG_OVERLAPPED
    );
    if (clisock == INVALID_SOCKET) {
        cout << "socket() error" << endl;
        return 1;
    }

    char ip[16] = "";
    cout << "Input server IP: "; cin >> ip;

    SOCKADDR_IN servaddr;
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr(ip);
    servaddr.sin_port = htons(12345);

    if (connect(clisock, (SOCKADDR*)&servaddr, sizeof(servaddr)) == SOCKET_ERROR) {
        cout << "connect() error" << endl;
        return 1;
    }

    char sendbuf[1024] = {}, recvbuf[1024] = {};

    WSABUF sendWsabuf;
    sendWsabuf.buf = sendbuf;
    sendWsabuf.len = sizeof(sendbuf);
    WSABUF recvWsabuf;
    recvWsabuf.buf = recvbuf;
    recvWsabuf.len = sizeof(recvbuf);

    WSAOVERLAPPED sendOverlapped = {}, recvOverlapped = {};

    DWORD flags = 0, Bytes = 0;

while (true) {
    sendbuf[1024] = {}; recvbuf[1024] = {};

    cout << "Input: "; cin >> sendbuf;
    sendWsabuf.len = strlen(sendbuf);

    sendOverlapped = {}; recvOverlapped = {};
    flags = 0; Bytes = 0;

    sendWsabuf.buf = sendbuf;
    sendWsabuf.len = sizeof(sendbuf);
    recvWsabuf.buf = recvbuf;
    recvWsabuf.len = sizeof(recvbuf);

    // 데이터 전송
    if (WSASend(clisock, &sendWsabuf, 1, &Bytes, 0, &sendOverlapped, NULL) == SOCKET_ERROR) {
        if (WSAGetLastError() != WSA_IO_PENDING) {
            cout << "WSASend() error" << endl;
            break;
        }
    }

    // WSASend() 작업 완료 대기
    if (!WSAGetOverlappedResult(clisock, &sendOverlapped, &Bytes, TRUE, &flags)) {
        cout << "WSAGetOverlappedResult() error after send: " << WSAGetLastError() << endl;
        break;
    }

    // 데이터 수신
    if (WSARecv(clisock, &recvWsabuf, 1, &Bytes, &flags, &recvOverlapped, NULL) == SOCKET_ERROR) {
        if (WSAGetLastError() != WSA_IO_PENDING) {
            cout << "WSARecv() error" << endl;
            break;
        }
    }

    // WSARecv() 작업 완료 대기
    if (!WSAGetOverlappedResult(clisock, &recvOverlapped, &Bytes, TRUE, &flags)) {
        cout << "WSAGetOverlappedResult() error after recv: " << WSAGetLastError() << endl;
        continue;
    }

    cout << "Echo: " << recvbuf << endl;
}

    closesocket(clisock);
    WSACleanup();
    return 0;
}