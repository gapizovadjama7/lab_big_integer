#pragma once

#include <iostream>
#include <string>
#include <vector>

class BigInteger {
public:
    BigInteger();
    BigInteger(int val);
    BigInteger(long long val);
    explicit BigInteger(const std::string& s);

    BigInteger operator+ (const BigInteger& b) const;
    BigInteger operator- (const BigInteger& b) const;
    BigInteger operator* (const BigInteger& b) const;
    BigInteger operator/ (const BigInteger& b) const;
    BigInteger operator% (const BigInteger& b) const;

    BigInteger& operator+=(const BigInteger& b);
    BigInteger& operator-=(const BigInteger& b);
    BigInteger& operator*=(const BigInteger& b);
    BigInteger& operator/=(const BigInteger& b);
    BigInteger& operator%=(const BigInteger& b);

    bool operator==(const BigInteger& b) const;
    bool operator!=(const BigInteger& b) const;
    bool operator< (const BigInteger& b) const;
    bool operator> (const BigInteger& b) const;
    bool operator<=(const BigInteger& b) const;
    bool operator>=(const BigInteger& b) const;

    BigInteger operator-() const;
    BigInteger& operator++();
    BigInteger operator++(int);
    BigInteger& operator--();
    BigInteger operator--(int);

    friend std::ostream& operator<<(std::ostream& os, const BigInteger& val);
    friend std::istream& operator>>(std::istream& is, BigInteger& val);

    std::string to_string() const;
    bool is_zero() const;
    bool is_negative() const;
    explicit operator bool() const;

private:
    std::vector<int> v; // вектор для хранения цифр
    bool neg;           // переменная для знака (true если минус)
    static const int B = 1000000000; // основание системы (миллиард)

    void fix(); // убираем лишние нули
    int cmp_abs(const BigInteger& other) const; // сравниваем числа без знака
    void plus_abs(const BigInteger& other); // складываем без знака
    void minus_abs(const BigInteger& other); // вычитаем без знака
};
