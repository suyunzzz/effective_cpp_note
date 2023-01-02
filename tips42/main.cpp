#include "../common.h"


template<typename C>
void print2nd(const C& container){
    if(container.size()>=2){
        typename C::const_iterator iter = container.cbegin();
        ++iter;
        int value = *iter;
        std::cout << "second value: " << value << "\n";
    }
}

template<typename C>
void f(const C& container, typename C::const_iterator iter){
    std::cout << "iter's index: " << iter-container.cbegin() << "\n";
}

template <typename T>
class Drived:public Base<T>::Nested{        // base class list中不能出现typename
public:
    Drived():Base<T>::Nested(){             // mem.init.list 中不能出现typename修饰base class
        typename Base<T>:Nested temp;       // 一般性原则，需要加上typename修饰嵌套从属类型名称
    }
};

template <typename IterT>
void workWithIterator(IterT iter){
    typename std::iterator_traits<IterT>::value_type temp(*iter);
    // ...
}

template <typename IterT>
void workWithIterator(IterT iter){
    typedef typename std::iterator_traits<IterT>::value_type value_type;
    value_type temp(*iter);
    // ...
}

int main(){
    std::vector<int> vec{1,2,3,4,5};
    // print2nd(vec);

    // case2
    f(vec, vec.cbegin()+2);
    return 0;
}