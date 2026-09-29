#include "liba.hpp"

#include "md5.hpp"

#include <array>
#include <chrono>
#include <iostream>
#include <iterator>
#include <stdexcept>

namespace {

std::vector<char> readFile(const std::string &path) {
  std::ifstream in(path, std::ios::binary);
  if (!in)
    throw std::runtime_error("не удалось открыть файл: " + path);
  return std::vector<char>((std::istreambuf_iterator<char>(in)),
                           std::istreambuf_iterator<char>());
}

struct PublicKey {
  long long p, q, a, y;
};

struct PrivateKey {
  long long p, q, a, x;
};

std::pair<PublicKey, PrivateKey> generateKeys() {
  long long q;
  do {
    q = generate();
  } while (q < 100000);

  long long p;
  do {
    long long b = 2 * (1 + rand() % 500000);
    p = b * q + 1;
  } while (!ferma(p));

  long long a;
  do {
    long long g = 2 + rand() % (p - 3);
    a = pows(g, (p - 1) / q, p);
  } while (a <= 1);

  long long x = 1 + rand() % (q - 1);
  long long y = pows(a, x, p);

  return {PublicKey{p, q, a, y}, PrivateKey{p, q, a, x}};
}

void savePublicKey(const std::string &path, const PublicKey &k) {
  std::ofstream out(path);
  if (!out)
    throw std::runtime_error("не удалось создать файл ключа: " + path);
  out << k.p << "\n" << k.q << "\n" << k.a << "\n" << k.y << "\n";
}

void savePrivateKey(const std::string &path, const PrivateKey &k) {
  std::ofstream out(path);
  if (!out)
    throw std::runtime_error("не удалось создать файл ключа: " + path);
  out << k.p << "\n" << k.q << "\n" << k.a << "\n" << k.x << "\n";
}

PublicKey loadPublicKey(const std::string &path) {
  std::ifstream in(path);
  if (!in)
    throw std::runtime_error("не удалось открыть файл ключа: " + path);
  PublicKey k;
  in >> k.p >> k.q >> k.a >> k.y;
  if (!in || k.q < 3)
    throw std::runtime_error("повреждённый файл открытого ключа: " + path);
  return k;
}

PrivateKey loadPrivateKey(const std::string &path) {
  std::ifstream in(path);
  if (!in)
    throw std::runtime_error("не удалось открыть файл ключа: " + path);
  PrivateKey k;
  in >> k.p >> k.q >> k.a >> k.x;
  if (!in || k.q < 3)
    throw std::runtime_error("повреждённый файл закрытого ключа: " + path);
  return k;
}

long long hashToNumber(const std::array<uint8_t, 16> &hash, long long q) {
  long long h = 0;
  for (uint8_t byte : hash)
    h = (h * 256 + byte) % q;
  return h == 0 ? 1 : h;
}

// Подпись (r, s)
std::pair<long long, long long> sign(long long h, const PrivateKey &priv) {
  long long r = 0, s = 0;
  do {
    long long k = 1 + rand() % (priv.q - 1);        
    r = pows(priv.a, k, priv.p) % priv.q;          
    if (r == 0)
      continue;
    s = (priv.x * r + k * h) % priv.q;             
  } while (r == 0 || s == 0);
  return {r, s};
}

bool verify(long long h, long long r, long long s, const PublicKey &pub) {
  if (r <= 0 || r >= pub.q || s <= 0 || s >= pub.q)
    return false;

  long long v = pows(h, pub.q - 2, pub.q);        
  long long z1 = (s * v) % pub.q;
  long long z2 = ((pub.q - r) * v) % pub.q;

  __int128 t = (__int128)pows(pub.a, z1, pub.p) * pows(pub.y, z2, pub.p);
  long long u = (long long)(t % pub.p) % pub.q; 

  return u == r;
}

void saveSignature(const std::string &path, long long r, long long s) {
  std::ofstream out(path);
  if (!out)
    throw std::runtime_error("не удалось создать файл подписи: " + path);
  out << r << "\n" << s << "\n";
}

std::pair<long long, long long> loadSignature(const std::string &path) {
  std::ifstream in(path);
  if (!in)
    throw std::runtime_error("не удалось открыть файл подписи: " + path);
  long long r, s;
  in >> r >> s;
  if (!in)
    throw std::runtime_error("повреждённый файл подписи: " + path);
  return {r, s};
}

void cmdKeygen(const std::string &pubPath, const std::string &privPath) {
  auto [pub, priv] = generateKeys();
  savePublicKey(pubPath, pub);
  savePrivateKey(privPath, priv);
  std::cout << "Ключи сгенерированы: p = " << pub.p << ", q = " << pub.q
            << ", a = " << pub.a << ", y = " << pub.y << "\n";
  std::cout << "Открытый ключ: " << pubPath << "\n";
  std::cout << "Закрытый ключ: " << privPath << "\n";
}

void cmdSign(const std::string &filePath, const std::string &privPath) {
  PrivateKey priv = loadPrivateKey(privPath);
  long long h = hashToNumber(md5(readFile(filePath)), priv.q);
  auto [r, s] = sign(h, priv);

  std::string sigPath = filePath + ".sig";
  saveSignature(sigPath, r, s);
  std::cout << "Подпись сохранена: " << sigPath << "\n";
}

void cmdVerify(const std::string &filePath, const std::string &pubPath) {
  PublicKey pub = loadPublicKey(pubPath);
  auto [r, s] = loadSignature(filePath + ".sig");
  long long h = hashToNumber(md5(readFile(filePath)), pub.q);

  std::cout << (verify(h, r, s, pub) ? "Подпись верна.\n" : "Подпись НЕВЕРНА.\n");
}

void printUsage(const char *prog) {
  std::cerr << "Использование:\n"
            << "  " << prog << " keygen  <public.key> <private.key>\n"
            << "  " << prog << " sign    <файл> <private.key>\n"
            << "  " << prog << " verify  <файл> <public.key>\n";
}

}

int main(int argc, char **argv) {
  auto seed = std::chrono::high_resolution_clock::now().time_since_epoch().count();
  srand(static_cast<unsigned>(seed));

  if (argc != 4) {
    printUsage(argv[0]);
    return 1;
  }

  std::string mode = argv[1], a1 = argv[2], a2 = argv[3];

  try {
    if (mode == "keygen") {
      cmdKeygen(a1, a2);
    } else if (mode == "sign") {
      cmdSign(a1, a2);
    } else if (mode == "verify") {
      cmdVerify(a1, a2);
    } else {
      printUsage(argv[0]);
      return 1;
    }
  } catch (const std::exception &e) {
    std::cerr << "Ошибка: " << e.what() << "\n";
    return 1;
  }

  return 0;
}