cmake_minimum_required(VERSION 3.10)
project(TBANK VERSION 1.0 LANGUAGES CXX)

# Устанавливаем стандарт C++
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Пути для поиска собственных заголовков
include_directories(${CMAKE_SOURCE_DIR}/include)

# -------------------------------------------------------------------
# Библиотека bank_lib: бизнес-логика банка
# -------------------------------------------------------------------
add_library(bank_lib STATIC
    src/Bank.cpp
)
# Указываем, где искать заголовки для bank_lib
target_include_directories(bank_lib
    PUBLIC
        ${CMAKE_SOURCE_DIR}/include
)

# -------------------------------------------------------------------
# Shared-memory Initializer
# -------------------------------------------------------------------
add_executable(initializer
    src/Initializer.cpp
)
target_link_libraries(initializer
    PRIVATE bank_lib
)

# -------------------------------------------------------------------
# Shared-memory Deinitializer
# -------------------------------------------------------------------
add_executable(deinitializer
    src/Deinitializer.cpp
)
# deinitializer не зависит от bank_lib

# -------------------------------------------------------------------
# Библиотека colorprint
# -------------------------------------------------------------------
add_library(colorprint STATIC
    lib/colorprint/colorprint.cpp
)
# Скомпилируем и подключим заголовки из этого же каталога
target_include_directories(colorprint PUBLIC
    ${CMAKE_SOURCE_DIR}/lib/colorprint
)


# -------------------------------------------------------------------
# CLI-клиент для Shared-Memory Mode (с colorprint)
# -------------------------------------------------------------------
add_executable(client
    src/main.cpp
    src/Client.cpp
)
# Пути для поиска заголовков colorprint
target_include_directories(client
    PRIVATE ${CMAKE_SOURCE_DIR}/lib/colorprint
)
# Линкуем client с бизнес-логикой банка и статической библиотекой colorprint
target_link_libraries(client
    PRIVATE
        bank_lib
        colorprint
        readline            # <-- сюда
)

# -------------------------------------------------------------------
# TCP-сокетный сервер
# -------------------------------------------------------------------
add_executable(server
    src/Server.cpp
)
target_link_libraries(server
    PRIVATE
        bank_lib
        pthread
)

# -------------------------------------------------------------------
# Unit-тесты (опционально)
# -------------------------------------------------------------------
enable_testing()

add_executable(test_bank
    tests/test_bank.cpp
)
target_link_libraries(test_bank
    PRIVATE bank_lib
)
add_test(NAME bank_tests COMMAND test_bank)
