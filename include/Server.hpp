#ifndef SERVER_HPP
#define SERVER_HPP

#include "Bank.hpp"    // Класс Bank, с которым будет работать сервер

// Порт по умолчанию, на котором слушаем подключения, можно переопределить в main()
static constexpr int DEFAULT_PORT = 12345;

/*
 * startServer()
 * -------------
 * Запускает сервер на указанном порту.
 *
 * @param port  — номер TCP‑порта, на котором сервер будет слушать входящие подключения.
 * @param bank  — ссылка на объект Bank, содержащий массив счетов и методы для операций.
 *
 * @return 0 при успешном запуске (и завершении работы сервера),
 *         или ненулевой код ошибки, если при старте что‑то пошло не так.
 */
int startServer(int port, Bank& bank);

#endif // SERVER_HPP
