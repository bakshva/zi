#include <cstring>
#include <iostream>
#include <limits>
#include <string>
#include <unordered_map>
#include <vector>

#include <openssl/bn.h>
#include <openssl/evp.h>
#include <openssl/rand.h>

const char *names[] = {"Да", "Нет", "Воздержался"};
const char *svc = "vote#1@localhost";
BN_CTX *ctx = BN_CTX_new();

void clear_input() {
  std::cin.clear();
  std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

std::string hx(const BIGNUM *x) {
  char *t = BN_bn2hex(x);
  std::string s = t;
  OPENSSL_free(t);
  return s;
}

void sha3(const BIGNUM *n, BIGNUM *h) {
  unsigned char buf[128], dg[32];
  unsigned int len;
  BN_bn2binpad(n, buf, 128);
  EVP_Digest(buf, 128, dg, &len, EVP_sha3_256(), nullptr);
  BN_bin2bn(dg, 32, h);
}

struct Server {
  BIGNUM *P, *Q, *N, *C, *D;
  std::unordered_map<std::string, bool> signed_ids;
  std::vector<BIGNUM *> db_n, db_s;
  int tally[3] = {0, 0, 0};

  Server() {
    P = BN_new();
    Q = BN_new();
    N = BN_new();
    C = BN_new();
    D = BN_new();

    BN_CTX_start(ctx);
    BIGNUM *phi = BN_CTX_get(ctx), *t = BN_CTX_get(ctx), *g = BN_CTX_get(ctx);

    do {
      BN_generate_prime_ex(P, 1024, 0, nullptr, nullptr, nullptr);
      BN_generate_prime_ex(Q, 1024, 0, nullptr, nullptr, nullptr);
    } while (BN_cmp(P, Q) == 0);

    BN_mul(N, P, Q, ctx);
    BN_sub(phi, P, BN_value_one());
    BN_sub(t, Q, BN_value_one());
    BN_mul(phi, phi, t, ctx);

    do {
      BN_rand_range(D, phi);
      BN_gcd(g, D, phi, ctx);
    } while (BN_cmp(D, BN_value_one()) <= 0 || !BN_is_one(g));
    BN_mod_inverse(C, D, phi, ctx);

    std::cout << "[Сервер] P = " << hx(P) << "\n\n"
              << "[Сервер] Q = " << hx(Q) << "\n\n"
              << "[Сервер] N = P * Q = " << hx(N) << "\n\n"
              << "[Сервер] открытый ключ D = " << hx(D) << "\n\n"
              << "[Сервер] секретный ключ C = " << hx(C) << "\n\n";
    BN_CTX_end(ctx);
  }

  ~Server() {
    BN_free(P);
    BN_free(Q);
    BN_free(N);
    BN_free(C);
    BN_free(D);
    for (auto x : db_n)
      BN_free(x);
    for (auto x : db_s)
      BN_free(x);
  }

  BIGNUM *sign(const std::string &id, const BIGNUM *hbar) {
    std::cout << "[Сервер] запрос на подпись от избирателя " << id << "\n";
    if (signed_ids[id]) {
      std::cout << "[Сервер] избиратель уже получал бюллетень, отказ\n";
      return nullptr;
    }
    signed_ids[id] = true;

    BIGNUM *sbar = BN_new();
    BN_mod_exp(sbar, hbar, C, N, ctx);
    std::cout << "[Сервер] избиратель отмечен как получивший бюллетень\n"
              << "[Сервер] h' = " << hx(hbar) << "\n"
              << "[Сервер] s' = h'^C mod N = " << hx(sbar) << "\n";
    return sbar;
  }

  bool accept(const BIGNUM *n, const BIGNUM *s) {
    std::cout << "[Сервер] получен бюллетень\n"
              << "[Сервер] n = " << hx(n) << "\n"
              << "[Сервер] s = " << hx(s) << "\n";

    if (BN_num_bits(n) > 1024) {
      std::cout << "[Сервер] n длиннее 1024 бит, бюллетень отклонён\n";
      return false;
    }

    BN_CTX_start(ctx);
    BIGNUM *h = BN_CTX_get(ctx), *w = BN_CTX_get(ctx);
    sha3(n, h);
    BN_mod_exp(w, s, D, N, ctx);
    std::cout << "[Сервер] SHA3(n) = " << hx(h) << "\n"
              << "[Сервер] s^D mod N = " << hx(w) << "\n";

    unsigned char buf[128];
    BN_bn2binpad(n, buf, 128);
    int choice = buf[127];

    bool ok = false;
    if (BN_cmp(h, w) != 0) {
      std::cout << "[Сервер] SHA3(n) != s^D mod N, подпись неверна, "
                   "бюллетень отклонён\n";
    } else if (memcmp(buf + 64, svc, strlen(svc)) != 0 || choice > 2) {
      std::cout << "[Сервер] служебная информация или результат "
                   "голосования некорректны, бюллетень отклонён\n";
    } else {
      ok = true;
      for (auto x : db_n) {
        if (BN_cmp(x, n) == 0)
          ok = false;
      }
      if (!ok) {
        std::cout << "[Сервер] такой бюллетень уже учтён, отклонён\n";
      }
    }

    if (ok) {
      db_n.push_back(BN_dup(n));
      db_s.push_back(BN_dup(s));
      tally[choice]++;
      std::cout << "[Сервер] SHA3(n) = s^D mod N, бюллетень принят и занесён "
                   "в открытую базу\n";
    }
    BN_CTX_end(ctx);
    return ok;
  }
};

int main() {
  Server server;
  const BIGNUM *N = server.N, *D = server.D;

  BIGNUM *last_n = nullptr, *last_s = nullptr;

  std::string command;
  while (true) {
    std::cout << "Выберите команду:\n"
              << "1 - проголосовать\n"
              << "2 - повторно отправить последний бюллетень\n"
              << "3 - отправить последний бюллетень с изменённым выбором\n"
              << "4 - результаты\n"
              << "5 - открытая база бюллетеней\n"
              << "exit - выход\n";
    std::cin >> command;
    clear_input();

    if (command == "exit") {
      break;
    } else if (command == "1") {
      std::string id;
      std::cout << "введите имя избирателя: ";
      std::cin >> id;
      clear_input();

      int choice = 0;
      std::cout << "1 - Да, 2 - Нет, 3 - Воздержался\nваш выбор: ";
      std::cin >> choice;
      clear_input();
      if (choice < 1 || choice > 3) {
        std::cout << "Неверный выбор\n\n";
        continue;
      }
      choice--;

      BN_CTX_start(ctx);
      BIGNUM *n = BN_CTX_get(ctx), *r = BN_CTX_get(ctx),
             *rinv = BN_CTX_get(ctx), *h = BN_CTX_get(ctx),
             *hbar = BN_CTX_get(ctx), *t = BN_CTX_get(ctx),
             *s = BN_CTX_get(ctx), *g = BN_CTX_get(ctx);

      unsigned char buf[128] = {0};
      RAND_bytes(buf, 64);
      memcpy(buf + 64, svc, strlen(svc));
      buf[127] = choice;
      BN_bin2bn(buf, 128, n);

      do {
        BN_rand_range(r, N);
        BN_gcd(g, r, N, ctx);
      } while (BN_cmp(r, BN_value_one()) <= 0 || !BN_is_one(g));

      sha3(n, h);
      BN_mod_exp(t, r, D, N, ctx);
      BN_mod_mul(hbar, h, t, N, ctx);

      std::cout << "[Клиент] открытый ключ сервера:\n"
                << "[Клиент] N = " << hx(N) << "\n"
                << "[Клиент] D = " << hx(D) << "\n"
                << "[Клиент] выбор: " << names[choice] << " (" << choice
                << ")\n"
                << "[Клиент] n = rnd|v = " << hx(n) << "\n"
                << "[Клиент] r = " << hx(r) << "\n"
                << "[Клиент] h = SHA3(n) = " << hx(h) << "\n"
                << "[Клиент] h' = h * r^D mod N = " << hx(hbar) << "\n"
                << "[Клиент] h' отправлено серверу\n";

      BIGNUM *sbar = server.sign(id, hbar);
      if (!sbar) {
        BN_CTX_end(ctx);
        std::cout << "\n";
        continue;
      }

      BN_mod_inverse(rinv, r, N, ctx);
      BN_mod_mul(s, sbar, rinv, N, ctx);
      std::cout << "[Клиент] r^-1 = " << hx(rinv) << "\n"
                << "[Клиент] s = s' * r^-1 mod N = " << hx(s) << "\n"
                << "[Клиент] анонимная отправка бюллетеня <n, s> (без имени "
                   "избирателя)\n";
      BN_free(sbar);

      BN_free(last_n);
      BN_free(last_s);
      last_n = BN_dup(n);
      last_s = BN_dup(s);

      server.accept(n, s);
      BN_CTX_end(ctx);
      std::cout << "\n";
    } else if (command == "2") {
      if (!last_n) {
        std::cout << "Бюллетеней ещё нет\n\n";
        continue;
      }
      server.accept(last_n, last_s);
      std::cout << "\n";
    } else if (command == "3") {
      if (!last_n) {
        std::cout << "Бюллетеней ещё нет\n\n";
        continue;
      }
      BIGNUM *fake = BN_dup(last_n);
      unsigned char buf[128];
      BN_bn2binpad(last_n, buf, 128);
      BN_sub_word(fake, buf[127]);
      BN_add_word(fake, (buf[127] + 1) % 3);
      server.accept(fake, last_s);
      BN_free(fake);
      std::cout << "\n";
    } else if (command == "4") {
      for (int i = 0; i < 3; i++) {
        std::cout << names[i] << ": " << server.tally[i] << "\n";
      }
      std::cout << "\n";
    } else if (command == "5") {
      for (size_t i = 0; i < server.db_n.size(); i++) {
        std::cout << i + 1 << ":\nn = " << hx(server.db_n[i])
                  << "\ns = " << hx(server.db_s[i]) << "\n";
      }
      std::cout << "\n";
    } else {
      std::cout << "Неизвестная команда\n\n";
    }
  }

  BN_free(last_n);
  BN_free(last_s);
  return 0;
}