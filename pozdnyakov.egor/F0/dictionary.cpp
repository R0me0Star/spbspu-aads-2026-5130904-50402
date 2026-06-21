#include "dictionary.hpp"
#include <iostream>

namespace pozdnyakov
{
  namespace detail
  {

    WordNode::WordNode(const std::string &engWord):
      engWord_(engWord),
      height_(1),
      left_(nullptr),
      right_(nullptr)
    {}

    WordNode::~WordNode()
    {
      delete left_;
      delete right_;
    }

  }

  AvlDictionary::AvlDictionary():
    root_(nullptr)
  {}

  AvlDictionary::~AvlDictionary()
  {
    delete root_;
  }

  std::size_t AvlDictionary::getHeight(const detail::WordNode *node) const
  {
    if (node == nullptr) {
      return 0;
    } else {
      return node->height_;
    }
  }

  int AvlDictionary::getBalanceFactor(const detail::WordNode *node) const
  {
    if (node == nullptr) {
      return 0;
    }
    return static_cast< int >(getHeight(node->right_)) - static_cast< int >(getHeight(node->left_));
  }

  void AvlDictionary::updateHeight(detail::WordNode *node)
  {
    if (node != nullptr) {
      std::size_t leftHeight = getHeight(node->left_);
      std::size_t rightHeight = getHeight(node->right_);
      node->height_ = 1 + (leftHeight > rightHeight ? leftHeight : rightHeight);
    }
  }

  detail::WordNode *AvlDictionary::rotateRight(detail::WordNode *node)
  {
    detail::WordNode *leftChild = node->left_;
    detail::WordNode *rightOfLeftChild = leftChild->right_;

    leftChild->right_ = node;
    node->left_ = rightOfLeftChild;

    updateHeight(node);
    updateHeight(leftChild);

    return leftChild;
  }

  detail::WordNode *AvlDictionary::rotateLeft(detail::WordNode *node)
  {
    detail::WordNode *rightChild = node->right_;
    detail::WordNode *leftOfRightChild = rightChild->left_;

    rightChild->left_ = node;
    node->right_ = leftOfRightChild;

    updateHeight(node);
    updateHeight(rightChild);

    return rightChild;
  }

  detail::WordNode *AvlDictionary::balance(detail::WordNode *node)
  {
    updateHeight(node);
    int balanceFactor = getBalanceFactor(node);

    if (balanceFactor > 1) {
      if (getBalanceFactor(node->right_) < 0) {
        node->right_ = rotateRight(node->right_);
      }
      return rotateLeft(node);
    }

    if (balanceFactor < -1) {
      if (getBalanceFactor(node->left_) > 0) {
        node->left_ = rotateLeft(node->left_);
      }
      return rotateRight(node);
    }

    return node;
  }

  void AvlDictionary::addTranslationToList(detail::WordNode *wordNode, const std::string &rusWord,
                                           const std::string &partOfSpeech) const
  {
    for (auto it = wordNode->translations_.begin(); it != wordNode->translations_.end(); ++it) {
      if ((*it).rusWord_ == rusWord) {
        return;
      }
    }
    wordNode->translations_.pushFront(detail::Translation{rusWord, partOfSpeech});
  }

  detail::WordNode *AvlDictionary::findNode(detail::WordNode *node, const std::string &engWord) const
  {
    if (node == nullptr) {
      return nullptr;
    }
    if (engWord < node->engWord_) {
      return findNode(node->left_, engWord);
    } else if (engWord > node->engWord_) {
      return findNode(node->right_, engWord);
    } else {
      return node;
    }
  }

  detail::WordNode *AvlDictionary::findMin(detail::WordNode *node) const
  {
    if (node == nullptr) {
      return nullptr;
    }
    while (node->left_ != nullptr) {
      node = node->left_;
    }
    return node;
  }

