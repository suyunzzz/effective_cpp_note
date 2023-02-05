#include "../common.h"

class Top{};
class Middle: public Top{};
class Bottom: public Middle{};

template <typename T>
class SmartPtr{
public:
    SmartPtr(T* ptr):_ptr(ptr) {
        std::cout << "SmartPtr::SmartPtr constructor\n";
    }

    template <typename U>
    SmartPtr(const SmartPtr<U>& other):_ptr(other.get()){
        std::cout << "SmartPtr::SmartPtr generalize copy constructor\n";
    }

    SmartPtr(const SmartPtr& other){
        std::cout << "SmartPtr::SmartPtr copy constructor\n";
    }

    T* get() const{
        return _ptr;
    }
private:
    T* _ptr=nullptr;
};

int main(){

    SmartPtr<Top> s_ptr = SmartPtr<Bottom>(new Bottom);
    std::cout << "===\n";
    SmartPtr<Top> p(s_ptr);

    return 0;
}