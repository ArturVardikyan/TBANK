#include "Server.hpp"
#include "Bank.hpp"        // Класс Bank, с которым работает сервер

#include <sys/socket.h>      // Для socket, bind, listen, accept
#include <netinet/in.h>      // Для sockaddr_in, htons
#include <arpa/inet.h>       // Для inet_ntoa
#include <unistd.h>          // Для close()
#include <pthread.h>         // Для POSIX threads
#include <cstring>           // Для memset()
#include <iostream>          // Для std::cout, std::cerr
#include <sstream>           // Для std::istringstream
#include <string>            // Для std::string
#include <exception>         // Для std::exception

// Структура аргументов для потока обработки клиента
struct ThreadArgs {
    int client_socket;   // Сокет для общения с клиентом
    Bank* bank;          // Указатель на объект Bank
};

// Отправка строки с '\n' клиенту
static void sendLine(int sock, const std::string& line) {
    std::string msg = line + "\n";
    send(sock, msg.c_str(), msg.size(), 0);
}

// Функция, выполняемая в потоке для каждого клиента
static void* handleClientThread(void* arg) {
    ThreadArgs* ta = static_cast<ThreadArgs*>(arg);
    int sock = ta->client_socket;
    Bank* bank = ta->bank;
    delete ta;  // удаляем аргументы сразу, чтобы освободить память

    char buffer[1024];
    std::string line;

    while (true) {
        ssize_t n = recv(sock, buffer, sizeof(buffer) - 1, 0);
        if (n <= 0) break;  // клиент закрыл соединение или ошибка
        buffer[n] = '\0';
        line.assign(buffer);
        if (!line.empty() && (line.back() == '\n' || line.back() == '\r')) {
            line.pop_back();
        }

        try {
            if (line == "shutdown") {
                sendLine(sock, "Server shutting down...");
                close(sock);
                std::exit(0);  // немедленный выход из процесса
            }

            std::istringstream iss(line);
            std::string cmd;
            iss >> cmd;

            if (cmd == "transfer") {
                int from, to;
                int32_t amt;
                if (!(iss >> from >> to >> amt)) {
                    sendLine(sock, "Usage: transfer <from> <to> <amount>");
                } else {
                    bank->transferFunds(from, to, amt);
                    sendLine(sock, "OK: transferred " + std::to_string(amt));
                }
            }
            else if (cmd == "freeze") {
                int id;
                if (!(iss >> id)) {
                    sendLine(sock, "Usage: freeze <id>");
                } else {
                    bank->freezeAccount(id);
                    sendLine(sock, "OK: account " + std::to_string(id) + " frozen");
                }
            }
            else if (cmd == "unfreeze") {
                int id;
                if (!(iss >> id)) {
                    sendLine(sock, "Usage: unfreeze <id>");
                } else {
                    bank->unfreezeAccount(id);
                    sendLine(sock, "OK: account " + std::to_string(id) + " unfrozen");
                }
            }
            else if (cmd == "mass_update") {
                int32_t amt;
                if (!(iss >> amt)) {
                    sendLine(sock, "Usage: mass_update <amount>");
                } else {
                    bank->massUpdate(amt);
                    sendLine(sock, "OK: all balances updated by " + std::to_string(amt));
                }
            }
            else if (cmd == "set_limits") {
                int id;
                int32_t mn, mx;
                if (!(iss >> id >> mn >> mx)) {
                    sendLine(sock, "Usage: set_limits <id> <min> <max>");
                } else {
                    bank->setLimits(id, mn, mx);
                    sendLine(sock, "OK: limits set for account " + std::to_string(id));
                }
            }
            else {
                sendLine(sock, "Unknown command: " + cmd);
            }
        }
        catch (const std::exception& ex) {
            sendLine(sock, std::string("Error: ") + ex.what());
        }
    }

    close(sock);
    return nullptr;
}

// Реализация функции startServer(), объявленной в Server.hpp
int startServer(int port, Bank& bank) {
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("socket");
        return 1;
    }

    sockaddr_in addr;
    std::memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port);

    if (bind(server_fd, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) < 0) {
        perror("bind");
        close(server_fd);
        return 1;
    }

    if (listen(server_fd, SOMAXCONN) < 0) {
        perror("listen");
        close(server_fd);
        return 1;
    }

    std::cout << "Server listening on port " << port << "...\n";

    while (true) {
        sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);
        int client_fd = accept(server_fd, reinterpret_cast<sockaddr*>(&client_addr), &client_len);
        if (client_fd < 0) {
            perror("accept");
            continue;
        }
        std::cout << "New connection from " << inet_ntoa(client_addr.sin_addr)
                  << ":" << ntohs(client_addr.sin_port) << "\n";

        // Создаём и запускаем поток для клиента
        ThreadArgs* ta = new ThreadArgs{client_fd, &bank};
        pthread_t tid;
        if (pthread_create(&tid, nullptr, handleClientThread, ta) != 0) {
            perror("pthread_create");
            close(client_fd);
            delete ta;
            continue;
        }
        pthread_detach(tid);
    }

    close(server_fd);
    return 0;
}

// Точка входа для сервера TBANK
int main(int argc, char** argv) {
    size_t N = 100;            // Количество счетов (можно считывать из argv)
    int32_t max_balance = 100000;
    int port = DEFAULT_PORT;   // из Server.hpp

    // Выделяем массив счетов (обычный new[]). Для shared-memory версии используйте mmap/shm_open.
    Account* accounts = new Account[N];
    for (size_t i = 0; i < N; ++i) {
        accounts[i].account_id = static_cast<int>(i);
        accounts[i].balance = 0;
        accounts[i].min_balance = 0;
        accounts[i].max_balance = max_balance;
        accounts[i].frozen = false;
    }

    Bank bank(accounts, N);
    return startServer(port, bank);
}
