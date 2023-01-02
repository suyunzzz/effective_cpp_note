# Tips 42: 了解typename的双重意义

>  本文将介绍typename相关知识

[TOC]

## typename与class

在声明template参数时，无论使用关键字class或者typename，意义完全相同，然而有时候必须要使用typename



先看一个例子

```cpp
template<typename C>
void print2nd(const C& container){
    if(container.size()>=2){
        C::const_iterator iter = container.cbegin();
        iter++;
        int value = *iter;
        std::cout << "second value: " << value << "\n";
    }
}
```

在上述例子中，`C::const_iterator`被称为 ***从属嵌套类型***，其类型依赖于`C`。`value`，其类型是int，与`C`类型无关，被称为非从属名称(non-dependent names)。

上述代码是不能通过编译的，因为如果编译器在template中遭遇一个嵌套从属名称，他便假设这个名称不是个类型，除非你告诉它是。所以在缺省的情况下，嵌套从属名称(`C::const_iterator`)不是类型。

## 一般性规则

对上述代码，我们需要在嵌套从属名称前添加typename，告诉编译器它是一个类型，即可通过编译

```cpp
template<typename C>
void print2nd(const C& container){
    if(container.size()>=2){
        typename C::const_iterator iter = container.cbegin();
        iter++;
        int value = *iter;
        std::cout << "second value: " << value << "\n";
    }
}
```

typename仅仅用来表示嵌套从属类型名称，其他名称不该有它的存在：
```cpp
template<typename C>
void f(const C& container, typename C::const_iterator iter){
    std::cout << "iter's index: " << iter-container.cbegin() << "\n";
}
```

## 一般性原则之外

“typename必须作为嵌套从属类型名称的前缀词“这一规则的例外是：

***1. typename不可以出现在base class list内的嵌套从属类型名称之前*** 

***2. 且不可以出现在成员初始化列表中作为base class修饰符***

```cpp
template <typename T>
class Drived:public Base<T>::Nested{        // base class list中不能出现typename
public:
    Drived():Base<T>::Nested(){             // mem.init.list 中不能出现typename修饰base class
        typename Base<T>:Nested temp;       // 一般性原则，需要加上typename修饰嵌套从属类型名称
    }
};
```



## 最后一个typename例子

考虑如下代码：

```cpp
template <typename IterT>
void workWithIterator(IterT iter){
    typename std::iterator_traits<IterT>::value_type temp(*iter);
    // ...
}
```
上述代码的意义就是 ***根据iter所指的对象创建一个新的对象*** ，其中，`std::iterator_traits<IterT>::value_type`代表类型为`IterT`之对象所指之物的类型，如果`IterT`是`std::vector<int>::iterator`，`temp`的类型就是`int`。
由于`std::iterator_traits<IterT>::value_type`是一个 ***嵌套从属类型名称*** ，所以我们必须在它之前放置`typename`。

为了简化输入，可以使用`typedef`:

```cpp
template <typename IterT>
void workWithIterator(IterT iter){
    typedef typename std::iterator_traits<IterT>::value_type value_type;
    value_type temp(*iter);
    // ...
}
```

## Summary
1. 声明`template`参数时，前缀关键字`class`和`typename`可互换

2. 使用`typename`标识嵌套从属类型名称，但不得在`base class lists`和`member initialization list`内将它作为`base class`修饰符