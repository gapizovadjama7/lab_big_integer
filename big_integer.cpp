#include "big_integer.h"
#include <algorithm>
#include <iomanip>
#include <string>
#include <vector>

void BigInteger::fix() {
    while (v.size() > 1 && v.back() == 0) {
        v.pop_back();
    }
    if (v.empty()) {
        v.push_back(0);
        neg = false;
    }
    if (v.size() == 1 && v[0] == 0) {
        neg = false;
    }
}

BigInteger::BigInteger() : neg(false) {
    v.push_back(0);
}

BigInteger::BigInteger(int val) {
    neg = (val < 0);
    long long t = std::abs(static_cast<long long>(val));
    if (t == 0) v.push_back(0);
    while (t > 0) {
        v.push_back(static_cast<int>(t % B));
        t /= B;
    }
    fix();
}

BigInteger::BigInteger(long long val) {
    neg = (val < 0);
    unsigned long long t = (val < 0) ? -static_cast<unsigned long long>(val) : val;
    if (t == 0) v.push_back(0);
    while (t > 0) {
        v.push_back(static_cast<int>(t % B));
        t /= B;
    }
    fix();
}

BigInteger::BigInteger(const std::string& s) {
    if (s.empty() || s == "-" || s == "-0") {
        neg = false;
        v.push_back(0);
        return;
    }
    size_t start = 0;
    if (s[0] == '-') {
        neg = true;
        start = 1;
    } else {
        neg = false;
    }
    v.clear();
    for (int i = static_cast<int>(s.size()) - 1; i >= static_cast<int>(start); i -= 9) {
        int val = 0;
        int m = 1;
        for (int j = i; j > i - 9 && j >= static_cast<int>(start); --j) {
            val += (s[j] - '0') * m;
            m *= 10;
        }
        v.push_back(val);
    }
    fix();
}

int BigInteger::cmp_abs(const BigInteger& other) const {
    if (v.size() != other.v.size()) {
        return v.size() > other.v.size() ? 1 : -1;
    }
    for (int i = static_cast<int>(v.size()) - 1; i >= 0; i--) {
        if (v[i] != other.v[i]) return v[i] > other.v[i] ? 1 : -1;
    }
    return 0;
}

bool BigInteger::operator==(const BigInteger& b) const {
    return neg == b.neg && cmp_abs(b) == 0;
}
bool BigInteger::operator!=(const BigInteger& b) const { return !(*this == b); }
bool BigInteger::operator<(const BigInteger& b) const {
    if (neg != b.neg) return neg;
    return neg ? cmp_abs(b) > 0 : cmp_abs(b) < 0;
}
bool BigInteger::operator<=(const BigInteger& b) const { return *this < b || *this == b; }
bool BigInteger::operator>(const BigInteger& b) const { return !(*this <= b); }
bool BigInteger::operator>=(const BigInteger& b) const { return !(*this < b); }

void BigInteger::plus_abs(const BigInteger& other) {
    int c = 0;
    for (size_t i = 0; i < std::max(v.size(), other.v.size()) || c; ++i) {
        if (i == v.size()) v.push_back(0);
        long long cur = v[i] + c + (i < other.v.size() ? other.v[i] : 0);
        v[i] = static_cast<int>(cur % B);
        c = static_cast<int>(cur / B);
    }
}

void BigInteger::minus_abs(const BigInteger& other) {
    int c = 0; 
    for (size_t i = 0; i < other.v.size() || c; ++i) {
        long long cur = v[i] - c - (i < other.v.size() ? other.v[i] : 0);
        c = cur < 0;
        if (c) cur += B;
        v[i] = static_cast<int>(cur);
    }
    fix();
}

BigInteger& BigInteger::operator+=(const BigInteger& b) {
    if (neg == b.neg) {
        plus_abs(b);
    } else {
        if (cmp_abs(b) >= 0) {
            minus_abs(b);
        } else {
            BigInteger t = b;
            std::swap(v, t.v);
            std::swap(neg, t.neg);
            minus_abs(t);
        }
    }
    return *this;
}

