#include "liba.hpp"

int main(){
    int a = 7;
    int x = 19;
    int p = 100;

    int y = mod_step(a,x,p);

    std::cout << "ответ: "<<y <<"\n";
}