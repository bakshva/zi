#include "liba.hpp"
#include <cstdint>
#include <fstream>
#include <iostream>
#include <limits>
#include <string>

void clear_input() {
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

void RSAImage(long long *p_ptr, long long *q_ptr, long long *d_b_ptr) {
    long long p, q, N, d, c, fi, command;
    std::string inputFile, outputFile;


    p = (p_ptr && *p_ptr > 1 && ferma(*p_ptr)) ? *p_ptr : generate();
    q = (q_ptr && *q_ptr > 1 && ferma(*q_ptr)) ? *q_ptr : generate();

    N = p * q;
    fi = (p - 1) * (q - 1);


    if (N <= 255) {
        std::cerr << "Ошибка: N = p*q должно быть больше 255.\n";
        std::cerr << "Выберите более крупные простые p и q.\n";
        return;
    }

  
    if (!d_b_ptr || *d_b_ptr <= 1 || gcd(false, d_b_ptr, &fi).first != 1) {
        long long g;
        do {
            d = 2 + rand() % (fi - 2);
            long long a = d, b = fi;
            g = gcd(false, &a, &b).first;
        } while (g != 1);
    } else {
        d = *d_b_ptr;
    }

    std::cout << "p = " << p << "\n";
    std::cout << "q = " << q << "\n";
    std::cout << "N = " << N << "\n";
    std::cout << "d = " << d << "\n";

    std::cout << "Введите имя файла: ";
    std::cin >> inputFile;

    std::cout << "1 - зашифровать изображение\n"
              << "2 - расшифровать изображение\n";
    std::cin >> command;

    if (command == 1) {
        std::ifstream in(inputFile, std::ios::binary);
        if (!in.is_open()) {
            std::cerr << "Ошибка открытия файла.\n";
            return;
        }

        std::cout << "Введите имя выходного файла: ";
        std::cin >> outputFile;

        std::ofstream out(outputFile, std::ios::binary);
        if (!out.is_open()) {
            std::cerr << "Ошибка создания выходного файла.\n";
            return;
        }

       
        std::uint8_t byte;
        while (in.read(reinterpret_cast<char*>(&byte), sizeof(byte))) {
          
            long long encrypted = pows(static_cast<long long>(byte), d, N);

            std::uint64_t block = static_cast<std::uint64_t>(encrypted);
            out.write(reinterpret_cast<const char*>(&block), sizeof(block));
        }

        std::cout << "Изображение зашифровано.\n";

    } else if (command == 2) {
        std::ifstream in(inputFile, std::ios::binary);
        if (!in.is_open()) {
            std::cerr << "Ошибка открытия зашифрованного файла.\n";
            return;
        }

        std::cout << "Введите имя выходного изображения: ";
        std::cin >> outputFile;

        std::ofstream out(outputFile, std::ios::binary);
        if (!out.is_open()) {
            std::cerr << "Ошибка создания выходного файла.\n";
            return;
        }

      
        c = gcd(false, &d, &fi).second;

 
        std::uint64_t block;
        while (in.read(reinterpret_cast<char*>(&block), sizeof(block))) {
            long long encrypted = static_cast<long long>(block);

          
            long long decrypted = pows(encrypted, c, N);

            if (decrypted < 0 || decrypted > 255) {
                std::cerr << "Ошибка: поврежденный файл или неправильные ключи.\n";
                return;
            }

            std::uint8_t byte = static_cast<std::uint8_t>(decrypted);
            out.write(reinterpret_cast<const char*>(&byte), sizeof(byte));
        }

        std::cout << "Изображение расшифровано.\n";
    }
}

void menu() {
    std::cout << "Выберите команду:\n"
              << "1 - RSA для изображений\n"
              << "exit - выход\n";
}

int main() {
    std::string command;

    while (true) {
        menu();
        std::cin >> command;
        clear_input();
         if (command == "exit") {
            break;
        } else if (command == "1") {
            long long p = 0, q = 0, d_b = 0;
            long long *p_ptr = nullptr;
            long long *q_ptr = nullptr;
            long long *d_b_ptr = nullptr;

            std::string line;

            std::cout << "p (пусто = сгенерировать): ";
            std::getline(std::cin, line);
            if (!line.empty()) {
                std::stringstream ss(line);
                if (ss >> p)
                    p_ptr = &p;
            }

            std::cout << "q (пусто = сгенерировать): ";
            std::getline(std::cin, line);
            if (!line.empty()) {
                std::stringstream ss(line);
                if (ss >> q)
                    q_ptr = &q;
            }

            std::cout << "d (пусто = сгенерировать): ";
            std::getline(std::cin, line);
            if (!line.empty()) {
                std::stringstream ss(line);
                if (ss >> d_b)
                    d_b_ptr = &d_b;
            }

            RSAImage(p_ptr, q_ptr, d_b_ptr);
        } else {
            std::cout << "Неизвестная команда\n\n";
        }
    }

    return 0;
}