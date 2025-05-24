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

#endif 


#include "Initializer.hpp"
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <cstring>
#include <iostream>

Bank* initializeBankShared(const std::string& shm_name, size_t N, int32_t max_balance) {
    int shm_fd = shm_open(shm_name.c_str(), O_CREAT | O_RDWR, 0666);
    if (shm_fd < 0) { perror("shm_open"); return nullptr; }

    if (ftruncate(shm_fd, N * sizeof(Account)) < 0) {
        perror("ftruncate"); close(shm_fd); return nullptr;
    }

    void* ptr = mmap(nullptr, N * sizeof(Account), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    close(shm_fd); 
    if (ptr == MAP_FAILED) { perror("mmap"); return nullptr; }

    Account* accounts = static_cast<Account*>(ptr);
    for (size_t i = 0; i < N; ++i) {
        accounts[i].account_id = static_cast<int>(i);
        accounts[i].balance    = 0;
        accounts[i].min_balance= 0;
        accounts[i].max_balance= max_balance;
        accounts[i].frozen     = false;
    }

    return new Bank(accounts, N);
}

int main(int argc, char** argv) {
    if (argc < 4) {
        std::cerr << "Usage: initializer <shm_name> <count> <max_balance>\n";
        return 1;
    }
    std::string shm_name = argv[1];
    size_t N            = static_cast<size_t>(std::stoul(argv[2]));
    int32_t max_balance = static_cast<int32_t>(std::stoi(argv[3]));

    Bank* bank = initializeBankShared(shm_name, N, max_balance);
    if (!bank) {
        std::cerr << "Failed to initialize bank\n";
        return 1;
    }
    std::cout << "Bank initialized in shared memory: " << shm_name << "\n";
    return 0;
}