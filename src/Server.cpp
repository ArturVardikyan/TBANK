#include "Server.hpp"
#include "Bank.hpp"

#include <arpa/inet.h>  // inet_ntoa, htons
#include <csignal>      // signal, SIGINT, SIGTERM
#include <cstring>      // memset
#include <iostream>     // cout, cerr
#include <netinet/in.h> // sockaddr_in
#include <pthread.h>    // pthread_*
#include <sys/socket.h> // socket, bind, listen, accept
#include <unistd.h>     // close
#include <atomic>       // std::atomic
#include <sstream>      // std::istringstream
#include <utility>      // std::pair
#include <string>       // std::string
#include <iomanip>      // for std::setw
#include <mutex>
// Глобальные переменные для shutdown и статистики
static std::atomic<int> listen_fd{-1};
static std::atomic<bool> shutdownFlag(false);
static std::mutex cout_mutex;
static pthread_mutex_t stats_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t stats_cond = PTHREAD_COND_INITIALIZER;
static size_t request_count = 0;

// Обработчик сигналов для graceful shutdown
static void handleSignal(int /*sig*/)
{
    shutdownFlag.store(true);
    if (listen_fd >= 0)
    {
        close(listen_fd);
        listen_fd = -1;
    }
}

// Статистический поток: ждёт сигнала каждый запрос и печатает каждые 5
static void *statsThread(void * /*arg*/)
{
    pthread_mutex_lock(&stats_mutex);
    while (true)
    {
        pthread_cond_wait(&stats_cond, &stats_mutex);
        if (request_count % 5 == 0)
        {
            std::cout << "[Stats] Processed " << request_count << " requests\n";
        }
    }
    // никогда не доходит до pthread_mutex_unlock
    return nullptr;
}

// Отправка строки с '\n' клиенту
static void sendLine(int sock, const std::string &line)
{
    std::string msg = line + "\n";
    send(sock, msg.c_str(), msg.size(), 0);
}

