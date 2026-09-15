#include <gtest/gtest.h>
#include "liba.hpp"


TEST(ShamirTest, test1_p_bolshe_255) {
  srand((unsigned)time(nullptr));
  long long p, C_A, C_B, D_A, D_B;
  genKeys(p, C_A, C_B, D_A, D_B);

  EXPECT_GT(p, 255);
}

TEST(ShamirTest, test2_klyuchi_obratnye) {
  srand((unsigned)time(nullptr));
  long long p, C_A, C_B, D_A, D_B;
  genKeys(p, C_A, C_B, D_A, D_B);

  long long mod_p = p - 1;
  EXPECT_EQ((C_A * D_A) % mod_p, 1);
  EXPECT_EQ((C_B * D_B) % mod_p, 1);
}

TEST(ShamirTest, test3_shifrovanie_i_deshifrovanie) {
  srand((unsigned)time(nullptr));

  // создаём тестовый файл
  std::ofstream f("in.bin", std::ios::binary);
  unsigned char data[] = {0, 1, 42, 100, 255, 128};
  f.write((char *)data, sizeof(data));
  f.close();

  long long p, C_A, C_B, D_A, D_B;
  genKeys(p, C_A, C_B, D_A, D_B);

  encryptFile("in.bin", "enc.bin", p, C_A, C_B);
  decryptFile("enc.bin", "dec.bin", p, D_A, D_B);

  std::ifstream f1("in.bin", std::ios::binary);
  std::ifstream f2("dec.bin", std::ios::binary);

  unsigned char b1, b2;
  while (f1.read((char *)&b1, 1)) {
    f2.read((char *)&b2, 1);
    EXPECT_EQ(b1, b2);
  }

  f1.close();
  f2.close();
  std::remove("in.bin");
  std::remove("enc.bin");
  std::remove("dec.bin");
}

TEST(ShamirTest, test4_pustoy_fayl) {
  srand((unsigned)time(nullptr));

  std::ofstream f("empty_in.bin", std::ios::binary);
  f.close();

  long long p, C_A, C_B, D_A, D_B;
  genKeys(p, C_A, C_B, D_A, D_B);

  encryptFile("empty_in.bin", "empty_enc.bin", p, C_A, C_B);
  decryptFile("empty_enc.bin", "empty_dec.bin", p, D_A, D_B);

  std::ifstream f2("empty_dec.bin", std::ios::binary);
  unsigned char b;
  bool est_baity = (bool)f2.read((char *)&b, 1);
  EXPECT_FALSE(est_baity);

  f2.close();
  std::remove("empty_in.bin");
  std::remove("empty_enc.bin");
  std::remove("empty_dec.bin");
}

TEST(ShamirTest, test5_razmer_fayla) {
  srand((unsigned)time(nullptr));

  std::ofstream f("size_in.bin", std::ios::binary);
  unsigned char data[] = {5, 10, 15, 20};
  f.write((char *)data, sizeof(data));
  f.close();

  long long p, C_A, C_B, D_A, D_B;
  genKeys(p, C_A, C_B, D_A, D_B);

  encryptFile("size_in.bin", "size_enc.bin", p, C_A, C_B);

  std::ifstream f2("size_enc.bin", std::ios::binary | std::ios::ate);
  long long razmer = f2.tellg();
  EXPECT_EQ(razmer, 4 * (long long)sizeof(long long));

  f2.close();
  std::remove("size_in.bin");
  std::remove("size_enc.bin");
}