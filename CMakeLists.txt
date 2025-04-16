cmake_minimum_required(VERSION 3.10)
project(TBANK VERSION 1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(${CMAKE_SOURCE_DIR}/include)

add_library(bank_lib STATIC
    src/bank.cpp
)

add_executable(initializer src/initializer.cpp)
target_link_libraries(initializer bank_lib)

add_executable(deinitializer src/deinitializer.cpp)
target_link_libraries(deinitializer bank_lib)

add_executable(client src/client.cpp)
target_link_libraries(client bank_lib)

add_executable(server src/server.cpp)
target_link_libraries(server bank_lib pthread)

# (Опционально) добавление тестового исполняемого файла
# add_executable(test_bank tests/test_bank.cpp)
# target_link_libraries(test_bank bank_lib)