  detail::WordNode *AvlDictionary::insertWord(detail::WordNode *node, const std::string &engWord,
                                              const std::string &rusWord, const std::string &partOfSpeech)
  {
    if (node == nullptr) {
      detail::WordNode *newNode = new detail::WordNode(engWord);
      newNode->translations_.pushFront(detail::Translation{rusWord, partOfSpeech});
      return newNode;
    }

    if (engWord < node->engWord_) {
      node->left_ = insertWord(node->left_, engWord, rusWord, partOfSpeech);
    } else if (engWord > node->engWord_) {
      node->right_ = insertWord(node->right_, engWord, rusWord, partOfSpeech);
    } else {
      addTranslationToList(node, rusWord, partOfSpeech);
      return node;
    }

    return balance(node);
  }

  detail::WordNode *AvlDictionary::removeWordNode(detail::WordNode *node, const std::string &engWord)
  {
    if (node == nullptr) {
      return nullptr;
    }

    if (engWord < node->engWord_) {
      node->left_ = removeWordNode(node->left_, engWord);
    } else if (engWord > node->engWord_) {
      node->right_ = removeWordNode(node->right_, engWord);
    } else {
      detail::WordNode *leftChild = node->left_;
      detail::WordNode *rightChild = node->right_;

      if (rightChild == nullptr) {
        node->left_ = nullptr;
        node->right_ = nullptr;
        delete node;
        return leftChild;
      } else if (leftChild == nullptr) {
        node->left_ = nullptr;
        node->right_ = nullptr;
        delete node;
        return rightChild;
      } else {
        detail::WordNode *minNode = findMin(rightChild);

        node->engWord_ = minNode->engWord_;
        node->translations_.clear();
        for (auto it = minNode->translations_.begin(); it != minNode->translations_.end(); ++it) {
          node->translations_.pushFront(*it);
        }

        node->right_ = removeWordNode(node->right_, minNode->engWord_);
      }
    }

    return balance(node);
  }

  void AvlDictionary::addWord(const std::string &engWord, const std::string &rusWord, const std::string &partOfSpeech)
  {
    root_ = insertWord(root_, engWord, rusWord, partOfSpeech);
  }

  void AvlDictionary::addTrans(const std::string &engWord, const std::string &rusWord)
  {
    detail::WordNode *node = findNode(root_, engWord);
    if (node == nullptr) {
      std::cout << "<Word '" << engWord << "' not found>\n";
      return;
    }

    std::string pos = "";
    if (!node->translations_.empty()) {
      pos = node->translations_.front().partOfSpeech_;
    }
    addTranslationToList(node, rusWord, pos);
  }

  void AvlDictionary::delWord(const std::string &engWord)
  {
    root_ = removeWordNode(root_, engWord);
  }

  void AvlDictionary::delTrans(const std::string &engWord, const std::string &rusWord)
  {
    detail::WordNode *node = findNode(root_, engWord);
    if (node == nullptr) {
      std::cout << "<Word '" << engWord << "' not found>\n";
      return;
    }

    pozdnyakov::List< detail::Translation > temp;
    while (!node->translations_.empty()) {
      detail::Translation t = node->translations_.front();
      node->translations_.popFront();
      if (t.rusWord_ != rusWord) {
        temp.pushFront(t);
      }
    }

    while (!temp.empty()) {
      node->translations_.pushFront(temp.front());
      temp.popFront();
    }

    if (node->translations_.empty()) {
      delWord(engWord);
    }
  }

  void AvlDictionary::translate(const std::string &engWord) const
  {
    detail::WordNode *node = findNode(root_, engWord);
    if (node == nullptr) {
      std::cout << "<NOT FOUND: " << engWord << ">\n";
      return;
    }

    std::cout << engWord << ":\n";
    for (auto it = node->translations_.begin(); it != node->translations_.end(); ++it) {
      std::cout << "  [" << (*it).partOfSpeech_ << "]: " << (*it).rusWord_ << "\n";
    }
  }

  bool AvlDictionary::containsWord(const std::string &engWord) const
  {
    return findNode(root_, engWord) != nullptr;
  }

  bool AvlDictionary::containsTranslation(const std::string &engWord, const std::string &rusWord) const
  {
    detail::WordNode *node = findNode(root_, engWord);
    if (node == nullptr) {
      return false;
    }
    for (auto it = node->translations_.begin(); it != node->translations_.end(); ++it) {
      if ((*it).rusWord_ == rusWord) {
        return true;
      }
    }
    return false;
  }

