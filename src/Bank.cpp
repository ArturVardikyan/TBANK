#include "Bank.hpp"

// Перевод средств между двумя ID
int Bank::transferFunds(int from_id, int to_id, int32_t amount) {
    if (amount <= 0) {
        throw std::invalid_argument("transferFunds: amount must be positive");
    }

    Account &src = findAccount(from_id);
    Account &dst = findAccount(to_id);

    if (src.frozen || dst.frozen) {
        throw std::runtime_error("transferFunds: one of the accounts is frozen");
    }
    if (src.balance - amount < src.min_balance) {
        throw std::runtime_error("transferFunds: insufficient funds on source account");
    }
    if (dst.balance + amount > dst.max_balance) {
        throw std::runtime_error("transferFunds: would exceed max balance on destination");
    }

    src.balance -= amount;
    dst.balance += amount;
    return 0;
}

// Заморозить счёт
void Bank::freezeAccount(int id) {
    Account &acc = findAccount(id);
    acc.frozen = true;
}

// Разморозить счёт
void Bank::unfreezeAccount(int id) {
    Account &acc = findAccount(id);
    acc.frozen = false;
}

size_t Bank::getAccountCount() const noexcept {
    return count_;       
}

const Account& Bank::getAccount(size_t idx) const {
    if (idx >= count_) throw std::out_of_range("Account index");
    return accounts_[idx];
}

// Массовое обновление балансов
int Bank::massUpdate(int32_t amount) {
    for (size_t i = 0; i < count_; ++i) {
        int32_t new_bal = accounts_[i].balance + amount;
        if (new_bal < accounts_[i].min_balance || new_bal > accounts_[i].max_balance) {
            throw std::runtime_error("massUpdate: balance would violate limits");
        }
        accounts_[i].balance = new_bal;
    }
    return 0;
}

// Установка лимитов для одного счёта
int Bank::setLimits(int id, int32_t new_min, int32_t new_max) {
    if (new_min > new_max) {
        throw std::invalid_argument("setLimits: new_min > new_max");
    }
    Account &acc = findAccount(id);
    if (acc.balance < new_min || acc.balance > new_max) {
        throw std::runtime_error("setLimits: current balance out of new limits");
    }
    acc.min_balance = new_min;
    acc.max_balance = new_max;
    return 0;
}
