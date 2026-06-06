#ifndef COMMANDS_HPP
#define COMMANDS_HPP

#include <iostream>
#include <string>
#include "bstree.hpp"

namespace pozdnyakov
{

  using Dataset = BSTree< int, std::string >;
  using Datasets = BSTree< std::string, Dataset >;

  void cmdPrint(std::istream &, std::ostream &, Datasets &);
  void cmdIntersect(std::istream &, std::ostream &, Datasets &);
  void cmdComplement(std::istream &, std::ostream &, Datasets &);
  void cmdUnion(std::istream &, std::ostream &, Datasets &);

  bool isInteger(const std::string &str);

}

#endif