  void AvlDictionary::printInOrder(const detail::WordNode *node) const
  {
    if (node == nullptr) {
      return;
    }

    printInOrder(node->left_);

    std::cout << node->engWord_ << " [";
    if (!node->translations_.empty()) {
      std::cout << node->translations_.front().partOfSpeech_;
    }
    std::cout << "]: ";

    auto it = node->translations_.begin();
    while (it != node->translations_.end()) {
      std::cout << (*it).rusWord_;
      ++it;
      if (it != node->translations_.end()) {
        std::cout << ", ";
      }
    }
    std::cout << "\n";

    printInOrder(node->right_);
  }

  void AvlDictionary::show() const
  {
    std::cout << "<DICTIONARY>\n";
    printInOrder(root_);
  }

  void AvlDictionary::countNodesAndTranslations(const detail::WordNode *node, std::size_t &wordsCount,
                                                std::size_t &transCount) const
  {
    if (node == nullptr) {
      return;
    }

    countNodesAndTranslations(node->left_, wordsCount, transCount);

    wordsCount++;
    for (auto it = node->translations_.begin(); it != node->translations_.end(); ++it) {
      transCount++;
    }

    countNodesAndTranslations(node->right_, wordsCount, transCount);
  }

  void AvlDictionary::count() const
  {
    std::size_t wordsCount = 0;
    std::size_t transCount = 0;
    countNodesAndTranslations(root_, wordsCount, transCount);
    std::cout << "<STATS: " << wordsCount << " words, " << transCount << " translations>\n";
  }

  void filterDictionary(const pozdnyakov::AvlDictionary &source, pozdnyakov::AvlDictionary &target,
                        const std::string &partOfSpeech)
  {
    source.traverse([&target, &partOfSpeech](const std::string &engWord,
                                             const pozdnyakov::List< detail::Translation > &trans) -> void {
      for (auto it = trans.begin(); it != trans.end(); ++it) {
        if ((*it).partOfSpeech_ == partOfSpeech) {
          target.addWord(engWord, (*it).rusWord_, (*it).partOfSpeech_);
        }
      }
    });
  }

  void mergeDictionaries(pozdnyakov::AvlDictionary &target,
                         const pozdnyakov::Vector< const pozdnyakov::AvlDictionary * > &dicts)
  {
    for (std::size_t i = 0; i < dicts.size(); ++i) {
      dicts[i]->traverse(
          [&target](const std::string &engWord, const pozdnyakov::List< detail::Translation > &trans) -> void {
            for (auto it = trans.begin(); it != trans.end(); ++it) {
              target.addWord(engWord, (*it).rusWord_, (*it).partOfSpeech_);
            }
          });
    }
  }

  void unionDictionaries(pozdnyakov::AvlDictionary &target,
                         const pozdnyakov::Vector< const pozdnyakov::AvlDictionary * > &dicts)
  {
    mergeDictionaries(target, dicts);
  }

  void intersectDictionaries(pozdnyakov::AvlDictionary &target,
                             const pozdnyakov::Vector< const pozdnyakov::AvlDictionary * > &dicts)
  {
    if (dicts.empty()) {
      return;
    }

    dicts[0]->traverse(
        [&target, &dicts](const std::string &engWord, const pozdnyakov::List< detail::Translation > &trans) -> void {
          bool inAll = true;
          for (std::size_t i = 1; i < dicts.size(); ++i) {
            if (!dicts[i]->containsWord(engWord)) {
              inAll = false;
              break;
            }
          }

          if (inAll) {
            for (auto it = trans.begin(); it != trans.end(); ++it) {
              bool transInAll = true;
              for (std::size_t i = 1; i < dicts.size(); ++i) {
                if (!dicts[i]->containsTranslation(engWord, (*it).rusWord_)) {
                  transInAll = false;
                  break;
                }
              }

              if (transInAll) {
                target.addWord(engWord, (*it).rusWord_, (*it).partOfSpeech_);
              }
            }
          }
        });
  }

