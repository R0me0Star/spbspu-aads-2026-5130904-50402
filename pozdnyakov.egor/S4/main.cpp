#include <algorithm>
#include <fstream>
#include <functional>
#include <iostream>
#include <stdexcept>
#include <string>
#include <utility>

namespace pozdnyakov
{

  namespace detail
  {
    template < typename Key, typename Value >
    struct TreeNode
    {
      Key key;
      Value value;
      TreeNode *parent;
      TreeNode *left;
      TreeNode *right;

      TreeNode(const Key &initKey, const Value &initValue, TreeNode *parentNode = nullptr):
        key(initKey),
        value(initValue),
        parent(parentNode),
        left(nullptr),
        right(nullptr)
      {}
    };
  }

  template < class Key, class Value, class Compare = std::less< Key > >
  class BSTree;

  template < class Key, class Value >
  class BSTIterator
  {
  private:
    using Node = detail::TreeNode< Key, Value >;
    Node *current;

  public:
    explicit BSTIterator(Node *node = nullptr):
      current(node)
    {}

    std::pair< const Key &, Value & > operator*() const
    {
      return {current->key, current->value};
    }

    Node *getNode() const
    {
      return current;
    }

    BSTIterator &operator++()
    {
      if (!current) {
        return *this;
      }

      if (current->right) {
        current = current->right;
        while (current->left) {
          current = current->left;
        }
      } else {
        Node *parentNode = current->parent;
        while (parentNode && current == parentNode->right) {
          current = parentNode;
          parentNode = parentNode->parent;
        }
        current = parentNode;
      }
      return *this;
    }

    BSTIterator operator++(int)
    {
      BSTIterator temp = *this;
      ++(*this);
      return temp;
    }

    bool operator==(const BSTIterator &other) const
    {
      return current == other.current;
    }

    bool operator!=(const BSTIterator &other) const
    {
      return current != other.current;
    }
  };

  template < class Key, class Value >
  class BSTConstIterator
  {
  private:
    using Node = detail::TreeNode< Key, Value >;
    const Node *current;

  public:
    explicit BSTConstIterator(const Node *node = nullptr):
      current(node)
    {}

    BSTConstIterator(const BSTIterator< Key, Value > &other):
      current(other.getNode())
    {}

    std::pair< const Key &, const Value & > operator*() const
    {
      return {current->key, current->value};
    }

    const Node *getNode() const
    {
      return current;
    }

    BSTConstIterator &operator++()
    {
      if (!current) {
        return *this;
      }

      if (current->right) {
        current = current->right;
        while (current->left) {
          current = current->left;
        }
      } else {
        const Node *parentNode = current->parent;
        while (parentNode && current == parentNode->right) {
          current = parentNode;
          parentNode = parentNode->parent;
        }
        current = parentNode;
      }
      return *this;
    }

    BSTConstIterator operator++(int)
    {
      BSTConstIterator temp = *this;
      ++(*this);
      return temp;
    }

    bool operator==(const BSTConstIterator &other) const
    {
      return current == other.current;
    }

    bool operator!=(const BSTConstIterator &other) const
    {
      return current != other.current;
    }
  };

  template < class Key, class Value, class Compare >
  class BSTree
  {
  public:
    using iterator = BSTIterator< Key, Value >;
    using const_iterator = BSTConstIterator< Key, Value >;

  private:
    using Node = detail::TreeNode< Key, Value >;
    Node *root;
    Compare comparator;

    void clear(Node *node)
    {
      if (node) {
        clear(node->left);
        clear(node->right);
        delete node;
      }
    }

    Node *copyTree(const Node *node, Node *parentNode = nullptr)
    {
      if (!node) {
        return nullptr;
      }
      Node *newNode = new Node(node->key, node->value, parentNode);
      newNode->left = copyTree(node->left, newNode);
      newNode->right = copyTree(node->right, newNode);
      return newNode;
    }

    size_t calculateHeight(const Node *node) const
    {
      if (!node) {
        return 0;
      }
      const size_t leftHeight = calculateHeight(node->left);
      const size_t rightHeight = calculateHeight(node->right);

      if (leftHeight > rightHeight) {
        return 1 + leftHeight;
      } else {
        return 1 + rightHeight;
      }
    }

