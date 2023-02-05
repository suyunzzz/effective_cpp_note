#include "../common.h"

using namespace std;

template <bool b>
class algorithm_selector{
public:
    template <typename T>
    static void implementation(const T& obj);
    
};

template <bool b>
template <typename T>
void algorithm_selector<b>::implementation(const T& obj){
    printf("default implementation\n");
}

template <>
class algorithm_selector<true>{
public:
    template <typename T>
    static void implementation(const T& obj);
};

template <typename T>
void algorithm_selector<true>::implementation(const T& obj){
    printf("optimized implementation\n");
    obj.optimized_algorithm();
}

// just OnjectB can run optimized_algorithm
class ObjectA{
public:
};

class ObjectB{
public:
    void optimized_algorithm() const{
        printf("ObjectB::optimized_algorithm\n");
    }
};

// traits, just for ObjectB is true, others are false
template <typename T>
struct supports_optimised_implementation{
    static const bool value = false;
};

template <>
struct supports_optimised_implementation<ObjectB>{
    static const bool value = true;
};

template <typename T>
void algorithm( T& object ) {
    algorithm_selector<supports_optimised_implementation<T>::value>::implementation(object);
}


int main(){
    printf("test ObjectA\n");
    ObjectA a;
    algorithm(a);
    
    printf("test ObjectB\n");
    ObjectB b;
    algorithm(b);
    return 0;
}