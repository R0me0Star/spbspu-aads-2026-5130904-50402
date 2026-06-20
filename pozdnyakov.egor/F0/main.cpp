#include "dictionaryTypes.hpp"

int main()
{
  pozdnyakov::AvlDictionary myDictionary;

  // adding words command test
  myDictionary.addWord("run", "бегать", "verb");
  myDictionary.addWord("apple", "яблоко", "noun");
  myDictionary.addWord("zebra", "зебра", "noun");
  myDictionary.addWord("cat", "кот", "noun");

  // additional translate test
  myDictionary.addTrans("run", "управлять");
  myDictionary.addTrans("apple", "яблочко");

  // show command test
  myDictionary.show();

  // count & translate test
  myDictionary.count();
  myDictionary.translate("run");
  myDictionary.translate("dog"); // this word does not exist

  return 0;
}
