#include "../common.h"

using namespace std;

template <typename T>
class Rational;

template <typename T>
Rational<T> helper(const Rational<T>& lhs, const Rational<T>& rhs){
    return Rational<T>(lhs.numerator()*rhs.numerator(), lhs.denominator()*rhs.denominator());
}

template <typename T>
class Rational{
public: 
    Rational(T n = 0, T d = 1):_numerator(n),_denominator(d){
        ;
    }
    T numerator() const{
        return _numerator;
    }
    T denominator() const{
        return _denominator;
    }

    friend Rational operator*(const Rational& lhs, const Rational& rhs){
        return helper(lhs, rhs);
    }
private:
    T _numerator;
    T _denominator;
};

int main(){
    Rational<int> oneHalf(1, 2);

    Rational<int> result = oneHalf * oneHalf;   // OK
    result = oneHalf * 2;                  // OK
    result = 2 * oneHalf;                  // OK

    return 0;
}