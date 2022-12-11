#include "../common.h"

using namespace std;

class B
{

public:
    virtual void mf()
    {
        printf("B:mf()\n");
    }

public:
    void mf2() const
    {
        printf("B:mf2()\n");
    }
};

class D : public B
{
public:
    virtual void mf() override
    {
        printf("D:mf()\n");
    }

public:
    void mf2() const
    { // 禁止重新定义继承来的non-virtual函数
        printf("D:mf2()\n");
    }
};

int main()
{

    // 多态性
    // 因为virtual函数是动态绑定的，因此会呈现出多态性
    B b;
    D d;
    B *pb1 = &b;
    B *pb2 = &d;
    pb1->mf(); // B:mf()
    pb2->mf(); // D:mf()

    cout << "====\n";

    // non-virtual函数是静态绑定的，会根据指针的类型确定调用哪个函数
    pb1->mf2(); // B:mf2()
    pb2->mf2(); // B:mf2()
    cout << "====\n";

    // 即使是使用两个不同的指针指向同一个对象，也会根据指针的类型选择对应的函数进行调用
    D *pd1 = &d;
    pb2->mf2(); // B:mf2()
    pd1->mf2(); // D:mf2()
    cout << "====\n";

    // 直接使用对象调用，ok
    b.mf2(); // B:mf2()
    d.mf2(); // D:mf2()

    return 0;
}