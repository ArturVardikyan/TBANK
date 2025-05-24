#include "Bank.hpp"

int Bank::transferFunds(int from_id, int to_id, int32_t amount)
{
    if (amount <= 0)
    {
        throw std::invalid_argument("transferFunds: amount must be positive");
    }

    Account &src = findAccount(from_id);
    Account &dst = findAccount(to_id);

    if (src.frozen || dst.frozen)
    {
        throw std::runtime_error("transferFunds: one of the accounts is frozen");
    }
    if (src.balance - amount < src.min_balance)
    {
        throw std::runtime_error("transferFunds: insufficient funds on source account");
    }
    if (dst.balance + amount > dst.max_balance)
    {
        throw std::runtime_error("transferFunds: would exceed max balance on destination");
    }

    src.balance -= amount;
    dst.balance += amount;
    return 0;
}

void Bank::freezeAccount(int id)
{
    Account &acc = findAccount(id);
    acc.frozen = true;
}

void Bank::unfreezeAccount(int id)
{
    Account &acc = findAccount(id);
    acc.frozen = false;
}

size_t Bank::getAccountCount() const noexcept
{
    return count_;
}

const Account &Bank::getAccount(size_t idx) const
{
    if (idx >= count_)
        throw std::out_of_range("Account index");
    return accounts_[idx];
}

int Bank::massUpdate(int32_t amount)
{
    for (size_t i = 0; i < count_; ++i)
    {
        int32_t new_bal = accounts_[i].balance + amount;
        if (new_bal < accounts_[i].min_balance || new_bal > accounts_[i].max_balance)
        {
            throw std::runtime_error("massUpdate: balance would violate limits");
        }
        accounts_[i].balance = new_bal;
    }
    return 0;
}

void Bank::setLimits(size_t id, int32_t newMin, int32_t newMax) {
    if (newMin > newMax) {
        throw std::runtime_error(
            "setLimits: newMin (" + std::to_string(newMin) +
            ") cannot be greater than newMax (" + std::to_string(newMax) + ")"
        );
    }
    Account& acc = accounts_[id];
    if (acc.balance < newMin || acc.balance > newMax) {
        throw std::runtime_error(
            "setLimits: current balance (" + std::to_string(acc.balance) +
            ") is outside the new limits [" + std::to_string(newMin) +
            "," + std::to_string(newMax) + "]"
        );
    }
    acc.min_balance = newMin;
    acc.max_balance = newMax;
}