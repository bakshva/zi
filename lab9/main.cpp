#include "liba.hpp"
#include "md5.hpp"

#include <array>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <iostream>
#include <limits>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

void clear_input() {
  std::cin.clear();
  std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

bool readOptional(const std::string &prompt, long long &value) {
  std::cout << prompt;
  std::string line;
  std::getline(std::cin, line);
  if (line.empty()) {
    return false;
  }
  std::stringstream ss(line);
  return static_cast<bool>(ss >> value);
}

std::vector<char> readFileBinary(const std::string &filename) {
  std::ifstream file(filename, std::ios::binary | std::ios::ate);
  if (!file.is_open()) {
    throw std::runtime_error("ошибка открытия файла: " + filename);
  }
  std::streamsize size = file.tellg();
  file.seekg(0, std::ios::beg);

  std::vector<char> buffer(static_cast<size_t>(size));
  if (size > 0 && !file.read(buffer.data(), size)) {
    throw std::runtime_error("ошибка чтения файла: " + filename);
  }
  return buffer;
}

struct SigPair {
  long long r;
  long long s;
};

SigPair signByte(long long m, long long p, long long g, long long x) {
  long long p1 = p - 1;
  long long k, r, s;

  while (true) {
    k = 1 + rand() % (p - 2);

    long long kk = k;
    long long mm = p1;
    auto pr = gcd(false, &kk, &mm);

    if (pr.first != 1) {
      continue;
    }

    long long kinv = ((pr.second % p1) + p1) % p1;

    r = pows(g, k, p);

    long long tmp = ((m - x * (r % p1)) % p1 + p1) % p1;
    s = (tmp * kinv) % p1;

    if (s != 0) {
      break;
    }
  }
  return {r, s};
}

bool verifyByte(long long m, const SigPair &sig, long long p, long long g,
                long long y) {
  long long r = sig.r, s = sig.s;
  if (r <= 0 || r >= p) {
    return false;
  }
  long long left = (pows(y, r, p) * pows(r, s, p)) % p;
  long long right = pows(g, m, p);
  return left == right;
}

void signFile() {
  std::string filename;
  std::cout << "введите название файла для подписи\n";
  std::cin >> filename;
  clear_input();

  std::vector<char> data;
  try {
    data = readFileBinary(filename);
  } catch (const std::exception &e) {
    std::cerr << e.what() << "\n";
    return;
  }

  auto hash = md5(data);

  long long p = 0, g = 0, x = 0;
  std::cout << "параметры \n";
  bool hasP = readOptional("p: ", p);
  bool hasG = readOptional("g: ", g);
  bool hasX = readOptional("x (секретный ключ): ", x);

  if (!hasP || p <= 255 || !ferma(p)) {
    if (hasP) {
      std::cout << "p должно быть простым и > 255, генерирую\n";
    }
    do {
      p = generate();
    } while (p <= 255);
  }

  if (!hasG || g < 2 || g >= p || !isPrimitiveRoot(p, g)) {
    if (hasG) {
      std::cout << "g не подходит (нужен первообразный корень по модулю p), "
                   "генерирую\n";
    }
    do {
      g = 2 + rand() % (p - 2);
    } while (!isPrimitiveRoot(p, g));
  }

  if (!hasX || x < 2 || x > p - 2) {
    if (hasX) {
      std::cout << "x должно быть в диапазоне [2, p-2], генерирую\n";
    }
    x = 2 + rand() % (p - 3);
  }

  long long y = pows(g, x, p);
  std::cout << "p = " << p << "; g = " << g << "; y = " << y << "\n";

  std::vector<SigPair> sig;
  sig.reserve(hash.size());
  for (uint8_t byte : hash) {
    sig.push_back(signByte((long long)byte, p, g, x));
  }

  std::string sigFilename = filename + ".sig";
  std::ofstream sigFile(sigFilename);
  if (!sigFile.is_open()) {
    std::cerr << "ошибка создания файла подписи\n";
    return;
  }

  sigFile << p << " " << g << " " << y << "\n";
  for (const auto &pr : sig) {
    sigFile << pr.r << " " << pr.s << "\n";
  }
  sigFile.close();

  std::cout << "файл подписан\n"
            << "подпись сохранена в " << sigFilename << "\n";
}

void verifyFile() {
  std::string filename, sigFilename;
  std::cout << "введите название файла для проверки\n";
  std::cin >> filename;
  std::cout << "введите название файла подписи (например, имя.sig)\n";
  std::cin >> sigFilename;

  std::vector<char> data;
  try {
    data = readFileBinary(filename);
  } catch (const std::exception &e) {
    std::cerr << e.what() << "\n";
    return;
  }

  std::ifstream sf(sigFilename);
  if (!sf.is_open()) {
    std::cerr << "ошибка открытия файла подписи\n";
    return;
  }

  long long p, g, y;
  if (!(sf >> p >> g >> y)) {
    std::cerr << "некорректный файл подписи\n";
    return;
  }

  std::vector<SigPair> sig;
  long long r, s;
  while (sf >> r >> s) {
    sig.push_back({r, s});
  }

  auto hash = md5(data);

  if (sig.size() != hash.size()) {
    std::cout << "подпись некорректна: неверное число пар (" << sig.size()
              << " вместо " << hash.size() << ")\n";
    return;
  }

  bool ok = true;
  for (size_t i = 0; i < hash.size(); i++) {
    if (!verifyByte((long long)hash[i], sig[i], p, g, y)) {
      std::cout << "подпись НЕ верна (расхождение на байте хеша №" << i
                << ")\n";
      ok = false;
      break;
    }
  }

  if (ok) {
    std::cout << "подпись верна\n";
  }
}

void menu() {
  std::cout << "Выберите команду:\n"
            << "1 - подписать файл (ЭЦП Эль-Гамаля)\n"
            << "2 - проверить подпись\n"
            << "exit - выход\n";
}

int main() {
  std::srand((unsigned)std::time(nullptr));

  std::string command;
  while (true) {
    menu();
    std::cin >> command;
    clear_input();

    if (command == "exit") {
      break;
    } else if (command == "1") {
      signFile();
    } else if (command == "2") {
      verifyFile();
    } else {
      std::cout << "Неизвестная команда\n\n";
    }
  }

  return 0;
}