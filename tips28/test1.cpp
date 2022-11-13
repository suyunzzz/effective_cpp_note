// tips28 返回私有成员为指针
#include "../common.h"

using namespace std;

class Point
{
public:
    Point(int x, int y) : _x(x), _y(y) {}
    void set_x(int a)
    {
        _x = a;
    }
    int get_x() const
    {
        return _x;
    }

    void set_y(int a)
    {
        _y = a;
    }
    int get_y() const
    {
        return _y;
    }

private:
    int _x;
    int _y;
};

struct RectData
{
    RectData(const Point &a, const Point &b) : ulhc(a), lrhc(b) {}
    Point ulhc;
    Point lrhc;
};

class Rectangle
{

public:
    Rectangle(const std::shared_ptr<RectData> &rect_data) : _rect_data(rect_data) {}

    Point &get_ul_point() const
    {
        return _rect_data->ulhc;
    }
    // 返回类内部的handle(reference, pointer, iterator)，记得要加const
    const Point &get_lr_point() const
    {
        return _rect_data->lrhc;
    }

private:
    std::shared_ptr<RectData> _rect_data;
};
int main()
{
    std::shared_ptr<RectData> rect_data = std::make_shared<RectData>(RectData({0, 0}, {100, 100}));
    Rectangle rectangle(rect_data);

    auto &p1 = rectangle.get_lr_point();
    printf("before p1: (%d, %d)\n", p1.get_x(), p1.get_y()); // before p1: (100, 100)

    p1.set_x(5);
    printf("after p1: (%d, %d)\n", p1.get_x(), p1.get_y()); // after p1: (5, 100)
}