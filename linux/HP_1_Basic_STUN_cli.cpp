#include "lib.h"

char *getIPonDNS(string domain);

void getTargetIP (char *buf, string &ip, string &port, bool isLocalNet);

int main() {
    string server;
    cout << "Input server IP or domain: "; cin >> server;

    char *ip = new char[16];
    ip = getIPonDNS(server);



    int clisock = socket(AF_INET, SOCK_DGRAM, 0);
    if (clisock == INVALID_SOCKET) {
        cout << "socket() error" << endl;
        return 0;
    }

    sockaddr_in servAddr;
    memset(&servAddr, 0, sizeof(servAddr));
    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.s_addr = inet_addr(ip);
    servAddr.sin_port = htons(3478);

    if (connect(clisock, (sockaddr*)&servAddr, sizeof(servAddr)) == SOCKET_ERROR) {
        cout << "connect() error" << endl;
        close(clisock);
        return 0;
    }



    sockaddr_in localAddr;
    uint addrSize = sizeof(localAddr);
    if (getsockname(clisock, (sockaddr*)&localAddr, &addrSize) == SOCKET_ERROR) {
        cout << "getsockname() error" << endl;
        close(clisock);
        return 0;
    }

    char buf[1024];
    sprintf(buf, "%s:%d", inet_ntoa(localAddr.sin_addr), ntohs(localAddr.sin_port));



    int sendlen = sendto(clisock, buf, strlen(buf) + 1, 0, (sockaddr*)&servAddr, sizeof(servAddr));
    if (sendlen == SOCKET_ERROR) {
        cout << "sendto() error" << endl;
        return 0;
    }

    int recvlen = recvfrom(clisock, buf, sizeof(buf), 0, NULL, NULL);
    if (recvlen == SOCKET_ERROR) {
        cout << "recvfrom() error" << endl;
        return 0;
    }

    cout << "STUN: " << buf << endl;



    bool isLocalNet = false; // 로컬 넷(같은 NAT)에서 홀펀칭을 하려면 true로 변경
    string destIP, destPort;
    getTargetIP(buf, destIP, destPort, isLocalNet);

    cout << "Destination IP: " << destIP << ":" << destPort << endl;

    memset(&servAddr, 0, sizeof(servAddr));
    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.s_addr = inet_addr(destIP.c_str());
    servAddr.sin_port = htons(atoi(destPort.c_str()));

    if (connect(clisock, (sockaddr*)&servAddr, sizeof(servAddr)) == SOCKET_ERROR) {
        cout << "connect() error" << endl;
        close(clisock);
        return 0;
    }



    while (true) {
        string message = "send from linux";

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


    return 0;
}

char *getIPonDNS(string domain) {
    struct addrinfo hints, *res;
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    int status = getaddrinfo(domain.c_str(), "http", &hints, &res);
    if (status != 0) {
        cout << "getaddrinfo() error" << endl;
        return nullptr;
    }

    char *ip = new char[16];
    inet_ntop(res->ai_family, &((sockaddr_in*)res->ai_addr)->sin_addr, ip, 16);

    return ip;
}

void getTargetIP (char *buf, string &ip, string &port, bool isLocalNet) {
    istringstream iss(buf);
    vector<string> tokens;
    string token;
    while (iss >> token) {
        tokens.push_back(token);
    }
    
    // [0]: 자신의 pub IP:Port, [1]: 상대의 pub IP:Port,
    // [2]: 자신의 priv IP:Port, [3]: 상대의 priv IP:Port
    
    if (isLocalNet) { // 내부망(같은 NAT)일 경우
        ip = tokens[3].substr(0, tokens[3].find(':'));
        port = tokens[3].substr(tokens[3].find(':') + 1);
        return;
    }

    ip = tokens[1].substr(0, tokens[1].find(':'));
    port = tokens[1].substr(tokens[1].find(':') + 1);
}