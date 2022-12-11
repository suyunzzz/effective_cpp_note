# Tips 39: 明智而谨慎地使用private继承  

>  本文将介绍C++ private相关的问题, 1. private继承的特点 2. private继承的意义 3. 合理使用private继承

[TOC]

## private继承特点

`Item 32`提出public继承表示"is-a"的关系，这是因为编译器会在需要的时候将子类对象隐式转换为父类对象。 然而private继承则不然。

先看一个例子：

```cpp
class Person { ... };
class Student: private Person { ... };     // inheritance is now private
void eat(const Person& p);                 // anyone can eat

Person p;                                  // p is a Person
Student s;                                 // s is a Student
eat(p);                                    // fine, p is a Person
eat(s);                                    // error! a Student isn't a Person
```
Person可以eat，但Student却不能eat。

在看一个例子：
```cpp
#include "../common.h"

using namespace std;

class Person{
public:
    int a = 10;
    int GetNum() const {
        return a;
    }
};

class Student : private Person{

};

void eat(const Person& p){
    printf("eat\n");
}
int main(){
    Person p;
    Student s;
    printf("p.num: %d\n", p.GetNum());
    printf("p.num: %d\n", p.a);
    int s_num = s.GetNum();         // error
    printf("s.num: %d\n", s.a);     // error
    return 0;
}
```
> private继承来的派生类中所有继承来的成员变量、成员函数均变为private.

这是private继承和public继承的不同之处：

1. 编译器不会把子类对象转换为父类对象
2. 父类成员（即使是public、protected）都变成了private




## private继承的意义

***子类继承了父类的实现，而没有继承任何接口*** （因为public成员都变成private了）。 因此private继承是 ***软件实现*** 中的概念，与 ***软件设计*** 无关。 private继承和对象组合类似，都可以表示"is-implemented-in-terms-with"的关系。那么它们有什么区别呢？  ***在面向对象设计中，对象组合往往比继承提供更大的灵活性，只要可以使用对象组合就不要用`private`继承。***
    

## 合理使用

### private继承
我们的Widget类需要执行周期性任务，于是希望继承Timer的实现。 因为Widget不是一个Timer，所以我们选择了private继承：

```cpp
#include "../common.h"

using namespace std;

class Timer
{
public:
    explicit Timer(int tickFrequency) : _freq(tickFrequency)
    {
    }
    virtual ~Timer()
    {
    }
    virtual void onTick() const
    { // automatically called for each tick
        for (int i = 0; i < 10; i += _freq)
        {
            printf("clock [%d/%d]\n", i, 10);
        }
    }

private:
    int _freq;

protected:
    int _current_time = 9;
};

class Widget : private Timer
{

public:
    Widget(int n) : Timer(n) {}
    void triger()
    {
        printf("_current_time: %d\n", _current_time); // 访问base class的protectd成员
        onTick();
    }

private:
    virtual void onTick() const
    {                                 // look at Widget usage data, etc.
        printf("Widget::onTick()\n"); // 重写onTick函数
        Timer::onTick();
    }

private:
    // int _freq=3;
};

int main()
{
    Widget w(2);
    w.triger();
    return 0;
}
// output
_current_time: 9
Widget::onTick()
clock [0/10]
clock [2/10]
clock [4/10]
clock [6/10]
clock [8/10]
```

在Widget中重写虚函数onTick，使得Widget可以周期性地执行某个任务。为什么Widget要把onTick声明为private呢？ 因为onTick只是Widget的内部实现而非公共接口，我们不希望客户调用它（Item 18指出接口应设计得不易被误用）。

private继承的实现非常简单，而且有时只能使用private继承：

1. 当Widget需要访问Timer的protected成员时。因为对象组合后只能访问public成员，而private继承后可以访问protected成员。
2. 当Widget需要重写Timer的虚函数时。比如上面的例子中，由于需要重写onTick单纯的对象组合是做不到的。

### 对象复合

我们知道对象组合也可以表达"is-implemented-in-terms-of"的关系， 上面的需求当然也可以使用对象组合的方式实现。但由于需要重写（override）Timer的虚函数，所以还是需要一个继承关系的：