    void replaceNodeInParent(Node *oldNode, Node *newNode)
    {
      if (!oldNode->parent) {
        root = newNode;
      } else if (oldNode == oldNode->parent->left) {
        oldNode->parent->left = newNode;
      } else {
        oldNode->parent->right = newNode;
      }

      if (newNode) {
        newNode->parent = oldNode->parent;
      }
    }

    void removeNode(Node *node)
    {
      if (!node->left && !node->right) {
        replaceNodeInParent(node, nullptr);
      } else if (node->left && !node->right) {
        replaceNodeInParent(node, node->left);
      } else if (!node->left && node->right) {
        replaceNodeInParent(node, node->right);
      } else {
        Node *successor = node->right;
        while (successor->left) {
          successor = successor->left;
        }

        replaceNodeInParent(successor, successor->right);

        successor->parent = node->parent;
        if (node->parent) {
          if (node == node->parent->left) {
            node->parent->left = successor;
          } else {
            node->parent->right = successor;
          }
        } else {
          root = successor;
        }

        successor->left = node->left;
        if (successor->left) {
          successor->left->parent = successor;
        }

        successor->right = node->right;
        if (successor->right) {
          successor->right->parent = successor;
        }
      }
      delete node;
    }

  public:
    BSTree():
      root(nullptr)
    {}

    BSTree(const BSTree &other):
      root(nullptr),
      comparator(other.comparator)
    {
      root = copyTree(other.root);
    }

    BSTree(BSTree &&other) noexcept:
      root(other.root),
      comparator(std::move(other.comparator))
    {
      other.root = nullptr;
    }

    BSTree &operator=(const BSTree &other)
    {
      if (this != &other) {
        BSTree temp(other);
        std::swap(root, temp.root);
        std::swap(comparator, temp.comparator);
      }
      return *this;
    }

    BSTree &operator=(BSTree &&other) noexcept
    {
      if (this != &other) {
        clear(root);
        root = other.root;
        comparator = std::move(other.comparator);
        other.root = nullptr;
      }
      return *this;
    }

    ~BSTree()
    {
      clear(root);
    }

    void push(const Key &targetKey, const Value &targetValue)
    {
      if (!root) {
        root = new Node(targetKey, targetValue);
        return;
      }

      Node *current = root;
      Node *parentNode = nullptr;

      while (current) {
        parentNode = current;
        if (comparator(targetKey, current->key)) {
          current = current->left;
        } else if (comparator(current->key, targetKey)) {
          current = current->right;
        } else {
          current->value = targetValue;
          return;
        }
      }

      Node *newNode = new Node(targetKey, targetValue, parentNode);
      if (comparator(targetKey, parentNode->key)) {
        parentNode->left = newNode;
      } else {
        parentNode->right = newNode;
      }
    }

    Value &get(const Key &targetKey)
    {
      Node *current = root;
      while (current) {
        if (comparator(targetKey, current->key)) {
          current = current->left;
        } else if (comparator(current->key, targetKey)) {
          current = current->right;
        } else {
          return current->value;
        }
      }
      throw std::out_of_range("Key not found");
    }

    const Value &get(const Key &targetKey) const
    {
      Node *current = root;
      while (current) {
        if (comparator(targetKey, current->key)) {
          current = current->left;
        } else if (comparator(current->key, targetKey)) {
          current = current->right;
        } else {
          return current->value;
        }
      }
      throw std::out_of_range("Key not found");
    }

    Value drop(const Key &targetKey)
    {
      Node *current = root;
      while (current) {
        if (comparator(targetKey, current->key)) {
          current = current->left;
        } else if (comparator(current->key, targetKey)) {
          current = current->right;
        } else {
          break;
        }
      }

      if (!current) {
        throw std::out_of_range("Key not found");
      }

      const Value removedValue = current->value;
      removeNode(current);
      return removedValue;
    }

    size_t height() const
    {
      return calculateHeight(root);
    }

