# Tips 30: understand_the_ins_and_outs_of_inlining

>本文将从inline的原理、特点以及使用场景进行介绍

[TOC]

## inline原理
`inline`最理想的功能就是***减少代码调用成本***，在调用处直接对函数进行替换


- 显式提出

> 直接将函数声明为`inline`

```cpp
inline int get_max(int a, int b){
    return a>b?a:b;
}
```

***建议将inline函数的定义放在头文件中***，因为调用`inline`要求函数函数定义随处可见，所以要求每一个调用内联函数的文件都出现了函数定义。

`inline`是用于实现的关键字，***要求将inline与函数的定义（而非声明）放在一起***。只需在定义前加`inline`关键字即可。

```cpp
inline void Foo(int x, int y); // inline 仅与函数声明放在一起
void Foo(int x, int y){}
```

```cpp
void Foo(int x, int y); // inline 仅与函数声明放在一起
inline void Foo(int x, int y){}
```

- 隐式提出

***定义在类中的成员函数都是内联的***, 如果一个函数在类中没有给出定义（仅仅给出了声明）而又想内联的话，需要在定义处加上`inline`关键字:

```cpp
class A{
public:
    int get_num() const{        // 默认是内联的
        return num;
    }
private:
    int num;
};
```

```cpp
class B{
public:
    int get_num() const;
    int get_age() const;
private:
    int num;
    int age;
}

int B::get_num() const{     // 非内联
    return num;
}

inline int B::get_age() const{     // 内联
    return num;
}
```

## inline特点

- inline是对编译器提出的一个申请，并非强制命令。这一申请可以显示提出，也可以隐式提出

- inline通常情况下是编译期间的行为

### 优势

- 减少调用成本（栈内存）

### 劣势

- 代码膨胀，增加编译对象大小

inline是在调用出替换展开。

- 不利于后续升级

inline函数无法随着程序库的升级而升级。对于一个函数`f()`，后续想要在其内部增加新功能，假如用户使用的inline，那么所有调用f()的文件都需要重新编译；反观如果不使用`inline`，一旦函数库有修改，那么客户仅仅需要重新链接即可。

## 慎用inline

### inline与函数模板

```cpp
template<typename T>
inline const T& std::max(const T& a, const T& b){
    return a<b?b:a;
}
```
1. 上述代码被置于头文件中，我们发现``inline``函数和template两者通常都被放在头文件中，这是因为二者大多是情况下都是编译器行为

2. 函数模板并不一定要`inline`， 以上面代码为例，加入期望max函数模板具体化出来的每一个函数都是要`inline`的，那么这个函数模板可以`inline`，否则不能`inline`

### 通过函数指针的调用不能被inline

```cpp
inline void f() {}  
void(*pt)() = f;

f();        // 可以inline
pf();       // 编译器通常不可以inline，因为是函数指针调用

```

### 构造函数、析构函数不要inline

考虑这样一个场景： 

```cpp
class Base{
public:

private:
    std::string a,b;
};

class Drived : public Base{
public:
    Drived(){}      // 隐式inline
private:
    std::string c, d, e;
};
```

其中的Drived构造函数内部其实具有：`Base`构造， 成员对象`c` `d` `e`的构造，如下，如果将其声明为`inline`（即使是隐式）那么就会具有非常多的代码替换，这会增大调用者的体积。

```cpp
Drived::Drived(){
    Base::Base();
    try{
        dml.std::string::string();
    } catch(...){
        Base::~Base();
        throw;
    }
    try{
        dm2.std::string::string();      // 如果构造失败，则对已经构造好的对象进行析构
    } catch(...){
        ~dml.std::string::string();
        Base::~Base();
        throw;
    }
    try{
        dm3.std::string::string();
    } catch(...){
        ~dm2.std::string::string();
        ~dm1.std::string::string();
        Base::~Base();
        throw;
    }
}
```

> 要当心构造函数和析构函数可能会隐藏一些行为，如"偷偷地"执行了基类或成员对象的构造函数和析构函数。***所以不要随便地将构造函数和析构函数的定义体放在类声明中***

## summary
1. 小型、被频繁使用的函数可以`inline`，可以降低后续代码库升级的难度，并使得潜在的代码膨胀问题最小化，使程序的速度提升机会最大化。

2. 不要一位内`function template`声明在头文件中就将他们`inline`

## reference
1. [cpp-inline-usage](https://www.runoob.com/w3cnote/cpp-inline-usage.html)