// main.cpp
#include "Client.hpp"
#include "Bank.hpp"

#include <sys/mman.h>   // mmap, PROT_*, MAP_*
#include <fcntl.h>      // shm_open, O_CREAT, O_RDWR
#include <unistd.h>     // close
#include <iostream>
#include <string>
#include <cstdlib>      // std::stoul
#include <cerrno>
#include <cstring>

int main(int argc, char** argv) {
    if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << " <shm_name> <account_count>\n";
        return 1;
    }

    std::string shm_name = argv[1];
    size_t      N        = std::stoul(argv[2]);

    // 1. Открываем сегмент shared memory
    int shm_fd = shm_open(shm_name.c_str(), O_RDWR, 0666);
    if (shm_fd < 0) {
        std::cerr << "shm_open: " << std::strerror(errno) << "\n";
        return 1;
    }

    // 2. Мапим массив Account[N] из общего сегмента
    void* ptr = mmap(nullptr,
                     N * sizeof(Account),
                     PROT_READ | PROT_WRITE,
                     MAP_SHARED,
                     shm_fd,
                     0);
    close(shm_fd);  // дескриптор больше не нужен

    if (ptr == MAP_FAILED) {
        std::cerr << "mmap: " << std::strerror(errno) << "\n";
        return 1;
    }

    // 3. Строим объект Bank на этом массиве
    Account* accounts = static_cast<Account*>(ptr);
    Bank bank(accounts, N);

    // 4. Запускаем CLI
    Client cli(bank);
    cli.run();

    // 5. Отмэпим память перед выходом
    if (munmap(ptr, N * sizeof(Account)) < 0) {
        std::cerr << "munmap: " << std::strerror(errno) << "\n";
    }

    return 0;
}
