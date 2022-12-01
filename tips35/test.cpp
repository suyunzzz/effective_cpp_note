#include "../common.h"

using namespace std;
class GameCharacter;

class Calculator
{
public:
    virtual int calc(const GameCharacter *gc) const;
};

class SlowCalculator : public Calculator
{
public:
    int calc(const GameCharacter *gc) const override;
};

Calculator *defaultHealthCalc = new Calculator();
class GameCharacter
{
public:
    explicit GameCharacter(Calculator *hcf = defaultHealthCalc) : _healthFunc(hcf)
    {
    }
    void setHealth(int a)
    {
        _health_value = a;
    }
    int healthValue() const
    {
        // ...
        int value = _healthFunc->calc(this);
        return value;
    }

    int getHealth() const
    {
        return _health_value;
    }

private:
    Calculator *_healthFunc;

protected:
    int _health_value = 10;
};

class person : public GameCharacter
{
public:
    person(int a, Calculator *calc) : GameCharacter(calc)
    {
        _health_value = a;
    }
};

inline int Calculator::calc(const GameCharacter *gc) const
{
    printf("Calculator::calc()\n");
    return gc->getHealth();
}

inline int SlowCalculator::calc(const GameCharacter *gc) const
{
    printf("SlowCalculator::calc()\n");
    return gc->getHealth() - 1;
}

int main()
{
    GameCharacter obj1;
    int v = obj1.healthValue();
    printf("obj1's health value: %d\n", v);

    Calculator *slow_calc = new SlowCalculator();
    GameCharacter obj2(slow_calc);
    v = obj2.healthValue();
    printf("obj2's health value: %d\n", v);

    GameCharacter *obj3 = new person(5, slow_calc);
    v = obj3->healthValue();
    printf("obj3's health value: %d\n", v);

    return 0;
}