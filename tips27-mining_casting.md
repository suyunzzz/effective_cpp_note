# Tips27 尽可能少做转型动作（minimize casting）


> 本文将从1.访问权限 , 2.static_cast, 3.dynamic_cast问题及其避免方式进行介绍

[TOC]

## 访问权限


C++的访问权限如下：

基类的protected成员才可被派生类访问, privated成员是不可以的（只能被当前类访问）

当我们需要在基类中为派生类预留成员变量时，可以这样做：

```cpp
#include <iostream>

using namespace std;

class Window{
public:
    virtual void Resize(){
        size = 4;
        std::cout << "Window::Resize()\n";
    }
protected:
    int size = 2;
};

class SubWindow:public Window{
public:
    void Resize() override{
        size = 3;
        std::cout << "SubWindow::Resize()\n";
    }
};

int main(){
    Window* p_window = new SubWindow();
    p_window->Resize();
    delete p_window;
    return 0;
}

```

## static_cast

**派生类对象想调用基类的方法，在对应的虚函数override内这样写是不对的（其本质是对副本的调用）：**

```cpp

#include <iostream>

using namespace std;

class Window{
public:
    virtual void Resize(){
        size = 4;
        std::cout << "Window::Resize()\n";
    }
    int getSize(){
        return size;
    }
protected:
    int size = 2;
};

class SubWindow:public Window{
public:
    void Resize() override{
        static_cast<Window>(*this).Resize(); // 调用对象是当前对象的副本
    }
};

int main(){
    Window* p_window = new SubWindow();
    std::cout << "before resize, size: " << p_window->getSize() << "\n";
    p_window->Resize();
    std::cout << "size: " << p_window->getSize() << "\n";

    delete p_window;
    return 0;
}

output:
before resize, size: 2
Window::Resize()
size: 2
```

正确的做法应该是**直接调用基类的方法**:

```cpp
#include <iostream>
 
using namespace std;
 
class Window{
public:
    virtual void Resize(){
        size = 4;
        std::cout << "Window::Resize()\n";
    }
    int getSize(){
        return size;
    }
protected:
    int size = 2;
};
class SubWindow:public Window{
public:
    void Resize() override{
        Window::Resize();
    }
};
 
int main(){
    Window* p_window = new SubWindow();
    std::cout << "before resize, size: " << p_window->getSize() << "\n";
    p_window->Resize();
    std::cout << "size: " << p_window->getSize() << "\n";
 
    delete p_window;
    return 0;
}

output:
before resize, size: 2
Window::Resize()
size: 4

```

## dynameic_cast

使用场景
向下转换，也就是***基类指针或者引用转为派生类的指针或者引用***，如果转换成功会返回目标类型的指针，如果转换失败会返回空指针或者抛出异常。

pointor转换成功

```cpp

#include <iostream>
 
using namespace std;
 
class Window{
public:
    virtual void Resize(){
        size = 4;
        std::cout << "Window::Resize()\n";
    }
    void PrintSize(){
        printf("Window.size: %d\n", size);
    }
   
protected:
    int size = 2;
};
 
class SubWindow:public Window{
public:
    void Resize() override{
        Window::Resize();
    }
    void PrintSize(){
        printf("SubWindow.size: %d\n", size);
    }
};
 
int main(){
    Window* p_w = new SubWindow();
    p_w->PrintSize();
    printf("===> after dynamic_cast<SubWindow*>()\n");
    SubWindow* p_sw = dynamic_cast<SubWindow*>(p_w);
    p_sw->PrintSize();
    return 0;
}

output:
Window.size: 2
===> after dynamic_cast<SubWindow*>()
SubWindow.size: 2

```

### pointor转换失败

```cpp
#include <iostream>
 
using namespace std;
 
class Window{
public:
    virtual void Resize(){
        size = 4;
        std::cout << "Window::Resize()\n";
    }
    void PrintSize(){
        printf("Window.size: %d\n", size);
    }
   
protected:
    int size = 2;
};
 
class SubWindow:public Window{
public:
    void Resize() override{
        Window::Resize();
    }
    void PrintSize(){
        printf("SubWindow.size: %d\n", size);
    }
};
 
int main(){
    Window* p_w = new Window();
    p_w->PrintSize();
    printf("===> after dynamic_cast<SubWindow*>()\n");
    SubWindow* p_sw = dynamic_cast<SubWindow*>(p_w);
    if(p_sw){
        p_sw->PrintSize();
    } else {
        printf("Failed to dynamic_cast\n");
    }
    return 0;
}

output:
Window.size: 2
===> after dynamic_cast<SubWindow*>()
Failed to dynamic_cas
```

### reference类型转换

