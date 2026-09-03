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
    
    for (size_t i = 0; i < 100; i++)
    {
        long long a = (rand() % (p-2)) + 2;
        if (gcd(0, a, p) != 1)
            return false;

        if (pows(a, p - 1, p) != 1)
            return false;
    }

    return true;
}

/*пока ничего лучше отдельной функции генерации с while не придумал
сложно придумать что-то более изящное чтобы генерировать число
и при этом чтобы оно сразу было простым, надо кумекать*/
long long generate(){
    while (true){
        long long prime = (rand() % 1000000) + 2;
         
        if (ferma(prime) == 1)
            return prime;
    }
}

long long gcd(bool mode, long long a, long long b) {
	long long result = 0;

    if (a == -1 || b == -1){
        if (mode){
            a = generate();
            b = generate();
        } else {
            a = (rand() % 1000000) + 2;
            b = (rand() % 1000000) + 2;
        }  
    }

    if (a < b){
        long long temp = a;
        a = b;
        b = temp;
    }

    std::vector<long long> u = {a, 1, 0};
    std::vector<long long> v = {b, 0, 1};

    while(v[0]) {
        long long q = u[0] / v[0];
        std::vector<long long> t = {u[0] % v[0], u[1] - q * v[1], u[2] - q * v[2]};
        for(size_t i = 0; i < u.size(); ++i) {
            u[i] = v[i];
            v[i] = t[i];
        }
    }

    a = u[1];
    b = u[2];
    result = u[0];

	return result; /* просто НОД но если возвращать еще и а и б 
                    то нужно либо через структуру либо переделывать
                    чтобы функция возвращала vector, переделаю */
}