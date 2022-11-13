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