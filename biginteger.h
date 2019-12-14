#include <iostream>
#include <vector>
#include <string>


constexpr int base10 = 10000;

class BigInteger {
public:
    BigInteger() : factorisation(std::vector<int>(1, 0)), sign(true) {}
    BigInteger(int n) { convert_from_int(n); }
    explicit BigInteger(const std::string& str);
    BigInteger(const BigInteger& another) {
        sign = another.sign;
        factorisation = another.factorisation;
    }
    BigInteger(BigInteger&& another) noexcept {
        sign = another.sign;
        factorisation.swap(another.factorisation);
    }

    BigInteger& operator=(const BigInteger& another)& {
        sign = another.sign;
        factorisation = another.factorisation;
        return *this;
    };
    BigInteger& operator=(BigInteger&& another)& noexcept {
        sign = another.sign;
        factorisation.swap(another.factorisation);
        return *this;
    }

    BigInteger& operator++()&;
    BigInteger& operator--()&;
    BigInteger operator++(int)&;
    BigInteger operator--(int)&;
    BigInteger operator-() const;


    BigInteger& operator+=(const BigInteger& another)&;
    BigInteger& operator-=(const BigInteger& another)&;
    BigInteger& operator*=(const BigInteger& another)&;
    BigInteger& operator/=(const BigInteger& another)&;
    BigInteger& operator%=(const BigInteger& another)&;

    friend BigInteger operator+(const BigInteger& first, const BigInteger& second);
    friend BigInteger operator-(const BigInteger& first, const BigInteger& second);
    friend BigInteger operator*(const BigInteger& first, const BigInteger& second);
    friend BigInteger operator/(const BigInteger& first, const BigInteger& second);
    friend BigInteger operator%(const BigInteger& first, const BigInteger& second);

    friend bool operator<(const BigInteger& first, const BigInteger& second);
    friend bool operator==(const BigInteger& first, const BigInteger& second) {
        return !(first < second) && !(second < first);
    }
    friend bool operator>=(const BigInteger& first, const BigInteger& second) {
        return !(first < second);
    }
    friend bool operator>(const BigInteger& first, const BigInteger& second) {
        return second < first;
    }
    friend bool operator<=(const BigInteger& first, const BigInteger& second) {
        return !(second < first);
    }
    friend bool operator!=(const BigInteger& first, const BigInteger& second) {
        return (first < second) || (second < first);
    }

    explicit operator bool() const {
        return *this != 0;
    }

    std::string toString() const;

    friend std::ostream& operator<<(std::ostream& stream, const BigInteger& object) {
        stream << object.toString();
        return stream;
    }

    friend std::istream& operator>>(std::istream& stream, BigInteger& object);


private:
    std::vector<int> factorisation;
    bool sign = true;
    void convert_from_int(int n);

    BigInteger(const BigInteger& another, int base10_deg) {
        for (int i = 0; i < base10_deg; ++i) {
            factorisation.push_back(0);
        }
        sign = another.sign;
        factorisation.insert(factorisation.begin() + base10_deg, another.factorisation.begin(),
                another.factorisation.end());
    }

    BigInteger(const BigInteger& another, int from, int to) {
        if (from >= to || to > static_cast<int>(another.factorisation.size())) {
            factorisation.assign(1,0);
        }

        auto start_it = another.factorisation.begin() + from;
        auto end_it = another.factorisation.begin() + to;
        factorisation.insert(factorisation.begin(), start_it, end_it);
    }

    friend bool abs_less(const BigInteger& first, const BigInteger& second);
    friend bool abs_equal(const BigInteger& first, const BigInteger& second) {
        return !(abs_less(first, second)) && !(abs_less(second, first));
    }
    BigInteger& unsigned_add(const BigInteger& another);
    bool unsigned_subtract(const BigInteger &another);
    BigInteger divide(const BigInteger& right);
    friend BigInteger karatsuba_multiplication(const BigInteger& first, const BigInteger& second);

