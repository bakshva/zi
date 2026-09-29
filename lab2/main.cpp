#include <iostream>
#include <liba.hpp>
#include <limits>
#include <string>
#include <vector>

void menu() {
  std::cout << "Выберите команду:\n"
            << "1 - Функция, которая решает задачу нахождения дискретного "
               "логарифма при помощи алгоритма «Шаг младенца, шаг великана»\n"
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
    } else if (command == "1") {
      long long a, y, p;
      std::cout << "Уравнение (a^x) % p = y. Введите a, p, y:\n";

      if (!(std::cin >> a >> p >> y)) {
        std::cout << "Ошибка: нужно ввести три числа\n\n";
        clear_input();
        continue;
      }

      std::vector<long long> solutions = lit_big(a, y, p);
      std::cout << "\n";
    } else {
      std::cout << "Неизвестная команда\n\n";
    }
  }

  return 0;
}
