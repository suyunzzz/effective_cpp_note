#include "../common.h"

using namespace std;
class Base
{
public:
    // virtual void func() = 0;
    virtual void func(int a)
    {
        printf("Base::func(%d)\n", a);
    }
};

class Drived1 : private Base
{
public:
    void log()
    {
        func(1);
    }
};

int main()
{
    Drived1 d;
    // d.func(1); // ERROR, func in drived1 class is privete!
    d.log();
    return 0;
}