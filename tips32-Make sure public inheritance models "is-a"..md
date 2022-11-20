# Tips32 Make sure public inheritance models "is-a".

>  本文将介绍`public`继承背后的`is-a`的关系

[TOC]

## 什么是is-a

C++面向对象程序设计中，最重要的规则便是：`public`继承应当是`"is-a"`的关系。当`Derived public`继承自`Base`时， 相当于你告诉编译器和所有看到你代码的人：`Derived`就是一个`Base`，任何时候`Derived`都可以代替`Base`使用。

>当然这只适合`public`继承，如果是`private`继承那是另外一回事了，见Item 39。

比如我们有一个Person，然后有一个派生类Student,那么就有`Student is a Person`，凡是Person能做的Student都能做：

```cpp
void eat(const Person& p);
void study(const Person& p);

Person p; Student s;
eat(p); eat(s);
study(p); study(s);
```

## 语言的二义性

考虑这样一个场景，我们有一个Bird类:
```cpp
// bird.h
class Bird{
public:
    virtual void fly() const {
        std::cout << "Bird::fly()\n";
    }
private:

};
```
Penguin派生自Bird,但是企鹅不会飞，为了描述这种情况，我们有两种做法

1. 对Bird进行划分，分为会飞的鸟和不会飞的鸟

```cpp
class Bird{
    ...     // 没有声明fly函数
}

class Penguin:public Bird{
    ...     // 没有声明fly函数
}
```

2. 重写企鹅的`fly()`函数，使它在运行期抛出一个error，代表企鹅不会飞

```cpp
#include "../common.h"
#include <exception>

struct my_exception : public std::exception
{
    char *what()
    {
        return "Penguin can not fly!\n";
    }
};

class Bird
{
public:
    virtual void fly() const
    {
        std::cout << "Bird::fly()\n";
    }

private:
};

class Penguin : public Bird
{
public:
    void fly() const override
    {
        throw my_exception();
    }

private:
};

int main()
{
    Bird *pb = new Bird();
    pb->fly();
    delete pb;

    pb = new Penguin();

    try
    {
        pb->fly();
    }
    catch (my_exception &e)
    {
        std::cout << "exception: " << e.what() << "\n";
    }
    delete pb;
    pb = nullptr;

    return 0;
}
// output:
Bird::fly()
exception: Penguin can not fly!
```

***以上两种做法更推荐第一种，因为可以在编译期就知道我们调用penguin.fly()是错误的，更符合我们的认知（第二种方法更像：企鹅可以飞，但是飞失败了）*** 






## 错误的继承
生活的经验给了我们关于对象继承的直觉，然而并不一定正确。比如我们来实现一个正方形继承自矩形：

```cpp
class Rect{...};
void makeBigger(Rect& r){
    int oldHeight = r.height();
    r.setWidth(r.width()+10);
    assert(r.height() == oldHeight);        // true
}
class Square: public Rect{...};

Square s;
assert(s.width() == s.height());            // true
makeBigger(s);
assert(s.width() == s.height());            // false
```

根据正方形的定义，宽高相等是任何时候都需要成立的。然而`makeBigger`却破坏了正方形的属性， 所以正方形并不是一个矩形（因为矩形需要有这样一个性质：增加宽度时高度不会变）。即`Square`继承自`Rect`是错误的做法。 `C++`类的继承比现实世界中的继承关系更加严格： ***任何适用于父类的性质都要适用于子类！*** 

> 本节讨论了`is-a`的关系，`has-a`和`is-implemented-in-terms-of`会在tips38, tips39中分别介绍




## summary

1. `public`继承意味着`is-a`的关系。适用于base class身上的每一件事情一定也适用于其派生类，因此每一个派生类对象也都是一个`base class`对象

## reference

1. https://rollbar.com/blog/cpp-custom-exceptions/

2. https://harttle.land/2015/08/30/effective-cpp-32.html
