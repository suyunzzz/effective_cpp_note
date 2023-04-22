#include "../common.h"


class B{
public:
    virtual void f() const;
};
class D:public B{
public:
    virtual void f();
};

void B::f() const{
    std::cout << "B::f()\n";
}

void D::f(){
    std::cout << "D::f()\n";
}
int main() {
    D d;
    d.f();  // D::f() 
    
    B* pb = new D;
    // 因为f()并不具有多态性，所以还是调用的基类的函数
    pb->f();        // B::f()

    return 0;
}