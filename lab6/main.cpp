#include "liba.hpp"

void clear_input() {
  std::cin.clear();
  std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

void RSA(long long *p_ptr, long long *q_ptr, long long *d_b_ptr) {
  long long p, q, N, d, c, fi, e, m, command;
  std::string f;

  p = (p_ptr && *p_ptr > 1 && ferma(*p_ptr)) ? *p_ptr : generate();
  q = (q_ptr && *q_ptr > 1 && ferma(*q_ptr)) ? *q_ptr : generate();

  N = p * q;
  fi = (p - 1) * (q - 1);

  if (!d_b_ptr || *d_b_ptr <= 1 || gcd(false, d_b_ptr, &fi).first != 1) {
    long long g;
    do {
      d = 2 + rand() % (fi - 2);

      long long a = d, b = fi;
      g = gcd(false, &a, &b).first;
    } while (g != 1);
  } else {
    d = *d_b_ptr;
  }
  std::cout << "d_b = "<<d << "; N = " << N <<"\n";

  std::cout << "введите название файла\n";
  std::cin >> f;
  std::ifstream file(f);
  if (!file.is_open()) {
    std::cerr << "ошибка открытия файла\n";
    return;
  }

  std::cout << "1 - зашифровать;\n2 - расшифровать \n";
  std::cin >> command;

  if (command == 1) {
    file >> m;
    std::cout << m << "\n";
    e = pows(m, d, N);    
    std::cout << "зашифровано\ne = " << e << "\n";
  } else if (command == 2) {
    file >> e;
    c = gcd(false, &d, &fi).second;
    m = pows(e, c, N);
    std::cout << "расшифровано\nm = " << m << "\n";
  }

}

void menu() {
  std::cout << "Выберите команду:\n"
            << "1 - шифр RSA\n"
            << "exit - выход\n";
}

int main() {
  std::string command;

  while (true) {
    menu();
    std::cin >> command;
    clear_input();

    if (command == "exit") {
      break;
    } else if (command == "1") {
      long long p = 0, g = 0, d_b = 0;
      long long *p_ptr = nullptr;
      long long *g_ptr = nullptr;
      long long *d_b_ptr = nullptr;

      std::string line;

      std::cout << "p: ";
      std::getline(std::cin, line);
      if (!line.empty()) {
        std::stringstream ss(line);
        if (ss >> p)
          p_ptr = &p;
      }

      std::cout << "g: ";
      std::getline(std::cin, line);
      if (!line.empty()) {
        std::stringstream ss(line);
        if (ss >> g)
          g_ptr = &g;
      }

      std::cout << "d_b: ";
      std::getline(std::cin, line);
      if (!line.empty()) {
        std::stringstream ss(line);
        if (ss >> d_b)
          d_b_ptr = &d_b;
      }

      RSA(p_ptr, g_ptr, d_b_ptr);

    } else {
      std::cout << "Неизвестная команда\n\n";
    }
  }

  return 0;
}