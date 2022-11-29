# Tips 34: inheritance-of-interface_and_inheritance-of-implementation

>  本文将介绍C++接口继承和实现继承的区别



[TOC]


先看一个例子

```cpp
class Shape{
public:
    // 纯虚函数
    virtual void draw() const = 0;
    // 不纯的虚函数，impure...
    virtual void error(const string& msg);
    // 普通函数
    int id() const;
};
class Rect: public Shape{...};
class Ellipse: public Shape{...};
```
在这个例子中，有三种函数

1. 纯虚函数，只继承接口，不继承实现

2. 虚函数，继承接口，且继承缺省实现

3. 非虚函数，继承全部，不允许重写

## 虚函数的缺省实现

### 危险的缺省实现

看下面这个例子
```cpp
#include "../common.h"

using namespace std;

class Airplane
{
public:
    virtual void fly()
    {
        printf("just for A or B\n");
    }
};

class A : public Airplane
{
};

class B : public Airplane
{
};

class C : public Airplane
{
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
    Airplane *b = new B();
    printf("\n===airplane b fly...\n");
    FlyToSomewhere(*b);
    Airplane *c = new C();
    printf("\n===airplane c fly...\n");
    FlyToSomewhere(*c);

    return 0;
}
```
在例子中给出了虚函数的缺省实现，但是对于class C，这种飞行方式不适合，但是我们在定义C class的时候忘记了override虚函数，这就导致了危险的行为：
```cpp
===airplane a fly...
just for A or B

===airplane b fly...
just for A or B

===airplane c fly...
just for A or B
```

### 安全的缺省实现
对于上述的问题，为了避免意外的默认继承，我们有两种方式可以避免：

1. 定义一个default_fly函数，将虚函数的接口和定义分开（即将虚函数的接口部分使用纯虚函数代替，定义部分使用default_fly()，最后将其定义为`inline`）

2. 将fly()定义为纯虚函数，继承时必须给出override，否则不能编译（因为还是一个抽象类）

上述两种方式的目的都是通过 ***解耦接口继承和实现继承*** 来达到“安全继承”的目的

#### 方法1

> 改为一个纯虚函数，再定义一个default_fly函数，将虚函数的接口和定义分开

```cpp
#include "../common.h"

using namespace std;

class Airplane
{
public:
    virtual void fly() = 0;

protected:
    void default_fly()
    {
        printf("default_fly: just for A or B\n");
    }
};

class A : public Airplane
{
public:
    virtual void fly()
    {
        // 以下两种方式均可，因为protected可以被派生类访问
        Airplane::default_fly(); // 非虚函数，被完全继承
        // default_fly();      // 非虚函数，被完全继承
    }
};

class B : public Airplane
{
public:
    virtual void fly()
    {
        default_fly(); // 非虚函数，被完全继承
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
    Airplane *b = new B();
    printf("\n===airplane b fly...\n");
    FlyToSomewhere(*b);
    Airplane *c = new C(); // error, C is a abtract Class
    printf("\n===airplane c fly...\n");
    FlyToSomewhere(*c);

    return 0;
}

// output
===airplane a fly...
default_fly: just for A or B

===airplane b fly...
default_fly: just for A or B
```



#### 方法2

上面这种方式可能会造成名字的混淆(fly, default_fly)，因此我们提供一个更优雅的方式： ***给纯虚函数提供实现***

这样， ***接口存在于纯虚函数的声明中，实现存在于纯虚函数的定义中***

```cpp
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
    Airplane *b = new B();
    printf("\n===airplane b fly...\n");
    FlyToSomewhere(*b);
    // Airplane *c = new C(); // error, C is a abtract Class
    // printf("\n===airplane c fly...\n");
    // FlyToSomewhere(*c);

    return 0;
}

// output:
===airplane a fly...
default_fly: just for A or B

===airplane b fly...
default_fly: just for A or B
```

但是这种合并是有副作用的，本来为protected的default_fly()，现在在类的外部也可以访问：

```cpp
Airplane *a = new A();
a->Airplane::fly();

// output:
default_fly: just for A or B
```

## summary
1. 接口继承和实现继承不同，在public继承下，派生类总是继承基类的接口

2. prue virtual函数 ***只能继承接口***

3. imprue virtual函数（非纯虚函数） ***继承接口+默认实现***

4. non-virtual函数 ***继承接口+强制性的实现***

## reference

1. https://harttle.land/2015/09/01/effective-cpp-34.html
