#include "liba.hpp"
#include "md5.hpp"

struct Signature {
  long long r;
  long long s;
};

void clear_input() {
  std::cin.clear();
  std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

bool readFile(const std::string &f, std::vector<char> &buffer) {
  std::ifstream file(f, std::ios::binary | std::ios::ate);
  if (!file.is_open()) {
    std::cerr << "ошибка открытия файла\n";
    return false;
  }

  std::streamsize size = file.tellg();
  file.seekg(0, std::ios::beg);

  buffer.resize(size);
  if (size > 0 && !file.read(buffer.data(), size)) {
    std::cerr << "ошибка чтения файла\n";
    return false;
  }
  return true;
}

long long inverse(long long a, long long mod) {
  long long b = mod;
  long long x = gcd(false, &a, &b).second;
  return ((x % mod) + mod) % mod;
}

long long hashModQ(const std::array<uint8_t, 16> &hash, long long q) {
  long long h = 0;
  for (uint8_t byte : hash) {
    h = (h * 256 + byte) % q;
  }
  return h;
}

void genParams(long long &p, long long &q) {
  q = generate();

  long long t;
  do {
    t = 2 * (1 + rand() % 500);
    p = q * t + 1;
  } while (!ferma(p));
}

long long genG(long long p, long long q) {
  long long g;
  do {
    long long h = 2 + rand() % (p - 3);
    g = pows(h, (p - 1) / q, p);
  } while (g <= 1);
  return g;
}

Signature signHash(long long H, long long p, long long q, long long g,
                   long long x) {
  long long k, r, s;

  while (true) {
    k = 1 + rand() % (q - 1);

    r = pows(g, k, p) % q;
    if (r == 0)
      continue;

    long long kinv = inverse(k, q);
    s = (kinv * ((H + x * r) % q)) % q;
    if (s == 0)
      continue;

    break;
  }
  return {r, s};
}

bool verifyHash(long long H, const Signature &sig, long long p, long long q,
                long long g, long long y) {
  long long r = sig.r, s = sig.s;
  if (r <= 0 || r >= q || s <= 0 || s >= q)
    return false;

  long long w = inverse(s, q);
  long long u1 = (H * w) % q;
  long long u2 = (r * w) % q;
  long long v = ((pows(g, u1, p) * pows(y, u2, p)) % p) % q;

  return v == r;
}

void signFile(long long *p_ptr, long long *q_ptr, long long *g_ptr,
              long long *x_ptr) {
  long long p, q, g, x, y;
  std::string f;

  std::cout << "введите название файла\n";
  std::cin >> f;

  std::vector<char> data;
  if (!readFile(f, data))
    return;

  auto hash = md5(data);

  bool ok_pq = p_ptr && q_ptr && *q_ptr > 1 && ferma(*q_ptr) &&
               *p_ptr > *q_ptr && ferma(*p_ptr) && (*p_ptr - 1) % *q_ptr == 0;

  if (ok_pq) {
    p = *p_ptr;
    q = *q_ptr;
  } else {
    genParams(p, q);
  }

  if (g_ptr && *g_ptr > 1 && *g_ptr < p && pows(*g_ptr, q, p) == 1) {
    g = *g_ptr;
  } else {
    g = genG(p, q);
  }

  x = (x_ptr && *x_ptr >= 1 && *x_ptr < q) ? *x_ptr : 1 + rand() % (q - 1);
  y = pows(g, x, p);

  std::cout << "p = " << p << "\n";
  std::cout << "q = " << q << "\n";
  std::cout << "g = " << g << "\n";
  std::cout << "x = " << x << "\n";
  std::cout << "y = " << y << "\n";

  long long H = hashModQ(hash, q);
  Signature sig = signHash(H, p, q, g, x);

  std::string sigFilename = f + ".sig";
  std::ofstream sigFile(sigFilename);
  if (!sigFile.is_open()) {
    std::cerr << "ошибка создания файла подписи\n";
    return;
  }

  sigFile << p << " " << q << " " << g << " " << y << "\n";
  sigFile << sig.r << " " << sig.s << "\n";

  std::cout << "подписано\nподпись сохранена в " << sigFilename << "\n";
}

void verifyFile() {
  std::string f, sigFilename;

  std::cout << "введите название файла\n";
  std::cin >> f;
  std::cout << "введите название файла подписи\n";
  std::cin >> sigFilename;

  std::vector<char> data;
  if (!readFile(f, data))
    return;

  std::ifstream sigFile(sigFilename);
  if (!sigFile.is_open()) {
    std::cerr << "ошибка открытия файла подписи\n";
    return;
  }

  long long p, q, g, y;
  Signature sig;
  if (!(sigFile >> p >> q >> g >> y) || !(sigFile >> sig.r >> sig.s)) {
    std::cerr << "некорректный файл подписи\n";
    return;
  }

  auto hash = md5(data);
  long long H = hashModQ(hash, q);

  if (verifyHash(H, sig, p, q, g, y)) {
    std::cout << "подпись верна\n";
  } else {
    std::cout << "подпись неверна\n";
  }
}

void menu() {
  std::cout << "Выберите команду:\n"
            << "1 - подписать файл (FIPS 186)\n"
            << "2 - проверить подпись\n"
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

      signFile(p_ptr, q_ptr, g_ptr, x_ptr);

    } else if (command == "2") {
      verifyFile();
    } else {
      std::cout << "Неизвестная команда\n\n";
    }
  }

  return 0;
}