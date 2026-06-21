#ifndef COMMANDS_HPP
#define COMMANDS_HPP

#include <iosfwd>
#include <string>
#include <utility>
#include "dictionary.hpp"
#include "list.hpp"
#include "vector.hpp"

namespace pozdnyakov
{

  using DictTable = pozdnyakov::List< std::pair< std::string, pozdnyakov::AvlDictionary * > >;
  using CommandFunc = void (*)(DictTable &, std::istream &);

  AvlDictionary *getDict(DictTable &dicts, const std::string &name);
  pozdnyakov::Vector< const AvlDictionary * > readDictList(DictTable &dicts, std::istream &in, std::size_t n);

  void cmdMake(DictTable &dicts, std::istream &in);
  void cmdDrop(DictTable &dicts, std::istream &in);
  void cmdAddWord(DictTable &dicts, std::istream &in);
  void cmdAddTrans(DictTable &dicts, std::istream &in);
  void cmdDelWord(DictTable &dicts, std::istream &in);
  void cmdDelTrans(DictTable &dicts, std::istream &in);
  void cmdTranslate(DictTable &dicts, std::istream &in);
  void cmdReverse(DictTable &dicts, std::istream &in);
  void cmdShow(DictTable &dicts, std::istream &in);
  void cmdCount(DictTable &dicts, std::istream &in);
  void cmdFilter(DictTable &dicts, std::istream &in);
  void cmdUnion(DictTable &dicts, std::istream &in);
  void cmdIntersect(DictTable &dicts, std::istream &in);
  void cmdDiff(DictTable &dicts, std::istream &in);
  void cmdMerge(DictTable &dicts, std::istream &in);

}

#endif