```cpp
#include "../common.h"

using namespace std;

class Timer
{
public:
    explicit Timer(int tickFrequency) : _freq(tickFrequency)
    {
    }
    virtual ~Timer()
    {
    }
    virtual void onTick() const
    { // automatically called for each tick
        for (int i = 0; i < 10; i += _freq)
        {
            printf("clock [%d/%d]\n", i, 10);
        }
    }

private:
    int _freq;

protected:
    int _current_time = 9;
};

/*
class Widget : private Timer
{

public:
    Widget(int n) : Timer(n) {}
    void triger()
    {
        printf("_current_time: %d\n", _current_time); // 访问base class的protectd成员
        onTick();
    }

private:
    virtual void onTick() const
    {                                 // look at Widget usage data, etc.
        printf("Widget::onTick()\n"); // 重写onTick函数
        Timer::onTick();
    }

private:
    // int _freq=3;
};
*/
class Widget
{
public:
    Widget(int n) : wTimer(n)
    {
    }

    void triger()
    {
        printf("_current_time: %d\n", wTimer._current_time); // 访问base class的protectd成员
        wTimer.onTick();
    }

private:
    class WidgetTimer : public Timer
    {
    public:
        WidgetTimer(int n) : Timer(n)
        {
        }
        int _current_time = 9; // 需要访问，将其声明为public
        virtual void onTick() const
        { // automatically called for each tick
            printf("WidgetTimer::onTick()\n");
            Timer::onTick();
        }
    };

    WidgetTimer wTimer;
};

int main()
{
    Widget w(2);
    w.triger();
    return 0;
}
// output
_current_time: 9
WidgetTimer::onTick()
clock [0/10]
clock [2/10]
clock [4/10]
clock [6/10]
clock [8/10]
```
内部类WidgetTimerpublic继承自Timer，然后在Widget中保存一个WidgetTimer对象。 这是public继承+对象组合的方式，比private继承略为复杂。但对象组合仍然拥有它的好处：
1. 你可能希望禁止Widget的子类重定义onTick。如果采用第一种方案(private继承，但仍是成员函数)，那么子类是可以重写`onTick()`函数的,不过在C++11中可以使用`final`关键字来禁止这种行为：

- 加final关键字前，可以重写

```cpp
#include "../common.h"

using namespace std;

class Timer
{
public:
    explicit Timer(int tickFrequency) : _freq(tickFrequency)
    {
    }
    virtual ~Timer()
    {
    }
    virtual void onTick() const
    { // automatically called for each tick
        for (int i = 0; i < 10; i += _freq)
        {
            printf("clock [%d/%d]\n", i, 10);
        }
    }

private:
    int _freq;

protected:
    int _current_time = 9;
};


class Widget : private Timer
{

public:
    Widget(int n) : Timer(n) {}
    void triger()
    {
        printf("_current_time: %d\n", _current_time); // 访问base class的protectd成员
        onTick();
    }

private:
    virtual void onTick() const 
    {                                 // look at Widget usage data, etc.
        printf("Widget::onTick()\n"); // 重写onTick函数
        Timer::onTick();
    }

private:
    // int _freq=3;
};

class DWidget:public Widget{
public:
    DWidget(int n):Widget(n),_freq(n){

    }
private:
    virtual void onTick() const {          
        printf("DWidget::onTick()\n");
        for (int i = 0; i < 10; i += _freq)
        {
            printf("clock [%d/%d]\n", i, 10);
        }
    }   

    int _freq;

};


/*
class Widget
{
public:
    Widget(int n) : wTimer(n)
    {
    }

    void triger()
    {
        printf("_current_time: %d\n", wTimer._current_time); // 访问base class的protectd成员
        wTimer.onTick();
    }

private:
    class WidgetTimer : public Timer
    {
    public:
        WidgetTimer(int n) : Timer(n)
        {
        }
        int _current_time = 9; // 需要访问，将其声明为public
        virtual void onTick() const
        { // automatically called for each tick
            printf("WidgetTimer::onTick()\n");
            Timer::onTick();
        }
    };

    WidgetTimer wTimer;
};
*/

int main()
{
    Widget w(2);
    w.triger();

    printf("===\n");
    DWidget dw(3);
    dw.triger();
    return 0;
}

// output:
_current_time: 9
Widget::onTick()
clock [0/10]
clock [2/10]
clock [4/10]
clock [6/10]
clock [8/10]
===
_current_time: 9
DWidget::onTick()
clock [0/10]
clock [3/10]
clock [6/10]
clock [9/10]

```

