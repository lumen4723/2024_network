#include "lib.h"

char *getIPonDNS(string domain);

void getTargetIP (char *buf, string &ip, string &port);

int main() {
    WSAData wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    string server;
    cout << "Input server IP or domain: "; cin >> server;

    char *ip = getIPonDNS(server);



    SOCKET clisock = socket(AF_INET, SOCK_DGRAM, 0);
    if (clisock == INVALID_SOCKET) {
        cout << "socket() error" << endl;
        return 0;
    }

    SOCKADDR_IN servAddr;
    memset(&servAddr, 0, sizeof(servAddr));
    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.s_addr = inet_addr(ip);
    servAddr.sin_port = htons(3478);

    if (connect(clisock, (SOCKADDR*)&servAddr, sizeof(servAddr)) == SOCKET_ERROR) {
        cout << "connect() error" << endl;
        closesocket(clisock);
        return 0;
    }



    SOCKADDR_IN localAddr;
    int addrSize = sizeof(localAddr);
    if (getsockname(clisock, (SOCKADDR*)&localAddr, &addrSize) == SOCKET_ERROR) {
        cout << "getsockname() error" << endl;
        closesocket(clisock);
        return 0;
    }

    char buf[1024];
    sprintf(buf, "%s:%d", inet_ntoa(localAddr.sin_addr), ntohs(localAddr.sin_port));



    int sendlen = sendto(clisock, buf, strlen(buf) + 1, 0, (SOCKADDR *)&servAddr, sizeof(servAddr));
    if (sendlen == SOCKET_ERROR) {
        cout << "sendto() error" << endl;
        return 0;
    }

    int recvlen = recvfrom(clisock, buf, sizeof(buf), 0, NULL, NULL);
    if (recvlen == SOCKET_ERROR) {
        cout << "recvfrom() error" << endl;
        return 0;
    }

    cout << "Echo: " << buf << endl;



    string destIP, destPort;
    getTargetIP(buf, destIP, destPort);

    cout << "Destination IP: " << destIP << ":" << destPort << endl;

    memset(&servAddr, 0, sizeof(servAddr));
    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.s_addr = inet_addr(destIP.c_str());
    servAddr.sin_port = htons(atoi(destPort.c_str()));

    if (connect(clisock, (sockaddr*)&servAddr, sizeof(servAddr)) == SOCKET_ERROR) {
        cout << "connect() error" << endl;
        closesocket(clisock);
        return 0;
    }



    while (true) {
        string message = "send from windows";

        int sendlen = sendto(clisock, message.c_str(), message.length() + 1, 0, (sockaddr*)&servAddr, sizeof(servAddr));
        if (sendlen == SOCKET_ERROR) {
            cout << "sendto() error" << endl;
            return 0;
        }

        int recvlen = recvfrom(clisock, buf, sizeof(buf), 0, NULL, NULL);
        if (recvlen == SOCKET_ERROR) {
            cout << "recvfrom() error" << endl;
            return 0;
        }

        cout << "Echo: " << buf << endl;

        this_thread::sleep_for(100ms);
    }



    closesocket(clisock);
    WSACleanup();
    return 0;
}

char *getIPonDNS(string domain) {
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    struct hostent *host;
    char *ip = new char[16];
    host = gethostbyname(domain.c_str());
    if (!host) {
        cout << "gethostbyname() error" << endl;
        return nullptr;
    }
    strcpy(ip, inet_ntoa(*(struct in_addr *)host->h_addr_list[0]));
    WSACleanup();
    return ip;
}

void getTargetIP(char *buf, string &ip, string &port) {
    istringstream iss(buf);
    vector<string> tokens;
    string token;
    while (iss >> token) {
        tokens.push_back(token);
    }

    // [0]: 자신의 pub IP:Port, [1]: 상대의 pub IP:Port,
    // [2]: 자신의 priv IP:Port, [3]: 상대의 priv IP:Port

    if ( // 자신의 pub IP와 상대의 pub IP가 같다면 사설 IP:Port를 사용
        tokens[0].substr(0, tokens[0].find(':'))
        == tokens[1].substr(0, tokens[1].find(':'))
    ) {
        ip = tokens[3].substr(0, tokens[3].find(':'));
        port = tokens[3].substr(tokens[3].find(':') + 1);
        return;
    }

    ip = tokens[1].substr(0, tokens[1].find(':'));
    port = tokens[1].substr(tokens[1].find(':') + 1);
}