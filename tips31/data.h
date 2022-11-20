#pragma once
#include <iostream>

class Date
{
public:
    Date(int a, int b, int c) : year(a), month(b), day(c)
    {
        std::cout << "Date constructor\n";
    }
    std::string DebugString() const
    {
        return std::to_string(year) + std::string("-") + std::to_string(month) + "-" + std::to_string(day) + "\n";
    }

private:
    int year;
    int month;
    int day;
};