#include "dictionaryTypes.hpp"

int main()
{
  pozdnyakov::AvlDictionary myDictionary;

  myDictionary.addWord("apple", "яблоко", "noun");
  myDictionary.addWord("run", "бегать", "verb");

  return 0;
}
