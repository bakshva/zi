#include "liba.hpp"
#include <cctype>
#include <filesystem>
#include <format>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

std::uintmax_t sizeVal(long long k) {
  if (k == 0)
    return 1;

  unsigned long long uk = static_cast<unsigned long long>(k < 0 ? -k : k);
  std::uintmax_t bytes = 0;

  while (uk > 0) {
    bytes++;
    uk >>= 8;
  }

  return bytes;
}

void Vernam(long long k) {
  long long m = 0, command, e;
  std::string f;
  std::cout << "введите название файла\n";
  std::cin >> f;

  std::ifstream file(f, std::ios::binary | std::ios::ate);
  if (!file.is_open()) {
    std::cerr << "ошибка открытия файла\n";
    return;
  }

  std::streamsize size = file.tellg();
  file.seekg(0, std::ios::beg);

  std::vector<char> buffer(size);
  if (!file.read(buffer.data(), size)) {
    std::cerr << "ошибка чтения файла\n";
    return;
  }

  std::string s = "";
  for (char c : buffer) {
    if (c != ' ' && c != '\n' && c != '\r' && c != '\t') {
      s += c;
    }
  }

  bool isNumberFile = false;

  if (!s.empty()) {
    bool isBinaryText = true;
    bool isAllDigits = true;

    for (char c : s) {
      if (c != '0' && c != '1') {
        isBinaryText = false;
      }
      if (c < '0' || c > '9') {
        isAllDigits = false;
      }
    }

    if (isAllDigits && s.length() <= 20) {
      try {
        if (isBinaryText) {
          m = std::stoll(s, nullptr, 2);
        } else {
          m = std::stoll(s, nullptr, 10);
        }
        isNumberFile = true;
      } catch (...) {
      }
    }
  }
  std::uintmax_t sizeFile = std::filesystem::file_size(f);

  if (k == -1) {
    do {
      k = DiffieHellman(nullptr, nullptr, nullptr, nullptr);
    } while (sizeFile <= sizeVal(k));
  }

  std::cout << "k = " << std::format("{:b}", k) << "\n";

  std::cout << "1 - зашифровать;\n2 - расшифровать \n";
  std::cin >> command;

  if (isNumberFile) {
    if (command == 1) {
      e = m ^ k;
      std::cout << "зашифровано\ne = " << std::format("{:b}", e) << "\n";
    } else if (command == 2) {
      e = m ^ k;
      std::cout << "расшифровано\nm = " << std::format("{:b}", e) << "\n";
    }
  } else {
    std::vector<char> output_buffer(buffer.size());
    for (size_t i = 0; i < buffer.size(); ++i) {
      char key_byte = static_cast<char>((k >> ((i % 8) * 8)) & 0xFF);
      output_buffer[i] = buffer[i] ^ key_byte;
    }

    std::ofstream outfile(f, std::ios::binary);
    if (outfile.is_open()) {
      outfile.write(output_buffer.data(), output_buffer.size());
      if (command == 1) {
        std::cout << "зашифровано\nФайл перезаписан.\n";
      } else {
        std::cout << "расшифровано\nФайл перезаписан.\n";
      }
    } else {
      std::cerr << "ошибка сохранения файла\n";
    }
  }
}

void clear_input() {
  std::cin.clear();
  std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

void menu() {
  std::cout << "Выберите команду:\n"
            << "1 - шифр Вернама\n"
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
      long long k = 0;

      std::string line;

      std::cout << "k: ";
      std::getline(std::cin, line);
      if (!line.empty()) {
        bool isBinary = true;
        for (char c : line) {
          if (c != '0' && c != '1') {
            isBinary = false;
            break;
          }
        }

        try {
          if (isBinary) {
            k = std::stoll(line, nullptr, 2);
          } else {
            k = std::stoll(line, nullptr, 10);
          }
        } catch (...) {
          k = -1;
        }
      } else {
        k = -1;
      }
      Vernam(k);

    } else {
      std::cout << "Неизвестная команда\n\n";
    }
  }

  return 0;
}
