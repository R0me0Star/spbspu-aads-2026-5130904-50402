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
    int getBalanceFactor(const detail::WordNode *node) const;
    void updateHeight(detail::WordNode *node);

    detail::WordNode *rotateRight(detail::WordNode *node);
    detail::WordNode *rotateLeft(detail::WordNode *node);
    detail::WordNode *balance(detail::WordNode *node);
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

  void AvlDictionary::addWord(const std::string &engWord, const std::string &rusWord, const std::string &partOfSpeech)
  {
    std::cout << "Method prepared for: " << engWord << "\n";
  }

}
