#include <fstream>
#include <iostream>
#include <limits>
#include <string>
#include "bstree.hpp"
#include "commands.hpp"

int main(int argc, char **argv)
{
  if (argc != 2) {
    std::cerr << "Bad num of args" << '\n';
    return 1;
  }

  pozdnyakov::Datasets datasets;
  std::ifstream file(argv[1]);
  if (!file.is_open()) {
    std::cerr << "Cannot open file" << '\n';
    return 1;
  }

  std::string token;
  std::string currentDataset;
  while (file >> token) {
    if (pozdnyakov::isInteger(token)) {
      int key = std::stoi(token);
      std::string value;
      if (file >> value) {
        datasets.get(currentDataset).push(key, value);
      }
    } else {
      currentDataset = token;
      try {
        datasets.get(currentDataset);
      } catch (const std::out_of_range &) {
        datasets.push(currentDataset, pozdnyakov::Dataset{});
      }
    }
  }
  file.close();

  using cmd_t = void (*)(std::istream &, std::ostream &, pozdnyakov::Datasets &);
  pozdnyakov::BSTree< std::string, cmd_t > commands;

  commands.push("print", pozdnyakov::cmdPrint);
  commands.push("complement", pozdnyakov::cmdComplement);
  commands.push("intersect", pozdnyakov::cmdIntersect);
  commands.push("union", pozdnyakov::cmdUnion);

  std::string cmd;
  while (std::cin >> cmd) {
    try {
      commands.get(cmd)(std::cin, std::cout, datasets);
    } catch (const std::exception &) {
      std::cout << "<INVALID COMMAND>" << '\n';
      std::cin.clear();
      std::cin.ignore(std::numeric_limits< std::streamsize >::max(), '\n');
    }
  }

  return 0;
}
