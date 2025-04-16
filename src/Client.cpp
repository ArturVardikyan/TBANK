// Client.cpp
// Реализация методов класса Client — интерфейса командной строки для работы с объектом Bank.

#include "Client.hpp"
#include <sstream>   // Для std::istringstream — парсинг строки на токены
#include <iostream>  // Для std::cout, std::cerr, std::getline
#include <string>    // Для std::string
#include <exception> // Для std::exception

// Конструктор: сохраняем ссылку на переданный объект Bank.
// explicit Client(Bank& bank);
Client::Client(Bank& bank)
    : bank_(bank)
{
    // Ничего дополнительно не нужно: bank_ указывает на существующий объект Bank.
}

// Вывод списка доступных команд.
// const void displayHelp() const;
void Client::displayHelp() const {
    std::cout << "Available commands:\n"
              << "  help                         — show this help message\n"
              << "  exit                         — exit the program\n"
              << "  transfer <from> <to> <amt>   — transfer amt from account <from> to <to>\n"
              << "  freeze <id>                  — freeze account <id>\n"
              << "  unfreeze <id>                — unfreeze account <id>\n"
              << "  mass_update <amt>            — add (or subtract, if negative) <amt> to all accounts\n"
              << "  set_limits <id> <min> <max>  — set new [min,max] limits on account <id>\n"
              << std::endl;
}

// Главный цикл клиента: читаем команды и обрабатываем до "exit" или EOF.
// void run();
void Client::run() {
    std::cout << "Welcome to TBANK client!\n";
    displayHelp();

    std::string line;
    // Считываем строки из std::cin пока пользователь не введет EOF (Ctrl+D) или команда exit.
    while (true) {
        std::cout << "> ";                    // Приглашение к вводу
        if (!std::getline(std::cin, line)) { // Если достигнут EOF или ошибка ввода
            std::cout << "\nGoodbye!\n";
            break;
        }
        if (!processCommand(line)) {         // processCommand вернет false при команде exit
            std::cout << "Exiting client. Goodbye!\n";
            break;
        }
    }
}

// Обработка одной строки команды.
// Возвращает true, чтобы продолжать работу, false — чтобы выйти.
// bool processCommand(const std::string& line);
bool Client::processCommand(const std::string& line) {
    // Используем istringstream для разбора строки на слова (токены).
    std::istringstream iss(line);
    std::string cmd;
    if (!(iss >> cmd)) {
        // Пустая строка — ничего не делаем.
        return true;
    }

    try {
        if (cmd == "help") {
            displayHelp();
        }
        else if (cmd == "exit") {
            return false;  // сигнализируем о выходе из run()
        }
        else if (cmd == "transfer") {
            int from_id, to_id;
            int32_t amount;
            // Читаем три параметра: два ID и сумму
            if (!(iss >> from_id >> to_id >> amount)) {
                std::cerr << "Usage: transfer <from> <to> <amt>\n";
            } else {
                bank_.transferFunds(from_id, to_id, amount);
                std::cout << "Transferred " << amount
                          << " from " << from_id
                          << " to " << to_id << "\n";
            }
        }
        else if (cmd == "freeze") {
            int id;
            if (!(iss >> id)) {
                std::cerr << "Usage: freeze <id>\n";
            } else {
                bank_.freezeAccount(id);
                std::cout << "Account " << id << " frozen\n";
            }
        }
        else if (cmd == "unfreeze") {
            int id;
            if (!(iss >> id)) {
                std::cerr << "Usage: unfreeze <id>\n";
            } else {
                bank_.unfreezeAccount(id);
                std::cout << "Account " << id << " unfrozen\n";
            }
        }
        else if (cmd == "mass_update") {
            int32_t amount;
            if (!(iss >> amount)) {
                std::cerr << "Usage: mass_update <amt>\n";
            } else {
                bank_.massUpdate(amount);
                std::cout << "All accounts updated by " << amount << "\n";
            }
        }
        else if (cmd == "set_limits") {
            int id;
            int32_t new_min, new_max;
            if (!(iss >> id >> new_min >> new_max)) {
                std::cerr << "Usage: set_limits <id> <min> <max>\n";
            } else {
                bank_.setLimits(id, new_min, new_max);
                std::cout << "Limits for account " << id
                          << " set to [" << new_min << "," << new_max << "]\n";
            }
        }
        else {
            // Неизвестная команда
            std::cerr << "Unknown command: " << cmd << "\n";
            displayHelp();
        }
    }
    catch (const std::exception& ex) {
        // Ловим любые исключения из Bank (например, неверные ID или превышение лимитов)
        std::cerr << "Error: " << ex.what() << "\n";
    }

    return true;  // продолжаем работу
}