    void shrink_to_fit() {
        for (size_t i = factorisation.size() - 1; i > 0 && factorisation[i] == 0; --i) {
            factorisation.erase(factorisation.end() - 1);
        }
    }

    BigInteger fix_length_for_multipl(BigInteger another) {
        for (int i = 0; i < static_cast<int>(factorisation.size())
                        || i < static_cast<int>(another.factorisation.size()); ++i) {
            if (i >= static_cast<int>(factorisation.size())) {
                factorisation.push_back(0);
            }
            else if (i >= static_cast<int>(another.factorisation.size())) {
                another.factorisation.push_back(0);
            }
        }
        return another;
    }
};

void BigInteger::convert_from_int(int n) {
    if (n < 0) {
        sign = false;
        n = -n;
    }
    factorisation.assign(1, n % base10);

    n /= base10;
    while (n != 0) {
        factorisation.push_back(n % base10);
        n /= base10;
    }
}

BigInteger::BigInteger(const std::string& tmp) {
    if (tmp.length() == 0) {
        factorisation.assign(1, 0);
        sign = true;
        return;
    }
    int base_length = static_cast<int>(std::to_string(base10).length()) - 1;

    int valid_length = static_cast<int>(tmp.length());
    if (tmp[0] == '-') {
        sign = false;
        --valid_length;
    }
    size_t reminder = (valid_length % base_length > 0 ? 1 : 0);
    factorisation.assign(valid_length / base_length + reminder, 0);

    for (size_t i = 0; i < factorisation.size() - reminder; ++i) {
        std::string t = tmp.substr(tmp.length() - base_length * (i + 1), base_length);
        factorisation[i] = stoi(t);
    }
    if (reminder > 0) {
        int first_pos = (sign ? 0 : 1);
        factorisation[factorisation.size() - 1] =
                stoi(tmp.substr(first_pos, valid_length % base_length));
    }
    if (factorisation.size() == 1 && factorisation[0] == 0) sign = true;
    shrink_to_fit();
}

std::istream& operator>>(std::istream& stream, BigInteger& newBig) {
    std::string tmp;
    stream >> tmp;
    newBig = BigInteger(tmp);
    return stream;
}

BigInteger& BigInteger::operator++()& {
    *this += 1;
    return *this;
}

BigInteger& BigInteger::operator--()& {
    *this -= 1;
    return *this;
}

BigInteger BigInteger::operator++(int)& {
    BigInteger prev(*this);
    *this += 1;
    return prev;
}

BigInteger BigInteger::operator--(int)&{
    BigInteger prev(*this);
    *this -= 1;
    return prev;
}

BigInteger BigInteger::operator-() const {
    BigInteger copy(*this);
    copy.sign = !copy.sign;
    if (factorisation[0] == 0 && factorisation.size() == 1) {
        copy.sign = true;
    }
    return copy;
}

BigInteger& BigInteger::unsigned_add(const BigInteger &another) {
    int reminder = 0;
    ssize_t another_len = another.factorisation.size();
    ssize_t cur_len = factorisation.size();

    for (int i = 0; i < cur_len || i < another_len; ++i) {
        int tmp = reminder;
        if (i < another_len) tmp += another.factorisation[i];
        if (i < cur_len) {
            tmp += factorisation[i];
            factorisation[i] = tmp % base10;
        }
        else {
            factorisation.push_back(tmp % base10);
        }
        reminder = tmp / base10;
    }
    if (reminder > 0) {
        factorisation.push_back(reminder);
    }
    shrink_to_fit();
    return *this;
}

BigInteger& BigInteger::operator+=(const BigInteger& another)& {
    if (sign && !another.sign) {
        if (abs_less(*this, another)) sign = false;
        unsigned_subtract(another);
    }
    else if (!sign && another.sign) {
        if (abs_less(*this, another)) sign = true;
        unsigned_subtract(another);
    }
    else {
        *this = unsigned_add(another);
    }

    if (abs_equal(*this, 0)) {
        sign = true;
    }
    return *this;
}

