# Tips 41: 了解隐式接口和编译器多态

>  本文将介绍C++隐式接口和编译器多态

[TOC]

## 显式接口和运行期多态

先看一个例子

```cpp
class Widget{
public:
    Widget();
    virtual ~Widget();
    virtual std::size_t size() const;
    virtual void normalize();
    void swap(Widget& other);

};

void doProcessing(Widget& w){
    if(w.size()>10 && w!=someNastyWidget){
        Widget temp(w);
        temp.normalize();
        temp.swap(w);
    }
}
```

在这个例子中，我们可以这样描述`doProcessing`中的形参`w`:

1. w必须支持Widget的接口，具体是什么接口我们可以在Widget.h中看到，所以是显式接口
2. 由于Widget的某些函数是virtual，w对于那些函数调用所表现出运行期多态。



## 隐式接口和编译期多态
在Templates及泛型编程的世界里，与面向对象有根本上的不同。在此世界中显式接口和运行期多态仍然存在，但重要性降低。反倒是 ***隐式接口*** 和 ***编译期多态*** 移到前头了：

```cpp
template <typename T>
void doProcessing(T& w){
    if(w.size()>10 && w!=someNastyWidget){
        T temp(w);
        temp.normalize();
        temp.swap(w);
    }
}
```
在上述例子中，`w`看起来是一个`T`类型的对象，且其必须具备`size()`返回大小，以及和`someNastyWidget`对象进行比较， ***拷贝构造***，`normalize()`以及`swap`的接口。
但是事实并非如此，完全可以“再扩大范围”：***以size()接口为例，我们可以不要求其返回一个int或者size_t，只需要其返回值可以和10做比较即可*** 。

### 显示接口
通常显式接口是由函数的签名式（也就是函数名称，参数类型，返回值类型）构成。

### 隐式接口
隐式接口并不基于函数签名式，而是由有效表达式(valid experssion)组成。


## Summary

1. class和template都支持接口和多态
2. 对class而言，接口是显式的(explict)，以函数签名为中心。多态则是通过virtual函数发生在运行期
3. 对template参数而言，接口是隐式的(implicit)，基于有效表达式。多态则是通过template具现化和函数重载解析发生与编译器

