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

#define PRINT(x) std::cout << #x << " = " << (x) << std::endl;

long long pows(long long a, long long x, long long p);
bool ferma(long long p);
long long generate();
long long gcd(bool mode, long long* a_ptr = nullptr, long long* b_ptr = nullptr);
std::vector<long long> lit_big(long long a, long long y, long long p);
bool isPrimitiveRoot(long long p, long long g);
long long DiffieHellman(long long *p_ptr, long long *g_ptr, long long *x_a_ptr,
                         long long *x_b_ptr);

#endif