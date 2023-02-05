#include "../common.h"

using namespace std;

template <typename T>
class SquareBase{
protected:  
    void invert(int n){     
        printf("invert(%d)\n", n);
    }

};

template <typename T, int n>
class Square : private SquareBase<T>
{
private:
    using SquareBase<T>::invert;
public:
    void invert()
    {
        printf("Square::invert(%d)\n", n);
        invert(n);
    }
};

int main(){
    Square<int, 3> matrix;
    matrix.invert();


    return 0;
}