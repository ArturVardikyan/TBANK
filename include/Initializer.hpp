// Initializer.hpp
#ifndef INITIALIZER_HPP
#define INITIALIZER_HPP

#include "Bank.hpp"
#include <string>

/*
 * initializeBankShared
 * ---------------------
 * Создаёт и инициализирует банк в сегменте общей памяти.
 *
 * @param shm_name    - имя сегмента shared memory
 * @param N           - количество счетов
 * @param max_balance - максимальный баланс для каждого счета
 * @return Указатель на созданный объект Bank или nullptr при ошибке.
 */
Bank* initializeBankShared(const std::string& shm_name, size_t N, int32_t max_balance);

#endif // INITIALIZER_HPP