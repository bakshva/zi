#include <gtest/gtest.h>
#include <vector>
#include <algorithm>
#include "liba.hpp"

bool has_solution(const std::vector<long long>& res, long long expected) {
    return std::find(res.begin(), res.end(), expected) != res.end();
}

TEST(LitBigNarrowCases, ModuloJustAbovePerfectSquare) {
    long long a = 2;
    long long p = 17;

    std::vector<long long> result = lit_big(a, 4, p);
    
    ASSERT_FALSE(result.empty());
    EXPECT_TRUE(has_solution(result, 2));
}

TEST(LitBigNarrowCases, MaxPossibleExponent) {
    long long a = 3;
    long long p = 11; 
    
    std::vector<long long> result = lit_big(a, 4, p);
    
    ASSERT_FALSE(result.empty());
    EXPECT_TRUE(has_solution(result, 9));
}

TEST(LitBigNarrowCases, UnnormalizedY) {
    long long a = 2;
    long long p = 13;
    
    std::vector<long long> result = lit_big(a, 16, p);
    
    ASSERT_FALSE(result.empty());
    EXPECT_TRUE(has_solution(result, 4));
}

TEST(LitBigNarrowCases, TargetIsZero) {
    long long a = 5;
    long long p = 23;
    
    std::vector<long long> result = lit_big(a, 0, p);
    
    EXPECT_TRUE(result.empty());
}

TEST(LitBigNarrowCases, LargePrimeModulo) {
    long long a = 2;
    long long p = 1000003; 
    long long y = 48573;
    
    std::vector<long long> result = lit_big(a, y, p);
    
    ASSERT_FALSE(result.empty());
    EXPECT_TRUE(has_solution(result, 20));
}

TEST(LitBigNarrowCases, SmallestValidModulo) {
    long long a = 2;
    long long y = 2;
    long long p = 3; 
    
    std::vector<long long> result = lit_big(a, y, p);
    
    ASSERT_FALSE(result.empty());
    EXPECT_TRUE(has_solution(result, 1));
}