    size_t height(const_iterator iterator) const
    {
      return calculateHeight(iterator.getNode());
    }

    const_iterator rotateLeft(const_iterator iterator)
    {
      Node *const nodeA = const_cast< Node * >(iterator.getNode());
      if (!nodeA || !nodeA->right) {
        throw std::invalid_argument("Cannot perform left rotation");
      }

      Node *const nodeB = nodeA->right;

      nodeA->right = nodeB->left;
      if (nodeB->left) {
        nodeB->left->parent = nodeA;
      }

      nodeB->parent = nodeA->parent;
      if (!nodeA->parent) {
        root = nodeB;
      } else if (nodeA == nodeA->parent->left) {
        nodeA->parent->left = nodeB;
      } else {
        nodeA->parent->right = nodeB;
      }

      nodeB->left = nodeA;
      nodeA->parent = nodeB;

      return const_iterator(nodeB);
    }

    const_iterator rotateRight(const_iterator iterator)
    {
      Node *const nodeA = const_cast< Node * >(iterator.getNode());
      if (!nodeA || !nodeA->left) {
        throw std::invalid_argument("Cannot perform right rotation");
      }

      Node *const nodeB = nodeA->left;

      nodeA->left = nodeB->right;
      if (nodeB->right) {
        nodeB->right->parent = nodeA;
      }

      nodeB->parent = nodeA->parent;
      if (!nodeA->parent) {
        root = nodeB;
      } else if (nodeA == nodeA->parent->left) {
        nodeA->parent->left = nodeB;
      } else {
        nodeA->parent->right = nodeB;
      }

      nodeB->right = nodeA;
      nodeA->parent = nodeB;

      return const_iterator(nodeB);
    }

    const_iterator rotateLargeLeft(const_iterator iterator)
    {
      Node *const nodeA = const_cast< Node * >(iterator.getNode());
      if (!nodeA || !nodeA->right) {
        throw std::invalid_argument("Cannot perform large left rotation");
      }

      rotateRight(const_iterator(nodeA->right));
      return rotateLeft(iterator);
    }

    const_iterator rotateLargeRight(const_iterator iterator)
    {
      Node *const nodeA = const_cast< Node * >(iterator.getNode());
      if (!nodeA || !nodeA->left) {
        throw std::invalid_argument("Cannot perform large right rotation");
      }

      rotateLeft(const_iterator(nodeA->left));
      return rotateRight(iterator);
    }

    iterator begin()
    {
      Node *current = root;
      if (!current) {
        return iterator(nullptr);
      }
      while (current->left) {
        current = current->left;
      }
      return iterator(current);
    }

    iterator end()
    {
      return iterator(nullptr);
    }

    const_iterator begin() const
    {
      const Node *current = root;
      if (!current) {
        return const_iterator(nullptr);
      }
      while (current->left) {
        current = current->left;
      }
      return const_iterator(current);
    }

    const_iterator end() const
    {
      return const_iterator(nullptr);
    }
  };

  template < class Key, class Value, class Compare >
  BSTree< Key, Value, Compare > intersect(const BSTree< Key, Value, Compare > &tree1,
                                          const BSTree< Key, Value, Compare > &tree2)
  {
    BSTree< Key, Value, Compare > result;
    auto iterator1 = tree1.begin();
    auto iterator2 = tree2.begin();
    const Compare comparator{};

    while (iterator1 != tree1.end() && iterator2 != tree2.end()) {
      if (comparator((*iterator1).first, (*iterator2).first)) {
        ++iterator1;
      } else if (comparator((*iterator2).first, (*iterator1).first)) {
        ++iterator2;
      } else {
        result.push((*iterator1).first, (*iterator1).second);
        ++iterator1;
        ++iterator2;
      }
    }

    return result;
  }

