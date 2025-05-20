// tests/test_bank.cpp
#include "Bank.hpp"
#include <cassert>
#include <iostream>
#include <stdexcept>

int main() {
    const size_t N = 3;
    const int32_t maxBal = 1000;

    // 1) Выделяем и инициализируем массив счетов
    Account* accounts = new Account[N];
    for (size_t i = 0; i < N; ++i) {
        accounts[i].account_id  = static_cast<int>(i);
        accounts[i].balance     = 0;
        accounts[i].min_balance = 0;
        accounts[i].max_balance = maxBal;
        accounts[i].frozen      = false;
    }

    Bank bank(accounts, N);

    // 2) Задаём начальные балансы для теста переводов
    accounts[0].balance = 500;
    accounts[1].balance = 200;
    accounts[2].balance = 0;

    // --- Тест 1: успешный перевод ---
    bank.transferFunds(0, 1, 300);
    assert(accounts[0].balance == 200);
    assert(accounts[1].balance == 500);

    // --- Тест 2: перевод с недостаточными средствами должен выбросить исключение ---
    bool caught = false;
    try {
        bank.transferFunds(0, 1, 500);
    } catch (const std::runtime_error&) {
        caught = true;
    }
    assert(caught);

    // --- Тест 3: заморозка счета блокирует переводы ---
    bank.freezeAccount(2);
    caught = false;
    try {
        bank.transferFunds(1, 2, 100);
    } catch (const std::runtime_error&) {
        caught = true;
    }
    assert(caught);
    bank.unfreezeAccount(2);

    // --- Тест 4: массовое обновление балансов ---
    bank.massUpdate(50);
    assert(accounts[0].balance == 250);
    assert(accounts[1].balance == 550);
    assert(accounts[2].balance == 50);

    // --- Тест 5: корректная установка лимитов ---
    bank.setLimits(1, 100, 600);
    assert(accounts[1].min_balance == 100);
    assert(accounts[1].max_balance == 600);

    // --- Тест 6: некорректная установка лимитов выбрасывает исключение ---
    caught = false;
    try {
        bank.setLimits(1, 700, 600);
    } catch (const std::invalid_argument&) {
        caught = true;
    }
    assert(caught);

    std::cout << "All tests passed!\n";

    delete[] accounts;
    return 0;
}
