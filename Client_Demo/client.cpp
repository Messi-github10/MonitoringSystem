#include <iostream>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
using namespace std;

struct TLV{
    int type;
    int length;
    char data[1024];
};

// 客户端（测试版）
int main()
{
    int client_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (client_fd < 0)
    {
        perror("socket");
        return EXIT_FAILURE;
    }

    struct sockaddr_in serveraddr;
    memset(&serveraddr, 0, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(8000);
    serveraddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    int ret = connect(client_fd, (const struct sockaddr *)&serveraddr, sizeof(serveraddr));
    if (ret < 0)
    {
        perror("connect");
        close(client_fd);
        return EXIT_FAILURE;
    }

    printf("Client has connected.\n");

    fd_set set;

    while (1)
    {
        cout << ">> Input Username: ";
        string line;
        cin >> line;
        int id = 1;
        int length = line.length();

        // 测试：发送用户名
        send(client_fd, &id, 4, 0);
        send(client_fd, &length, 4, 0);
        send(client_fd, line.c_str(), line.size(), 0);
        cout << "send [" << 8 + length << "] Bytes" << endl;
    
        // 测试：接收服务器返回的TLV数据
        TLV tlv = {0};
        recv(client_fd, &tlv.type, 4, 0);
        recv(client_fd, &tlv.length, 4, 0);
        recv(client_fd, tlv.data, tlv.length, 0);
        cout << ">> recv from server [" << tlv.data << "], length [" << tlv.length << "]" << endl;
    }

    close(client_fd);

    return 0;
}