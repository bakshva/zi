#include "liba.hpp"

void clear_input() {
  std::cin.clear();
  std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

void ElGamal(long long *p_ptr, long long *g_ptr, long long *c_b_ptr) {
  long long p, g, c_b, command, m, d_i, r, e, k;
  std::string f;
  p = (p_ptr && *p_ptr >= 1 && ferma(*p_ptr)) ? *p_ptr : generate();

  if (!g_ptr || *g_ptr < 1) {
    do {
      g = 2 + rand() % (p - 2);
    } while (!isPrimitiveRoot(p, g));
  } else {
    g = *g_ptr;
  }
  c_b = (c_b_ptr && *c_b_ptr >= 1) ? *c_b_ptr : 2 + rand() % (p - 3);

  std::cout << "введите название файла\n";
  std::cin >> f;
  std::ifstream file(f);
  if (!file.is_open()) {
    std::cerr << "ошибка открытия файла\n";
    return;
  }

  d_i = pows(g, c_b, p);

  std::cout << "1 - зашифровать;\n2 - расшифровать \n";
  std::cin >> command;

  if (command == 1) {
    file >> m;
    std::cout
        << "введите k (или нажмите Enter, чтобы сгенерировать случайно)\n";
    clear_input(); // очищаем буфер/флаги перед getline

    std::string line;
    std::getline(std::cin, line);

    if (line.empty()) {
      k = 1 + rand() % (p - 2);
    } else {
      k = std::stoi(line);
    }
    std::cout << "k = " << k << "\n";
    r = pows(g, k, p);
    e = pows(m * pows(d_i, k, p), 1, p);
    std::cout << "зашифровано\nr = " << r << "\ne = " << e << "\n";
  } else if (command == 2) {
    file >> r >> e;
    m = pows(e * pows(r, p - 1 - c_b, p), 1, p);
    std::cout << "расшифровано\nm = " << m << "\n";
  }
}

void menu() {
  std::cout << "Выберите команду:\n"
            << "1 - шифр Эль-Гамаля\n"
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
      long long p = 0, g = 0, c_b = 0;
      long long *p_ptr = nullptr;
      long long *g_ptr = nullptr;
      long long *c_b_ptr = nullptr;

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

      std::cout << "c_b: ";
      std::getline(std::cin, line);
      if (!line.empty()) {
        std::stringstream ss(line);
        if (ss >> c_b)
          c_b_ptr = &c_b;
      }

      ElGamal(p_ptr, g_ptr, c_b_ptr);

    } else {
      std::cout << "Неизвестная команда\n\n";
    }
  }

  return 0;
}