bool BigInteger::unsigned_subtract(const BigInteger &another) {
    int reminder = 0;
    ssize_t another_len = another.factorisation.size();
    ssize_t cur_len = factorisation.size();

    bool right_order = true;
    if (abs_less(*this, another)) {
        right_order = false;
    }

    for (int i = 0; i < cur_len || i < another_len; ++i) {
        int tmp = base10 - reminder;
        if (i < another_len) {
            if (right_order) tmp -= another.factorisation[i];
            else tmp += another.factorisation[i];
        }
        if (i < cur_len) {
            if (right_order) tmp += factorisation[i];
            else tmp -= factorisation[i];
            factorisation[i] = tmp % base10;
        }
        else {
            factorisation.push_back(tmp % base10);
        }
        reminder = tmp < base10 ? 1 : 0;
    }
    if (reminder > 0) {
        factorisation.push_back(base10 - reminder);
    }

    shrink_to_fit();
    return right_order;
}

BigInteger& BigInteger::operator-=(const BigInteger& another)& {
    if (sign && !another.sign) {
        *this = unsigned_add(another);
        sign = true;
    }
    else if (!sign && another.sign) {
        *this = unsigned_add(another);
        sign = false;
    }
    else {
        if (sign && *this < another) {
            sign = false;
        } else if (!sign && *this > another) {
            sign = true;
        }

        unsigned_subtract(another);
    }

    if (abs_equal(*this, 0)) {
        sign = true;
    }
    return *this;
}

//(a0+c*a1)*(b0 + c*b1) = a0*b0 + c*((a1 + a0) * (b1 + b0) - a0*b0 - a1*b1) + c*c*(a1*b1), where c = base10^(n/2)
BigInteger karatsuba_multiplication(const BigInteger& first, const BigInteger& second) {
    int n = first.factorisation.size();
    BigInteger result;
    result.sign = true;
    if (n / 2 == 0) {
        result = first.factorisation[0] * second.factorisation[0];
        return result;
    }
    BigInteger a0b0, a1b1, a0a1b0b1, a0a1, b0b1;
    BigInteger a0 = BigInteger(first, 0, n / 2);
    BigInteger b0 = BigInteger(second, 0, n / 2);
    BigInteger a1 = BigInteger(first, n / 2, n);
    BigInteger b1 = BigInteger(second, n / 2, n);
    a0.sign = a1.sign = b0.sign = b1.sign = true;
    a0b0 = karatsuba_multiplication(a0, b0);
    a1b1 = karatsuba_multiplication(a1, b1);
    a0a1 = a0 + a1;
    b0b1 = b0 + b1;
    b0b1 = a0a1.fix_length_for_multipl(b0b1);
    a0a1b0b1 = karatsuba_multiplication(a0a1, b0b1);
    result = a0b0;
    result.shrink_to_fit();
    BigInteger tmp = BigInteger(a0a1b0b1 - a0b0 - a1b1, n / 2);
    tmp.shrink_to_fit();
    result += tmp;
    tmp = BigInteger(a1b1, n - (n % 2));
    tmp.shrink_to_fit();
    result += tmp;
    result.shrink_to_fit();
    return result;
}

BigInteger& BigInteger::operator*=(const BigInteger& another)& {
    BigInteger another_copy(another);
    bool true_sign = (sign && another_copy.sign) || ((!sign) && (!another_copy.sign));
    if (abs_equal(another, 1)) {
        sign = true_sign;
        return *this;
    }
    for (int i = 0; i < static_cast<int>(factorisation.size())
                    || i < static_cast<int>(another_copy.factorisation.size()); ++i) {
        if (i >= static_cast<int>(factorisation.size())) {
            factorisation.push_back(0);
        }
        else if (i >= static_cast<int>(another_copy.factorisation.size())) {
            another_copy.factorisation.push_back(0);
        }
    }
    *this = karatsuba_multiplication(*this, another_copy);
    sign = true_sign;
    return *this;
}