  void diffDictionaries(pozdnyakov::AvlDictionary &target, const pozdnyakov::AvlDictionary &first,
                        const pozdnyakov::Vector< const pozdnyakov::AvlDictionary * > &others)
  {
    first.traverse(
        [&target, &others](const std::string &engWord, const pozdnyakov::List< detail::Translation > &trans) -> void {
          bool inOthers = false;
          for (std::size_t i = 0; i < others.size(); ++i) {
            if (others[i]->containsWord(engWord)) {
              inOthers = true;
              break;
            }
          }

          if (!inOthers) {
            for (auto it = trans.begin(); it != trans.end(); ++it) {
              target.addWord(engWord, (*it).rusWord_, (*it).partOfSpeech_);
            }
          }
        });
  }

  void reverseSearch(const pozdnyakov::AvlDictionary &dict, const std::string &rusWord)
  {
    std::cout << "<REVERSE '" << rusWord << "'>: ";
    bool isFirst = true;

    dict.traverse(
        [&rusWord, &isFirst](const std::string &engWord, const pozdnyakov::List< detail::Translation > &trans) -> void {
          for (auto it = trans.begin(); it != trans.end(); ++it) {
            if ((*it).rusWord_ == rusWord) {
              if (!isFirst) {
                std::cout << ", ";
              }
              std::cout << engWord;
              isFirst = false;
              break;
            }
          }
        });

    if (isFirst) {
      std::cout << "None";
    }
    std::cout << "\n";
  }

  AvlDictionary *getDict(DictTable &dicts, const std::string &name)
  {
    for (auto it = dicts.begin(); it != dicts.end(); ++it) {
      if ((*it).first == name) {
        return (*it).second;
      }
    }
    return nullptr;
  }

  pozdnyakov::Vector< const AvlDictionary * > readDictList(DictTable &dicts, std::istringstream &ss, std::size_t n)
  {
    pozdnyakov::Vector< const AvlDictionary * > list;
    for (std::size_t i = 0; i < n; ++i) {
      std::string name;
      ss >> name;
      AvlDictionary *d = getDict(dicts, name);
      if (d != nullptr) {
        list.pushBack(d);
      } else {
        std::cout << "Dictionary '" << name << "' not found>\n";
      }
    }
    return list;
  }

  void cmdMake(DictTable &dicts, std::istringstream &ss)
  {
    std::string name;
    ss >> name;
    if (getDict(dicts, name) != nullptr) {
      std::cout << "<INVALID COMMAND>\n";
    } else {
      dicts.pushFront(std::make_pair(name, new AvlDictionary()));
      std::cout << "<DICT: " << name << " CREATED>\n";
    }
  }

  void cmdDrop(DictTable &dicts, std::istringstream &ss)
  {
    std::string name;
    ss >> name;
    AvlDictionary *d = getDict(dicts, name);
    if (d) {
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
    } else {
      std::cout << "<Dictionary not found>\n";
    }
  }

  void cmdAddWord(DictTable &dicts, std::istringstream &ss)
  {
    std::string name, eng, rus, pos;
    ss >> name >> eng >> rus >> pos;
    AvlDictionary *d = getDict(dicts, name);
    if (d)
      d->addWord(eng, rus, pos);
  }

  void cmdAddTrans(DictTable &dicts, std::istringstream &ss)
  {
    std::string name, eng, rus;
    ss >> name >> eng >> rus;
    AvlDictionary *d = getDict(dicts, name);
    if (d)
      d->addTrans(eng, rus);
  }

  void cmdDelWord(DictTable &dicts, std::istringstream &ss)
  {
    std::string name, eng;
    ss >> name >> eng;
    AvlDictionary *d = getDict(dicts, name);
    if (d)
      d->delWord(eng);
  }

  void cmdDelTrans(DictTable &dicts, std::istringstream &ss)
  {
    std::string name, eng, rus;
    ss >> name >> eng >> rus;
    AvlDictionary *d = getDict(dicts, name);
    if (d)
      d->delTrans(eng, rus);
  }

