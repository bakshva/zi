#include <liba.hpp>

void menu() {
    std::cout << "Выберите команду:\n"
              << "1 - Схема Диффи-Хеллмана\n"
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
        clear_input();

        if (command == "exit") {
            break;
        }
        else if (command == "1") {
            long long p = 0, g = 0, x_a = 0, x_b = 0;
            long long *p_ptr = nullptr;
            long long *g_ptr = nullptr;
            long long *x_a_ptr = nullptr;
            long long *x_b_ptr = nullptr;

            std::string line;

            std::cout << "p: ";
            std::getline(std::cin, line);
            if (!line.empty()) {
                std::stringstream ss(line);
                if (ss >> p) p_ptr = &p;
            }

            std::cout << "g: ";
            std::getline(std::cin, line);
            if (!line.empty()) {
                std::stringstream ss(line);
                if (ss >> g) g_ptr = &g;
            }

            std::cout << "x_a: ";
            std::getline(std::cin, line);
            if (!line.empty()) {
                std::stringstream ss(line);
                if (ss >> x_a) x_a_ptr = &x_a;
            }

            std::cout << "x_b: ";
            std::getline(std::cin, line);
            if (!line.empty()) {
                std::stringstream ss(line);
                if (ss >> x_b) x_b_ptr = &x_b;
            }

            long long solutions = DiffieHellman(p_ptr, g_ptr, x_a_ptr, x_b_ptr);

            if (solutions != -1) {
                std::cout << "Общий ключ: " << solutions << "\n\n";
            } else {
                std::cout << "\n";
            }
        }
        else {
            std::cout << "Неизвестная команда\n\n";
        }
    }

    return 0;
}