BigInteger BigInteger::divide(const BigInteger &right) {
    bool prev_sign = sign;
    sign = true;
    if (abs_equal(right, 1)) {
        BigInteger copy(*this);
        copy.sign = (prev_sign && right.sign) || ((!prev_sign) && (!right.sign));
        *this = 0;
        return copy;
    }
    if (*this == right) {
        return BigInteger(1);
    }
    BigInteger zero(0);
    if (abs_less(*this, right)) {
        return zero;
    }
    BigInteger result(0);
    factorisation.push_back(0);
    int n = static_cast<int>(factorisation.size()) - static_cast<int>(right.factorisation.size());
    result.factorisation.assign(n, 0);
    for (int i = static_cast<int>(factorisation.size() - right.factorisation.size()) - 1; i >= 0; --i) {
        BigInteger tmp(*this, i, i + static_cast<int>(right.factorisation.size() + 1));
        BigInteger prev = tmp;
        for (size_t j = i; j < i + right.factorisation.size() + 1; ++j) {
            factorisation[j] = 0;
        }
        tmp.shrink_to_fit();
        int counter = 0;
        while (tmp.unsigned_subtract(right)) {
            ++counter;
            tmp.shrink_to_fit();
            prev = tmp;
        }
        result.factorisation[i] = counter;
        *this += BigInteger(prev, i);
    }
    shrink_to_fit();
    sign = prev_sign;
    result.sign = (prev_sign && right.sign) || ((!prev_sign) && (!right.sign));
    result.shrink_to_fit();
    return result;
}

BigInteger& BigInteger::operator/=(const BigInteger& another)& {
    *this = divide(another);
    return *this;
}

BigInteger& BigInteger::operator%=(const BigInteger& another)& {
    *this -= (*this / another) * another;
    return *this;
}

BigInteger operator+(const BigInteger& first, const BigInteger& second) {
    BigInteger result(first);
    result += second;
    return result;
};

BigInteger operator-(const BigInteger& first, const BigInteger& second) {
    BigInteger result(first);
    result -= second;
    return result;
}

BigInteger operator*(const BigInteger& first, const BigInteger& second) {
    BigInteger result(first);
    result *= second;
    return result;
}

BigInteger operator/(const BigInteger& first, const BigInteger& second) {
    BigInteger result(first);
    result /= second;
    return result;
}

BigInteger operator%(const BigInteger& first, const BigInteger& second) {
    BigInteger result(first);
    result %= second;
    return result;
}

bool abs_less(const BigInteger &first, const BigInteger &second) {
    if (first.factorisation.size() > second.factorisation.size()) {
        return false;
    }
    else if (first.factorisation.size() < second.factorisation.size()) {
        return true;
    }
    else {
        for (int i = static_cast<int>(first.factorisation.size() - 1); i >= 0; --i) {
            if (first.factorisation[i] < second.factorisation[i]) {
                return true;
            }
            else if (first.factorisation[i] > second.factorisation[i]) {
                return false;
            }
        }
    }
    return false;
}

bool operator<(const BigInteger& first, const BigInteger& second) {
    if (first.sign && !second.sign) {
        return false;
    }
    else if (second.sign && !first.sign) {
        return true;
    }
    else {
        bool abs = abs_less(first, second);
        if (!first.sign) {
            abs = !abs;
        }
        return abs;
    }
}

std::string BigInteger::toString() const {
    std::string result, tmp;
    if (!sign) {
        result += '-';
    }
    for (int i = static_cast<int>(factorisation.size() - 1); i >= 0; --i) {
        tmp = std::to_string(factorisation[i]);
        for (int j = static_cast<int>(std::to_string(base10).length() - 1 - tmp.length());
             i != static_cast<int>(factorisation.size() - 1) && j > 0; --j) {

            result += '0';
        }
        result += tmp;
    }
    return result;
}
