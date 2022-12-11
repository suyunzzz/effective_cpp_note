#include "./Timer.h"
class WidgetTimer : public Timer
{
public:
    WidgetTimer(int n);
    int _current_time = 9; // 需要访问，将其声明为public
    virtual void onTick() const;
};