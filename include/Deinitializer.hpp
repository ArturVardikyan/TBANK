#ifndef DEINITIALIZER_HPP
#define DEINITIALIZER_HPP

#include <string>

/*
 * deinitializeBankShared
 * -----------------------
 * Удаляет сегмент общей памяти с заданным именем.
 *
 * @param shm_name - имя сегмента shared memory
 * @return 0 при успехе, ненулевой код при ошибке.
 */
int deinitializeBankShared(const std::string& shm_name);

#endif // DEINITIALIZER_HPP
