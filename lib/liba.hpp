#ifndef LIBA_HPP
#define LIBA_HPP

#include <iostream>
#include <format>
#include <vector>
#include <string>
#include <algorithm>
#include <numeric>
#include <cmath>
#include <cstdlib>
#include <unordered_map>

long long pows(long long a, long long x, long long p);
bool ferma(long long p);
long long generate();
long long gcd(bool mode, long long a = -1, long long b = -1);
std::vector<long long> lit_big(long long a, long long y, long long p);

#endif