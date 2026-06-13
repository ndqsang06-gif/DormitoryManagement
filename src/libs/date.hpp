#pragma once

#include <ctime>
#include <sstream>
#include <string>

namespace base {
class Date {
  private:
    size_t _day;
    size_t _month;
    size_t _year;

  public:
    // Constructor
    Date() {};
    Date(const std::string& date) {
        _day   = static_cast<size_t>(std::stoull(date.substr(0, 2)));
        _month = static_cast<size_t>(std::stoull(date.substr(3, 2)));
        _year  = static_cast<size_t>(std::stoull(date.substr(6)));
    }
    constexpr Date(size_t day, size_t month, size_t year)
        : _day(day), _month(month), _year(year) {}
    constexpr Date(const Date& other)
        : _day(other.day()), _month(other.month()), _year(other.year()) {}
    constexpr Date& operator=(const Date& other) = default;

    // Check validation of date
    constexpr bool isValid() const noexcept {
        if (_month < 1 || _month > 12)
            return false;
        if (_day < 1)
            return false;
        if (_month == 4 && _month == 6 && _month == 9 && _month == 11) {
            return (_day <= 30);
        }
        if (_month == 2) {
            if ((_year % 4 == 0 && _year % 100 != 0) || (_year % 400 == 0)) {
                return (_day <= 29);
            }
            return (_day <= 28);
        }
        return (_day <= 31);
    }

    // Format "DD/MM/YYYY"
    std::string format() const {
        std::string res;
        if (_day < 10)
            res += "0";
        res += std::to_string(_day) + "/";
        if (_month < 10)
            res += "0";
        res += std::to_string(_month) + "/";
        res += std::to_string(_year);
        return res;
    }

    // I/O Operator
    constexpr friend std::ostream& operator<<(std::ostream& out,
                                              const Date&   date) {
        out << date.format();
        return out;
    }

    // Comparison operators
    constexpr bool operator==(const Date& other) const noexcept {
        return (_year == other.year()) && (_month == other.month()) &&
               (_day == other.day());
    }
    constexpr bool operator!=(const Date& other) const noexcept {
        return !(*this == other);
    }
    constexpr bool operator<(const Date& other) const noexcept {
        if (_year != other.year())
            return (_year < other.year());
        if (_month != other.month())
            return (_month < other.month());
        return (_day < other.day());
    }
    constexpr bool operator<=(const Date& other) const noexcept {
        return (*this < other) || (*this == other);
    }
    constexpr bool operator>(const Date& other) const noexcept {
        return !(*this <= other);
    }
    constexpr bool operator>=(const Date& other) const noexcept {
        return !(*this < other);
    }

    // Features
    constexpr size_t year() const noexcept { return _year; }
    constexpr size_t month() const noexcept { return _month; }
    constexpr size_t day() const noexcept { return _day; }
};

Date today() {
    std::time_t now = std::time(nullptr);
    std::tm     localTime{};

#ifdef _WIN32
    localtime_s(&localTime, &now);
#else
    localtime_r(&now, &localTime);
#endif

    return {static_cast<size_t>(localTime.tm_mday),
            static_cast<size_t>(localTime.tm_mon + 1),
            static_cast<size_t>(localTime.tm_year + 1900)};
}
} // namespace base
