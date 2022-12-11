#include "../common.h"

using namespace std;

enum class Color
{
    RED,
    GREEN,
    NUM
};

std::map<Color, std::string> color2string = {{Color::RED, "Red"}, {Color::GREEN, "Green"}};

class B
{
public:
    void doDraw(Color color=Color::RED){
        draw(color);
    }
private:
    virtual void draw(Color color)
    {
        printf("draw(%s)\n", color2string.at(color).c_str());
    }
};

class D : public B
{
private:
    virtual void draw(Color color) override 
    {
        printf("draw(%s)\n", color2string.at(color).c_str());
    }
};

class D2 : public B
{
private:
    virtual void draw(Color color) override
    {
        printf("draw(%s)\n", color2string.at(color).c_str());
    }
};

int main()
{
    B b;
    D d;
    D2 d1;
    B *pb1 = &b;
    B *pb2 = &d;
    B *pb3 = &d1;

    // 通过对象调用
    b.doDraw();              // RED
    d.doDraw(Color::GREEN);              // GREEN
    d1.doDraw(Color::GREEN); // GREEN
    cout << "====\n";

    pb1->doDraw(); // RED
    pb2->doDraw(); // RED
    pb3->doDraw(); // RED
    return 0;
}