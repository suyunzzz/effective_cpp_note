# Tips 45 运用成员函数模板接受所有兼容类型

>  本文将介绍模板类构造函数重载以实现隐式类型转换

[TOC]

## 隐式类型转换
我们有以下代码：

```cpp
class Top{};
class Middle: public Top{};
class Bottom: public Middle{};
```

普通指针可以做到**派生类指针隐式转换为基类指针**：
```cpp
Top *p1 = new Bottom;
const Top *p2 = p1;
```

对于我们定义的智能指针`SmartPtr`，我们需要让下面的代码通过编译：
```cpp
SmartPtr<Top> p1 = SmartPtr<Bottom>(new Bottom);
SmartPtr<const Top> p2 = p1;
```

**同一模板的不同实例之间是没有继承关系的**，在编译器看来`AutoPtr<Top>`和`AutoPtr<Bottom>`是完全不同的两个类。 所以上述代码直接编译是有问题的。



## 重载构造函数

为了支持用`SmartPtr<Bottom>`初始化`SmartPtr<Top>`，我们需要重载`SmartPtr`的构造函数。 原则上讲，有多少类的层级我们就需要写多少个重载函数。因为类的层级是会扩展的，因此需要重载的函数数目是无穷的。 这时便可以引入成员函数模板了：

```cpp
class Top{};
class Middle: public Top{};
class Bottom: public Middle{};

template <typename T>
class SmartPtr{
public:
    SmartPtr(T* ptr):_ptr(ptr) {
        std::cout << "SmartPtr::SmartPtr constructor\n";
    }

    template <typename U>
    SmartPtr(const SmartPtr<U>& other):_ptr(other.get()){
        std::cout << "SmartPtr::SmartPtr copy constructor\n";
    }

    T* get() const{
        return _ptr;
    }
private:
    T* _ptr=nullptr;
};

int main(){

    SmartPtr<Top> s_ptr = SmartPtr<Bottom>(new Bottom);

    return 0;
}
```

其中，在`ptr(other.get())`时编译器会进行类型的兼容性检查，只有当`U`可以隐式转换为`T`时，`SmartPtr<U>`才可以隐式转换为`SmartPtr<T>`。 这样就避免了不兼容指针的隐式转换。



**接受同一模板的其他实例的构造函数被称为通用构造函数** *（generalized copy constructor）*。




## 手动添加同类型的拷贝构造、拷贝赋值函数
> 模板类具象化后得代码中，如果没有定义拷贝构造函数，**那么会自动生成一个拷贝构造函数（而不会直接使用模板中的拷贝构造函数）**

简单来讲，上述代码中在拷贝构造函数模板中，假如当类型U和类型T相同时，拷贝构造函数不会是从模板中具象化，而是会自己创建一个默认的拷贝构造函数。

所以`shared_ptr`中还是手动声明了拷贝构造函数：
```cpp
template<class T>
class shared_ptr{
public:
    shared_ptr(shared_ptr const& r);
    template<class Y>
        shared_ptr(shared_ptr<Y> const& r);

    shared_ptr& operator=(shared_ptr const& r);
    template<class Y>
        shared_ptr& operator=(shared_ptr<Y> const& r);
};
```


实验如下：
```cpp
class Top{};
class Middle: public Top{};
class Bottom: public Middle{};

template <typename T>
class SmartPtr{
public:
    SmartPtr(T* ptr):_ptr(ptr) {
        std::cout << "SmartPtr::SmartPtr constructor\n";
    }

    template <typename U>
    SmartPtr(const SmartPtr<U>& other):_ptr(other.get()){
        std::cout << "SmartPtr::SmartPtr generalize copy constructor\n";
    }

    SmartPtr(const SmartPtr& other){
        std::cout << "SmartPtr::SmartPtr copy constructor\n";
    }

    T* get() const{
        return _ptr;
    }
private:
    T* _ptr=nullptr;
};

int main(){

    SmartPtr<Top> s_ptr = SmartPtr<Bottom>(new Bottom);
    std::cout << "===\n";
    SmartPtr<Top> p(s_ptr);

    return 0;
}

// output
SmartPtr::SmartPtr constructor
SmartPtr::SmartPtr generalize copy constructor
===
SmartPtr::SmartPtr copy constructor
```





## summary

1. **成员函数模板**可以使得函数可以接受所有兼容的类型（隐式转换）。
2. 如果你用成员函数模板声明了拷贝构造函数和赋值运算符，**仍然需要手动编写普通拷贝构造函数和拷贝运算符**。


## reference
1. https://harttle.land/2015/09/12/effective-cpp-45.html










