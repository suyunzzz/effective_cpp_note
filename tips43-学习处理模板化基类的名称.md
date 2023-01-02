# Tips 43: 学习处理模板化基类的名称

>  本文将介绍C++模板化基类的名称

[TOC]

## 模板化基类

考虑这样一个场景，我们要传递信息到不同得公司，如果我们有足够得信息确定哪一个信息传递到哪一家公司，就可以采用基于template的解法：


```cpp
#include "../common.h"

class CompanyA{
public:
    void sendCleartext(const std::string& msg){
        std::cout << "CompanyA::sendCleartext\n";
    }
    void sendEncrypted(const std::string& msg){
        std::cout << "CompanyA::sendEncrypted\n";
    }
};


class CompanyB{
public:
    void sendCleartext(const std::string& msg){
        std::cout << "CompanyB::sendCleartext\n";
    }
    void sendEncrypted(const std::string& msg){
        std::cout << "CompanyB::sendEncrypted\n";
    }
};

template<typename Company>
class MsgSender{
public:
    void sendClear(const std::string& info){
        Company c;
        c.sendCleartext(info);
    }
};

template<typename Company>
class LoggingMsgSender:public MsgSender<Company>{
public:
    void sendClearMsg(const std::string& info){
        // log info before send
        sendClear(info);            // call base class function, can not run
        // log info after send
    }
};
int main(){

    LoggingMsgSender<CompanyA> logger_a;
    logger_a.sendClearMsg("hello");

    return 0;
}
```

在上述代码中，`LoggingMsgSender::sendClearMsg(...)`中调用基类的`sendClear(info)`无法通过编译，编译器会抱怨`sendClear()`不存在，既使我们可以确定它存在。

***问题在于，编译器不确定会不会在MsgSender的某些具象化类中是否具有`sendClear()`这一接口。***

## 具体化

我们将问题更具体化，假设有一个CompanyZ，仅仅能传送密文：
```cpp
class CompanyA{
public:
    void sendCleartext(const std::string& msg){
        std::cout << "CompanyA::sendCleartext\n";
    }
    void sendEncrypted(const std::string& msg){
        std::cout << "CompanyA::sendEncrypted\n";
    }
};


class CompanyB{
public:
    void sendCleartext(const std::string& msg){
        std::cout << "CompanyB::sendCleartext\n";
    }
    void sendEncrypted(const std::string& msg){
        std::cout << "CompanyB::sendEncrypted\n";
    }
};

class CompanyZ{
public:
    void sendEncrypted(const std::string& msg){
        std::cout << "CompanyZ::sendEncrypted\n";
    }
};

template<typename Company>
class MsgSender{
public:
    void sendClear(const std::string& info){
        Company c;
        c.sendCleartext(info);
    }
    void sendSecret(const std::string& info){
        Company c;
        c.sendEncrypted(info);
    }
};

// 以下被称为模板全特化，即MsgSender得参数是CompanyZ，且没有其他template参数可供变化
template<>
class MsgSender<CompanyZ>{
public:
    void sendSecret(const std::string& info){
        CompanyZ c;
        c.sendEncrypted(info);
    }

};

template<typename Company>
class LoggingMsgSender:public MsgSender<Company>{
public:
    void sendClearMsg(const std::string& info){
        // log info before send
        // 如果Company是CompanyZ，那么这个函数将不存在
        sendClear(info);            // call base class function, can not run
        // log info after send
    }
};
```

以上便是C++为什么拒绝调用`sendClear()`这个函数的原因，***它知道`base template class`有可能被特化，而那个特化版本可能并不具有`sendClear()`这个接口。因此往往拒绝在`template base class`(本例是MsgSender<Company>)中寻找继承而来得名称。***

## 解决方案

我们有三种方法可以使得C++”不进入templatized base classes观察“的这一行为失效：

1. 在base class函数调用之前在前面加上`this->`，假设SendClear将被继承

```cpp
#include "../common.h"

class CompanyA{
public:
    void sendCleartext(const std::string& msg){
        std::cout << "CompanyA::sendCleartext, " << msg << "\n";
    }
    void sendEncrypted(const std::string& msg){
        std::cout << "CompanyA::sendEncrypted, " << msg << "\n";
    }
};


class CompanyB{
public:
    void sendCleartext(const std::string& msg){
        std::cout << "CompanyB::sendCleartext, " << msg << "\n";
    }
    void sendEncrypted(const std::string& msg){
        std::cout << "CompanyB::sendEncrypted, " << msg << "\n";
    }
};

class CompanyZ{
public:
    void sendEncrypted(const std::string& msg){
        std::cout << "CompanyZ::sendEncrypted, " << msg << "\n";
    }
};

template<typename Company>
class MsgSender{
public:
    void sendClear(const std::string& info){
        Company c;
        c.sendCleartext(info);
    }
    void sendSecret(const std::string& info){
        Company c;
        c.sendEncrypted(info);
    }
};

// 以下被称为模板全特化，即MsgSender得参数是CompanyZ，且没有其他template参数可供变化
template<>
class MsgSender<CompanyZ>{
public:
    void sendSecret(const std::string& info){
        CompanyZ c;
        c.sendEncrypted(info);
    }

};

template<typename Company>
class LoggingMsgSender:public MsgSender<Company>{
public:
    void sendClearMsg(const std::string& info){
        // log info before send
        // 如果Company是CompanyZ，那么这个函数将不存在
        this->sendClear(info);            // call base class function, can not run
        // log info after send
    }
};


int main(){

    LoggingMsgSender<CompanyA> logger_a;
    logger_a.sendClearMsg("hello");     // CompanyA::sendCleartext, hello         

    LoggingMsgSender<CompanyZ> logger_z;
    logger_z.sendClearMsg("hello");     // 编译错误
    
    return 0;
}
```

2. 使用using, 将”被掩盖的base class名称“带入到一个派生类作用域内，告诉编译器去base template class内部寻找sendClear函数

```cpp
template<typename Company>
class LoggingMsgSender:public MsgSender<Company>{
public:
    using MsgSender<Company>::sendClear;     // 通过using告诉编译器去base template class内部查找函数
    void sendClearMsg(const std::string& info){
        // log info before send
        // 如果Company是CompanyZ，那么这个函数将不存在
        sendClear(info);            // call base class function, can not run
        // log info after send
    }
};
```

3. 明确指出被调用得函数位于base class内部：

```cpp
template<typename Company>
class LoggingMsgSender:public MsgSender<Company>{
public:
    void sendClearMsg(const std::string& info){
        // log info before send
        // 如果Company是CompanyZ，那么这个函数将不存在
        MsgSender<Company>::sendClear(info);            // call base class function, can not run
        // log info after send
    }
};

LoggingMsgSender<CompanyZ> logger_z;
logger_z.sendClearMsg("hello");     // 编译错误
```


## Summary

1. 可以在drived class templates 内通过"this->"指涉base class templates内的成员名称，或通过一个明白写出的”base class资格修饰符“完成。
> 1. `using MsgSender<Company>::SendClear`
> 2. `MsgSender<Company>::SendClear(info)`