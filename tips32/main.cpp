#include "../common.h"
#include <exception>

struct my_exception : public std::exception
{
    char *what()
    {
        return "Penguin can not fly!\n";
    }
};

class Bird
{
public:
    virtual void fly() const
    {
        std::cout << "Bird::fly()\n";
    }

private:
};

class Penguin : public Bird
{
public:
    void fly() const override
    {
        throw my_exception();
    }

private:
};


/*
class Rect{
public:
    Rect(int h, int w):height(h), width(w){

    }
    int height() const {
        return height;
    }

    int width() const{
        return width;
    }

    void setWidth(int new_width){
        width = new_width;
        return;
    }

    virtual int getArea() const{
        return width*height;
    } 
private:
    int width;
    int height;
};

void makeBigger(Rect& r){
    int oldHeight = r.height();
    r.setWidth(r.width()+10);
    assert(r.height() == oldHeight);
}
class Square: public Rect {

}

Square s;
assert(s.width() == s.height());
makeBigger(s);
assert(s.width() == s.height());

*/
int main()
{
    Bird *pb = new Bird();
    pb->fly();
    delete pb;

    pb = new Penguin();

    try
    {
        pb->fly();
    }
    catch (my_exception &e)
    {
        std::cout << "exception: " << e.what() << "\n";
    }
    delete pb;
    pb = nullptr;

    return 0;
}