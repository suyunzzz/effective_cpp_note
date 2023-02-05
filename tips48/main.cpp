#include "../common.h"

template <unsigned n>
struct Factorial{
    enum {value = n * Factorial<n-1>::value};
};

template <>
struct Factorial<0>{
    enum {value = 1};
};

int main(){
    printf("Factorial<5>::value: %d\n", Factorial<5>::value);
    return 0;
}