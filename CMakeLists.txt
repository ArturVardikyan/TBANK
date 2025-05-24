cmake_minimum_required(VERSION 3.10)
project(TBANK VERSION 1.0 LANGUAGES CXX)

# ------------------------------------
# Общие настройки
# ------------------------------------
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Опция для code coverage
option(CODE_COVERAGE "Enable code coverage flags" OFF)
if(CODE_COVERAGE)
    message(STATUS "Building with code coverage enabled")
    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        add_compile_options(--coverage -O0 -g)
        add_link_options(--coverage)
    endif()
endif()

# Пути для собственных заголовков
include_directories(${CMAKE_SOURCE_DIR}/include)

# ------------------------------------
# Библиотека bank_lib: бизнес-логика
# ------------------------------------
add_library(bank_lib STATIC
    src/Bank.cpp
)
target_include_directories(bank_lib PUBLIC
    ${CMAKE_SOURCE_DIR}/include
)

# ------------------------------------
# Shared-memory Initializer
# ------------------------------------
add_executable(initializer
    src/Initializer.cpp
)
target_link_libraries(initializer PRIVATE bank_lib)

# ------------------------------------
# Shared-memory Deinitializer
# ------------------------------------
add_executable(deinitializer
    src/Deinitializer.cpp
)

# ------------------------------------
# Библиотека colorprint
# ------------------------------------
add_library(colorprint STATIC
    lib/colorprint/colorprint.cpp
)
target_include_directories(colorprint PUBLIC
    ${CMAKE_SOURCE_DIR}/lib/colorprint
)

# ------------------------------------
# CLI-клиент для Shared-Memory Mode
# ------------------------------------
add_executable(client
    src/main.cpp
    src/Client.cpp
)
target_include_directories(client PRIVATE
    ${CMAKE_SOURCE_DIR}/lib/colorprint
)
target_link_libraries(client PRIVATE
    bank_lib
    colorprint
    readline   # для истории команд через readline
)

# ------------------------------------
# TCP-сокетный сервер
# ------------------------------------
add_executable(server
    src/Server.cpp
)
target_link_libraries(server PRIVATE
    bank_lib
    pthread
)

# ------------------------------------
# Unit-тесты для Bank
# ------------------------------------
enable_testing()

add_executable(test_bank
    tests/test_bank.cpp
)
target_link_libraries(test_bank PRIVATE bank_lib)
add_test(NAME bank_unit COMMAND test_bank)

# ------------------------------------
# Интеграционные тесты
# ------------------------------------
add_test(NAME shared_integration
    COMMAND ${CMAKE_SOURCE_DIR}/tests/integration/shared_mode.sh
)
add_test(NAME socket_integration
    COMMAND ${CMAKE_SOURCE_DIR}/tests/integration/socket_mode.sh
)

add_custom_target(release
  COMMENT "Configure & build Release"
  COMMAND ${CMAKE_COMMAND} -DCMAKE_BUILD_TYPE=Release ${CMAKE_SOURCE_DIR}
  COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --config Release
)
add_custom_target(debug
  COMMENT "Configure, build Debug and run Valgrind"
  COMMAND ${CMAKE_COMMAND} -DCMAKE_BUILD_TYPE=Debug ${CMAKE_SOURCE_DIR}
  COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --config Debug
  # memcheck на тестах
  COMMAND valgrind --leak-check=full $<TARGET_FILE:test_bank>
  # helgrind на сервере (пример с N=5, max=1000)
  COMMAND valgrind --tool=helgrind $<TARGET_FILE:server> 5 1000
  DEPENDS test_bank server
)

find_program(LCOV_PATH lcov)
find_program(GENHTML_PATH genhtml)

add_custom_target(coverage
  COMMENT "Capture and generate coverage report"
  # захват всего, включая библиотеки
  COMMAND ${LCOV_PATH} --capture --directory . --output-file coverage.info
  # убрать системные файлы
  COMMAND ${LCOV_PATH} --remove coverage.info '/usr/*' --output-file coverage.cleaned.info
  # сгенерировать HTML
  COMMAND ${GENHTML_PATH} coverage.cleaned.info --output-directory coverage_html
  DEPENDS test_bank
)
# ------------------------------------
# Сборка в Release/Debug
# Пользователь может вызвать:
#   cmake -DCMAKE_BUILD_TYPE=Debug .. && make
#   cmake -DCMAKE_BUILD_TYPE=Release .. && make
# ------------------------------------
