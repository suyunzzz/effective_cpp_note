# Tips 33: Avoiding hiding inherited names

>  本文将介绍C++名称隐藏相关的问题

[TOC]

## 何为名称隐藏

先看一个例子

```cpp
int x;
void func(){
    double x;
    cin>>x;     // read a new value for local x
}
```

可以看到`double x`隐藏了`int x`，因为`C++`的名称隐藏规则（`name-hiding rules`）隐藏的是名称，和类型无关！



## public继承中的名称隐藏

public的含义就是`is-a`的关系，因此当我们继承一个类的时候，我们期望继承类中的所有方法，但是 ***名称隐藏*** 阻止了这种期望的行为（因为他想要避免当发生public继承时，派生类从遥远的基类中继承函数）

### 隐藏父类中的名称

子类中重写（override）与父类方法同名的方法， ***将会隐藏父类中所有同名的方法*** (注意是所有的同名方法)。例如：


```cpp
#include "../common.h"

using namespace std;
class Base{
public:
    virtual void func()=0;
    void func(int);
};
class Derived: public Base{
public:
    virtual void func();
};


int main(){
    Derived d;
    d.func(1);      // Error!
    d.func();       // Ok
    return 0;
}

```

为了解决这个问题，我们使用`using `

```cpp
#include "../common.h"

using namespace std;
class Base
{
public:
    virtual void func() = 0;
    void func(int a)
    {
        printf("Base::func(%d)\n", a);
    }
};
class Derived : public Base
{
public:
    using Base::func; // 让Base中的func在Drived的作用域中都可见（并且为public）
    virtual void func() override
    {
        printf("Drived::func()\n");
    }
};

int main()
{
    Derived d;
    d.func(1); // OK，调用的是Base::func(1);
    d.func();  // Ok
    return 0;
}

// output
Base::func(1)
Drived::func()
```

 **现在，继承机制将一如既往地工作**

## privete继承中的名称隐藏

### private继承

private继承后base类中的 ***所有对象、方法均变为private*** ，其本质是 ***根据某物实现出***


```cpp
#include "../common.h"

using namespace std;
class Base
{
public:
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

// output:
Base::func(1)
```

### 仅继承一个基类的方法

有时候我们并不想继承`Base`中的所有函数，这是可以理解的，但是这在`public`继承中是不被允许的（因为`public`继承就是一个`is-a`的关系，因此`Base`有的`Drived`必须也有，这也是为什么上述的`using`放在了`Drived`的`public`中，因为`func`在`Base`中是`public`，因此在`Drived`也必须是`public`的）。但是这在private继承中是有意义的，有时候我们仅仅需要继承重载函数中的其中一个版本的函数，如下：

```cpp
// 我们仅仅想继承Base的func()函数，如果使用using的话，那么base的所有同名函数都会在drived中可见，所以采用了 **转交函数** 
#include "../common.h"

using namespace std;
class Base
{
public:
    virtual void func() = 0;
    virtual void func(int a)
    {
        printf("Base::func(%d)\n", a);
    }
};

// note: 纯虚函数一般情况下是不能定义的，仅仅作为接口使用，这里为了更好理解，故给出定义
void Base::func()      
{
    printf("Base::func()\n");
}
class Derived : private Base
{
public:
    virtual void func() // 转发函数，在声明处定义，inline
    {
        Base::func();
    }
};

int main()
{
    Derived d;
    d.func(); // Ok
    return 0;
}

// output
Base::func()
```

## summary
1. 子类的名称会隐藏父类中同名的属性（或方法），`public`表示的是`is-a`的关系，应该避免出现这种情况，可以使用`using`来保留基类的同名属性（或方法）

2. 对于`private`继承的派生类，使用 ***转发函数*** 可以说使父类的方法或者成员再现（转发时注意利用`inline`的特点--减少函数调用）

## reference

1. https://harttle.land/2015/08/31/effective-cpp-33.html