  template < class Key, class Value, class Compare >
  BSTree< Key, Value, Compare > symmetricDifference(const BSTree< Key, Value, Compare > &tree1,
                                                    const BSTree< Key, Value, Compare > &tree2)
  {
    BSTree< Key, Value, Compare > result;
    auto iterator1 = tree1.begin();
    auto iterator2 = tree2.begin();
    const Compare comparator{};

    while (iterator1 != tree1.end() && iterator2 != tree2.end()) {
      if (comparator((*iterator1).first, (*iterator2).first)) {
        result.push((*iterator1).first, (*iterator1).second);
        ++iterator1;
      } else if (comparator((*iterator2).first, (*iterator1).first)) {
        result.push((*iterator2).first, (*iterator2).second);
        ++iterator2;
      } else {
        ++iterator1;
        ++iterator2;
      }
    }

    while (iterator1 != tree1.end()) {
      result.push((*iterator1).first, (*iterator1).second);
      ++iterator1;
    }

    while (iterator2 != tree2.end()) {
      result.push((*iterator2).first, (*iterator2).second);
      ++iterator2;
    }

    return result;
  }

}

std::string extractWord(const std::string &line, size_t &position)
{
  while (position < line.length() && (line[position] == ' ' || line[position] == '\t' || line[position] == '\r')) {
    position++;
  }
  if (position >= line.length()) {
    return "";
  }
  const size_t start = position;
  while (position < line.length() && line[position] != ' ' && line[position] != '\t' && line[position] != '\r') {
    position++;
  }
  return line.substr(start, position - start);
}

int main(int argc, char *argv[])
{
  if (argc < 2) {
    std::cerr << "Usage: " << argv[0] << " filename\n";
    return 1;
  }

  std::ifstream file(argv[1]);
  if (!file.is_open()) {
    std::cerr << "Failed to open file.\n";
    return 1;
  }

  using InnerTree = pozdnyakov::BSTree< int, std::string >;
  using OuterTree = pozdnyakov::BSTree< std::string, InnerTree >;
  OuterTree globalDicts;

  std::string line;
  while (std::getline(file, line)) {
    size_t position = 0;
    const std::string dictName = extractWord(line, position);
    const std::string keyString = extractWord(line, position);
    const std::string valueString = extractWord(line, position);

    if (!dictName.empty() && !keyString.empty() && !valueString.empty()) {
      const int key = std::stoi(keyString);
      try {
        globalDicts.get(dictName).push(key, valueString);
      } catch (const std::out_of_range &) {
        InnerTree newDict;
        newDict.push(key, valueString);
        globalDicts.push(dictName, std::move(newDict));
      }
    }
  }

  while (std::getline(std::cin, line)) {
    size_t position = 0;
    const std::string command = extractWord(line, position);

    if (command.empty()) {
      continue;
    }

    if (command == "print") {
      const std::string dictName = extractWord(line, position);
      try {
        InnerTree &dict = globalDicts.get(dictName);
        bool isEmpty = true;
        for (auto iterator = dict.begin(); iterator != dict.end(); ++iterator) {
          std::cout << (*iterator).first << " " << (*iterator).second << " ";
          isEmpty = false;
        }
        if (isEmpty) {
          std::cout << "EMPTY";
        }
        std::cout << "\n";
      } catch (const std::out_of_range &) {
        std::cout << "EMPTY\n";
      }
    } else if (command == "intersect") {
      const std::string dict1 = extractWord(line, position);
      const std::string dict2 = extractWord(line, position);
      const std::string resultName = extractWord(line, position);
      try {
        InnerTree intersected = pozdnyakov::intersect(globalDicts.get(dict1), globalDicts.get(dict2));
        globalDicts.push(resultName, std::move(intersected));
      } catch (const std::out_of_range &) {
        std::cout << "INVALID COMMAND\n";
      }
    } else if (command == "symmetric_difference") {
      const std::string dict1 = extractWord(line, position);
      const std::string dict2 = extractWord(line, position);
      const std::string resultName = extractWord(line, position);
      try {
        InnerTree symDiff = pozdnyakov::symmetricDifference(globalDicts.get(dict1), globalDicts.get(dict2));
        globalDicts.push(resultName, std::move(symDiff));
      } catch (const std::out_of_range &) {
        std::cout << "INVALID COMMAND\n";
      }
    } else {
      std::cout << "INVALID COMMAND>\n";
    }
  }

  return 0;
}
