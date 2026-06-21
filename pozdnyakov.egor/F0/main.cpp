#include <iostream>
#include <sstream>
#include <string>
#include <utility>
#include "dictionary.hpp"

int main()
{

  pozdnyakov::DictTable dicts;
  pozdnyakov::List< std::pair< std::string, pozdnyakov::CommandFunc > > cmds;

  cmds.pushFront(std::make_pair("merge", &pozdnyakov::cmdMerge));
  cmds.pushFront(std::make_pair("diff", &pozdnyakov::cmdDiff));
  cmds.pushFront(std::make_pair("intersect", &pozdnyakov::cmdIntersect));
  cmds.pushFront(std::make_pair("union", &pozdnyakov::cmdUnion));
  cmds.pushFront(std::make_pair("filter", &pozdnyakov::cmdFilter));
  cmds.pushFront(std::make_pair("count", &pozdnyakov::cmdCount));
  cmds.pushFront(std::make_pair("show", &pozdnyakov::cmdShow));
  cmds.pushFront(std::make_pair("reverse", &pozdnyakov::cmdReverse));
  cmds.pushFront(std::make_pair("translate", &pozdnyakov::cmdTranslate));
  cmds.pushFront(std::make_pair("del-trans", &pozdnyakov::cmdDelTrans));
  cmds.pushFront(std::make_pair("del-word", &pozdnyakov::cmdDelWord));
  cmds.pushFront(std::make_pair("add-trans", &pozdnyakov::cmdAddTrans));
  cmds.pushFront(std::make_pair("add-word", &pozdnyakov::cmdAddWord));
  cmds.pushFront(std::make_pair("drop", &pozdnyakov::cmdDrop));
  cmds.pushFront(std::make_pair("make", &pozdnyakov::cmdMake));

  std::string line;
  while (std::getline(std::cin, line)) {
    if (line.empty() || line == "exit") {
      break;
    }

    std::istringstream ss(line);
    std::string cmdName;
    ss >> cmdName;

    bool found = false;
    for (auto it = cmds.begin(); it != cmds.end(); ++it) {
      if ((*it).first == cmdName) {
        (*it).second(dicts, ss);
        found = true;
        break;
      }
    }

    if (!found) {
      std::cout << "<INVALID COMMAND>\n";
    }
  }

  for (auto it = dicts.begin(); it != dicts.end(); ++it) {
    delete (*it).second;
  }

  return 0;
}
