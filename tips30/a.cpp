#include "../common.h"

class Base{
public:

private:
    std::string a,b;
};

class Drived : public Base{
public:
    Drived(){}      // 隐式inline
private:
    std::string c, d, e;
};

Drived::Drived(){
    Base::Base();
    try{
        dml.std::string::string();
    } catch(...){
        Base::~Base();
        throw;
    }
    try{
        dm2.std::string::string();      // 如果构造失败，则对已经构造好的对象进行析构
    } catch(...){
        ~dml.std::string::string();
        Base::~Base();
        throw;
    }
    try{
        dm3.std::string::string();
    } catch(...){
        ~dm2.std::string::string();
        ~dm1.std::string::string();
        Base::~Base();
        throw;
    }
}

int main(){
    Drived d;


    return 0;
}

