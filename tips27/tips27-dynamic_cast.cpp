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