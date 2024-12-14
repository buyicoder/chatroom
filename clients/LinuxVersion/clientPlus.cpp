#include <iostream>
#include <string>
#include <thread>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#define PORT 8080
#define BUFFER_SIZE 1024

void receive_message(int sock) {
    char buffer[BUFFER_SIZE] = {0};
    while (true) {
        read(sock, buffer, BUFFER_SIZE);
        std::cout << buffer;
    }
}

int main() {
    int sock = 0;
    struct sockaddr_in serv_addr;
    char buffer[BUFFER_SIZE] = {0};

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        std::cout << "\n Socket creation error \n";
        getchar();
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    if (inet_pton(AF_INET, "8.218.40.119", &serv_addr.sin_addr) <= 0) {
        std::cout << "\nInvalid address / Address not supported \n";
        getchar();
        return -1;
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        std::cout << "\nConnection Failed \n";
        return -1;
    }

    std::cout << "Enter nickname: ";
    std::string nickname;
    std::getline(std::cin, nickname);
    send(sock, nickname.c_str(), nickname.size(), 0);

    std::thread receiver(receive_message, sock);

    while (true) {
        std::cout << "Enter message: ";
        std::string message;
        std::getline(std::cin, message);
        send(sock, message.c_str(), message.size(), 0);
    }

    return 0;
}

                                    