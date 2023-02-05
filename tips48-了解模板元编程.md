# Tips 48 认识template元编程

模板元编程(***Template Metaprogramming，TMP***)就是利用模板来编写那些在编译时运行的C++程序。 模板元程序（Template Metaprogram）是由C++写成的，运行在编译器中的程序。当程序运行结束后，它的输出仍然会正常地编译。

C++并不是为模板元编程设计的，但自90年代以来，模板元编程的用处逐渐地被世人所发现。

- 模板编程提供的很多便利在面向对象编程中很难实现；
- 程序的工作时间从运行期转移到编译期，可以更早发现错误，运行时更加高效。
- 在设计模式上，可以基于不同的策略，自动组合而生成具体的设计模式实现。
## 静态类型检查
先看一个例子，为什么会需要模板元编程，以**tips47**中的advance例子为例，如果我们选择typeid来对类型进行判断：

```cpp
template<typename IterT, typename DistT>
void advance(IterT& iter, DistT d) {
  if (typeid(typename std::iterator_traits<IterT>::iterator_category) ==
    typeid(std::random_access_iterator_tag)){
      iter += d;
  }
  ...
}

list<int>::iterator it;
advance(it, 10);
```

上述代码是不能通过编译的。虽然我们知道list::iterator不是random_access_iterator，代码不会执行iter+=d，但是在**编译期间并不知道，编译器需要保证所有得代码都是正确的，即使是不能执行得代码**。与之对比得是 ***Template metaprogramming***，其针对不同的类型选择对应得代码，使得类型测试(也就是if...else)发生于编译期而非运行期，***tips47*** 中的traits解法就是TMP。

## 模板元编程

TMP后来被证明是图灵完全的，这意味着TMP可以用来计算任何可计算的问题。你可以声明变量、执行循环、编写和调用函数等等。 但它的使用风格和普通C++完全不同。

下面通过计算阶乘来示范如何通过"递归模板具现化"(recursive template instantiation)实现循环，以及如何在TMP中创建和使用变量：

```cpp
#include "../common.h"

template <unsigned n>
struct Factorial{
    enum {value = n * Factorial<n-1>::value};
};

template <>
struct Factorial<0>{
    enum {value = 1};
};

int main(){
    printf("Factorial<5>::value: %d\n", Factorial<5>::value);
    return 0;
}
// output
Factorial<5>::value: 120
```

## TMP的用途
为了更好地理解TMP的重要性，我们来看看TMP能干什么：

1. 确保量纲正确。在科学计算中，量纲的结合要始终保持正确。比如一定要单位为"m"的变量和单位为"s"的变量相除才能得到一个速度变量（其单位为"m/s"）。 使用TMP时，编译器可以保证这一点。因为不同的量纲在TMP中会被映射为不同的类型。
2. 优化矩阵运算。比如矩阵连乘问题，TMP中有一项表达式模板（expression template）的技术，可以在编译期去除临时变量和合并循环。 可以做到更好的运行时效率。
3. 自定义设计模式的实现。设计模式往往有多种实现方式，而一项叫基于策略设计（policy-based design）的TMP技术可以帮你创建独立的设计策略（design choices），而这些设计策略可以以任意方式组合。生成无数的设计模式实现方式。


## summary
1. TMP将工作由运行期转移到编译期，因而得以实现早期错误侦测和更高的执行效率
2. TMP可以被用来生成“基于政策选择组合(`based on combinations of policy choices`)”的客户定制代码，也可以用来避免生成对某些特殊类型并不适合的代码

## reference
1. https://harttle.land/2015/09/16/effective-cpp-48.html









