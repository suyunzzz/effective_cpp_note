#include "../common.h"

using namespace std;

void print_num(int t_num=0);

int main(){
    print_num();
    print_num(1);
    return 0;   
}

void print_num(int t_num){
    printf("current num: %d\n", t_num);
    return;
}