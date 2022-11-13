#include "../common.h"

class A{
    

    int* a;

public:
    A(int* t_a):a(t_a){}
    int* get_a(){
        return a;
    }
};

int main(){

    int* pa = new int(5);
    A a(pa);
    printf("value: %d\n", *a.get_a());

    delete pa;
    printf("value: %d\n", *a.get_a());
    return 0;
}