#include "./widgetTimer.h"

WidgetTimer::WidgetTimer(int n) : Timer(n)
{
}
void WidgetTimer::onTick() const
{ // automatically called for each tick
    printf("WidgetTimer::onTick()\n");
    Timer::onTick();
}