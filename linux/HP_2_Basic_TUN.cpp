#include "lib.h"
#include <iomanip>
#include <linux/if.h>
#include <linux/if_tun.h>

#define ERROR -1

int CreateTun(string name, string ipv4);

// Basic Linux TUN Device
// 바이트 프로그래밍은 복잡한 관계로 이를 쓰지않고, 커스텀 L4패킷으로 프로젝트를 수행함
int main() {
    int tun_fd = CreateTun("custun0", "12.34.56.78");
    if (tun_fd == ERROR) {
        cout << "CreateTun() error" << endl;
        return 0;
    }

    while (true) {
        char buf[2048];
        int nread = read(tun_fd, buf, sizeof(buf));
        if (nread < 0) {
            cout << "read() error" << endl;
            return 0;
        }

        cout << "Read " << nread << " bytes from tun" << endl;
        for (int i = 0; i < nread; i++) {
            if (i != 0 && i % 16 == 0) {
                cout << endl;
            }

            cout
                << setw(2) << setfill('0')
                << hex << uppercase
                << (buf[i] & 0xFF) << " ";
        }
        cout << endl;
    }
}


int CreateTun(string name, string ipv4) {
    if (system("mkdir -p /dev/net") == ERROR) {
        cout << "system() error" << endl;
        return ERROR;
    }
    if (system("mknod /dev/net/custun c 10 200") == ERROR) {
        cout << "system() error" << endl;
        return ERROR;
    }
    if (system("chmod 666 /dev/net/custun") == ERROR) {
        cout << "system() error" << endl;
        return ERROR;
    }
    if (system("modprobe tun") == ERROR) {
        cout << "system() error" << endl;
        return ERROR;
    }

    int tun_fd = open("/dev/net/custun", O_RDWR);
    if (tun_fd < 0) {
        cout << "open() error" << endl;
        return ERROR;
    }

    struct ifreq ifr;
    memset(&ifr, 0, sizeof(ifr));

    ifr.ifr_flags = IFF_TUN | IFF_NO_PI | IFF_MULTI_QUEUE;
    strncpy(ifr.ifr_name, name.c_str(), IFNAMSIZ);

    if (ioctl(tun_fd, TUNSETIFF, (void *)&ifr) == ERROR) {
        cout << "ioctl() error" << endl;
        return ERROR;
    }

    string cmd = "sudo ip addr add " + ipv4 + "/24 dev " + name;
    if (system(cmd.c_str()) == ERROR) {
        cout << "system() error" << endl;
        return ERROR;
    }
    cmd = "sudo ip link set dev " + name + " up";
    if (system(cmd.c_str()) == ERROR) {
        cout << "system() error" << endl;
        return ERROR;
    }

    return tun_fd;
}
