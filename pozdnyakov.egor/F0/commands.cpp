#include "commands.hpp"
#include <iostream>
#include <stdexcept>
#include <utility>

namespace pozdnyakov
{

  static void addCmd(CmdList &cmds, const std::string &name, CommandFunc func)
  {
    std::pair< std::string, CommandFunc > p;
    p.first = name;
    p.second = func;
    cmds.pushFront(p);
  }

  AvlDictionary *getDict(DictTable &dicts, const std::string &name)
  {
    for (auto it = dicts.begin(); !(it == dicts.end()); ++it) {
      if ((*it).first == name) {
        return (*it).second;
      }
    }
    return nullptr;
  }

  pozdnyakov::Vector< const AvlDictionary * > readDictList(DictTable &dicts, std::istream &in, std::size_t n)
  {
    pozdnyakov::Vector< const AvlDictionary * > list;
    for (std::size_t i = 0; i < n; ++i) {
      std::string name;
      if (!(in >> name)) {
        throw std::invalid_argument("<INVALID COMMAND>");
      }
      AvlDictionary *d = getDict(dicts, name);
      if (d != nullptr) {
        list.pushBack(std::move(d));
      } else {
        throw std::invalid_argument("<dictionary '" + name + "' not found>");
      }
    }
    return list;
  }

  void cmdMake(DictTable &dicts, std::istream &in)
  {
    std::string name;
    if (!(in >> name)) {
      throw std::invalid_argument("<INVALID COMMAND>");
    }
    if (getDict(dicts, name) != nullptr) {
      throw std::invalid_argument("<INVALID COMMAND>");
    }

    AvlDictionary *newDict = new AvlDictionary();
    try {
      std::pair< std::string, AvlDictionary * > p;
      p.first = name;
      p.second = newDict;
      dicts.pushFront(p);
      std::cout << "<DICT: " << name << " CREATED>\n";
    } catch (...) {
      delete newDict;
      throw;
    }
  }

  void cmdDrop(DictTable &dicts, std::istream &in)
  {
    std::string name;
    if (!(in >> name)) {
      throw std::invalid_argument("<INVALID COMMAND>");
    }
    AvlDictionary *d = getDict(dicts, name);
    if (!d) {
      throw std::invalid_argument("<dictionary not found>");
    }
    delete d;

    pozdnyakov::List< std::pair< std::string, AvlDictionary * > > temp;
    while (!dicts.empty()) {
      std::pair< std::string, AvlDictionary * > p = dicts.front();
      dicts.popFront();
      if (p.first != name) {
        temp.pushFront(p);
      }
    }
    while (!temp.empty()) {
      std::pair< std::string, AvlDictionary * > p = temp.front();
      temp.popFront();
      dicts.pushFront(p);
    }

    std::cout << "<DROPPED: " << name << ">\n";
  }

  void cmdAddWord(DictTable &dicts, std::istream &in)
  {
    std::string name, eng, rus, pos;
    if (!(in >> name >> eng >> rus >> pos)) {
      throw std::invalid_argument("<INVALID COMMAND>");
    }
    AvlDictionary *d = getDict(dicts, name);
    if (!d) {
      throw std::invalid_argument("<dictionary not found>");
    }
    d->addWord(eng, rus, pos);
  }

  void cmdAddTrans(DictTable &dicts, std::istream &in)
  {
    std::string name, eng, rus;
    if (!(in >> name >> eng >> rus)) {
      throw std::invalid_argument("<INVALID COMMAND>");
    }
    AvlDictionary *d = getDict(dicts, name);
    if (!d) {
      throw std::invalid_argument("<dictionary not found>");
    }
    d->addTrans(eng, rus);
  }

  void cmdDelWord(DictTable &dicts, std::istream &in)
  {
    std::string name, eng;
    if (!(in >> name >> eng)) {
      throw std::invalid_argument("<INVALID COMMAND>");
    }
    AvlDictionary *d = getDict(dicts, name);
    if (!d) {
      throw std::invalid_argument("<dictionary not found>");
    }
    d->delWord(eng);
  }

  void cmdDelTrans(DictTable &dicts, std::istream &in)
  {
    std::string name, eng, rus;
    if (!(in >> name >> eng >> rus)) {
      throw std::invalid_argument("<INVALID COMMAND>");
    }
    AvlDictionary *d = getDict(dicts, name);
    if (!d) {
      throw std::invalid_argument("<dictionary not found>");
    }
    d->delTrans(eng, rus);
  }

  void cmdTranslate(DictTable &dicts, std::istream &in)
  {
    std::string name, eng;
    if (!(in >> name >> eng)) {
      throw std::invalid_argument("<INVALID COMMAND>");
    }
    AvlDictionary *d = getDict(dicts, name);
    if (!d) {
      throw std::invalid_argument("<dictionary not found>");
    }
    d->translate(eng);
  }

  void cmdReverse(DictTable &dicts, std::istream &in)
  {
    std::string name, rus;
    if (!(in >> name >> rus)) {
      throw std::invalid_argument("<INVALID COMMAND>");
    }
    AvlDictionary *d = getDict(dicts, name);
    if (!d) {
      throw std::invalid_argument("<dictionary not found>");
    }
    reverseSearch(*d, rus);
  }

  void cmdShow(DictTable &dicts, std::istream &in)
  {
    std::string name;
    if (!(in >> name)) {
      throw std::invalid_argument("<INVALID COMMAND>");
    }
    AvlDictionary *d = getDict(dicts, name);
    if (!d) {
      throw std::invalid_argument("<dictionary not found>");
    }
    d->show();
  }

