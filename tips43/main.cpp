#include "../common.h"

class CompanyA{
public:
    void sendCleartext(const std::string& msg){
        std::cout << "CompanyA::sendCleartext, " << msg << "\n";
    }
    void sendEncrypted(const std::string& msg){
        std::cout << "CompanyA::sendEncrypted, " << msg << "\n";
    }
};


class CompanyB{
public:
    void sendCleartext(const std::string& msg){
        std::cout << "CompanyB::sendCleartext, " << msg << "\n";
    }
    void sendEncrypted(const std::string& msg){
        std::cout << "CompanyB::sendEncrypted, " << msg << "\n";
    }
};

class CompanyZ{
public:
    void sendEncrypted(const std::string& msg){
        std::cout << "CompanyZ::sendEncrypted, " << msg << "\n";
    }
};

template<typename Company>
class MsgSender{
public:
    void sendClear(const std::string& info){
        Company c;
        c.sendCleartext(info);
    }
    void sendSecret(const std::string& info){
        Company c;
        c.sendEncrypted(info);
    }
};

// 以下被称为模板全特化，即MsgSender得参数是CompanyZ，且没有其他template参数可供变化
template<>
class MsgSender<CompanyZ>{
public:
    void sendSecret(const std::string& info){
        CompanyZ c;
        c.sendEncrypted(info);
    }

};

template<typename Company>
class LoggingMsgSender:public MsgSender<Company>{
public:
    void sendClearMsg(const std::string& info){
        // log info before send
        // 如果Company是CompanyZ，那么这个函数将不存在
        MsgSender<Company>::sendClear(info);            // call base class function, can not run
        // log info after send
    }
};


int main(){

    LoggingMsgSender<CompanyA> logger_a;
    logger_a.sendClearMsg("hello");

    LoggingMsgSender<CompanyZ> logger_z;
    logger_z.sendClearMsg("hello");     // 编译错误
    
    return 0;
}