#include "../common.h"

using namespace std;

class Timer
{
public:
    explicit Timer(int tickFrequency) : _freq(tickFrequency)
    {
    }
    virtual ~Timer()
    {
    }
    virtual void onTick() const
    { // automatically called for each tick
        for (int i = 0; i < 10; i += _freq)
        {
            printf("clock [%d/%d]\n", i, 10);
        }
    }

private:
    int _freq;

protected:
    int _current_time = 9;
};