- 加final关键字后，不能重写
```cpp
class Timer
{
public:
    explicit Timer(int tickFrequency) : _freq(tickFrequency)
    {
    }
    virtual ~Timer()
    {
    }
    virtual void onTick() const
    { // automatically called for each tick
        for (int i = 0; i < 10; i += _freq)
        {
            printf("clock [%d/%d]\n", i, 10);
        }
    }

private:
    int _freq;

protected:
    int _current_time = 9;
};


class Widget : private Timer
{

public:
    Widget(int n) : Timer(n) {}
    void triger()
    {
        printf("_current_time: %d\n", _current_time); // 访问base class的protectd成员
        onTick();
    }

private:
    virtual void onTick() const final
    {                                 // look at Widget usage data, etc.
        printf("Widget::onTick()\n"); // 重写onTick函数
        Timer::onTick();
    }

private:
    // int _freq=3;
};

class DWidget:public Widget{
public:
    DWidget(int n):Widget(n),_freq(n){

    }
private:
    virtual void onTick() const {          
        printf("DWidget::onTick()\n");
        for (int i = 0; i < 10; i += _freq)
        {
            printf("clock [%d/%d]\n", i, 10);
        }
    }   

    int _freq;
};

// output
test1.cpp:58:18: error: virtual function ‘virtual void DWidget::onTick() const’ overriding final function
   58 |     virtual void onTick() const {
      |                  ^~~~~~
test1.cpp:42:18: note: overridden function is ‘virtual void Widget::onTick() const’
   42 |     virtual void onTick() const final
      |                  ^~~~
```

2. 减小Widget和Timer的编译依赖。如果是private继承，在定义Widget的文件中势必需要引入#include"timer.h"。 但如果采用对象组合的方式，你可以把WidgetTimer放到另一个文件中，在Widget中保存WidgetTimer的指针并声明WidgetTimer即可， 见`Item 31`。
实现如下：
```cpp
// test1.cpp
#include "../common.h"
#include "./widgetTimer.h"

using namespace std;

class Widget
{
public:
    Widget(int n) : wTimer(new WidgetTimer(n))
    {
    }

    void triger()
    {
        printf("_current_time: %d\n", wTimer->_current_time); 
        wTimer->onTick();
    }

private:
    WidgetTimer* wTimer;
};


int main()
{
    Widget w(5);
    w.triger();

    return 0;
}
```

```cpp
// widgetTimer.cpp
#include "./widgetTimer.h"

WidgetTimer::WidgetTimer(int n) : Timer(n)
{
}
void WidgetTimer::onTick() const
{ // automatically called for each tick
    printf("WidgetTimer::onTick()\n");
    Timer::onTick();
}
```

```cpp
// WidgetTimer.h
#include "./Timer.h"
class WidgetTimer : public Timer
{
public:
    WidgetTimer(int n);
    int _current_time = 9; // 需要访问，将其声明为public
    virtual void onTick() const;
};
```

```cpp
// Timer.h
#include "../common.h"

using namespace std;

class Timer
{
public:
    explicit Timer(int tickFrequency) : _freq(tickFrequency)
    {
    }
    virtual ~Timer()
    {
    }
    virtual void onTick() const
    { // automatically called for each tick
        for (int i = 0; i < 10; i += _freq)
        {
            printf("clock [%d/%d]\n", i, 10);
        }
    }

private:
    int _freq;

protected:
    int _current_time = 9;
};
```

```cpp
编译：
g++ test1.cpp widgetTimer.cpp

// output:
_current_time: 9
WidgetTimer::onTick()
clock [0/10]
clock [5/10]
```


## EBO特性

我们讲虽然对象组合优于private继承，但有些特殊情况下仍然可以选择private继承。 需要EBO（empty base optimization）的场景便是另一个特例。 由于技术原因，C++中的独立空对象也必须拥有非零的大小，请看：

```cpp
class Empty {}; 
class HoldsAnInt {
private:
  int x;
  Empty e;        
};
```

`Empty e`是一个空对象，但你会发现`sizeof(HoldsAnInt) > sizeof(int)`。 因为C++中独立空对象必须有非零大小，所以编译器会在Empty里面插入一个char，这样Empty大小就是1。 由于字节对齐的原因，在多数编译器中HoldsAnInt的大小通常为`2*sizeof(int)`。更多字节对齐和空对象大小的讨论见Item 7。  ***但如果你继承了Empty，情况便会不同*** ：
```cpp
class HoldsAnInt: private Empty {
private:
  int x;
};
```
这时sizeof(HoldsAnInt) == sizeof(int)，这就是空基类优化（empty base optimization，EBO）。 当你需要EBO来减小对象大小时，可以使用private继承的方式。

继承一个空对象有什么用呢？ ***虽然空对象不可以有非静态成员，但它可以包含typedef, enum, 静态成员，非虚函数 （因为虚函数的存在会导致一个徐函数指针，它将不再是空对象）*** 。 STL就定义了很多有用的空对象，比如unary_function, binary_function等。

## summary
1. private继承的语义是"is-implemented-in-terms-of"，通常不如对象组合。但有时却是有用的：比如在派生类中访问protected成员、重写虚函数。（其实通过public继承+复合也可以实现，本文中以及给出了例子）
2. 不同于对象组合，private继承可以应用EBO，库的开发者可以用它来减小对象大小。（这个看起来是唯一的 ***仅有private能做的事情*** ）

## reference
1. https://harttle.land/2015/09/06/effective-cpp-39.html