// Поток-работник для клиента
static void *handleClient(void *arg)
{
    auto *args = static_cast<std::pair<int, Bank *> *>(arg);
    int sock = args->first;
    Bank *bank = args->second;
    delete args;

    char buffer[1024];
    std::string line;
    sendLine(sock, "Welcome To TBANK");
    sendLine(sock, "Available commands:");
    sendLine(sock, "  help                         - show help");
    sendLine(sock, "  shutdown                     - stop server");
    sendLine(sock, "  transfer <from> <to> <amt>   - transfer funds");
    sendLine(sock, "  freeze <id>                  - freeze account");
    sendLine(sock, "  unfreeze <id>                - unfreeze account");
    sendLine(sock, "  mass_update <amt>            - mass update balances");
    sendLine(sock, "  set_limits <id> <min> <max>  - set account limits");
    sendLine(sock, "  show_account_list            - showing min list");
    sendLine(sock, "  show_min <id>                - showing min balance for account <id>");
    sendLine(sock, "  show_max <id>                - showing max balance for account <id>");
    sendLine(sock, "  show_balance <id>            - showing balance for account <id>");

    while (true)
    {
        ssize_t n = recv(sock, buffer, sizeof(buffer) - 1, 0);
        if (n <= 0)
            break;
        buffer[n] = '\0';
        line.assign(buffer);
        if (!line.empty() && (line.back() == '\r' || line.back() == '\n'))
            line.pop_back();

        // Увеличиваем счётчик и сигналим статистике
        pthread_mutex_lock(&stats_mutex);
        ++request_count;
        pthread_cond_signal(&stats_cond);
        pthread_mutex_unlock(&stats_mutex);

        try
        {
            if (!line.empty() && (line.back() == '\r' || line.back() == '\n'))
            {
                line.pop_back();
            }

            if (line == "shutdown")
            {
                sendLine(sock, "Server shutting down...");
                shutdownFlag.store(true);
                if (listen_fd >= 0)
                {
                    close(listen_fd);
                    listen_fd = -1;
                }
                break;
            }

            std::istringstream iss(line);
            std::string cmd;
            iss >> cmd;

            if (cmd == "help")
            {
                sendLine(sock, "Available commands:");
                sendLine(sock, "  help                         - show help");
                sendLine(sock, "  shutdown                     - stop server");
                sendLine(sock, "  transfer <from> <to> <amt>   - transfer funds");
                sendLine(sock, "  freeze <id>                  - freeze account");
                sendLine(sock, "  unfreeze <id>                - unfreeze account");
                sendLine(sock, "  mass_update <amt>            - mass update balances");
                sendLine(sock, "  set_limits <id> <min> <max>  - set account limits");
                sendLine(sock, "  show_account_list            - showing accounts list");
                sendLine(sock, "  show_min <id>                - showing min balance for account <id>");
                sendLine(sock, "  show_max <id>                - showing max balance for account <id>");
                sendLine(sock, "  show_balance <id>            - showing balance for account <id>");
            }
            else if (cmd == "transfer")
            {
                int from, to;
                int32_t amt;
                if (!(iss >> from >> to >> amt))
                {
                    sendLine(sock, "Usage: transfer <from> <to> <amount>");
                }
                else
                {
                    bank->transferFunds(from, to, amt);
                    sendLine(sock, "OK: transferred " + std::to_string(amt));
                }
            }
            else if (cmd == "freeze")
            {
                int id;
                if (!(iss >> id))
                {
                    sendLine(sock, "Usage: freeze <id>");
                }
                else
                {
                    bank->freezeAccount(id);
                    sendLine(sock, "OK: account " + std::to_string(id) + " frozen");
                }
            }
            else if (cmd == "unfreeze")
            {
                int id;
                if (!(iss >> id))
                {
                    sendLine(sock, "Usage: unfreeze <id>");
                }
                else
                {
                    bank->unfreezeAccount(id);
                    sendLine(sock, "OK: account " + std::to_string(id) + " unfrozen");
                }
            }
            else if (cmd == "mass_update")
            {
                int32_t amt;
                if (!(iss >> amt))
                {
                    sendLine(sock, "Usage: mass_update <amount>");
                }
                else
                {
                    bank->massUpdate(amt);
                    sendLine(sock, "OK: balances updated by " + std::to_string(amt));
                }
            }
            else if (cmd == "set_limits")
            {
                int id;
                int32_t mn, mx;
                if (!(iss >> id >> mn >> mx))
                {
                    sendLine(sock, "Usage: set_limits <id> <min> <max>");
                }
                else
                {
                    bank->setLimits(id, mn, mx);
                    sendLine(sock, "OK: limits set for account " + std::to_string(id));
                }
            }
            else if (cmd == "show_account_list")
            {
                sendLine(sock, " ID |   Balance   |    Min    |    Max    | Frozen");
                sendLine(sock, "----+-------------+-----------+-----------+--------");
                size_t N = bank->getAccountCount();
                for (size_t i = 0; i < N; ++i)
                {
                    const Account &a = bank->getAccount(i);
                    std::ostringstream oss;
                    oss << std::setw(3) << a.account_id << " | "
                        << std::setw(11) << a.balance << " | "
                        << std::setw(9) << a.min_balance << " | "
                        << std::setw(9) << a.max_balance << " | "
                        << (a.frozen ? "true" : "false");
                    sendLine(sock, oss.str());
                }
            }
            else if (cmd == "show_balance")
            {
                int id;
                if (!(iss >> id))
                {
                    sendLine(sock, "Usage: show_balance <id>");
                }
                else
                {
                    // получаем ссылку на нужный аккаунт
                    const Account &a = bank->getAccount(static_cast<size_t>(id));
                    sendLine(sock,
                             "Account " + std::to_string(id) +
                                 " balance: " + std::to_string(a.balance));
                }
            }
            else if (cmd == "show_min")
            {
                int id;
                if (!(iss >> id))
                {
                    sendLine(sock, "Usage: show_min <id>");
                }
                else
                {
                    const Account &a = bank->getAccount(static_cast<size_t>(id));
                    sendLine(sock,
                             "Account " + std::to_string(id) +
                                 " min balance: " + std::to_string(a.min_balance));
                }
            }
            else if (cmd == "show_max")
            {
                int id;
                if (!(iss >> id))
                {
                    sendLine(sock, "Usage: show_max <id>");
                }
                else
                {
                    const Account &a = bank->getAccount(static_cast<size_t>(id));
                    sendLine(sock,
                             "Account " + std::to_string(id) +
                                 " max balance: " + std::to_string(a.max_balance));
                }
            }
            else
            {
                sendLine(sock, "Unknown command: " + cmd);
            }
        }
        catch (const std::exception &ex)
        {
            sendLine(sock, std::string("Error: ") + ex.what());
        }
    }

    close(sock);
    return nullptr;
}

