cmake_minimum_required(VERSION 3.10)
project(TBANK VERSION 1.0 LANGUAGES CXX)

# Устанавливаем стандарт C++
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Пути для поиска заголовочных файлов
include_directories(${CMAKE_SOURCE_DIR}/include)

# Библиотека с бизнес-логикой банка
add_library(bank_lib STATIC
    src/Bank.cpp
)

# Shared Memory Initializer
add_executable(initializer
    src/Initializer.cpp
)
target_link_libraries(initializer PRIVATE bank_lib)

# Shared Memory Deinitializer
add_executable(deinitializer
    src/Deinitializer.cpp
)

# Shared-Memory CLI Client
add_executable(client
    src/main.cpp
    src/Client.cpp
)
target_link_libraries(client PRIVATE bank_lib)

# TCP-сокетный сервер
add_executable(server
    src/Server.cpp
)
target_link_libraries(server PRIVATE bank_lib pthread)

# Unit-тесты (опционально)
enable_testing()
add_executable(test_bank
    tests/test_bank.cpp
)
target_link_libraries(test_bank PRIVATE bank_lib)
add_test(NAME bank_tests COMMAND test_bank)
