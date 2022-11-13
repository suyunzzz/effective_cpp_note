// logic constness 和 bitwise constness

#include "../common.h"

using namespace std;

class BigArray{
std::vector<int> v;
mutable int count = 0;

public:
    int GetItem(int idx) const  {
        count++;
        return v[idx];
    }
};

int main(){
    BigArray ba;
    std::cout << ba.count << "\n";      // 外部仍然不可访问

    return 0;
}