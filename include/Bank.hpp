#ifndef BANK_HPP
#define BANK_HPP

#include <cstdint>   // для int32_t
#include <cstddef>   // для size_t
#include <stdexcept> // для исключений

// Описание счёта
struct Account
{
    int account_id;      // Уникальный ID счёта
    int32_t balance;     // Текущий баланс
    int32_t min_balance; // Минимальный баланс
    int32_t max_balance; // Максимальный баланс
    bool frozen;         // true — заморожен, false — активен
};

/*
 * Класс Bank
 * -----------
 * Инкапсулирует логику работы с массивом счетов,
 * не владеет памятью сам по себе (не вызывает delete[]),
 * а лишь оперирует внешним массивом Account*.
 */
class Bank
{
public:
    /*
     * Конструктор
     * @param accounts_ptr — указатель на внешний массив Account[n]
     * @param count        — число элементов в этом массиве
     *
     * Никакой аллокации здесь не происходит: класс просто запоминает,
     * где лежат счета, и сколько их.
     */
    Bank(Account *accounts_ptr, size_t count)
        : accounts_(accounts_ptr), count_(count)
    {
        if (!accounts_ || count_ == 0)
        {
            throw std::invalid_argument("Bank: invalid accounts pointer or count");
        }
    }

    // Запрещаем копирование, чтобы случайно не получить два объекта, ссылающихся на один массив
    Bank(const Bank &) = delete;
    Bank &operator=(const Bank &) = delete;

    /*
     * Перевод средств
     * from_id, to_id — ID счетов, amount — строго положительная сумма.
     * Возвращает 0 при успехе, в остальных случаях выбрасывает исключение.
     */
    int transferFunds(int from_id, int to_id, int32_t amount);

    /*
     * Заморозка/разморозка счёта по ID.
     * Если ID некорректен — выбрасывает std::runtime_error.
     */
    void freezeAccount(int id);
    void unfreezeAccount(int id);
    /*
     * Массовое обновление балансов всех счетов.
     * Сумма может быть отрицательной (списание).
     * Проверяет, что не выйдем за лимиты.
     */
    int massUpdate(int32_t amount);

    /*
     * Установить новые лимиты для заданного ID.
     * new_min ≤ new_max и текущий баланс должен попадать в [new_min, new_max].
     */
    // устанавливает новые границы, бросает std::runtime_error, если newMin > newMax
    void setLimits(size_t accountId, int32_t newMin, int32_t newMax);

    size_t getAccountCount() const noexcept;

    const Account &getAccount(size_t idx) const;

private:
    Account *accounts_; // Внешний массив счетов (в shared‑memory или в куче)
    size_t count_;      // Число счетов

    // Вспомогательная функция — найти счёт по ID. Если не находятся, бросить исключение.
    Account &findAccount(int id)
    {
        for (size_t i = 0; i < count_; ++i)
        {
            if (accounts_[i].account_id == id)
            {
                return accounts_[i];
            }
        }
        throw std::runtime_error("Bank: account ID not found");
    }
};

#endif // BANK_HPP
