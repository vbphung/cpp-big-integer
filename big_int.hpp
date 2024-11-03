#ifndef BIG_INT_HPP
#define BIG_INT_HPP

#include <algorithm>
#include <cstring>
#include <iostream>
using namespace std;

class BigInt {
private:
  string value;
  bool sign;

  void trim();

public:
  BigInt();
  BigInt(int n);
  BigInt(float n);
  BigInt(string s);
  BigInt abs() const;
  BigInt operator+(const BigInt &number) const;
  BigInt operator-(const BigInt &number) const;
  BigInt operator-() const;
  BigInt operator*(const BigInt &number) const;
  BigInt operator/(const BigInt &number) const;
  bool operator>(const BigInt &n) const;
  bool operator<(const BigInt &n) const;
  bool operator>=(const BigInt &n) const;
  bool operator<=(const BigInt &n) const;
  bool operator==(const BigInt &n) const;
  bool operator!=(const BigInt &n) const;
  void operator+=(const BigInt &n);
  void operator-=(const BigInt &n);
  void operator*=(const BigInt &n);
  void operator/=(const BigInt &n);
  void operator++();
  void operator--();
  friend istream &operator>>(istream &fin, BigInt &n);
  friend ostream &operator<<(ostream &fout, const BigInt &n);
};

BigInt::BigInt() : value(""), sign(true) {}

BigInt::BigInt(int n) {
  sign = true;
  if (n < 0) {
    sign = false;
    n = -n;
  }
  while (n) {
    value += char(n % 10 + '0');
    n /= 10;
  }
}

BigInt::BigInt(float n) { BigInt(int(n)); }

BigInt::BigInt(string s) {
  sign = s[0] != '-';
  value = s.substr(sign ? 0 : 1);
  reverse(value.begin(), value.end());
  trim();
}

BigInt BigInt::abs() const {
  BigInt n = *this;
  n.sign = true;
  return n;
}

BigInt BigInt::operator+(const BigInt &number) const {
  if (sign == number.sign) {
    BigInt n = number;
    int current;
    for (int i = 0, carry = 0;
         i < max(value.length(), n.value.length()) || carry; ++i) {
      if (i == n.value.size())
        n.value += '0';
      current = int(n.value[i] - '0') + carry;
      if (i < value.length())
        current += int(value[i] - '0');
      if (current < 10) {
        n.value[i] = char(current + '0');
        carry = 0;
      } else {
        n.value[i] = char(current - 10 + '0');
        carry = 1;
      }
    }
    return n;
  }
  return *this - (-number);
}

BigInt BigInt::operator-(const BigInt &number) const {
  if (sign == number.sign) {
    if (abs() == number.abs()) {
      return 0;
    }

    if (abs() > number.abs()) {
      BigInt n = *this;
      int current;
      for (int i = 0, carry = 0; i < number.value.length() || carry; ++i) {
        current = int(n.value[i] - '0') - carry;
        if (i < number.value.length())
          current -= int(number.value[i] - '0');
        if (current >= 0) {
          n.value[i] = char(current + '0');
          carry = 0;
        } else {
          n.value[i] = char(current + 10 + '0');
          carry = 1;
        }
      }
      n.trim();
      return n;
    }
    return -(number - *this);
  }
  return *this + (-number);
}

BigInt BigInt::operator-() const {
  BigInt n = *this;
  n.sign = !n.sign;
  return n;
}

BigInt BigInt::operator*(const BigInt &number) const {
  BigInt n = 0;
  for (int i = 0; i < number.value.length(); ++i) {
    BigInt current;
    int c;
    for (int j = 0, carry = 0; j < value.length() || carry; ++j) {
      c = carry;
      if (j < value.length())
        c += int(number.value[i] - '0') * int(value[j] - '0');
      if (c < 10) {
        current.value += char(c + '0');
        carry = 0;
      } else {
        current.value += char(c % 10 + '0');
        carry = c / 10;
      }
    }
    for (int j = 0; j < i; ++j)
      current.value = '0' + current.value;
    n += current;
  }
  n.sign = sign == number.sign;
  return n;
}

BigInt BigInt::operator/(const BigInt &number) const {
  if (number == 0)
    throw invalid_argument("Division by zero");
  BigInt divisor = number.abs(), tmp = abs(), n = 0;
  while (tmp >= divisor) {
    BigInt dividend, remainder;
    for (int i = tmp.value.length() - divisor.value.length();
         i < tmp.value.length(); ++i)
      dividend.value += tmp.value[i];
    if (dividend < divisor)
      dividend.value =
          tmp.value[tmp.value.length() - divisor.value.length() - 1] +
          dividend.value;
    for (int i = 1; i <= 9; ++i)
      if (divisor * (i + 1) > dividend) {
        n.value = char(i + '0') + n.value;
        remainder = divisor * i;
        for (int j = 0; j < tmp.value.length() - divisor.value.length(); ++j)
          remainder.value = '0' + remainder.value;
        tmp -= remainder;
        tmp.trim();
        break;
      }
  }
  n.sign = sign == number.sign;
  return n;
}

bool BigInt::operator>(const BigInt &n) const {
  if (sign && !n.sign)
    return true;
  if (!sign && n.sign)
    return false;
  if (value.length() > n.value.length())
    return sign;
  if (value.length() < n.value.length())
    return !sign;
  for (int i = value.length() - 1; i >= 0; --i) {
    if (value[i] > n.value[i])
      return sign;
    else if (value[i] < n.value[i])
      return !sign;
  }
  return !sign;
}

bool BigInt::operator<(const BigInt &n) const {
  return !(*this > n) && !(*this == n);
}

bool BigInt::operator>=(const BigInt &n) const { return !(*this < n); }

bool BigInt::operator<=(const BigInt &n) const { return !(*this > n); }

bool BigInt::operator==(const BigInt &n) const {
  if (sign != n.sign)
    return false;
  return value.compare(n.value) == 0;
}

bool BigInt::operator!=(const BigInt &n) const { return !(*this == n); }

void BigInt::operator+=(const BigInt &n) { *this = *this + n; }

void BigInt::operator-=(const BigInt &n) { *this = *this - n; }

void BigInt::operator*=(const BigInt &n) { *this = *this * n; }

void BigInt::operator/=(const BigInt &n) { *this = *this / n; }

void BigInt::operator++() { *this += 1; }

void BigInt::operator--() { *this -= 1; }

istream &operator>>(istream &fin, BigInt &n) {
  string s;
  fin >> s;
  n = s;
  return fin;
}

ostream &operator<<(ostream &fout, const BigInt &n) {
  if (n.value.length() == 0) {
    fout << 0;
    return fout;
  }
  if (!n.sign)
    fout << '-';
  for (int i = n.value.length() - 1; i >= 0; --i)
    fout << n.value[i];
  return fout;
}

void BigInt::trim() {
  while (!value.empty() && value[value.length() - 1] == '0')
    value.pop_back();
  if (value.length() == 0)
    sign = true;
}

#endif
