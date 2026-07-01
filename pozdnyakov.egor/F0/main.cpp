#include <iostream>
#include <stdexcept>
#include <string>
#include <utility>
#include "commands.hpp"
#include "dictionary.hpp"
#include "../common/list.hpp"

int main()
{
  pozdnyakov::DictTable dicts;
  pozdnyakov::CmdList cmds;
  pozdnyakov::initCommands(cmds);

  std::string cmdName;
  while (std::cin >> cmdName) {
    if (cmdName == "exit") {
      break;
    }

    bool found = false;
    for (auto it = cmds.begin(); !(it == cmds.end()); ++it) {
      if ((*it).first == cmdName) {
        found = true;
        try {
          (*it).second(dicts, std::cin);
        } catch (const std::bad_alloc &) {
          std::cout << "<memory allocation failed>\n";
        } catch (const std::exception &e) {
          std::cout << e.what() << "\n";
          std::cin.clear();
          char c;
          while (std::cin.get(c) && c != '\n') {
          }
        }
        break;
      }
    }

    if (!found) {
      std::cout << "<INVALID COMMAND>\n";
      std::cin.clear();
      char c;
      while (std::cin.get(c) && c != '\n') {
      }
    }
  }

  for (auto it = dicts.begin(); !(it == dicts.end()); ++it) {
    delete (*it).second;
  }

  return 0;
}