```cpp
#include <iostream>
 
using namespace std;
 
class Window{
public:
    virtual void Resize(){
        size = 4;
        std::cout << "Window::Resize()\n";
    }
    void PrintSize(){
        printf("Window.size: %d\n", size);
    }
   
protected:
    int size = 2;
};
 
class SubWindow:public Window{
public:
    void Resize() override{
        Window::Resize();
    }
    void PrintSize(){
        printf("SubWindow.size: %d\n", size);
    }
};
 
int main(){
    SubWindow sub_w;
    Window& r_w = sub_w;
    printf("===> before dynamic_cast\n");
    r_w.PrintSize();
 
    printf("===> after dynamic_cast\n");
    SubWindow& r_sw = dynamic_cast<SubWindow&>(r_w);
    r_sw.PrintSize();
    return 0;
}
output:
===> before dynamic_cast
Window.size: 2
===> after dynamic_cast
SubWindow.size: 2

```

### reference dynamic_cast 失败

```cpp
#include <iostream>
 
using namespace std;
 
class Window{
public:
    virtual void Resize(){
        size = 4;
        std::cout << "Window::Resize()\n";
    }
    void PrintSize(){
        printf("Window.size: %d\n", size);
    }
   
protected:
    int size = 2;
};

class SubWindow:public Window{
public:
    void Resize() override{
        Window::Resize();
    }
    void PrintSize(){
        printf("SubWindow.size: %d\n", size);
    }
};
 
int main(){
    Window sub_w;
    Window& r_w = sub_w;
    printf("===> before dynamic_cast\n");
    r_w.PrintSize();
 
    printf("===> after dynamic_cast\n");
    try{
        SubWindow& r_sw = dynamic_cast<SubWindow&>(r_w);
        r_sw.PrintSize();
    }
    catch (const std::exception& e){
        printf("Failed dynamic_cast");
        std::cout << e.what() << "\n";
    }
    return 0;
}
output:
===> before dynamic_cast
Window.size: 2
===> after dynamic_cast
Failed dynamic_caststd::bad_cast
```

***dynamic_cast会逐级比较类名，所以应该尽量少用***




**针对dynamic_cast的问题，为了提升性能，避免dynamic_cast，有以下解决方法：**

1. **直接使用子类的容器而不是父类的容器， 比如：**

```cpp
vector<Window> v;
dynamic_cast<SubWindow>(v[0]).blink(); // blink()为SubWindow特有的方法

转换为如下：
vector<SubWindow> v;
v[0].blink();
```

2. **父类使用虚函数预留接口，而不实现**

> 假如在父类中使用虚函数预留接口，那么我们需要做一次dyanmic_cast才能调用派生类的”特有方法“（这里指PrintSize）

```cpp

# non-virtual
#include <iostream>

using namespace std;

class Window{
public:
    virtual void Resize(){
        size = 4;
        std::cout << "Window::Resize()\n"; 
    }
    void PrintSize()  {    // 仅仅为了编译不报错
    }
    
protected:
    int size = 2;
};

class SubWindow:public Window{
public:
    void Resize() override{
        Window::Resize();
    }
    void PrintSize()  {
        printf("SubWindow.size: %d\n", size);
    }
};

int main(){
    SubWindow sub_w;
    Window& r_w = sub_w;
    printf("===> before dynamic_cast\n");
    r_w.PrintSize();            // non-virtual function, call Window::PrintSize()

    printf("===> after dynamic_cast\n");
    try{
        SubWindow& r_sw = dynamic_cast<SubWindow&>(r_w);
        r_sw.PrintSize();
    }
    catch (const std::exception& e){
        printf("Failed dynamic_cast");
        std::cout << e.what() << "\n";
    }
    return 0;
}
output:
===> before dynamic_cast
===> after dynamic_cast
SubWindow.size: 2

```

> **但是如果我们在基类中使用虚函数为派生类的特有方法预留了接口，那么就可以避免dynamic_cast:**

```cpp

#include <iostream>

using namespace std;

class Window{
public:
    virtual void Resize(){
        size = 4;
        std::cout << "Window::Resize()\n"; 
    }
    virtual void PrintSize()  {
        printf("do nothing");
    }
    
protected:
    int size = 2;
};

class SubWindow:public Window{
public:
    void Resize() override{
        Window::Resize();
    }
    void PrintSize() override {
        printf("SubWindow.size: %d\n", size);
    }
};

int main(){
    SubWindow sub_w;
    Window& r_w = sub_w;
    printf("===> before dynamic_cast\n");
    r_w.PrintSize();        // no dynamic_cast
    return 0;
}
output:
===> before dynamic_cast
SubWindow.size: 2
```

## summary
1. 尽可能避免转型，尽量避免dynamic_cast这种

2. 把转型这一过程放在函数中，而不是交给客户去做

3. 尽可能使用C++风格的转型代码。

## reference
1. [Effective C++ 27：最小化类型转换](https://harttle.land/2015/08/25/effective-cpp-27.html)
2. [C++ dynamic_cast的使用_hp_cpp的博客](https://blog.csdn.net/hp_cpp/article/details/104095700)





