# Tips 47 使用 Traits 类提供类型信息


>  本文介绍如何定义一个traits类，以及如何使用它在编译器获取数据类型

C++中的 `Traits` 类可以在编译期提供类型信息，它是用`Traits`模板及其特化来实现的。 通过方法的重载，可以在编译期对类型进行"if…else"判断。我们通过STL中的一个例子来介绍`Traits`的实现和使用。 

本文以 `iterator_traits` 为例介绍了**如何实现`traits`类**，以及**如何使用`traits`类** （在Item 42中提到过iterator_traits）。 其实 C++ 标准库中还提供了很多其他的 traits，比如 char_traits, numeric_limits 等。

STL提供了很多的容器、迭代器和算法，其中的 advance 便是一个通用的算法，可以让一个迭代器移动n步：

```cpp
template<typename IterT, typename DistT>
void advance(IterT& iter, DistT d);
```


[TOC]

## STL迭代器

最简单的迭代器是输入迭代器（input iterator）和输出迭代器（output iterator）， 它们只能向前移动，可以读取/写入它的当前位置，但只能读写一次。比如 ostream_iterator 就是一个输出迭代器。

比它们稍强的是前向迭代器（forward iterator），可以多次读写它的当前位置。 单向链表（slist，STL并未提供）和TR1哈希容器的迭代器就属于前向迭代器。

双向迭代器（bidirectional iterator）支持前后移动，支持它的容器包括 set, multiset, map, multimap。

随机访问迭代器（random access iterator）是最强的一类迭代器，可以支持 +=, -= 等移动操作，支持它的容器包括 vector, deque, string 等。

对于上述得5种迭代器，STL中有对应得5种Tag来标识结构体得类型，它们之间是is-a的关系：

```cpp
struct input_iterator_tag {};
struct output_iterator_tag {};
struct forward_iterator_tag: public input_iterator_tag {};
struct bidirectional_iterator_tag: public forward_iterator_tag {};
struct random_access_iterator_tag: public bidirectional_iterator_tag {};
```

对于advance，其实现方式依赖于迭代器得类型，也就是上述得Tag:
```cpp
template<typename IterT, typename DistT>
void advance(IterT& iter, DistT d){
  if (iter is a random access iterator) {
    iter += d;                                      // use iterator arithmetic
  }                                                  // for random access iters
  else {
    if (d >= 0) { while (d--) ++iter; }              // use iterative calls to
    else { while (d++) --iter; }                     // ++ or -- for other
  }                                                  // iterator categories
}
```

而traits的作用就是帮助我们在编译器拿到iterator的类型

## Traits

Traits 不是关键字，也不是 std 类型或模板，它只是 C++ 中的一种编程惯例，允许我们在编译期得到类型的信息。 用 Bjarne 的话讲，Traits 是一个用来携带信息的很小的对象（或结构）， 在其他对象或算法中用这一信息来确定策略或实现细节。

traits的另一个需求在于 advance 对与基本数据类型也能正常工作，比如 `char*`。所以traits不能借助类来实现， 于是我们把traits放到模板中。比如：

```cpp
template<typename IterT>          // template for information about
struct iterator_traits;           // iterator types
```

iterator_traits<IterT> 将会标识 IterT 的迭代器类别。iterator_traits 的实现包括两部分：

- 用户定义类型的迭代器
- 基本数据类型的指针
  
### 用户定义类型的迭代器
  
在用户定义的类型中，typedef 该类型支持迭代器的 Tag，例如 deque 支持随机迭代器：


```cpp
template < ... >                    // template params elided
class deque {
public:
  class iterator {
  public:
    typedef random_access_iterator_tag iterator_category;
  }:
};
```
  
然后在全局的 iterator_traits 模板中 typedef 那个用户类型中的 Tag，以提供全局和统一的类型识别。
```cpp
template<typename IterT>
struct iterator_traits {
  typedef typename IterT::iterator_category iterator_category;
};
```
  
### 基本数据类型的指针
上述办法对基本数据类型的指针是不起作用的，我们总不能在指针里面 typedef 一个 Tag 吧？ **其实这时只需要偏特化 iterator_traits，因为内置类型指针都是可以随机访问的**：
  
```cpp
template<typename IterT>               // partial template specialization
struct iterator_traits<IterT*>{
  typedef random_access_iterator_tag iterator_category;
};
```
  
实现一个traits类的整个过程：

1. 确定你希望提供的类型信息。比如你希望提供 deque 的 iterator 类型；
2. 为那个信息起一个名字。比如 iterator_catetory；
3. 提供一个模板以及必要的特化，来包含你希望提供的类型信息。比如 iterator_traits。
  
## Traits的应用
通过实现一个advance来体现Traits的应用。
  
我们已经用 **iterator_traits 提供了迭代器的类型信息** ，是时候给出 advance 的实现了。
```cpp
template<typename IterT, typename DistT>
void advance(IterT& iter, DistT d) {
  if (typeid(typename std::iterator_traits<IterT>::iterator_category) ==
    typeid(std::random_access_iterator_tag))
  ...
}
```

注意到上述代码中需要是有if判断(发生在运行期)，但是我们想要在编译器就完成判断，因此使用函数重载：
> 上述实现其实并不完美，至少 if 语句中的条件在编译时就已经决定，它的判断却推迟到了运行时（显然是低效的）。 在编译时作此判断，需要为不同的 iterator 提供不同的方法，然后在 advance 里调用它们。
  
```cpp
template<typename IterT, typename DistT>
void advance(IterT& iter, DistT d) {
  doAdvance(                                              // call the version
    iter, d,                                              // of doAdvance
    typename std::iterator_traits<IterT>::iterator_category()
  );                                                     
}                                                       

// 随机访问迭代器
template<typename IterT, typename DistT>
void doAdvance(IterT& iter, DistT d, std::random_access_iterator_tag) {
  iter += d;
}

// 双向迭代器
template<typename IterT, typename DistT>
void doAdvance(IterT& iter, DistT d, std::bidirectional_iterator_tag) {
  if (d >= 0) { while (d--) ++iter; }
  else { while (d++) --iter; }
}

// 输入迭代器
template<typename IterT, typename DistT>
void doAdvance(IterT& iter, DistT d, std::input_iterator_tag) {
  if (d < 0 ) {
     throw std::out_of_range("Negative distance");    // see below
  }
  while (d--) ++iter;
}
```
           
总结一下如何使用Traits所提供的类型信息：

1. 创建一系列的“worker”函数，拥有不同的 traits 参数。根据 traits 参数来提供相应的实现；
2. 创建一个“master”函数来调用这些“worker”，并将traits类提供的信息传递给“worker”。






## summary
1. Traits class使得类型相关信息在编译期可用。Traits以templates和templates特化来实现
2. 在应用Traits所提供得类型信息时，使用函数重载可以在编译器对类型执行if...else测试

## reference
1. https://harttle.land/2015/09/15/effective-cpp-47.html
2. https://accu.org/journals/overload/9/43/frogley_442/









