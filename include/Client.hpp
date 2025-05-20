#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "Bank.hpp"      // Подключаем нашу логику работы с банком
#include <string>        // Для std::string, в котором будем хранить ввод пользователя
#include <iostream>      // Для std::cin, std::cout и std::cerr
#include <colorprint.hpp>
/*
 * Класс Client
 * ------------
 * Инкапсулирует логику пользовательского интерфейса:
 *  - Читает команды из консоли
 *  - Разбирает текст (парсит аргументы)
 *  - Вызывает методы Bank для выполнения операций
 */
class Client {
public:
    /*
     * Конструктор
     * @param bank  — ссылка на существующий объект Bank, через который будут выполняться все операции
     */
    explicit Client(Bank& bank);

    /*
     * Метод run()
     * ------------
     * Запускает главный цикл программы:
     *  - Выводит приветствие и справку
     *  - В цикле читает строки из std::cin
     *  - Для каждой строки вызывает processCommand()
     *  - При получении команды "exit" или EOF выходит из цикла
     */
    void run();

private:
    Bank& bank_;  // Ссылка на банк, с которым работаем

    /*
     * Метод displayHelp(Painter& p)
     * --------------------
     * Печатает на экран список доступных команд и краткое описание их синтаксиса.
     * Например:
     *   help
     *   exit
     *   init <count> <max_balance>
     *   transfer <from_id> <to_id> <amount>
     *   freeze <id>
     *   unfreeze <id>
     *   mass_update <amount>
     *   set_limits <id> <min> <max>
     */
     void displayHelp(Painter& p) const;
    /*
     * Метод processCommand()
     * -----------------------
     * Разбирает одну строку команды и выполняет соответствующее действие через Bank.
     *
     * @param line — введённая строка, например "transfer 0 1 500"
     * @return true, если нужно продолжить работу; false, чтобы выйти из цикла (команда exit)
     *
     * Логика:
     * 1) Разбить строку на слова (с помощью std::istringstream или вручную).
     * 2) Сравнить первый токен с именами команд.
     * 3) Считать нужные аргументы (конвертировать в int или int32_t).
     * 4) Вызвать у bank_ нужный метод, обернуть вызов в try/catch для обработки ошибок.
     */
     bool processCommand(const std::string& line, Painter& p);
};

#endif // CLIENT_HPP
