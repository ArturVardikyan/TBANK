#include "Bank.hpp"
#include <cassert>
#include <iostream>
#include <stdexcept>
#include <vector>

#define ASSERT_THROW(stmt, ex_type)      \
    do {                                 \
        bool thrown = false;             \
        try { stmt; }                    \
        catch (const ex_type&) { thrown = true; } \
        catch (...) {}                   \
        assert(thrown && #stmt " throws " #ex_type); \
    } while(0)

void test_initialization() {
    const size_t N = 5;
    const int32_t MAX_BAL = 1000;
    Account* accounts = new Account[N];
    for (size_t i = 0; i < N; ++i) {
        accounts[i].account_id  = static_cast<int>(i);
        accounts[i].balance     = 0;
        accounts[i].min_balance = 0;
        accounts[i].max_balance = MAX_BAL;
        accounts[i].frozen      = false;
    }
    Bank bank(accounts, N);

    assert(bank.getAccountCount() == N && "getAccountCount");

    for (size_t i = 0; i < N; ++i) {
        const Account& a = bank.getAccount(i);
        assert(a.account_id == static_cast<int>(i) && "account_id");
        assert(a.balance == 0 && "initial balance");
        assert(a.min_balance == 0 && "initial min_balance");
        assert(a.max_balance == MAX_BAL && "initial max_balance");
        assert(a.frozen == false && "initial frozen flag");
    }

    delete[] accounts;
}

void test_transfer_and_limits() {
    const size_t N = 3;
    Account* accounts = new Account[N];
    for (size_t i = 0; i < N; ++i) {
        accounts[i].account_id  = static_cast<int>(i);
        accounts[i].balance     = 100;
        accounts[i].min_balance = 0;
        accounts[i].max_balance = 500;
        accounts[i].frozen      = false;
    }
    Bank bank(accounts, N);

    bank.transferFunds(0, 1, 50);
    assert(bank.getAccount(0).balance == 50);
    assert(bank.getAccount(1).balance == 150);

    ASSERT_THROW(bank.transferFunds(0, 2, 1000), std::runtime_error);

    assert(bank.getAccount(0).balance == 50);
    assert(bank.getAccount(2).balance == 100);

    delete[] accounts;
}

void test_freeze() {
    const size_t N = 2;
    Account* accounts = new Account[N];
    for (size_t i = 0; i < N; ++i) {
        accounts[i].account_id  = static_cast<int>(i);
        accounts[i].balance     = 200;
        accounts[i].min_balance = 0;
        accounts[i].max_balance = 500;
        accounts[i].frozen      = false;
    }
    Bank bank(accounts, N);

    bank.freezeAccount(0);
    ASSERT_THROW(bank.transferFunds(0, 1, 50), std::runtime_error);

    bank.unfreezeAccount(0);
    bank.transferFunds(0, 1, 50);
    assert(bank.getAccount(0).balance == 150);
    assert(bank.getAccount(1).balance == 250);

    delete[] accounts;
}

void test_mass_update() {
    const size_t N = 4;
    Account* accounts = new Account[N];
    for (size_t i = 0; i < N; ++i) {
        accounts[i].account_id  = static_cast<int>(i);
        accounts[i].balance     = 100;
        accounts[i].min_balance = 0;
        accounts[i].max_balance = 150;
        accounts[i].frozen      = false;
    }
    Bank bank(accounts, N);

    bank.massUpdate(25);
    for (size_t i = 0; i < N; ++i) {
        assert(bank.getAccount(i).balance == 125);
    }

    ASSERT_THROW(bank.massUpdate(100), std::runtime_error);

    ASSERT_THROW(bank.massUpdate(-200), std::runtime_error);

    delete[] accounts;
}

void test_set_limits() {
    const size_t N = 2;
    Account* accounts = new Account[N];
    for (size_t i = 0; i < N; ++i) {
        accounts[i].account_id  = static_cast<int>(i);
        accounts[i].balance     = 100;
        accounts[i].min_balance = 0;
        accounts[i].max_balance = 1000;
        accounts[i].frozen      = false;
    }
    Bank bank(accounts, N);

    bank.setLimits(1, 50, 200);
    assert(bank.getAccount(1).min_balance == 50);
    assert(bank.getAccount(1).max_balance == 200);

    ASSERT_THROW(bank.setLimits(0, 500, 100), std::runtime_error);

    ASSERT_THROW(bank.setLimits(0, 150, 200), std::runtime_error);

    delete[] accounts;
}

int main() {
    std::cout << "Running Bank unit tests...\n";
    test_initialization();
    test_transfer_and_limits();
    test_freeze();
    test_mass_update();
    test_set_limits();
    std::cout << "All tests passed successfully.\n";
    return 0;
}
