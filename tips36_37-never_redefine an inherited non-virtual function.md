# Tips 36: 不要重新定义继承而来的non-virtual函数

>  本文将介绍C++静态绑定和动态绑定相关的问题，1. 不要重新定义继承来的non-virtual函数, 2. 不要重新定义继承而来的缺省参数值

[TOC]

## 静态类型和动态类型
- 对象的所谓静态类型，就是在程序中被声明时的类型；
- 对象的动态类型是“目前所指对象的类型”，也就是说动态类型可以表现出一个对象将会有什么行为。动态类型可以在程序执行过程中改变（通常是通过赋值动作）。virtual函数系动态对象绑定而来，意思是调用一个virtual函数时，究竟调用哪一份函数实现代码取决于发出调用的那个对象的动态类型。

## 不要定义继承而来的的non-virtual函数

先看一个例子

```cpp
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
```

解释一下，上面例子中因为non-virtual函数是静态绑定，不具备多态的性质，因此若对其进行重写，那么在使用指针或者引用（妄想获得多态）时会根据指针或者引用的`静态类型`选择合适的函数调用，而不会像virtual函数那样表现出多态性。也就是在上例中，通过`pb1`, `pb2`对`mf2()`进行调用的时候，仍然调用的是`B::mf2()`。

## 不要重新定义继承而来的缺省参数值
> 本条款讨论继承一个带有缺省参数值的virtual函数

> 为什么会有本条款呢？因为virtual函数是动态绑定，而缺省参数值是 ***静态绑定*** 

先看一个例子：

```cpp
#include "../common.h"

using namespace std;

enum class Color
{
    RED,
    GREEN,
    NUM
};

std::map<Color, std::string> color2string = {{Color::RED, "Red"}, {Color::GREEN, "Green"}};

class B
{

public:
    virtual void draw(Color color = Color::RED)
    {
        printf("draw(%s)\n", color2string.at(color).c_str());
    }
};

class D : public B
{
public:
    virtual void draw(Color color = Color::GREEN) override // 重新定义了从基类继承来的缺省参数值
    {
        printf("draw(%s)\n", color2string.at(color).c_str());
    }
};

class D2 : public B
{
public:
    // 注意，若该函数使用对象进行调用，必须指定参数值，因为静态绑定不会从B class继承缺省参数值；若以动态绑定的方式调用，则可以继承，无需指定默认参数
    virtual void draw(Color color) override
    {
        printf("draw(%s)\n", color2string.at(color).c_str());
    }
};

int main()
{
    B b;
    D d;
    D2 d1;
    B *pb1 = &b;
    B *pb2 = &d;
    B *pb3 = &d1;

    // 通过对象调用，ok
    b.draw();              // RED
    d.draw();              // GREEN
    d1.draw(Color::GREEN); // 通过对象调用，必须指定参数值
    cout << "====\n";

    // 通过指针调用。virtual函数是动态绑定，函数的缺省参数值是静态绑定，因此会出现和我们预期不一致的行为
    pb1->draw(); // RED
    pb2->draw(); // 我们期待的是GREEN，但是却是RED
    pb3->draw(); // 通过指针调用，继承基类的缺省参数值 RED
    return 0;
}
```

**virtual函数是动态绑定，缺省参数是静态绑定**

### 解决方案

```cpp
class B
{
public:
    virtual void draw(Color color = Color::RED)
    {
        printf("draw(%s)\n", color2string.at(color).c_str());
    }
};

class D : public B
{
public:
    virtual void draw(Color color = Color::RED) override // 为了遵守规则，不对缺省参数值重新定义，但是这会造成代码重复
    {
        printf("draw(%s)\n", color2string.at(color).c_str());
    }
};
```
上述代码不光具有代码重复性的问题，而且具有 ***代码相依性质***，也就是， ***如果B中的缺省参数值变了， 那么所有给出缺省参数值的派生类都要做出对应的改变，否则就是违背了此条款***。

### 更好的解决方案

针对上述问题，我们有更好的设计方法，使用 ***NVI(non-virtual-interface)***(条款35)

```cpp
#include "../common.h"

using namespace std;

enum class Color
{
    RED,
    GREEN,
    NUM
};

std::map<Color, std::string> color2string = {{Color::RED, "Red"}, {Color::GREEN, "Green"}};

class B
{
public:
    void doDraw(Color color=Color::RED){
        draw(color);
    }
private:
    virtual void draw(Color color)
    {
        printf("draw(%s)\n", color2string.at(color).c_str());
    }
};

class D : public B
{
private:
    virtual void draw(Color color) override 
    {
        printf("draw(%s)\n", color2string.at(color).c_str());
    }
};

class D2 : public B
{
private:
    virtual void draw(Color color) override
    {
        printf("draw(%s)\n", color2string.at(color).c_str());
    }
};

int main()
{
    B b;
    D d;
    D2 d1;
    B *pb1 = &b;
    B *pb2 = &d;
    B *pb3 = &d1;

    // 通过对象调用
    b.doDraw();              // RED
    d.doDraw(Color::GREEN);              // GREEN
    d1.doDraw(Color::GREEN); // GREEN
    cout << "====\n";

    pb1->doDraw(); // RED
    pb2->doDraw(); // RED
    pb3->doDraw(); // RED
    return 0;
}
```

因为继承来的`non-virtual`函数不能重写(条款36)，保证了缺省参数永远是`RED`

## summary
1. 不要重新定义继承而来的non-virtual函数
2. 不要重新定义继承而来的缺省参数值，因为其是静态绑定，而你唯一需要重写的东西：virtual函数，是动态绑定

## reference

