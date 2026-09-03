#include "../lib/liba.hpp"

int main(){
    int a = 7;
    int x = 19;
    int p = 100;
    int n1 = 67;
    int n2 = 1488;

    int y = pows(a,x,p);

    std::cout << "ответ: " << y << "\n";

    std::cout << ferma(n1) <<  " " << ferma(n2) << "\n";
}