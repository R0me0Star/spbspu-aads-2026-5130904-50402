#include <cstddef>
#include <iostream>
#include <string>

namespace pozdnyakov
{
  namespace detail
  {

    class TranslationNode
    {
    public:
      std::string rusWord_;
      std::string partOfSpeech_;
      TranslationNode *next_;

      TranslationNode(const std::string &rusWord, const std::string &partOfSpeech);

      ~TranslationNode();
    };

    TranslationNode::TranslationNode(const std::string &rusWord, const std::string &partOfSpeech):
      rusWord_(rusWord),
      partOfSpeech_(partOfSpeech),
      next_(nullptr)
    {}

    TranslationNode::~TranslationNode()
    {
      delete next_;
    }

    class WordNode
    {
    public:
      std::string engWord_;
      TranslationNode *translationsHead_;
      std::size_t height_;
      WordNode *left_;
      WordNode *right_;

      WordNode(const std::string &engWord);

      ~WordNode();
    };

    WordNode::WordNode(const std::string &engWord):
      engWord_(engWord),
      translationsHead_(nullptr),
      height_(1),
      left_(nullptr),
      right_(nullptr)
    {}

    WordNode::~WordNode()
    {
      delete translationsHead_;
      delete left_;
      delete right_;
    }

  }

  class AvlDictionary
  {
  public:
    AvlDictionary();
    AvlDictionary(const AvlDictionary &other) = delete;
    AvlDictionary &operator=(const AvlDictionary &other) = delete;
    ~AvlDictionary();

    void addWord(const std::string &engWord, const std::string &rusWord, const std::string &partOfSpeech);

  private:
    detail::WordNode *root_;

    std::size_t getHeight(const detail::WordNode *node) const;
  };

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

  void AvlDictionary::addWord(const std::string &engWord, const std::string &rusWord, const std::string &partOfSpeech)
  {
    std::cout << "Added word: " << engWord << " - " << rusWord << "\n";
  }

}