  void cmdCount(DictTable &dicts, std::istream &in)
  {
    std::string name;
    if (!(in >> name)) {
      throw std::invalid_argument("<INVALID COMMAND>");
    }
    AvlDictionary *d = getDict(dicts, name);
    if (!d) {
      throw std::invalid_argument("<dictionary not found>");
    }
    d->count();
  }

  void cmdFilter(DictTable &dicts, std::istream &in)
  {
    std::string newName, targetName, pos;
    if (!(in >> newName >> targetName >> pos)) {
      throw std::invalid_argument("<INVALID COMMAND>");
    }
    AvlDictionary *d = getDict(dicts, targetName);
    if (!d) {
      throw std::invalid_argument("<dictionary not found>");
    }
    if (getDict(dicts, newName) != nullptr) {
      throw std::invalid_argument("<INVALID COMMAND>");
    }

    AvlDictionary *newDict = new AvlDictionary();
    try {
      filterDictionary(*d, *newDict, pos);

      std::pair< std::string, AvlDictionary * > p;
      p.first = newName;
      p.second = newDict;
      dicts.pushFront(p);

      std::cout << "<FILTER: " << newName << " CREATED>\n";
    } catch (...) {
      delete newDict;
      throw;
    }
  }

  void cmdUnion(DictTable &dicts, std::istream &in)
  {
    std::string targetName;
    std::size_t n;
    if (!(in >> targetName >> n)) {
      throw std::invalid_argument("<INVALID COMMAND>");
    }
    pozdnyakov::Vector< const AvlDictionary * > sources = readDictList(dicts, in, n);
    if (getDict(dicts, targetName) != nullptr) {
      throw std::invalid_argument("<INVALID COMMAND>");
    }

    AvlDictionary *newDict = new AvlDictionary();
    try {
      unionDictionaries(*newDict, sources);

      std::pair< std::string, AvlDictionary * > p;
      p.first = targetName;
      p.second = newDict;
      dicts.pushFront(p);

      std::cout << "<UNION COMPLETED: " << targetName << ">\n";
    } catch (...) {
      delete newDict;
      throw;
    }
  }

  void cmdIntersect(DictTable &dicts, std::istream &in)
  {
    std::string targetName;
    std::size_t n;
    if (!(in >> targetName >> n)) {
      throw std::invalid_argument("<INVALID COMMAND>");
    }
    pozdnyakov::Vector< const AvlDictionary * > sources = readDictList(dicts, in, n);
    if (getDict(dicts, targetName) != nullptr) {
      throw std::invalid_argument("<INVALID COMMAND>");
    }

    AvlDictionary *newDict = new AvlDictionary();
    try {
      intersectDictionaries(*newDict, sources);

      std::pair< std::string, AvlDictionary * > p;
      p.first = targetName;
      p.second = newDict;
      dicts.pushFront(p);

      std::cout << "<INTERSECT COMPLETED: " << targetName << ">\n";
    } catch (...) {
      delete newDict;
      throw;
    }
  }

  void cmdDiff(DictTable &dicts, std::istream &in)
  {
    std::string targetName;
    std::size_t n;
    if (!(in >> targetName >> n)) {
      throw std::invalid_argument("<INVALID COMMAND>");
    }
    pozdnyakov::Vector< const AvlDictionary * > sources = readDictList(dicts, in, n);
    if (getDict(dicts, targetName) != nullptr) {
      throw std::invalid_argument("<INVALID COMMAND>");
    }

    AvlDictionary *newDict = new AvlDictionary();
    try {
      if (n > 0) {
        pozdnyakov::Vector< const AvlDictionary * > others;
        for (std::size_t i = 1; i < sources.size(); ++i) {
          others.pushBack(std::move(sources[i]));
        }
        diffDictionaries(*newDict, *sources[0], others);
      }

      std::pair< std::string, AvlDictionary * > p;
      p.first = targetName;
      p.second = newDict;
      dicts.pushFront(p);

      std::cout << "<DIFF COMPLETED: " << targetName << ">\n";
    } catch (...) {
      delete newDict;
      throw;
    }
  }

  void cmdMerge(DictTable &dicts, std::istream &in)
  {
    std::string targetName;
    std::size_t n;
    if (!(in >> targetName >> n)) {
      throw std::invalid_argument("<INVALID COMMAND>");
    }
    pozdnyakov::Vector< const AvlDictionary * > sources = readDictList(dicts, in, n);

    AvlDictionary *target = getDict(dicts, targetName);
    if (!target) {
      throw std::invalid_argument("<dictionary not found>");
    }
    mergeDictionaries(*target, sources);
    std::cout << "<MERGED>\n";
  }

  void initCommands(CmdList &cmds)
  {
    addCmd(cmds, "merge", &cmdMerge);
    addCmd(cmds, "diff", &cmdDiff);
    addCmd(cmds, "intersect", &cmdIntersect);
    addCmd(cmds, "union", &cmdUnion);
    addCmd(cmds, "filter", &cmdFilter);
    addCmd(cmds, "count", &cmdCount);
    addCmd(cmds, "show", &cmdShow);
    addCmd(cmds, "reverse", &cmdReverse);
    addCmd(cmds, "translate", &cmdTranslate);
    addCmd(cmds, "del-trans", &cmdDelTrans);
    addCmd(cmds, "del-word", &cmdDelWord);
    addCmd(cmds, "add-trans", &cmdAddTrans);
    addCmd(cmds, "add-word", &cmdAddWord);
    addCmd(cmds, "drop", &cmdDrop);
    addCmd(cmds, "make", &cmdMake);
  }

}
