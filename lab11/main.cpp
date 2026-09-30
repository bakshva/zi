#include "liba.hpp"
#include "md5.hpp"

void clear_input() {
  std::cin.clear();
  std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

void DSA(long long *p_ptr, long long *q_ptr, long long *g_ptr, long long *x_ptr) {
  long long p, q, g, x, y, command;
  std::string f;

  q = (q_ptr && *q_ptr > 1 && ferma(*q_ptr)) ? *q_ptr : generate();

  if (p_ptr && *p_ptr > q && ferma(*p_ptr) && (*p_ptr - 1) % q == 0) {
    p = *p_ptr;
  } else {
    long long t;
    do {
      t = 2 * (1 + rand() % 500);
      p = q * t + 1;
    } while (!ferma(p));
  }

  if (g_ptr && *g_ptr > 1 && *g_ptr < p && pows(*g_ptr, q, p) == 1) {
    g = *g_ptr;
  } else {
    do {
      long long h = 2 + rand() % (p - 3);
      g = pows(h, (p - 1) / q, p);
    } while (g <= 1);
  }

  x = (x_ptr && *x_ptr >= 1 && *x_ptr < q) ? *x_ptr : 1 + rand() % (q - 1);
  y = pows(g, x, p);

  std::cout << "p = " << p << "\n";
  std::cout << "q = " << q << "\n";
  std::cout << "g = " << g << "\n";
  std::cout << "x = " << x << "\n";
  std::cout << "y = " << y << "\n";

  std::cout << "введите название файла\n";
  std::cin >> f;

  std::ifstream file(f, std::ios::binary | std::ios::ate);
  if (!file.is_open()) {
    std::cerr << "ошибка открытия файла\n";
    return;
  }

  std::streamsize size = file.tellg();
  file.seekg(0, std::ios::beg);

  std::vector<char> buffer(size);
  if (size > 0 && !file.read(buffer.data(), size)) {
    std::cerr << "ошибка чтения файла\n";
    return;
  }

  auto hash = md5(buffer);

  std::cout << "1 - подписать;\n2 - проверить \n";
  std::cin >> command;

  if (command == 1) {
    long long H = 0;
    for (uint8_t byte : hash) {
      H = (H * 256 + byte) % q;
    }

    long long k, r, s;
    while (true) {
      k = 1 + rand() % (q - 1);

      r = pows(g, k, p) % q;
      if (r == 0)
        continue;

      long long kk = k, qq = q;
      long long kinv = ((gcd(false, &kk, &qq).second % q) + q) % q;

      s = (kinv * ((H + x * r) % q)) % q;
      if (s == 0)
        continue;

      break;
    }

    std::string sigFilename = f + ".sig";
    std::ofstream sigFile(sigFilename);
    if (!sigFile.is_open()) {
      std::cerr << "ошибка создания файла подписи\n";
      return;
    }

    sigFile << p << " " << q << " " << g << " " << y << "\n";
    sigFile << r << " " << s << "\n";

    std::cout << "подписано\nподпись сохранена в " << sigFilename << "\n";

  } else if (command == 2) {
    std::string sigFilename;
    std::cout << "введите название файла подписи\n";
    std::cin >> sigFilename;

    std::ifstream sigFile(sigFilename);
    if (!sigFile.is_open()) {
      std::cerr << "ошибка открытия файла подписи\n";
      return;
    }

    long long pp, qq, gg, yy, r, s;
    if (!(sigFile >> pp >> qq >> gg >> yy) || !(sigFile >> r >> s)) {
      std::cerr << "некорректный файл подписи\n";
      return;
    }

    if (r <= 0 || r >= qq || s <= 0 || s >= qq) {
      std::cout << "подпись неверна\n";
      return;
    }

    long long H = 0;
    for (uint8_t byte : hash) {
      H = (H * 256 + byte) % qq;
    }

    long long ss = s, mm = qq;
    long long w = ((gcd(false, &ss, &mm).second % qq) + qq) % qq;

    long long u1 = (H * w) % qq;
    long long u2 = (r * w) % qq;
    long long v = ((pows(gg, u1, pp) * pows(yy, u2, pp)) % pp) % qq;

    if (v == r) {
      std::cout << "подпись верна\n";
    } else {
      std::cout << "подпись неверна\n";
    }
  }
}

void menu() {
  std::cout << "Выберите команду:\n"
            << "1 - ЭЦП FIPS 186\n"
            << "exit - выход\n";
}

int main() {
  srand((unsigned)time(nullptr));
  std::string command;

  while (true) {
    menu();
    std::cin >> command;
    clear_input();

    if (command == "exit") {
      break;
    } else if (command == "1") {
      long long p = 0, q = 0, g = 0, x = 0;
      long long *p_ptr = nullptr;
      long long *q_ptr = nullptr;
      long long *g_ptr = nullptr;
      long long *x_ptr = nullptr;

      std::string line;

      std::cout << "p: ";
      std::getline(std::cin, line);
      if (!line.empty()) {
        std::stringstream ss(line);
        if (ss >> p)
          p_ptr = &p;
      }

      std::cout << "q: ";
      std::getline(std::cin, line);
      if (!line.empty()) {
        std::stringstream ss(line);
        if (ss >> q)
          q_ptr = &q;
      }

      std::cout << "g: ";
      std::getline(std::cin, line);
      if (!line.empty()) {
        std::stringstream ss(line);
        if (ss >> g)
          g_ptr = &g;
      }

      std::cout << "x: ";
      std::getline(std::cin, line);
      if (!line.empty()) {
        std::stringstream ss(line);
        if (ss >> x)
          x_ptr = &x;
      }

      DSA(p_ptr, q_ptr, g_ptr, x_ptr);

    } else {
      std::cout << "Неизвестная команда\n\n";
    }
  }

  return 0;
}
