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
    void addTrans(const std::string &engWord, const std::string &rusWord);
    void translate(const std::string &engWord) const;
    void show() const;
    void count() const;

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

    detail::WordNode *findNode(detail::WordNode *node, const std::string &engWord) const;
    void addTranslationToList(detail::WordNode *wordNode, const std::string &rusWord,
                              const std::string &partOfSpeech) const;
    void printInOrder(const detail::WordNode *node) const;
    void countNodesAndTranslations(const detail::WordNode *node, std::size_t &wordsCount,
                                   std::size_t &transCount) const;
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

  void AvlDictionary::addTranslationToList(detail::WordNode *wordNode, const std::string &rusWord,
                                           const std::string &partOfSpeech) const
  {
    if (wordNode->translationsHead_ == nullptr) {
      wordNode->translationsHead_ = new detail::TranslationNode(rusWord, partOfSpeech);
      return;
    }

    detail::TranslationNode *current = wordNode->translationsHead_;
    while (current->next_ != nullptr) {
      if (current->rusWord_ == rusWord) {
        return;
      }
      current = current->next_;
    }

    if (current->rusWord_ != rusWord) {
      current->next_ = new detail::TranslationNode(rusWord, partOfSpeech);
    }
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

  detail::WordNode *AvlDictionary::insertWord(detail::WordNode *node, const std::string &engWord,
                                              const std::string &rusWord, const std::string &partOfSpeech)
  {
    if (node == nullptr) {
      detail::WordNode *newNode = new detail::WordNode(engWord);
      newNode->translationsHead_ = new detail::TranslationNode(rusWord, partOfSpeech);
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

  void AvlDictionary::addWord(const std::string &engWord, const std::string &rusWord, const std::string &partOfSpeech)
  {
    root_ = insertWord(root_, engWord, rusWord, partOfSpeech);
  }

  void AvlDictionary::addTrans(const std::string &engWord, const std::string &rusWord)
  {
    detail::WordNode *node = findNode(root_, engWord);
    if (node == nullptr) {
      std::cout << "<ERROR: Word '" << engWord << "' not found>\n";
      return;
    }

    std::string pos = "";
    if (node->translationsHead_ != nullptr) {
      pos = node->translationsHead_->partOfSpeech_;
    }
    addTranslationToList(node, rusWord, pos);
  }

  void AvlDictionary::translate(const std::string &engWord) const
  {
    detail::WordNode *node = findNode(root_, engWord);
    if (node == nullptr) {
      std::cout << "<NOT FOUND: " << engWord << ">\n";
      return;
    }

    std::cout << engWord << ":\n";
    detail::TranslationNode *current = node->translationsHead_;
    while (current != nullptr) {
      std::cout << "  [" << current->partOfSpeech_ << "]: " << current->rusWord_ << "\n";
      current = current->next_;
    }
  }

  void AvlDictionary::printInOrder(const detail::WordNode *node) const
  {
    if (node == nullptr) {
      return;
    }

    printInOrder(node->left_);

    std::cout << node->engWord_ << " [";
    if (node->translationsHead_ != nullptr) {
      std::cout << node->translationsHead_->partOfSpeech_;
    }
    std::cout << "]: ";

    detail::TranslationNode *current = node->translationsHead_;
    while (current != nullptr) {
      std::cout << current->rusWord_;
      if (current->next_ != nullptr) {
        std::cout << ", ";
      }
      current = current->next_;
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
    detail::TranslationNode *current = node->translationsHead_;
    while (current != nullptr) {
      transCount++;
      current = current->next_;
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

}
