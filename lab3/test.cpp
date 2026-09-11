#include <gtest/gtest.h>
#include <liba.hpp>

#include <cstdlib>
#include <ctime>


TEST(DiffieHellmanTest, Basic) {
    long long p = 23, g = 5, x_a = 6, x_b = 15;
    EXPECT_EQ(DiffieHellman(&p, &g, &x_a, &x_b), 2);
}

TEST(DiffieHellmanTest, AnotherKnown) {
    long long p = 19, g = 2, x_a = 5, x_b = 7;
    EXPECT_EQ(DiffieHellman(&p, &g, &x_a, &x_b), 10);
}


TEST(DiffieHellmanTest, AllNulls) {
    EXPECT_NE(DiffieHellman(nullptr, nullptr, nullptr, nullptr), -1);
}

TEST(DiffieHellmanTest, InvalidValues) {
    long long p = 23, g = 0, x_a = -3, x_b = 0;
    EXPECT_NE(DiffieHellman(&p, &g, &x_a, &x_b), -1);
}

TEST(DiffieHellmanTest, OnlyP) {
    long long p = 23;
    EXPECT_NE(DiffieHellman(&p, nullptr, nullptr, nullptr), -1);
}

TEST(DiffieHellmanTest, BoundarySecrets) {
    long long p = 23, g = 5, x_a = 1, x_b = 1;
    EXPECT_EQ(DiffieHellman(&p, &g, &x_a, &x_b), 5);  
}

TEST(DiffieHellmanTest, SameSecrets) {
    long long p = 23, g = 5, x_a = 8, x_b = 8;
    long long res = DiffieHellman(&p, &g, &x_a, &x_b);
    EXPECT_NE(res, -1);
    EXPECT_EQ(res, pows(pows(g, 8, p), 8, p));
}