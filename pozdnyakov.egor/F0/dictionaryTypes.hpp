#include <string>

struct translationNode
{
  std::string rusWord;
  std::string partOfSpeech;
  translationNode *next;
};

struct wordNode
{
  std::string engWord;
  translationNode *translationsHead;
  size_t height;
  wordNode *left;
  wordNode *right;
};

class avlDictionary
{
private:
  wordNode *root;

  size_t getHeight(wordNode *node)
  {
    if (node == nullptr) {
      return 0;
    }
    return node->height;
  }

public:
  avlDictionary()
  {
    root = nullptr;
  }

  void addWord(std::string engWord, std::string rusWord, std::string partOfSpeech)
  {
    if (engWord.empty()) {
      return;
    }
  }
};
