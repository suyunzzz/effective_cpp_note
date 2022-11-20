# Tips 31: Minimize compilation dependencies between files

> 本文将从编译依赖问题的产生、如何解决编译依赖问题来进行介绍

[TOC]

## 编译依赖问题的产生

考虑以下代码：

```cpp
// person.cpp
#include <string>
#include "date.h"

class Person;
std::ostream &operator<<(std::ostream &osm, const Person &person);

class Person
{
public:
    Person(const Date &birthday, const std::string &name) : _birthday(birthday), _name(name)
    {
        std::cout << "Person constructor\n";
    }
    friend std::ostream &operator<<(std::ostream &osm, const Person &person)
    {
        osm << person._name << ",\t" << person._birthday.DebugString() << "\n";
        return osm;
    }

private:
    Date _birthday;
    std::string _name;
};

int main()
{

    Person p1(Date{1997, 12, 19}, "syz");
    std::cout << p1 << "\n";
    return 0;
}
```

```cpp
// date.h
#pragma once
#include <iostream>

class Date
{
public:
    Date(int a, int b, int c) : year(a), month(b), day(c)
    {
        std::cout << "Date constructor\n";
    }
    std::string DebugString() const
    {
        return std::to_string(year) + std::string("-") + std::to_string(month) + "-" + std::to_string(day) + "\n";
    }

private:
    int year;
    int month;
    int day;
};
```

因为在`person.cpp`中我们 *inlcude* 了`date.h`， 那么`date.h`的内容在编译时就会被拷贝到`person.cpp`中。在第一次编译完成后，我们在`Date`类中增加了新的借口（并不影响`Person`中的使用），此时再次编译代码`Person`类就得需要重新编译， ***因为其在编译期需要确定内部成员变量`_date`的空间占用*** 。


## 如何解决编译依赖问题

为了解决编译依赖问题，你可能会说，“只在`Person`中声明`Date`，而不引入定义不久可以了吗？”， 代码如下：

```cpp
class Date;

class Person{
    ...

privete:
    Date _date;
}
...
Person p;

```

事实上这个编译是通过不了的，以内对于`Person.cpp`这个编译单元，编译器在执行到`Person p`这一句时，会为`p`分配内存，但是此时并不知道`Person`的内存大小，所以编译期需要知道`Date`的内部实现，仅仅声明`Date`是不够的。

> 在介绍下面几种方法之前，我们需要先明确我们的目的：接触Person类和Date类之间的依赖

### 1. 使用指针代替对象

一个去除编译依赖的办法是：依赖项使用指针而不是对象，同时依赖于类的声明而非定义。比如我们把`_birthday`改成指针，并声明`class Date;`：
```cpp
// person.h
#pragma once
#include <string>
#include <iostream>
class Date;
class Person
{
public:
    Person(const Date *birthday, const std::string &name) : _birthday(birthday), _name(name)
    {
        std::cout << "Person constructor\n";
    }
    friend std::ostream &operator<<(std::ostream &osm, const Person &person);

private:
    const Date *_birthday{nullptr};
    std::string _name;
};
```

```cpp
// date.h
#pragma once
#include <iostream>

class Date
{
public:
    Date(int a, int b, int c) : year(a), month(b), day(c)
    {
        std::cout << "Date constructor\n";
    }
    std::string DebugString() const
    {
        return std::to_string(year) + std::string("-") + std::to_string(month) + "-" + std::to_string(day) + "\n";
    }

private:
    int year;
    int month;
    int day;
};
```

这样在编译`person.h`时，就不需要知道`Date`的内存大小了，因此解决了`Person`依赖`Date`的问题。
剩余代码如下:

```cpp
// person.cpp
#include "date.h"
#include "person.h"

std::ostream &operator<<(std::ostream &osm, const Person &person)
{
    osm << person._name << ",\t" << person._birthday->DebugString() << "\n";
    return osm;
}
```

```cpp
// main.cpp
#include "person.h"
#include "date.h"

int main()
{
    Date *date(new Date(1997, 12, 19));
    Person p(date, "syz");
    std::cout << p << "\n";
    return 0;
}
```


### 2. 单独提供声明
既然我们希望依赖于声明而非定义，那么我们需要为每个类单独地提供声明文件，和定义文件。 比如`date.h`便需要分为两个文件，一个是声明`class Date`;，一个是定义`class Date{}`。

```cpp
// file: datefwd.h
class Date;

// file: date.h
class Date{
    ...
};
```

