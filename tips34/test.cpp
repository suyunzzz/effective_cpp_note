#include "../common.h"

using namespace std;

class Airplane
{
public:
    virtual void fly() = 0;
};

void Airplane::fly()
{ // 纯虚函数的实现
    printf("default_fly: just for A or B\n");
}

class A : public Airplane
{
public:
    virtual void fly() override     // 重写
    {
        Airplane::fly();
    }
};

class B : public Airplane
{
public:
    virtual void fly() override
    {
        Airplane::fly();
    }
};

class C : public Airplane
{
    // 不给出
};

void FlyToSomewhere(Airplane &airplane)
{
    airplane.fly();
}

int main()
{
    Airplane *a = new A();
    printf("\n===airplane a fly...\n");
    FlyToSomewhere(*a);
    a->Airplane::fly();
    Airplane *b = new B();
    printf("\n===airplane b fly...\n");
    FlyToSomewhere(*b);
    // Airplane *c = new C(); // error, C is a abtract Class
    // printf("\n===airplane c fly...\n");
    // FlyToSomewhere(*c);

    return 0;
}