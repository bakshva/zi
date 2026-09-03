#include "liba.hpp"

long long pows(long long a, long long x, long long p){
    std::vector<int> mods;
    std::string x_bin = std::format("{:b}", x);
    std::reverse(x_bin.begin(), x_bin.end()); 
    long long result =1;
    for (size_t i =0; i<x_bin.length(); i++){
        int mod_i;
        if (i == 0){
            mod_i = a%p;
            
        }else{
            //std::cout<<mods[i-1]<<"\n";
            mod_i = (mods[i-1]*mods[i-1])%p;
        }

        //std::cout<<"шаг "<< i<<" "<<mod_i<<"\n";
        mods.push_back(mod_i);
        if (x_bin[i] == '1'){
        result =result*mod_i;
    }
    }

    return result%p;
}

bool ferma(long long p){
    if (p == 2)
        return true;
    
    srand(time(NULL));
    for (size_t i = 0; i < 100; i++)
    {
        long long a = (rand() % (p-2)) + 2;
        if (std::gcd(a, p) != 1)
            return false;

        if (pows(a, p - 1, p) != 1)
            return false;
    }
    
    return true;
}