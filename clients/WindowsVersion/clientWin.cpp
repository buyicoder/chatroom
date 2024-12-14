#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <unistd.h>
#include <thread>
#pragma comment(lib, "Ws2_32.lib") // 链接到Winsock库

#define PORT 8080
#define BUFFER_SIZE 1024
void receive_message(int sock) {
    while(true){
    char buffer[1024] = {0};
    int valread = recv(sock, buffer, 1024, 0);
        if (valread > 0) {
            buffer[valread] = '\0'; // 确保字符串以空字符结尾
            std::cout << buffer << std::endl;
        } else if (valread == 0) {
            std::cout << "Connection closed" << std::endl;
            return;
        } else {
            std::cout << "Recv failed with error: " << WSAGetLastError() << std::endl;
            closesocket(sock);
            WSACleanup();
            return;
        }
    }
}

int main() {
    WSADATA wsaData;
    SOCKET sock = INVALID_SOCKET;
    struct sockaddr_in serv_addr;
    char buffer[1024] = {0};
    char nickname[256] = {0}; // 用于存储昵称
    int result;

    // 初始化Winsock
    result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0) {
        std::cout << "WSAStartup failed: " << result << std::endl;
        return 1;
    }

    // 创建套接字
    sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock == INVALID_SOCKET) {
        std::cout << "Socket creation failed with error: " << WSAGetLastError() << std::endl;
        WSACleanup();
        getchar();
        return 1;
    }

    // 设置服务器地址
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    inet_pton(AF_INET, "8.218.40.119", &serv_addr.sin_addr);

    // 连接到服务器
    result = connect(sock, (SOCKADDR*)&serv_addr, sizeof(serv_addr));
    if (result == SOCKET_ERROR) {
        std::cout << "Connection failed with error: " << WSAGetLastError() << std::endl;
        closesocket(sock);
        WSACleanup();
        getchar();
        return 1;
    }

    // 提示用户输入昵称
    std::cout << "Enter your nickname: ";
    std::cin.getline(nickname, 256);

    // 发送昵称到服务器
    send(sock, nickname, strlen(nickname) + 1, 0);

    // 提示用户输入消息
    std::cout << "\n Enter message to send (type 'exit' to quit): \n";
    std::thread receiver(receive_message, sock);
    while (true) {
        std::cin.getline(buffer, 1024);
        if (strcmp(buffer, "exit") == 0) {
            break;
        }
        // 发送消息到服务器
        std::cout<<"You: "<<buffer<<std::endl;
        result = send(sock, buffer, strlen(buffer) + 1, 0);
        if (result == SOCKET_ERROR) {
            std::cout << "Send failed with error: " << WSAGetLastError() << std::endl;
            closesocket(sock);
            WSACleanup();
            return 1;
        }
    }

    // 关闭套接字
    closesocket(sock);

    // 清理Winsock
    WSACleanup();
    return 0;
}