  void cmdTranslate(DictTable &dicts, std::istringstream &ss)
  {
    std::string name, eng;
    ss >> name >> eng;
    AvlDictionary *d = getDict(dicts, name);
    if (d)
      d->translate(eng);
  }

  void cmdReverse(DictTable &dicts, std::istringstream &ss)
  {
    std::string name, rus;
    ss >> name >> rus;
    AvlDictionary *d = getDict(dicts, name);
    if (d)
      reverseSearch(*d, rus);
  }

  void cmdShow(DictTable &dicts, std::istringstream &ss)
  {
    std::string name;
    ss >> name;
    AvlDictionary *d = getDict(dicts, name);
    if (d)
      d->show();
  }

  void cmdCount(DictTable &dicts, std::istringstream &ss)
  {
    std::string name;
    ss >> name;
    AvlDictionary *d = getDict(dicts, name);
    if (d)
      d->count();
  }

  void cmdFilter(DictTable &dicts, std::istringstream &ss)
  {
    std::string newName, targetName, pos;
    ss >> newName >> targetName >> pos;
    AvlDictionary *d = getDict(dicts, targetName);
    if (d && getDict(dicts, newName) == nullptr) {
      AvlDictionary *newDict = new AvlDictionary();
      filterDictionary(*d, *newDict, pos);
      dicts.pushFront(std::make_pair(newName, newDict));
      std::cout << "<FILTER: " << newName << " CREATED>\n";
    }
  }

  void cmdUnion(DictTable &dicts, std::istringstream &ss)
  {
    std::string targetName;
    std::size_t n;
    ss >> targetName >> n;
    pozdnyakov::Vector< const AvlDictionary * > sources = readDictList(dicts, ss, n);
    if (sources.size() != n)
      return;

    if (getDict(dicts, targetName) == nullptr) {
      AvlDictionary *newDict = new AvlDictionary();
      unionDictionaries(*newDict, sources);
      dicts.pushFront(std::make_pair(targetName, newDict));
      std::cout << "<UNION COMPLETED: " << targetName << ">\n";
    }
  }

  void cmdIntersect(DictTable &dicts, std::istringstream &ss)
  {
    std::string targetName;
    std::size_t n;
    ss >> targetName >> n;
    pozdnyakov::Vector< const AvlDictionary * > sources = readDictList(dicts, ss, n);
    if (sources.size() != n)
      return;

    if (getDict(dicts, targetName) == nullptr) {
      AvlDictionary *newDict = new AvlDictionary();
      intersectDictionaries(*newDict, sources);
      dicts.pushFront(std::make_pair(targetName, newDict));
      std::cout << "<INTERSECT COMPLETED: " << targetName << ">\n";
    }
  }

  void cmdDiff(DictTable &dicts, std::istringstream &ss)
  {
    std::string targetName;
    std::size_t n;
    ss >> targetName >> n;
    pozdnyakov::Vector< const AvlDictionary * > sources = readDictList(dicts, ss, n);
    if (sources.size() != n)
      return;

    if (getDict(dicts, targetName) == nullptr) {
      AvlDictionary *newDict = new AvlDictionary();
      if (n > 0) {
        pozdnyakov::Vector< const AvlDictionary * > others;
        for (std::size_t i = 1; i < sources.size(); ++i) {
          others.pushBack(sources[i]);
        }
        diffDictionaries(*newDict, *sources[0], others);
      }
      dicts.pushFront(std::make_pair(targetName, newDict));
      std::cout << "<DIFF COMPLETED: " << targetName << ">\n";
    }
  }

  void cmdMerge(DictTable &dicts, std::istringstream &ss)
  {
    std::string targetName;
    std::size_t n;
    ss >> targetName >> n;
    pozdnyakov::Vector< const AvlDictionary * > sources = readDictList(dicts, ss, n);
    if (sources.size() != n)
      return;

    AvlDictionary *target = getDict(dicts, targetName);
    if (target) {
      mergeDictionaries(*target, sources);
      std::cout << "<MERGED>\n";
    }
  }

}
