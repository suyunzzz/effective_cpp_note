# Tips 38: 通过复合塑模出has-a或者“根据某物实现出”

>  本文将介绍C++复合的两种功能，1. has-a, 2.根据某物实现出

一个类型包含另一个类型的对象时，我们这两个类型之间是组合关系。组合是比继承更加灵活的软件复用方法。 Item 32提到`public`继承的语义是"is-a"的关系。对象组合也同样拥有它的语义：

就对象关系来讲，组合意味着一个对象拥有另一个对象，是"has-a"的关系；
就实现方式来讲，组合意味着一个对象是通过另一个对象来实现的，是"is-implemented-in-terms-of"的关系。

[TOC]

## has-a

has-a比较好理解，如下，我们会说`Person` has a `name`

```cpp
class Person{
public:
    string name;
};
```


## 根据某物实现出

假如想要实现一个Set，为了代码复用，借助已有的List。因为List元素允许重复但是Set不行，所以 ***Set is not a List*** ，而是此条款的第二点： ***根据某物实现出*** 的关系

```cpp
#include "../common.h"

using namespace std;

template <class T>
class Set
{
public:
    bool member(const T &item) const;
    void insert(const T &item);
    void remove(const T &item);
    std::size_t size() const;

private:
    std::list<T> _rep;
};

template <typename T>
inline bool Set<T>::member(const T &item) const
{
    return std::find(_rep.begin(), _rep.end(), item) != _rep.end();
}

template <typename T>
inline void Set<T>::insert(const T &item)
{
    if (!member(item))
    {
        _rep.push_back(item);
    }
}

template <typename T>
inline void Set<T>::remove(const T &item)
{
    typename std::list<T>::iterator iter = std::find(_rep.begin(), _rep.end(), item);
    if (iter == _rep.end())
    {
        return;
    }
    _rep.erase(iter);
}

template <typename T>
inline std::size_t Set<T>::size() const
{
    return _rep.size();
}

int main()
{

    Set<int> s;
    for (int i = 0; i < 10; i++)
    {
        s.insert(i);
    }
    printf("size: %d\n", (int)s.size());

    s.remove(3);

    return 0;
}

// output:
size: 10
```

> 复用List的实现使得Set的方法都足够简单，它们很适合声明成inline函数（见Item 30）。


## summary
1. 复合的意义(has-a或者根据某物实现出)和public继承(is-a)完全不同
2. 在应用域，复合意味着 ***has-a*** ，在实现域，复合意味着 ***根据某物实现出***

## reference
1. https://harttle.land/2015/09/05/effective-cpp-38.html