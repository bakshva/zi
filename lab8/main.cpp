#include "liba.hpp"

#include "md5.hpp"

#include <array>

namespace {

std::vector<char> readFile(const std::string &path) {
  std::ifstream in(path, std::ios::binary);
  if (!in)
    throw std::runtime_error("не удалось открыть файл: " + path);
  return std::vector<char>((std::istreambuf_iterator<char>(in)),
                           std::istreambuf_iterator<char>());
}

struct PublicKey {
  long long N;
  long long d;
};

struct PrivateKey {
  long long N;
  long long c;
};

std::pair<PublicKey, PrivateKey> generateKeys() {
  long long P, Q, N, phi;
  do {
    P = generate();
    Q = generate();
    N = P * Q;
    phi = (P - 1) * (Q - 1);
  } while (P == Q || N <= 255);

  long long d, c;
  while (true) {
    d = 2 + static_cast<long long>(rand()) % (phi - 2);
    long long a = d, b = phi;
    auto res = gcd(false, &a, &b);
    if (res.first == 1) {
      c = res.second % phi;
      if (c < 0)
        c += phi;
      break;
    }
  }

  return {PublicKey{N, d}, PrivateKey{N, c}};
}

void savePublicKey(const std::string &path, const PublicKey &pub) {
  std::ofstream out(path);
  if (!out)
    throw std::runtime_error("не удалось создать файл ключа: " + path);
  out << pub.N << "\n" << pub.d << "\n";
}

void savePrivateKey(const std::string &path, const PrivateKey &priv) {
  std::ofstream out(path);
  if (!out)
    throw std::runtime_error("не удалось создать файл ключа: " + path);
  out << priv.N << "\n" << priv.c << "\n";
}

PublicKey loadPublicKey(const std::string &path) {
  std::ifstream in(path);
  if (!in)
    throw std::runtime_error("не удалось открыть файл ключа: " + path);
  PublicKey pub;
  in >> pub.N >> pub.d;
  if (!in)
    throw std::runtime_error("повреждённый файл открытого ключа: " + path);
  return pub;
}

PrivateKey loadPrivateKey(const std::string &path) {
  std::ifstream in(path);
  if (!in)
    throw std::runtime_error("не удалось открыть файл ключа: " + path);
  PrivateKey priv;
  in >> priv.N >> priv.c;
  if (!in)
    throw std::runtime_error("повреждённый файл закрытого ключа: " + path);
  return priv;
}

std::vector<long long> signHash(const std::array<uint8_t, 16> &hash,
                                 const PrivateKey &priv) {
  std::vector<long long> sig;
  sig.reserve(hash.size());
  for (uint8_t y : hash)
    sig.push_back(pows(static_cast<long long>(y), priv.c, priv.N));
  return sig;
}

bool verifyHash(const std::array<uint8_t, 16> &hash,
                 const std::vector<long long> &sig, const PublicKey &pub) {
  if (hash.size() != sig.size())
    return false;
  for (size_t i = 0; i < hash.size(); ++i) {
    long long w = pows(sig[i], pub.d, pub.N);
    if (w != static_cast<long long>(hash[i]))
      return false;
  }
  return true;
}

void saveSignature(const std::string &path, const std::vector<long long> &sig) {
  std::ofstream out(path);
  if (!out)
    throw std::runtime_error("не удалось создать файл подписи: " + path);
  out << sig.size() << "\n";
  for (long long v : sig)
    out << v << "\n";
}

std::vector<long long> loadSignature(const std::string &path) {
  std::ifstream in(path);
  if (!in)
    throw std::runtime_error("не удалось открыть файл подписи: " + path);
  size_t n;
  in >> n;
  std::vector<long long> sig(n);
  for (size_t i = 0; i < n; ++i)
    in >> sig[i];
  if (!in)
    throw std::runtime_error("повреждённый файл подписи: " + path);
  return sig;
}

void cmdKeygen(const std::string &pubPath, const std::string &privPath) {
  auto [pub, priv] = generateKeys();
  savePublicKey(pubPath, pub);
  savePrivateKey(privPath, priv);
  std::cout << "Ключи сгенерированы: N = " << pub.N << ", d = " << pub.d << "\n";
  std::cout << "Открытый ключ: " << pubPath << "\n";
  std::cout << "Закрытый ключ: " << privPath << "\n";
}

void cmdSign(const std::string &filePath, const std::string &privPath) {
  PrivateKey priv = loadPrivateKey(privPath);
  std::vector<char> data = readFile(filePath);
  std::array<uint8_t, 16> hash = md5(data);
  std::vector<long long> sig = signHash(hash, priv);

  std::string sigPath = filePath + ".sig";
  saveSignature(sigPath, sig);
  std::cout << "Подпись сохранена: " << sigPath << "\n";
}

void cmdVerify(const std::string &filePath, const std::string &pubPath) {
  PublicKey pub = loadPublicKey(pubPath);
  std::vector<char> data = readFile(filePath);
  std::vector<long long> sig = loadSignature(filePath + ".sig");
  std::array<uint8_t, 16> hash = md5(data);

  std::cout << (verifyHash(hash, sig, pub) ? "Подпись верна.\n" : "Подпись НЕВЕРНА.\n");
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