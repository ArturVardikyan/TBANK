# TBANK: Прозрачный банк

Это учебный проект на C++ (C++11) с использованием CMake, демонстрирующий две версии «Прозрачного банка»:

1. **Shared Memory**: хранение данных банка в разделяемом сегменте памяти (shared memory). Утилиты:
   - **initializer** — создаёт сегмент, инициализирует счета;
   - **deinitializer** — удаляет сегмент;
   - **(опционально) client** — CLI, работающий напрямую с shared memory.

2. **Client-Server**: удалённое управление банком через TCP‑сокеты. Программы:
   - **server** — многопоточный TCP‑сервер, обрабатывающий запросы клиентов;
   - **любые TCP‑клиенты** (telnet/netcat) — можно использовать для отправки команд серверу.

---

## Возможности

- Инициализация N счетов с указанным максимальным балансом  
- Вывод баланса, заморозка/разморозка счёта  
- Перевод средств между счетами  
- Массовое обновление балансов  
- Установка индивидуальных лимитов для счетов  
- Удалённое управление через сокеты (Client-Server)

---

## Требования

- Linux (или UNIX-подобная ОС)  
- CMake ≥ 3.10  
- Компилятор с поддержкой C++11 (например, g++ из пакета `build-essential`)  
- POSIX API (sockets, pthread)  

---

## Сборка проекта

```bash
# 1. Клонируем репозиторий и переходим в директорию
git clone https://github.com/ArturVardikyan/TBANK.git
cd TBANK

# 2. Создаём папку для сборки и генерируем файлы CMake
mkdir build && cd build
cmake ..

# 3. Сборка всех целей
make

После этого в папке build появятся исполняемые файлы:

    initializer — утилита инициализации shared memory

    deinitializer — утилита удаления shared memory

    server — TCP‑сервер банка

    (при наличии) client — локальный CLI (необязателен)

Shared Memory Mode

    Инициализация (создаёт сегмент /TBANK_SHM с N счетами):

./initializer /TBANK_SHM <N> <max_balance>

Работа с данными:

    Через локальный CLI client (если есть) или через отдельный код, мапящий /TBANK_SHM.

Удаление сегмента:

    ./deinitializer /TBANK_SHM

    Примечание: Shared Memory Mode требует клиент, умеющий мапить и работать с /TBANK_SHM.

Client-Server Mode

    Запуск сервера (по умолчанию порт 12345):

./server

Подключение клиента (telnet или netcat):

telnet localhost 12345
# или
nc localhost 12345

Пример команд:

    help
    transfer 0 1 500
    freeze 2
    mass_update -100
    set_limits 3 0 10000
    shutdown

    Завершение работы сервера:

        Отправьте команду shutdown из клиента.

Структура проекта

TBANK/
├── Bank.hpp, Bank.cpp
├── Client.hpp, Client.cpp
├── Server.hpp, server.cpp
├── Initializer.hpp, Initializer.cpp
├── Deinitializer.hpp, Deinitializer.cpp
└── CMakeLists.txt

    bank_lib — статическая библиотека с бизнес-логикой банка.

    initializer/deinitializer — утилиты для shared memory.

    server — TCP‑сервер.

    (client) — локальный клиент (CLI).

Внесение изменений и тестирование

    Изменения в логике счёта — в Bank.cpp.

    Изменения в протоколе — в server.cpp / Client.cpp.

    Для повторного тестирования shared memory запускайте deinitializer перед initializer.

Лицензия

Данный проект предоставляется без лицензии — используйте на свой страх и риск.
