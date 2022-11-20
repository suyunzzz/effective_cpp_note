#pragma once
#include <string>
#include <iostream>
#include <memory>
#include "personimpl.h"
class Person
{
public:
    Person(const Date &birthday, const std::string &name) : _person_impl(std::make_shared<PersonImpl>(birthday, name))
    {
        std::cout << "Person constructor\n";
    }
    friend std::ostream &operator<<(std::ostream &osm, const Person &person);

private:
    std::shared_ptr<PersonImpl> _person_impl{nullptr}; // only need allocate a heap memory for _person_impl
};
