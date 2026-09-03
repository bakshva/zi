#include "liba.hpp"

int mod_step(int a, int x, int p) {
  std::vector<int> mods;
  std::string x_bin = std::format("{:b}", x);
  std::cout << x << "(10) = "<< x_bin << "(2)\n";
  std::reverse(x_bin.begin(), x_bin.end());
  int result = 1;
  for (size_t i = 0; i < x_bin.length(); i++) {
    int mod_i;
    std::cout << i << ") ";
    if (i == 0) {
      mod_i = a % p;
    std::cout << a << " % " << p << " = " << mod_i << "\n";
    } else {
      mod_i = (mods[i - 1] * mods[i - 1]) % p;
      std::cout << mods[i - 1] * mods[i - 1] << " % " << p << " = " << mod_i << "\n";
    }
    
    mods.push_back(mod_i);
    if (x_bin[i] == '1') {
      result = result * mod_i;
    }
  }

  std::cout<< result << " % " << p << " = " << result%p << "\n";
  return result % p;
}

int lit_big(int a, int y, int p){
    int root = 1;
    std::vector<int> lit;
    std::vector<int> big;
    std::vector<int> result;
    while (root * root < p) {
        root = root + 1;
    }
    int m = 0;
    int k = 0;
    if (root * root == p) {
        m = root;
        k = root;
    } 
    else {
        int sum = root + root;
        m = 1;
        k = sum - m;

        while (m * k <= p) {
            m = m + 1;
            k = sum - m;
        }
    }
    std::cout << "m = " << m << "\n";
    std::cout << "k = " << k << "\n";

for (int j = 0; j < m; j++){
int x = (y * static_cast<unsigned int>(pow(a, j)))%p;
std::cout << "lit " << x << "\n";
lit.push_back(x);
}

for (int i = 1; i < k+1; i++){
int x = static_cast<unsigned int>(pow(a, i*m))%p;
std::cout << "big " << x << "\n";
big. push_back(x);
}


int count = 1;
    for (size_t j = 0; j < lit.size(); j++){
        for (size_t i = 0; i < big.size(); i++){
            if (lit[j]==big[i]){
                int x = (i+1)*m - j;
                std::cout << "X(" << count << ") = " << x << "\n";
                result.push_back(x);
                count++;
            }
        }
    }

    return 0;
}