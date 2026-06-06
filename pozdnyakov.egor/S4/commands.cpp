#include "commands.hpp"
#include <limits>

namespace pozdnyakov
{

  bool isInteger(const std::string &str)
  {
    if (str.empty()) {
      return false;
    }
    size_t pos = 0;
    std::stoi(str, &pos);
    return pos == str.length();
  }

  void cmdPrint(std::istream &, std::ostream &out, Datasets &datasets)
  {
    std::string name;
    std::cin >> name;
    try {
      Dataset &dict = datasets.get(name);
      bool isEmpty = true;
      for (auto it = dict.begin(); it != dict.end(); ++it) {
        out << (*it).first << " " << (*it).second << " ";
        isEmpty = false;
      }
      if (isEmpty) {
        out << "<EMPTY>";
      }
      out << "\n";
    } catch (const std::out_of_range &) {
      out << "<EMPTY>\n";
    }
  }

  void cmdIntersect(std::istream &, std::ostream &out, Datasets &datasets)
  {
    std::string resultName, dict1, dict2;
    std::cin >> resultName >> dict1 >> dict2;
    try {
      Dataset intersected = intersect(datasets.get(dict1), datasets.get(dict2));
      datasets.push(resultName, std::move(intersected));
    } catch (const std::out_of_range &) {
      out << "<INVALID COMMAND>\n";
    }
  }

  void cmdComplement(std::istream &, std::ostream &out, Datasets &datasets)
  {
    std::string resultName, dict1, dict2;
    std::cin >> resultName >> dict1 >> dict2;
    try {
      Dataset comp = complement(datasets.get(dict1), datasets.get(dict2));
      datasets.push(resultName, std::move(comp));
    } catch (const std::out_of_range &) {
      out << "<INVALID COMMAND>\n";
    }
  }

  void cmdUnion(std::istream &, std::ostream &out, Datasets &datasets)
  {
    std::string resultName, dict1, dict2;
    std::cin >> resultName >> dict1 >> dict2;
    try {
      Dataset unionTree = union_(datasets.get(dict1), datasets.get(dict2));
      datasets.push(resultName, std::move(unionTree));
    } catch (const std::out_of_range &) {
      out << "<INVALID COMMAND>\n";
    }
  }

}
