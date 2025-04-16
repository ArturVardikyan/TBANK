#include "Deinitializer.hpp"
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <iostream>

int deinitializeBankShared(const std::string& shm_name) {
    if (shm_unlink(shm_name.c_str()) < 0) {
        perror("shm_unlink");
        return 1;
    }
    return 0;
}

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "Usage: deinitializer <shm_name>\n";
        return 1;
    }
    std::string shm_name = argv[1];
    int res = deinitializeBankShared(shm_name);
    if (res == 0) {
        std::cout << "Shared memory segment removed: " << shm_name << "\n";
    }
    return res;
}