BigInteger& BigInteger::operator-=(const BigInteger& b) {
    if (neg != b.neg) {
        plus_abs(b);
    } else {
        if (cmp_abs(b) >= 0) {
            minus_abs(b);
        } else {
            BigInteger t = b;
            std::swap(v, t.v);
            std::swap(neg, t.neg);
            minus_abs(t);
            neg = !neg;
        }
    }
    fix();
    return *this;
}

BigInteger& BigInteger::operator*=(const BigInteger& b) {
    if (is_zero() || b.is_zero()) {
        *this = BigInteger(0);
        return *this;
    }
    BigInteger res;
    res.v.assign(v.size() + b.v.size(), 0);
    res.neg = neg != b.neg;
    for (size_t i = 0; i < v.size(); ++i) {
        int c = 0;
        for (size_t j = 0; j < b.v.size() || c; ++j) {
            long long cur = res.v[i + j] + v[i] * 1LL * (j < b.v.size() ? b.v[j] : 0) + c;
            res.v[i + j] = static_cast<int>(cur % B);
            c = static_cast<int>(cur / B);
        }
    }
    res.fix();
    *this = res;
    return *this;
}

BigInteger& BigInteger::operator/=(const BigInteger& b) {
    if (b.is_zero()) return *this;
    bool res_neg = neg != b.neg;
    BigInteger d = b; d.neg = false;
    BigInteger q, rem;
    q.v.resize(v.size());
    for (int i = static_cast<int>(v.size()) - 1; i >= 0; i--) {
        rem *= B;
        rem += v[i];
        int low = 0, high = B - 1, dig = 0;
        while (low <= high) {
            int m = low + (high - low) / 2;
            if (d * m <= rem) {
                dig = m; low = m + 1;
            } else {
                high = m - 1;
            }
        }
        q.v[i] = dig;
        rem -= d * dig;
    }
    q.neg = res_neg;
    q.fix();
    *this = q;
    return *this;
}

BigInteger& BigInteger::operator%=(const BigInteger& b) {
    BigInteger q = *this / b;
    *this -= q * b;
    return *this;
}

BigInteger BigInteger::operator+(const BigInteger& b) const { return BigInteger(*this) += b; }
BigInteger BigInteger::operator-(const BigInteger& b) const { return BigInteger(*this) -= b; }
BigInteger BigInteger::operator*(const BigInteger& b) const { return BigInteger(*this) *= b; }
BigInteger BigInteger::operator/(const BigInteger& b) const { return BigInteger(*this) /= b; }
BigInteger BigInteger::operator%(const BigInteger& b) const { return BigInteger(*this) %= b; }

BigInteger BigInteger::operator-() const {
    BigInteger res = *this;
    if (!res.is_zero()) res.neg = !res.neg;
    return res;
}

BigInteger& BigInteger::operator++() { return *this += 1; }
BigInteger BigInteger::operator++(int) { BigInteger t = *this; ++(*this); return t; }
BigInteger& BigInteger::operator--() { return *this -= 1; }
BigInteger BigInteger::operator--(int) { BigInteger t = *this; --(*this); return t; }

std::string BigInteger::to_string() const {
    if (is_zero()) return "0";
    std::string s = neg ? "-" : "";
    s += std::to_string(v.back());
    for (int i = static_cast<int>(v.size()) - 2; i >= 0; i--) {
        std::string t = std::to_string(v[i]);
        s += std::string(9 - t.size(), '0') + t;
    }
    return s;
}

bool BigInteger::is_zero() const { return v.size() == 1 && v[0] == 0; }
bool BigInteger::is_negative() const { return neg; }
BigInteger::operator bool() const { return !is_zero(); }

std::ostream& operator<<(std::ostream& os, const BigInteger& val) {
    os << val.to_string();
    return os;
}

std::istream& operator>>(std::istream& is, BigInteger& val) {
    std::string s;
    if (is >> s) val = BigInteger(s);
    return is;
}
