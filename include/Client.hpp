#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "Bank.hpp"
#include <string>
#include <iostream>
#include <colorprint.hpp>

/*
 * Client — локальный CLI для режима Shared-Memory.
 * Поддерживает цветной вывод через библиотеку colorprint.
 */
class Client {
public:
    explicit Client(Bank& bank);

    // Запускает главный цикл ввода-вывода
    void run();

private:
    Bank& bank_;  // ссылка на логику банка

    // Печатает справку (через Painter)
    void displayHelp(Painter& p) const;

    // Обрабатывает одну введённую строку. Возвращает false, чтобы выйти.
    bool processCommand(const std::string& line, Painter& p);

    // Новые команды:
    void showAccountList(Painter& p) const;
    void showBalance(int id, Painter& p) const;
    void showMin(int id, Painter& p) const;
    void showMax(int id, Painter& p) const;
};

#endif // CLIENT_HPP
