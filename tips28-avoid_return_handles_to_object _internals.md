# Tips 28: Avoid returning "handles" to object internals.

> 避免范围对象私有成员的句柄(引用，指针，迭代器)，这样可以增加类的封装性，也可以避免虚空指针问题(dangling handles)

[TOC]

## 返回私有成员的指针
首先如果不加限制，直接返回私有成员的指针会导致私有成员被完全暴露。
```cpp
class Rectangle {
  int _left, _top;
public:
  int& left() { return _left; }
};
Rectangle rec;
rec.left() = 3;   // rec._left被修改
```
其实这已经足以说明*返回私有成员指针相当于完全暴露了私有成员*。





## 2.从const函数中传出引用

### 2.1 C++ logic constness 和 bitwise constness
对于以下类，我们期望vector是核心成员，count仅作为计数，为非核心成员。并且期望getItem成员函数不会修改核心成员vector，而不关注非核心成员是否被修改。此时`getItem`具备的`const`特性就是`logic constness`

```cpp
class BigArray{
std::vector<int> v;
int count = 0;

public:
    int GetItem(int idx) const {
        count++;
        return v[idx];
    }
};
```

上面的代码不会通过编译，因为编译器不会考虑`logic constness`，于是就有了 `bitwise constness` 这个术语，可以理解为字面上的 `constness` 属性，编译器只认 `bitwise constness`。

为了解决这个矛盾，可以把`count`改为`mutable`，这样既保留了`logic constness`特性，又可以通过编译：

```cpp
// logic constness 和 bitwise constness

#include "../common.h"

using namespace std;

class BigArray{
std::vector<int> v;
mutable int count = 0;

public:
    int GetItem(int idx) const  {
        count++;
        return v[idx];
    }
};

int main(){
    BigArray ba;
    std::cout << ba.count << "\n";      // 外部仍然不可访问

    return 0;
}

```

反过来，如果你的成员是指针类型，在函数中我们修改了指针所指的数据，此时字面上指针变量并没有被修改，编译器依然只会维护 `bitwise constness`，即便我们将这样的函数声明为 `const`，依然是没有问题的，例如下面的代码可以通过编译：

```cpp
class BigArray {
    int* v2;
    // ...
    void setV2Item(int index, int x) const {
        v2[index] = x;
    }
};
```






### 2.2 从const函数中传出引用
如果一个const成员函数传出一个reference，且该引用的内存在当前对象之外，那么这个函数的调用者可以修改这个数据，这也就是"bitwise constness"的一个附带结果。

```cpp
// tips28 返回私有成员为指针
#include "../common.h"

using namespace std;

class Point
{
public:
    Point(int x, int y) : _x(x), _y(y) {}
    void set_x(int a)
    {
        _x = a;
    }
    int get_x() const
    {
        return _x;
    }

    void set_y(int a)
    {
        _y = a;
    }
    int get_y() const
    {
        return _y;
    }

private:
    int _x;
    int _y;
};

struct RectData
{
    RectData(const Point &a, const Point &b) : ulhc(a), lrhc(b) {}
    Point ulhc;
    Point lrhc;
};

class Rectangle
{

public:
    Rectangle(const std::shared_ptr<RectData> &rect_data) : _rect_data(rect_data) {}

    Point &get_ul_point() const
    {
        return _rect_data->ulhc;
    }
    // 返回类内部的handle(reference, pointer, iterator)，记得要加const
    const Point &get_lr_point() const
    {
        return _rect_data->lrhc;
    }

private:
    std::shared_ptr<RectData> _rect_data;
};
int main()
{
    std::shared_ptr<RectData> rect_data = std::make_shared<RectData>(RectData({0, 0}, {100, 100}));
    Rectangle rectangle(rect_data);

    auto &p1 = rectangle.get_lr_point();
    printf("before p1: (%d, %d)\n", p1.get_x(), p1.get_y()); // before p1: (100, 100)

    p1.set_x(5);
    printf("after p1: (%d, %d)\n", p1.get_x(), p1.get_y()); // after p1: (5, 100)
}
```

## 悬空指针问题
dangling pointer指的是传出一个临时对象的指针，在通过该指针调用对象时，对象已经消失：

```cpp
#include "../common.h"

class A{
    

    int* a;

public:
    A(int* t_a):a(t_a){}
    int* get_a(){
        return a;
    }
};

int main(){

    int* pa = new int(5);
    A a(pa);
    printf("value: %d\n", *a.get_a());

    delete pa;
    printf("value: %d\n", *a.get_a());
    return 0;
}

output:
value: 5
value: 0
```



## summary
避免返回handle(指针、引用、迭代器)指向对象内部，这样可以使得封装性更高，并且降低dangling pointer出现的概率

## reference

1. https://www.jianshu.com/p/6b1cf551cb20
2. https://harttle.land/2015/08/26/effective-cpp-28.html


