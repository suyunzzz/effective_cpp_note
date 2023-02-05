# Tips 44: 将与参数无关的代码抽离templates

>  本文将介绍templates相关知识

[TOC]


**模板提供的是编译期的多态**， 即使你的代码看起来非常简洁短小，生成的二进制文件也可能包含大量的冗余代码。 因为**模板每次实例化都会生成一个完整的副本**，所以其中与模板参数无关的部分会造成代码膨胀（code bloat）。

**把模板中参数无关的代码重构到模板外便可以有效地控制模板产生的代码膨胀**。 另外代码膨胀也可以由类型模板参数产生：

- 对于**非类型模板参数产生的代码膨胀**，用*函数参数或数据成员来代替模板参数即可消除冗余*；
- 对于**类型模板参数产生的代码膨胀**，可以*让不同实例化的模板类共用同样的二进制表示*。


## 代码冗余

在避免代码冗余的问题上，抽取公共代码（*commonality and variability analysis*）是我们每天都在用的方法。 当你写几个函数时，会把其中的公共部分抽取到另一个函数；当你声明类时，也会把它们的公共部分抽取到父类中。

于是你希望在模板编程中也用该办法来避免代码重复，但模板和非模板代码在这一点上是不同的：

- 非模板的代码中，冗余的显式的（*explicit*）。只要有重复代码你都会看到它；
- 模板代码中，冗余是隐式的（*implicit*）。模板代码只有一份，模板被实例化时产生的冗余需要你的直觉才能感受到。


## 模板产生的代码膨胀

先看一个例子，实现一个矩阵类，其支持转秩运算：


```cpp
template<typename T, int n>
class Square{
public:
    void invert();
};
```

在这个例子中，使用不同的模板参数对其实例化会在编译器造成多个副本，他们之间仅仅是数据类型或者矩阵大小不同，如：`Square<int, 3>`, `Square<int, 5>`, `Square<double, 3>`....

## 解决：抽取父类模板
结局模板产生的代码膨胀，仍然是用抽取公共代码的办法。如果你真的看到了二进制代码中两个相同的`invert`函数， 你的直觉肯定是把它抽取到另一个类中：
```cpp
template <typename T>
class SquareBase{
protected:  
    void invert(int n){
        printf("invert(%d)\n", n);
    }

};

template <typename T, int n>
class Square : private SquareBase<T>
{
private:
public:
    void invert()
    {
        printf("Square::invert(%d)\n", n);
        invert(n);
    }
};
```

但是上述改动是不会通过编译的，在g++ 9.4中会报错：

```cpp
main.cpp: In instantiation of ‘void Square<T, n>::invert() [with T = int; int n = 3]’:
main.cpp:28:19:   required from here
main.cpp:22:9: error: no matching function for call to ‘Square<int, 3>::invert(int)’
   22 |         invert(n);
      |         ^~~~~~
main.cpp:19:10: note: candidate: ‘void Square<T, n>::invert() [with T = int; int n = 3]’
   19 |     void invert()
      |          ^~~~~~
main.cpp:19:10: note:   candidate expects 0 arguments, 1 provided
```

对于这个问题，tips43中已经介绍过了（编译器不会进入到templated base class中寻找对应的函数）

解决如下：
```cpp
template <typename T>
class SquareBase{
protected:  
    void invert(int n){      // 仅仅提供给子类，所以是protected
        printf("invert(%d)\n", n);
    }

};

template <typename T, int n>
class Square : private SquareBase<T>    // 不是is-a的关系，而是is implemented in terms of
{
private:
    using SquareBase<T>::invert;
public:
    void invert()      // 调用代价为0,因为是inline调用
    {
        printf("Square::invert(%d)\n", n);
        invert(n);
    }
};
```


## 数据存储问题

既然我们决定由父类来做`invert`操作，那么父类怎么访问数据呢？因为数据本来是在子类中的。 当然我们可以在调用`SquareBase::invert`时把内存地址也一起告知父类， 但如果矩阵类中有很多函数都需要这些信息呢？我们可能需要调用每个函数时都把这些信息传递给父类函数。 既然这样，何不把数据地址直接放在父类中？既然父类存放了数据，那么就把矩阵大小也一并存放了吧！

```cpp
template<typename T>
class SquareBase{
protected:
    SquareBase(int _n, T *_data): n(_n), data(_data){}
    void setData(T *_data){
        data = _data;
    }
private:
    int n;
    T* data;
};
```

父类中存储了矩阵数据的位置（`data`）以及大小（`n`），子类仍然可以决定如何分配地址空间。 可以存放在子类中作为成员属性，也可以动态申请内存。

## summary

本文讨论的是**非类型模板参数**，对于**类型模板参数**，代码膨胀的问题也是存在的，比如：
1. `int`和`long`在多数平台都是一样的底层实现，然而模板却会实例化为两份，因为它们类型不同。
2. `List<int *>`, `List<const int *>`, `List<double *>`的底层实现也是一样的。但因为指针类型不同，也会实例化为多份模板类。

## reference
1. https://harttle.land/2015/09/12/effective-cpp-44.html










