#include "person.h"
#include "data.h"

int main()
{
    Person p({1997, 12, 19}, "syz");
    std::cout << p << "\n";
    return 0;
}
