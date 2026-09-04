#include "liba.hpp"

long long pows(long long a, long long x, long long p) {
  std::vector<int> mods;
  std::string x_bin = std::format("{:b}", x);
  std::reverse(x_bin.begin(), x_bin.end());
  long long result = 1;
  for (size_t i = 0; i < x_bin.length(); i++) {
    int mod_i;
    if (i == 0) {
      mod_i = a % p;

    } else {
      mod_i = (mods[i - 1] * mods[i - 1]) % p;
    }

    mods.push_back(mod_i);
    if (x_bin[i] == '1') {
      result = result * mod_i;
    }
  }

  return result % p;
}

bool ferma(long long p) {
  if (p == 2)
    return true;

  for (size_t i = 0; i < 100; i++) {
    long long a = (rand() % (p - 2)) + 2;
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
long long generate() {
  while (true) {
    long long prime = (rand() % 1000000) + 2;

    if (ferma(prime) == 1)
      return prime;
  }
}

long long gcd(bool mode, long long a, long long b) {
  long long result = 0;

  if (a == -1 || b == -1) {
    if (mode) {
      a = generate();
      b = generate();
    } else {
      a = (rand() % 1000000) + 2;
      b = (rand() % 1000000) + 2;
    }
  }

  if (a < b) {
    long long temp = a;
    a = b;
    b = temp;
  }

  std::vector<long long> u = {a, 1, 0};
  std::vector<long long> v = {b, 0, 1};

  while (v[0]) {
    long long q = u[0] / v[0];
    std::vector<long long> t = {u[0] % v[0], u[1] - q * v[1], u[2] - q * v[2]};
    for (size_t i = 0; i < u.size(); ++i) {
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

void lit_big(long long a, long long y, long long p) {
  long long root = 1;
  std::vector<long long> lit;
  std::vector<long long> big;
  std::vector<long long> result;
  while (root * root < p) {
    root = root + 1;
  }
  long long m = 0;
  long long k = 0;
  if (root * root == p) {
    m = root;
    k = root;
  } else {
    long long sum = root + root;
    m = 1;
    k = sum - m;

    while (m * k <= p) {
      m = m + 1;
      k = sum - m;
    }
  }
  std::cout << "m = " << m << "\n";
  std::cout << "k = " << k << "\n";

  for (long long j = 0; j < m; j++) {
    long long x = (pows(y, 1, p) * pows(a, j, p)) % p;
    lit.push_back(x);
  }

  for (long long i = 1; i < k + 1; i++) {
    long long x = pows(a, m * i, p);
    big.push_back(x);
  }

  std::unordered_map<long long, std::vector<size_t>> pos;
  for (size_t i = 0; i < big.size(); ++i)
    pos[big[i]].push_back(i);

  int count = 1;
  for (size_t j = 0; j < lit.size(); ++j) {
    auto it = pos.find(lit[j]);
    if (it != pos.end()) {
      for (size_t i : it->second) {
        long long x = (i + 1) * m - static_cast<long long>(j);
        std::cout << "X(" << count << ") = " << x << "\n";
        result.push_back(x);
        ++count;
      }
    }
  }
}