我们在`person.h`中只`include "datefwd.h"`即可。





### 3. 使用句柄类(Handle Class)


`C++`中接口声明和内部实现必须同时引入， 但在`Java`等语言中便不存在这个问题。因为所有对象都是引用，比如`Person`中的`Date`只是一个引用，`Person`的
大小与`Date`的实现无关， 只需要为`Date`分配一个引用大小的空间即可。在`C++`中，我们也可以使用`"pImpl idiom"`来实现这个策略：

***相当于把实现放到了另外一个类中`PersonImpl`，这样的`Person`类称为句柄类(Handle class)*** 。 这样， ***当`Date`的内部实现发生改变时，仅仅需要重新编译`PersonImpl`类中的代码，而`Person`类中的代码因为不依赖`Date`类且PersonImpl为一个指针，所以不再需要重新编译了*** 。

完整代码如下：

```cpp
// date.h
#pragma once
#include <iostream>

class Date
{
public:
    Date(int a, int b, int c) : year(a), month(b), day(c)
    {
        std::cout << "Date constructor\n";
    }
    std::string DebugString() const
    {
        return std::to_string(year) + std::string("-") + std::to_string(month) + "-" + std::to_string(day) + "\n";
    }

private:
    int year;
    int month;
    int day;
};

```

```cpp
// person.h
#pragma once
#include <string>
#include <iostream>
#include <memory>
#include "personimpl.h"
class Person
{
public:
    Person(const Date &birthday, const std::string &name) : _person_impl(std::make_shared<PersonImpl>(birthday, name))
    {
        std::cout << "Person constructor\n";
    }
    friend std::ostream &operator<<(std::ostream &osm, const Person &person);

private:
    std::shared_ptr<PersonImpl> _person_impl{nullptr}; // only need allocate a heap memory for _person_impl
};

```


```cpp
// personimpl.h
#include "date.h"

class PersonImpl
{
public:
    PersonImpl(const Date &date, const std::string &name) : _name(name), _date(date)
    {
        std::cout << "PersonImpl constructor\n";
    }
    std::string DebugString() const
    {
        return _name + "\t" + _date.DebugString() + "\n";
    }

private:
    Date _date;
    std::string _name;
};
```


```cpp
// person.cpp
#include "date.h"

class PersonImpl
{
public:
    PersonImpl(const Date &date, const std::string &name) : _name(name), _date(date)
    {
        std::cout << "PersonImpl constructor\n";
    }
    std::string DebugString() const
    {
        return _name + "\t" + _date.DebugString() + "\n";
    }

private:
    Date _date;
    std::string _name;
};

```


```cpp
// main.cpp
#include "person.h"
#include "date.h"

int main()
{
    Person p({1997, 12, 19}, "syz");
    std::cout << p << "\n";
    return 0;
}
```

>这样浅显地看起来，`PersonImpl`类其实就是一个工具人，承接了很多脏活累活，隐藏`Person`类的细节，保证`Person`类的整洁

>关于 `impl idiom`这一策略后续我们还会重点介绍，敬请关注

### 4. 使用接口类

对于`Person`编译依赖于`Date`类的问题，我们还可以使用虚函数作为接口，客户不是实例化它，仅仅能使用其引用或者指针。 然而客户一定需要某种方法来获得一个实例，比如 ***工厂方法*** (见Item 13)或者虚构造函数。 它们动态地创建对象，并返回对象指针（最好是智能指针，见Item 18）。

比如`Person`有一个实体类叫做`RealPerson`，那么`Person`中可以提供一个工厂方法`create()`：

```cpp
class Person{
public:
    static shared_ptr<Person> create(string& name);     // 工厂模式
};
shared_ptr<Person> Person::create(string& name){    
    return shared_ptr<Person>(new RealPerson(name));        
}
...
shared_ptr<Person> p(Person::create("alice"));
```

这样我们就避免了`Person`和`Date`之间的编译依赖（需要`RealPerson`编译依赖于`Date`）



## summary

1. 最小化编译依赖的一般做法是依赖于声明而非定义

2. 句柄类和接口类的设计隐藏了`Person`类的具体设计细节，减小了实现细节的改动对客户的影响。但是无论是句柄类还是接口类，都会造成一些代价： ***多一个`pImpl`指针的空间，虚函数表指针的空间、间接调用的时间和寻找虚函数的时间*** 。

## reference

1. https://harttle.land/2015/08/29/effective-cpp-31.html