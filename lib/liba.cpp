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

