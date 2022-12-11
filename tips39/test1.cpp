#include "../common.h"
#include "./widgetTimer.h"

using namespace std;



/*
class Widget : private Timer
{

public:
    Widget(int n) : Timer(n) {}
    void triger()
    {
        printf("_current_time: %d\n", _current_time); // 访问base class的protectd成员
        onTick();
    }

private:
    virtual void onTick() const final
    {                                 // look at Widget usage data, etc.
        printf("Widget::onTick()\n"); // 重写onTick函数
        Timer::onTick();
    }

private:
    // int _freq=3;
};

class DWidget:public Widget{
public:
    DWidget(int n):Widget(n),_freq(n){

    }
private:
    virtual void onTick() const {          
        printf("DWidget::onTick()\n");
        for (int i = 0; i < 10; i += _freq)
        {
            printf("clock [%d/%d]\n", i, 10);
        }
    }   

    int _freq;

};
*/


class Widget
{
public:
    Widget(int n) : wTimer(new WidgetTimer(n))
    {
    }

    void triger()
    {
        printf("_current_time: %d\n", wTimer->_current_time); 
        wTimer->onTick();
    }

private:
    WidgetTimer* wTimer;
};


int main()
{
    Widget w(5);
    w.triger();

    return 0;
}