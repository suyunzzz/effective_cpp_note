#include "../common.h"

class Widget{
public:
    Widget();
    virtual ~Widget();
    virtual std::size_t size() const;
    virtual void normalize();
    void swap(Widget& other);

};

void doProcessing(Widget& w){
    if(w.size()>10 && w!=someNastyWidget){
        Widget temp(w);
        temp.normalize();
        temp.swap(w);
    }
}

template <typename T>
void doProcessing(T& w){
    if(w.size()>10 && w!=someNastyWidget){
        T temp(w);
        temp.normalize();
        temp.swap(w);
    }
}

int main(){

    return 0;
}