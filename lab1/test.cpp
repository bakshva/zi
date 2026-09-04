#include <gtest/gtest.h>
#include <liba.hpp>

#include <cstdlib>
#include <ctime>

TEST(PowsTest, Basic) {
    EXPECT_EQ(pows(2, 10, 1000), 24);
    EXPECT_EQ(pows(3, 5, 13), 9);          
    EXPECT_EQ(pows(5, 0, 7), 1);           
    EXPECT_EQ(pows(7, 1, 13), 7);
}

TEST(PowsTest, EdgeCases) {
    EXPECT_EQ(pows(0, 5, 7), 0);
    EXPECT_EQ(pows(0, 0, 7), 1);           
    EXPECT_EQ(pows(1, 100, 13), 1);
    EXPECT_EQ(pows(2, 10, 1), 0);          
    EXPECT_EQ(pows(2, 3, 2), 0);
}

TEST(PowsTest, Larger) {
    EXPECT_EQ(pows(2, 30, 1000000007), 73741817);
    EXPECT_EQ(pows(123456789, 987654321, 1000000007), 652541198);
}

TEST(GcdTest, Basic) {
    EXPECT_EQ(gcd(false, 48, 18), 6);
    EXPECT_EQ(gcd(false, 17, 13), 1);
    EXPECT_EQ(gcd(false, 100, 25), 25);
    EXPECT_EQ(gcd(false, 7, 7), 7);
}

TEST(GcdTest, OrderDoesNotMatter) {
    EXPECT_EQ(gcd(false, 18, 48), 6);
    EXPECT_EQ(gcd(false, 13, 17), 1);
}

TEST(GcdTest, WithOne) {
    EXPECT_EQ(gcd(false, 42, 1), 1);
    EXPECT_EQ(gcd(false, 1, 42), 1);
}

TEST(GcdTest, LargerNumbers) {
    EXPECT_EQ(gcd(false, 123456789, 987654321), 9);
}


TEST(FermaTest, KnownPrimes) {
    EXPECT_TRUE(ferma(2));
    EXPECT_TRUE(ferma(3));
    EXPECT_TRUE(ferma(5));
    EXPECT_TRUE(ferma(7));
    EXPECT_TRUE(ferma(11));
    EXPECT_TRUE(ferma(13));
    EXPECT_TRUE(ferma(17));
    EXPECT_TRUE(ferma(19));
    EXPECT_TRUE(ferma(23));
    EXPECT_TRUE(ferma(29));
    EXPECT_TRUE(ferma(31));
}

TEST(FermaTest, KnownComposites) {
    EXPECT_FALSE(ferma(1));
    EXPECT_FALSE(ferma(4));
    EXPECT_FALSE(ferma(6));
    EXPECT_FALSE(ferma(8));
    EXPECT_FALSE(ferma(9));
    EXPECT_FALSE(ferma(15));
    EXPECT_FALSE(ferma(21));
    EXPECT_FALSE(ferma(25));
    EXPECT_FALSE(ferma(27));
    EXPECT_FALSE(ferma(33));
    EXPECT_FALSE(ferma(35));
}

TEST(FermaTest, CarmichaelNumbers) {
    EXPECT_FALSE(ferma(561));    
    EXPECT_FALSE(ferma(1105));
    EXPECT_FALSE(ferma(1729));
}



int main(int argc, char **argv) {
    srand(time(nullptr));       
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}