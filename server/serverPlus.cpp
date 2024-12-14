#include <iostream>
#include <vector>
#include <thread>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <algorithm>

#define PORT 8080
#define BUFFER_SIZE 1024

std::mutex clients_mutex;
std::vector<int> clients;
std::unordered_map<int, std::string> client_names;

void broadcast(int sender_socket, const std::string &message) {
    for (auto &client : clients) {
        if (client != sender_socket) {
            if (send(client, message.c_str(), message.size(), 0) == -1) {
                std::cerr << "Failed to send message to client." << std::endl;
            }
        }
    }
}

void handle_client(int client_socket) {
    char buffer[BUFFER_SIZE];
    std::string client_name;
    bool first_message = true;

    while (true) {
        ssize_t bytes_received = recv(client_socket, buffer, BUFFER_SIZE, 0);
        std::cout<<"recv"<<std::endl;
        if (bytes_received <= 0) break;

        buffer[bytes_received] = '\0';
        std::string message(buffer);

        if (first_message) {
            client_names[client_socket] = message;
            client_name = message;
            std::cout << "Nickname: " << client_name << std::endl<< client_name << " has joined the chat."<<std::endl;
            for (auto &client : clients) {
                if (client != client_socket) {

                    send(client, (client_name + " has joined the chat.\n").c_str(), BUFFER_SIZE, 0);
                }
            }
            first_message = false;
        } else {
                std::cout << client_name << ": " << message << std::endl;
                 broadcast(client_socket, client_name + ": " + message);
                      }
    }
        for (auto &client : clients) {
                if (client != client_socket) {
                    send(client, (client_name + " has left the chat.\n").c_str(), BUFFER_SIZE, 0);
                }
            }
    std::cout << client_name << " has left the chat." << std::endl;
    client_names.erase(client_socket);
    clients.erase(std::remove(clients.begin(), clients.end(), client_socket), clients.end());
    close(client_socket);
}

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    socklen_t addrlen = sizeof(address);

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    std::cout << "Server listening on port " << PORT << std::endl;

    while (true) {
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, &addrlen)) < 0) {
            perror("accept");
            continue;
        }

        clients.push_back(new_socket);
        std::thread(handle_client, new_socket).detach();
    }
    return 0;
}
