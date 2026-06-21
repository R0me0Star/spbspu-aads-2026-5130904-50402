#ifndef DICTIONARY_HPP
#define DICTIONARY_HPP

#include <cstddef>
#include <sstream>
#include <string>
#include <utility>
#include "list.hpp"
#include "vector.hpp"

namespace pozdnyakov
{
  namespace detail
  {

    struct Translation
    {
      std::string rusWord_;
      std::string partOfSpeech_;
    };

    class WordNode
    {
    public:
      std::string engWord_;
      pozdnyakov::List< Translation > translations_;
      std::size_t height_;
      WordNode *left_;
      WordNode *right_;

      WordNode(const std::string &engWord);
      ~WordNode();
    };

  }

  class AvlDictionary
  {
  public:
    AvlDictionary();
    AvlDictionary(const AvlDictionary &other) = delete;
    AvlDictionary &operator=(const AvlDictionary &other) = delete;
    ~AvlDictionary();

    void addWord(const std::string &engWord, const std::string &rusWord, const std::string &partOfSpeech);
    void addTrans(const std::string &engWord, const std::string &rusWord);
    void delWord(const std::string &engWord);
    void delTrans(const std::string &engWord, const std::string &rusWord);
    void translate(const std::string &engWord) const;
    void show() const;
    void count() const;

    bool containsWord(const std::string &engWord) const;
    bool containsTranslation(const std::string &engWord, const std::string &rusWord) const;

    template < typename Func >
    void traverse(Func cb) const;

  private:
    detail::WordNode *root_;

    std::size_t getHeight(const detail::WordNode *node) const;
    int getBalanceFactor(const detail::WordNode *node) const;
    void updateHeight(detail::WordNode *node);

    detail::WordNode *rotateRight(detail::WordNode *node);
    detail::WordNode *rotateLeft(detail::WordNode *node);
    detail::WordNode *balance(detail::WordNode *node);

    detail::WordNode *insertWord(detail::WordNode *node, const std::string &engWord, const std::string &rusWord,
                                 const std::string &partOfSpeech);

    detail::WordNode *removeWordNode(detail::WordNode *node, const std::string &engWord);
    detail::WordNode *findMin(detail::WordNode *node) const;
    detail::WordNode *findNode(detail::WordNode *node, const std::string &engWord) const;

    void addTranslationToList(detail::WordNode *wordNode, const std::string &rusWord,
                              const std::string &partOfSpeech) const;
    void printInOrder(const detail::WordNode *node) const;
    void countNodesAndTranslations(const detail::WordNode *node, std::size_t &wordsCount,
                                   std::size_t &transCount) const;

    template < typename Func >
    void traverseNode(const detail::WordNode *node, Func cb) const;
  };

  template < typename Func >
  void AvlDictionary::traverseNode(const detail::WordNode *node, Func cb) const
  {
    if (node == nullptr) {
      return;
    }
    traverseNode(node->left_, cb);
    cb(node->engWord_, node->translations_);
    traverseNode(node->right_, cb);
  }

  template < typename Func >
  void AvlDictionary::traverse(Func cb) const
  {
    traverseNode(root_, cb);
  }

  void filterDictionary(const pozdnyakov::AvlDictionary &source, pozdnyakov::AvlDictionary &target,
                        const std::string &partOfSpeech);

  void mergeDictionaries(pozdnyakov::AvlDictionary &target,
                         const pozdnyakov::Vector< const pozdnyakov::AvlDictionary * > &dicts);

  void unionDictionaries(pozdnyakov::AvlDictionary &target,
                         const pozdnyakov::Vector< const pozdnyakov::AvlDictionary * > &dicts);

  void intersectDictionaries(pozdnyakov::AvlDictionary &target,
                             const pozdnyakov::Vector< const pozdnyakov::AvlDictionary * > &dicts);

  void diffDictionaries(pozdnyakov::AvlDictionary &target, const pozdnyakov::AvlDictionary &first,
                        const pozdnyakov::Vector< const pozdnyakov::AvlDictionary * > &others);

  void reverseSearch(const pozdnyakov::AvlDictionary &dict, const std::string &rusWord);

  using DictTable = pozdnyakov::List< std::pair< std::string, pozdnyakov::AvlDictionary * > >;
  using CommandFunc = void (*)(DictTable &, std::istringstream &);

  AvlDictionary *getDict(DictTable &dicts, const std::string &name);
  pozdnyakov::Vector< const AvlDictionary * > readDictList(DictTable &dicts, std::istringstream &ss, std::size_t n);

  void cmdMake(DictTable &dicts, std::istringstream &ss);
  void cmdDrop(DictTable &dicts, std::istringstream &ss);
  void cmdAddWord(DictTable &dicts, std::istringstream &ss);
  void cmdAddTrans(DictTable &dicts, std::istringstream &ss);
  void cmdDelWord(DictTable &dicts, std::istringstream &ss);
  void cmdDelTrans(DictTable &dicts, std::istringstream &ss);
  void cmdTranslate(DictTable &dicts, std::istringstream &ss);
  void cmdReverse(DictTable &dicts, std::istringstream &ss);
  void cmdShow(DictTable &dicts, std::istringstream &ss);
  void cmdCount(DictTable &dicts, std::istringstream &ss);
  void cmdFilter(DictTable &dicts, std::istringstream &ss);
  void cmdUnion(DictTable &dicts, std::istringstream &ss);
  void cmdIntersect(DictTable &dicts, std::istringstream &ss);
  void cmdDiff(DictTable &dicts, std::istringstream &ss);
  void cmdMerge(DictTable &dicts, std::istringstream &ss);

}

#endif
