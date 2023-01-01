#include "../common.h"

using namespace std;

/*
class A{
public:
    void check(){
        printf("A::check()\n");
    }
};

class B{
private:
    void check(){
        printf("A::check()\n");
    }
};

class A_B : public A, public B{
public:
    void call_check(){
        check();          
    }
};
*/

/*
class File{
public:
    File(int a):a(a){}
    int a;
};

class InFile:  public File{
public:
    InFile():File(1){}
};

class OutFile: virtualpublic File{
public:
    OutFile():File(0){}
};

class IOFile:public InFile, public OutFile{
public:
    void print(){
        std::cout << "InFile::a: " << InFile::a << ", OutFile::a: " << OutFile::a << "\n";
    }
};
*/

class IPerson{
public:
    IPerson(){}
    virtual ~IPerson(){}
    virtual std::string name() const = 0;
    virtual std::string data() const = 0;
};

const static std::vector<const char* > vec_of_name{"yunzheng", "ArriettyRain"};
const static std::vector<const char* > vec_of_data{"1997", "1998"};

class PersonInfo{
public:
    PersonInfo(int ID):_id(ID){}
    virtual ~PersonInfo(){}
    virtual const char* theName() const{
        return vec_of_name[_id];
    }
    virtual const char* theData() const{
        return vec_of_data[_id];
    }
    virtual const char* valueDelimOpen() const{
        return "{";
    }
    virtual const char* valueDelimClose() const{
        return "}";
    }
private:
    int _id;
};

class CPerson:public IPerson, private PersonInfo{
public:
    CPerson(int ID):PersonInfo(ID){}
    std::string name() const override{
        std::string name;
        name = name+valueDelimOpen()+std::string(theName())+valueDelimClose();
        return name;
    }
    std::string data() const override{
        std::string data;
        data = data+PersonInfo::valueDelimOpen()+std::string(theData())+PersonInfo::valueDelimClose();
        return data; 
    }
private:
    const char* valueDelimClose() const override{
        return "]";
    }
    const char* valueDelimOpen() const override{
        return "[";
    }

};





int main(){
    // A_B ab;
    // ab.call_check();

    // case2
    // IOFile io_file;
    // io_file.print();
    // printf("sizeof io_file without virtual inheritance: %ld\n", sizeof(io_file));
    // return 0;

    // case3
    IPerson* person_p = new CPerson(0);
    std::cout << "person_p.name: " << person_p->name() << "\n";
    std::cout << "person_p.data: " << person_p->data() << "\n";

    CPerson person_ref(1);
    IPerson& person_r  = person_ref;
    std::cout << "person_r.name: " << person_r.name() << "\n";
    std::cout << "person_r.data: " << person_r.data() << "\n";
}
// output
// person_p.name: [yunzheng]
// person_p.data: {1997}
// person_r.name: [ArriettyRain]
// person_r.data: {1998}   