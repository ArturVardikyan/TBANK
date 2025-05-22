#ifndef SERVER_HPP
#define SERVER_HPP

#include "Bank.hpp"
#include <cstdint>

/*
 * DEFAULT_PORT — порт по умолчанию, на котором сервер слушает входящие подключения.
 */
static constexpr int DEFAULT_PORT = 12345;

/*
 * startServer
 * -----------
 * Запускает TCP-сервер банка с указанными параметрами.
 *
 * @param port — TCP-порт для bind()/listen().
 * @param bank — ссылка на объект Bank, содержащий логику операций.
 * @return 0 при нормальном завершении, или код ошибки при неудаче.
 */
int startServer(int port, Bank& bank);

#endif // SERVER_HPP