int startServer(int port, Bank &bank)
{
    // Настраиваем signal handlers
    std::signal(SIGINT, handleSignal);
    std::signal(SIGTERM, handleSignal);

    // Создаём слушающий сокет
    listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_fd < 0)
    {
        perror("socket");
        return 1;
    }

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port);

    if (bind(listen_fd, (sockaddr *)&addr, sizeof(addr)) < 0)
    {
        perror("bind");
        close(listen_fd);
        return 1;
    }
    if (listen(listen_fd, SOMAXCONN) < 0)
    {
        perror("listen");
        close(listen_fd);
        return 1;
    }

    std::cout << "Server listening on port " << port << "...\n";

    // Запускаем stats-поток
    pthread_t stats_tid;
    pthread_create(&stats_tid, nullptr, statsThread, nullptr);
    pthread_detach(stats_tid);

    // Основной цикл accept()
    while (!shutdownFlag.load())
    {
        sockaddr_in client_addr{};
        socklen_t client_len = sizeof(client_addr);
        int client_fd = accept(listen_fd, (sockaddr *)&client_addr, &client_len);
        if (client_fd < 0)
        {
            if (shutdownFlag.load())
                break;
            perror("accept");
            continue;
        }

        std::cout << "New connection from "
                  << inet_ntoa(client_addr.sin_addr)
                  << ":" << ntohs(client_addr.sin_port) << "\n";

        auto *args = new std::pair<int, Bank *>(client_fd, &bank);
        pthread_t tid;
        pthread_create(&tid, nullptr, handleClient, args);
        pthread_detach(tid);
    }

    if (listen_fd >= 0)
        close(listen_fd);
    std::cout << "Server shutdown complete.\n";
    return 0;
}
int main(int argc, char **argv)
{
    // Параметры по умолчанию
    size_t N = 100;               // число счетов
    int32_t max_balance = 100000; // максимальный баланс
    int port = DEFAULT_PORT;      // порт из Server.hpp

    // Можно разобрать argc/argv, чтобы задать N, max_balance и порт из командной строки
    if (argc >= 2)
    {
        N = static_cast<size_t>(std::stoul(argv[1]));
    }
    if (argc >= 3)
    {
        max_balance = static_cast<int32_t>(std::stoi(argv[2]));
    }
    if (argc >= 4)
    {
        port = std::stoi(argv[3]);
    }

    // Инициализируем массив счетов
    Account *accounts = new Account[N];
    for (size_t i = 0; i < N; ++i)
    {
        accounts[i].account_id = static_cast<int>(i);
        accounts[i].balance = 0;
        accounts[i].min_balance = 0;
        accounts[i].max_balance = max_balance;
        accounts[i].frozen = false;
    }

    Bank bank(accounts, N);
    return startServer(port, bank);
}