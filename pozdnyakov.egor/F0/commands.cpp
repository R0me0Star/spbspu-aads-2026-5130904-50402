#include "commands.hpp"
#include <iostream>
#include <stdexcept>

namespace pozdnyakov
{

  AvlDictionary *getDict(DictTable &dicts, const std::string &name)
  {
    for (auto it = dicts.begin(); it != dicts.end(); ++it) {
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
        list.pushBack(d);
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
      dicts.pushFront(std::make_pair(name, newDict));
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
      auto p = dicts.front();
      dicts.popFront();
      if (p.first != name) {
        temp.pushFront(p);
      }
    }
    while (!temp.empty()) {
      dicts.pushFront(temp.front());
      temp.popFront();
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
      dicts.pushFront(std::make_pair(newName, newDict));
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
      dicts.pushFront(std::make_pair(targetName, newDict));
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
      dicts.pushFront(std::make_pair(targetName, newDict));
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
          others.pushBack(sources[i]);
        }
        diffDictionaries(*newDict, *sources[0], others);
      }
      dicts.pushFront(std::make_pair(targetName, newDict));
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

}
