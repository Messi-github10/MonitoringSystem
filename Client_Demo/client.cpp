#include <iostream>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
using namespace std;

enum TaskType
{
    TASK_TYPE_LOGIN_SECTION_1 = 1,
    TASK_TYPE_LOGIN_SECTION_1_RESPONSE_OK,
    TASK_TYPE_LOGIN_SECTION_1_RESPONSE_ERROR,
    TASK_TYPE_LOGIN_SECTION_2,
    TASK_TYPE_LOGIN_SECTION_2_RESPONSE_OK,
    TASK_TYPE_LOGIN_SECTION_2_RESPONSE_ERROR,
    TASK_TYPE_REGISTER_SECTION_1,
    TASK_TYPE_REGISTER_SECTION_1_RESPONSE_OK,
    TASK_TYPE_REGISTER_SECTION_1_RESPONSE_ERROR,
    TASK_TYPE_REGISTER_SECTION_2,
    TASK_TYPE_REGISTER_SECTION_2_RESPONSE_OK,
    TASK_TYPE_REGISTER_SECTION_2_RESPONSE_ERROR
};

struct TLV
{
    int type;
    int length;
    char data[1024] = {0};
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

#if 1
    // 测试登录时密码验证功能
    while (1)
    {
        cout << ">> Input username: ";
        string line;
        cin >> line;
        int type = TASK_TYPE_LOGIN_SECTION_1;
        int length = line.length();

        // 测试：发送用户名
        uint32_t net_type = htonl(type);
        uint32_t net_length = htonl(length);
        send(client_fd, &net_type, 4, 0); // 发送网络字节序
        send(client_fd, &net_length, 4, 0);
        send(client_fd, line.c_str(), line.size(), 0);
        cout << "send [" << 8 + length << "] Bytes" << endl;

        // 测试：接收服务器返回的TLV数据
        uint32_t net_type1;
        uint32_t net_length1;
        TLV tlv;
        recv(client_fd, &net_type1, 4, 0);
        recv(client_fd, &net_length1, 4, 0);

        tlv.type = ntohl(net_type1);
        tlv.length = ntohl(net_length1);

        cout << "tlv.type: " << tlv.type << endl;

        recv(client_fd, tlv.data, tlv.length, 0);
        cout << ">> recv from server [" << tlv.data << "], length [" << tlv.length << "]" << endl;

        // 测试：发送密码
        cout << ">> Input password: ";
        string line2;
        cin >> line2;
        int type2 = TASK_TYPE_LOGIN_SECTION_2;
        int length2 = line2.length();

        uint32_t net_type2;
        uint32_t net_length2;

        net_type2 = htonl(type2);
        net_length2 = htonl(length2);

        send(client_fd, &net_type2, 4, 0);
        send(client_fd, &net_length2, 4, 0);
        send(client_fd, line2.c_str(), line2.size(), 0);
        cout << "send [" << 8 + length2 << "] Bytes" << endl;

        // 测试：接收服务器返回的TLV数据
        uint32_t net_type3;
        uint32_t net_length3;
        TLV tlv2 = {0};
        recv(client_fd, &net_type3, 4, 0);
        recv(client_fd, &net_length3, 4, 0);

        tlv2.type = ntohl(net_type3);
        tlv2.length = ntohl(net_length3);

        cout << "tlv2.type: " << tlv2.type << endl;

        if(tlv2.type == TASK_TYPE_LOGIN_SECTION_2_RESPONSE_OK){
            cout << "成功连接..." << endl;
        }
    }
#endif

#if 0
    // 测试注册时密码验证功能
    while (1)
    {
        cout << ">> Input username: ";
        string line;
        cin >> line;
        int type = TASK_TYPE_REGISTER_SECTION_1;
        int length = line.length();

        // 测试：发送用户名
        uint32_t net_type = htonl(type);
        uint32_t net_length = htonl(length);
        send(client_fd, &net_type, 4, 0); // 发送网络字节序
        send(client_fd, &net_length, 4, 0);
        send(client_fd, line.c_str(), line.size(), 0);
        cout << "send [" << 8 + length << "] Bytes" << endl;

        // 测试：接收服务器返回的TLV数据
        uint32_t net_type1;
        uint32_t net_length1;
        TLV tlv;
        recv(client_fd, &net_type1, 4, 0);
        recv(client_fd, &net_length1, 4, 0);

        tlv.type = ntohl(net_type1);
        tlv.length = ntohl(net_length1);

        cout << "tlv.type: " << tlv.type << endl;

        recv(client_fd, tlv.data, tlv.length, 0);
        cout << ">> recv from server [" << tlv.data << "], length [" << tlv.length << "]" << endl;

        // 测试：发送密码
        cout << ">> Input password: ";
        string line2;
        cin >> line2;
        int type2 = TASK_TYPE_REGISTER_SECTION_2;
        int length2 = line2.length();

        uint32_t net_type2;
        uint32_t net_length2;

        net_type2 = htonl(type2);
        net_length2 = htonl(length2);

        send(client_fd, &net_type2, 4, 0);
        send(client_fd, &net_length2, 4, 0);
        send(client_fd, line2.c_str(), line2.size(), 0);
        cout << "send [" << 8 + length2 << "] Bytes" << endl;

        // 测试：接收服务器返回的TLV数据
        uint32_t net_type3;
        uint32_t net_length3;
        TLV tlv2 = {0};
        recv(client_fd, &net_type3, 4, 0);
        recv(client_fd, &net_length3, 4, 0);

        tlv2.type = ntohl(net_type3);
        tlv2.length = ntohl(net_length3);

        cout << "tlv2.type: " << tlv2.type << endl;

        if (tlv2.type == TASK_TYPE_LOGIN_SECTION_2_RESPONSE_OK)
        {
            cout << "成功连接..." << endl;
        }
    }

#endif

    cout << "GoodByte..." << endl;
    close(client_fd);

    return 0;
}