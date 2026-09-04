#include <liba.hpp>
#include <iostream>
#include <string>
#include <limits>

void menu() {
    std::cout << "Выберите команду:\n"
              << "1 - Быстрое возведение в степень по модулю\n"
              << "2 - Тест простоты Ферма\n"
              << "3 - Обобщённый алгоритм Евклида\n"
              << "exit - выход\n";
}

void clear_input() {
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

int main() {
    std::string command;

    while (true) {
        menu();
        std::cin >> command;

        if (command == "exit") {
            break;
        }
        else if (command == "1") {
            long long a, x, p;
            std::cout << "Уравнение (a^x) % p = y. Введите a, x, p:\n";

            if (!(std::cin >> a >> x >> p)) {
                std::cout << "Ошибка: нужно ввести три числа\n\n";
                clear_input();
                continue;
            }

            std::cout << "Результат: " << pows(a, x, p) << "\n\n";
        }
        else if (command == "2") {
            long long p;
            std::cout << "Введите число для проверки:\n";

            if (!(std::cin >> p)) {
                std::cout << "Ошибка: нужно ввести число\n\n";
                clear_input();
                continue;
            }

            std::cout << (ferma(p) ? "Вероятно простое\n\n" : "Составное\n\n");
        }
        else if (command == "3") {
            long long a, b;
            std::cout << "Введите a и b:\n";

            if (!(std::cin >> a >> b)) {
                std::cout << "Ошибка: нужно ввести два числа\n\n";
                clear_input();
                continue;
            }
            
            int gcd_var = gcd(false, a, b);
            std::cout << "НОД = " << gcd_var << "\n\n";
        }
        else {
            std::cout << "Неизвестная команда\n\n";
        }
    }

    return 0;
}