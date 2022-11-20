#include "data.h"

class PersonImpl
{
public:
    PersonImpl(const Date &data, const std::string &name) : _name(name), _data(data)
    {
        std::cout << "PersonImpl constructor\n";
    }
    std::string DebugString() const
    {
        return _name + "\t" + _data.DebugString() + "\n";
    }

private:
    Date _data;
    std::string _name;
};