# TBANK: Прозрачный банк

Учебный проект на C++11 с CMake, демонстрирующий две версии «Прозрачного банка» и полный цикл разработки:

- **Shared-Memory Mode**: хранение данных банка в POSIX shared memory  
- **Client-Server Mode**: удалённый доступ по TCP-сокетам, многопоточный сервер  
- **CLI-клиенты**: локальный (`client`) и сетевой (`socket_client`) с цветным выводом  
- **Статистика**: серверная нить считает и каждые 5 запросов выводит общее число  
- **Graceful shutdown**: сервер корректно ловит `SIGINT`/`SIGTERM`  
- **Unit-тесты**: для `Bank` и функциональные проверки  
- **CI & Coverage**: готовый GitHub Actions и сборка с gcov/lcov  

---

## Возможности

- **Инициализация N счетов** с заданным `max_balance`  
- **Баланс, заморозка/разморозка** счета  
- **Перевод средств** между счетами  
- **Массовое обновление** балансов  
- **Установка лимитов** по счёту  
- **Shared-Memory CLI** с цветным выводом (colorprint)  
- **Multithreaded TCP-сервер** (команда `shutdown`, статистика запросов)  
- **Socket-Client** с теми же цветными шаблонами  
- **Graceful shutdown** по сигналам  
- **Unit-тесты** (`test_bank`) через CTest  
- **CI** (GitHub Actions) и **Coverage** (gcov/lcov)

---

## Требования

- ОС: Linux или UNIX-подобная  
- CMake ≥ 3.10  
- Компилятор: g++ (C++11)  
- POSIX API: sockets, pthread  
- Для цветного CLI: библиотека [colorprint](https://github.com/ndreyg/colorprint)  
- Для тестов/coverage: gcov, lcov; для профили: valgrind (memcheck, helgrind)

---

## Быстрая сборка

```bash
# 1. Клонируем основной репозиторий и библиотеку colorprint
git clone https://github.com/ArturVardikyan/TBANK.git
cd TBANK
mkdir -p lib && cd lib
git clone https://github.com/ndreyg/colorprint.git
cd ..

# 2. Сборка
mkdir build && cd build
cmake ..
make

# Соберутся:
#   bank_lib        — статическая библиотека
#   initializer     — shared-memory инициализатор
#   deinitializer   — shared-memory деинициализатор
#   client          — локальный CLI с цветом
#   socket_client   — сетевой клиент с цветом
#   server          — multithread TCP-сервер
#   test_bank       — unit-тесты для Bank

Shared-Memory Mode

    Инициализация сегмента

./initializer /TBANK_SHM <N> <max_balance>

Запуск локального клиента

./client /TBANK_SHM <N>

Удаление сегмента

./deinitializer /TBANK_SHM

Client-Server Mode

    Запуск сервера

./server <N> <max_balance> [port]

— по умолчанию port=12345. После listen() сервер сразу выводит справку и статистику по 5 запросам.

Запуск цветного сетевого клиента

./socket_client <host> <port>

Тестирование и Cover­age

    Unit-тесты

cd build
ctest --output-on-failure

Valgrind

valgrind --tool=memcheck ./test_bank
valgrind --tool=helgrind ./server 5 1000

Coverage

make coverage    # цель собирает с gcov и выводит отчет lcov/html

CI (GitHub Actions)

В корне есть .github/workflows/ci.yml, на каждый пуш:

    Сборка в Release и Debug

    Запуск ctest

    Запуск Valgrind

    Отчет покрытия

    TBANK/
├── include/
│   ├── Bank.hpp
│   ├── Client.hpp
│   ├── Server.hpp
│   ├── Initializer.hpp
│   └── Deinitializer.hpp
├── src/
│   ├── Bank.cpp
│   ├── Client.cpp
│   ├── Server.cpp
│   ├── Initializer.cpp
│   ├── Deinitializer.cpp
│   └── SocketClient.cpp
├── lib/
│   └── colorprint/    # внешняя библиотека colorprint
├── tests/
│   └── test_bank.cpp
├── CMakeLists.txt
└── .github/workflows/ci.yml
Лицензия

Проект предоставляется без лицензии — используйте на свой страх и риск. ))))
