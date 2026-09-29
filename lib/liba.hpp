#ifndef LIBA_HPP
#define LIBA_HPP

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <format>
#include <fstream>
#include <iostream>
#include <limits>
#include <numeric>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

#define PRINT(x) std::cout << #x << " = " << (x) << std::endl;

long long pows(long long a, long long x, long long p);
bool ferma(long long p);
long long generate();
std::pair<long long, long long> gcd(bool mode, long long *a_ptr,
                                    long long *b_ptr);
std::vector<long long> lit_big(long long a, long long y, long long p);
bool isPrimitiveRoot(long long p, long long g);
long long DiffieHellman(long long *p_ptr, long long *g_ptr, long long *x_a_ptr,
                        long long *x_b_ptr);

#endif