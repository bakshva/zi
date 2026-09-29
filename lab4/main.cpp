#include "liba.hpp"

void genKeys(long long &p, long long &C_A, long long &C_B, long long &D_A,
             long long &D_B) {
  p = generate();
  while (p <= 255)
    p = generate();

  long long mod_p = p - 1;
  long long C[2], D[2];

  for (int i = 0; i < 2; i++) {
    long long g;
    do {
      C[i] = 2 + rand() % (mod_p - 2);
      long long a = C[i], b = mod_p;
      g = gcd(false, &a, &b).first;
    } while (g != 1);
  }

  for (int i = 0; i < 2; i++) {
    long long a = C[i], b = mod_p;
    std::pair<long long, long long> res = gcd(false, &a, &b);
    long long x = res.second;
    D[i] = ((x % mod_p) + mod_p) % mod_p;
  }

  C_A = C[0];
  C_B = C[1];
  D_A = D[0];
  D_B = D[1];
}

void encryptFile(const std::string &in, const std::string &out, long long p,
                 long long C_A, long long C_B) {
  std::ifstream f1(in, std::ios::binary);
  std::ofstream f2(out, std::ios::binary);

  unsigned char byte;
  while (f1.read((char *)&byte, 1)) {
    long long m = byte;
    long long x1 = pows(m, C_A, p);
    long long x2 = pows(x1, C_B, p);
    f2.write((char *)&x2, sizeof(x2));
  }
}

void decryptFile(const std::string &in, const std::string &out, long long p,
                 long long D_A, long long D_B) {
  std::ifstream f1(in, std::ios::binary);
  std::ofstream f2(out, std::ios::binary);

  long long x2;
  while (f1.read((char *)&x2, sizeof(x2))) {
    long long x3 = pows(x2, D_A, p);
    long long x4 = pows(x3, D_B, p);
    unsigned char byte = (unsigned char)x4;
    f2.write((char *)&byte, 1);
  }
}

int main() {
  srand((unsigned)time(nullptr));

  std::cout << "1 - зашифровать файл\n 2 - расшифровать файл\n> ";
  int mode;
  std::cin >> mode;

  std::cout << "ввести p, C_A, C_B вручную? (1 - да, 0 - сгенерировать)\n> ";
  int manual;
  std::cin >> manual;

  long long p, C_A, C_B, D_A, D_B;

  if (manual) {
    std::cout << "p = ";
    std::cin >> p;
    std::cout << "C_A = ";
    std::cin >> C_A;
    std::cout << "C_B = ";
    std::cin >> C_B;

    long long mod_p = p - 1;
    long long C[2] = {C_A, C_B};
    long long D[2];

    for (int i = 0; i < 2; i++) {
      long long a = C[i], b = mod_p;
      std::pair<long long, long long> res = gcd(false, &a, &b);
      long long x = res.second;
      D[i] = ((x % mod_p) + mod_p) % mod_p;
    }

    D_A = D[0];
    D_B = D[1];
  } else {
    genKeys(p, C_A, C_B, D_A, D_B);
  }

  std::cout << "p = " << p << "\nC_A = " << C_A << "\nC_B = " << C_B
            << "\nD_A = " << D_A << "\nD_B = " << D_B << "\n";

  std::string in, out;
  std::cout << "Входной файл: ";
  std::cin >> in;
  std::cout << "Выходной файл: ";
  std::cin >> out;

  if (mode == 1)
    encryptFile(in, out, p, C_A, C_B);
  else
    decryptFile(in, out, p, D_A, D_B);

  return 0;
}