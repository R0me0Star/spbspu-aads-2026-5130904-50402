#include <algorithm>
#include <fstream>
#include <functional>
#include <iostream>
#include <limits>
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
      bool isFake;

      TreeNode(const Key &initKey, const Value &initValue, TreeNode *parentNode = nullptr, bool fake = false):
        key(initKey),
        value(initValue),
        parent(parentNode),
        left(nullptr),
        right(nullptr),
        isFake(fake)
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
    Node *fakeLeaf;
    Compare comparator;

    void clear(Node *node)
    {
      if (node && node != fakeLeaf) {
        clear(node->left);
        clear(node->right);
        delete node;
      }
    }

    Node *copyTree(const Node *node, Node *parentNode = nullptr)
    {
      if (!node || node == fakeLeaf) {
        return (node == fakeLeaf) ? fakeLeaf : nullptr;
      }
      Node *newNode = new Node(node->key, node->value, parentNode);
      newNode->left = copyTree(node->left, newNode);
      newNode->right = copyTree(node->right, newNode);
      return newNode;
    }

    size_t calculateHeight(const Node *node) const
    {
      if (!node || node == fakeLeaf) {
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
      if (!node->left || node->left == fakeLeaf) {
        Node *rightChild = (node->right == fakeLeaf) ? nullptr : node->right;
        replaceNodeInParent(node, rightChild);
        if (rightChild) {
          rightChild->parent = node->parent;
        }
      } else if (!node->right || node->right == fakeLeaf) {
        Node *leftChild = (node->left == fakeLeaf) ? nullptr : node->left;
        replaceNodeInParent(node, leftChild);
        if (leftChild) {
          leftChild->parent = node->parent;
        }
      } else {
        Node *successor = node->right;
        while (successor->left && successor->left != fakeLeaf) {
          successor = successor->left;
        }
 
        const_cast< Key & >(node->key) = successor->key;
        node->value = successor->value;

        Node *successorRight = (successor->right == fakeLeaf) ? nullptr : successor->right;
        replaceNodeInParent(successor, successorRight);
        if (successorRight) {
          successorRight->parent = successor->parent;
        }
        delete successor;
      }
      delete node;
    }

  public:
    BSTree():
      root(nullptr),
      fakeLeaf(new Node(Key(), Value(), nullptr, true))
    {
      root = new Node(Key(), Value(), nullptr, true);
      root->left = fakeLeaf;
      root->right = fakeLeaf;
    }

    BSTree(const BSTree &other):
      root(nullptr),
      fakeLeaf(new Node(Key(), Value(), nullptr, true)),
      comparator(other.comparator)
    {
      root = new Node(Key(), Value(), nullptr, true);
      root->left = fakeLeaf;
      root->right = fakeLeaf;

      Node *otherRoot = other.root;
      if (otherRoot && otherRoot->left != other.fakeLeaf) {
        root->left = copyTree(otherRoot->left, root);
      }
    }

    BSTree(BSTree &&other) noexcept:
      root(other.root),
      fakeLeaf(other.fakeLeaf),
      comparator(std::move(other.comparator))
    {
      other.root = nullptr;
      other.fakeLeaf = nullptr;
    }

    BSTree &operator=(const BSTree &other)
    {
      if (this != &other) {
        BSTree temp(other);
        std::swap(root, temp.root);
        std::swap(fakeLeaf, temp.fakeLeaf);
        std::swap(comparator, temp.comparator);
      }
      return *this;
    }

    BSTree &operator=(BSTree &&other) noexcept
    {
      if (this != &other) {
        clear(root);
        root = other.root;
        fakeLeaf = other.fakeLeaf;
        comparator = std::move(other.comparator);
        other.root = nullptr;
        other.fakeLeaf = nullptr;
      }
      return *this;
    }

    ~BSTree()
    {
      if (root) {
        clear(root->left);
        delete root;
      }
      delete fakeLeaf;
    }

    void push(const Key &targetKey, const Value &targetValue)
    {
      if (!root) {
        root = new Node(Key(), Value(), nullptr, true);
        root->left = fakeLeaf;
        root->right = fakeLeaf;
      }

      Node *current = root->left;
      Node *parentNode = root;

      while (current && current != fakeLeaf) {
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
      newNode->left = fakeLeaf;
      newNode->right = fakeLeaf;

      if (parentNode == root || comparator(targetKey, parentNode->key)) {
        parentNode->left = newNode;
      } else {
        parentNode->right = newNode;
      }
    }

    Value &get(const Key &targetKey)
    {
      Node *current = root->left;
      while (current && current != fakeLeaf) {
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
      Node *current = root->left;
      while (current && current != fakeLeaf) {
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
      Node *current = root->left;
      Node *parentNode = root;

      while (current && current != fakeLeaf) {
        if (comparator(targetKey, current->key)) {
          parentNode = current;
          current = current->left;
        } else if (comparator(current->key, targetKey)) {
          parentNode = current;
          current = current->right;
        } else {
          break;
        }
      }

      if (!current || current == fakeLeaf) {
        throw std::out_of_range("Key not found");
      }

      const Value removedValue = current->value;
      removeNode(current);
      return removedValue;
    }

    size_t height() const
    {
      return calculateHeight(root->left);
    }

    size_t height(const_iterator iterator) const
    {
      return calculateHeight(iterator.getNode());
    }

    const_iterator rotateLeft(const_iterator iterator)
    {
      Node *const nodeA = const_cast< Node * >(iterator.getNode());
      if (!nodeA || nodeA == fakeLeaf || !nodeA->right || nodeA->right == fakeLeaf) {
        throw std::invalid_argument("Cannot perform left rotation");
      }

      Node *const nodeB = nodeA->right;

      nodeA->right = nodeB->left;
      if (nodeB->left && nodeB->left != fakeLeaf) {
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
      if (!nodeA || nodeA == fakeLeaf || !nodeA->left || nodeA->left == fakeLeaf) {
        throw std::invalid_argument("Cannot perform right rotation");
      }

      Node *const nodeB = nodeA->left;

      nodeA->left = nodeB->right;
      if (nodeB->right && nodeB->right != fakeLeaf) {
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
      if (!nodeA || nodeA == fakeLeaf || !nodeA->right || nodeA->right == fakeLeaf) {
        throw std::invalid_argument("Cannot perform large left rotation");
      }

      rotateRight(const_iterator(nodeA->right));
      return rotateLeft(iterator);
    }

    const_iterator rotateLargeRight(const_iterator iterator)
    {
      Node *const nodeA = const_cast< Node * >(iterator.getNode());
      if (!nodeA || nodeA == fakeLeaf || !nodeA->left || nodeA->left == fakeLeaf) {
        throw std::invalid_argument("Cannot perform large right rotation");
      }

      rotateLeft(const_iterator(nodeA->left));
      return rotateRight(iterator);
    }

    iterator begin()
    {
      if (!root || root->left == fakeLeaf) {
        return iterator(nullptr);
      }
      Node *current = root->left;
      while (current->left && current->left != fakeLeaf) {
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
      if (!root || root->left == fakeLeaf) {
        return const_iterator(nullptr);
      }
      const Node *current = root->left;
      while (current->left && current->left != fakeLeaf) {
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
  BSTree< Key, Value, Compare > complement(const BSTree< Key, Value, Compare > &tree1,
                                           const BSTree< Key, Value, Compare > &tree2)
  {
    BSTree< Key, Value, Compare > result;
    auto iterator1 = tree1.begin();
    const Compare comparator{};

    while (iterator1 != tree1.end()) {
      const Key &key = (*iterator1).first;
      bool found = false;
      auto iterator2 = tree2.begin();

      while (iterator2 != tree2.end()) {
        if (!comparator((*iterator2).first, key) && !comparator(key, (*iterator2).first)) {
          found = true;
          break;
        }
        ++iterator2;
      }
      if (!found) {
        result.push(key, (*iterator1).second);
      }
      ++iterator1;
    }

    return result;
  }

  template < class Key, class Value, class Compare >
  BSTree< Key, Value, Compare > union_(const BSTree< Key, Value, Compare > &tree1,
                                       const BSTree< Key, Value, Compare > &tree2)
  {
    BSTree< Key, Value, Compare > result = tree1;
    auto iterator2 = tree2.begin();
    const Compare comparator{};

    while (iterator2 != tree2.end()) {
      const Key &key = (*iterator2).first;
      bool found = false;
      auto it = result.begin();

      while (it != result.end()) {
        if (!comparator((*it).first, key) && !comparator(key, (*it).first)) {
          found = true;
          break;
        }
        ++it;
      }
      if (!found) {
        result.push(key, (*iterator2).second);
      }
      ++iterator2;
    }

    return result;
  }

  bool isInteger(const std::string &str)
  {
    if (str.empty()) {
      return false;
    }
    size_t pos = 0;
    std::stoi(str, &pos);
    return pos == str.length();
  }

  using Dataset = BSTree< int, std::string >;
  using Datasets = BSTree< std::string, Dataset >;

  void cmdPrint(std::istream &, std::ostream &out, Datasets &datasets)
  {
    std::string name;
    std::cin >> name;
    try {
      Dataset &dict = datasets.get(name);
      bool isEmpty = true;
      for (auto it = dict.begin(); it != dict.end(); ++it) {
        out << (*it).first << " " << (*it).second << " ";
        isEmpty = false;
      }
      if (isEmpty) {
        out << "<EMPTY>";
      }
      out << "\n";
    } catch (const std::out_of_range &) {
      out << "<EMPTY>\n";
    }
  }

  void cmdIntersect(std::istream &, std::ostream &out, Datasets &datasets)
  {
    std::string resultName, dict1, dict2;
    std::cin >> resultName >> dict1 >> dict2;
    try {
      Dataset intersected = intersect(datasets.get(dict1), datasets.get(dict2));
      datasets.push(resultName, std::move(intersected));
    } catch (const std::out_of_range &) {
      out << "<INVALID COMMAND>\n";
    }
  }

  void cmdComplement(std::istream &, std::ostream &out, Datasets &datasets)
  {
    std::string resultName, dict1, dict2;
    std::cin >> resultName >> dict1 >> dict2;
    try {
      Dataset comp = complement(datasets.get(dict1), datasets.get(dict2));
      datasets.push(resultName, std::move(comp));
    } catch (const std::out_of_range &) {
      out << "<INVALID COMMAND>\n";
    }
  }

  void cmdUnion(std::istream &, std::ostream &out, Datasets &datasets)
  {
    std::string resultName, dict1, dict2;
    std::cin >> resultName >> dict1 >> dict2;
    try {
      Dataset unionTree = union_(datasets.get(dict1), datasets.get(dict2));
      datasets.push(resultName, std::move(unionTree));
    } catch (const std::out_of_range &) {
      out << "<INVALID COMMAND>\n";
    }
  }

}

int main(int argc, char **argv)
{
  if (argc != 2) {
    std::cerr << "Bad num of args" << '\n';
    return 1;
  }

  pozdnyakov::Datasets datasets;
  std::ifstream file(argv[1]);
  if (!file.is_open()) {
    std::cerr << "Cannot open file" << '\n';
    return 1;
  }

  std::string token;
  std::string currentDataset;
  while (file >> token) {
    if (pozdnyakov::isInteger(token)) {
      int key = std::stoi(token);
      std::string value;
      if (file >> value) {
        datasets.get(currentDataset).push(key, value);
      }
    } else {
      currentDataset = token;
      try {
        datasets.get(currentDataset);
      } catch (const std::out_of_range &) {
        datasets.push(currentDataset, pozdnyakov::Dataset{});
      }
    }
  }
  file.close();

  using cmd_t = void (*)(std::istream &, std::ostream &, pozdnyakov::Datasets &);
  pozdnyakov::BSTree< std::string, cmd_t > commands;

  commands.push("print", pozdnyakov::cmdPrint);
  commands.push("complement", pozdnyakov::cmdComplement);
  commands.push("intersect", pozdnyakov::cmdIntersect);
  commands.push("union", pozdnyakov::cmdUnion);

  std::string cmd;
  while (std::cin >> cmd) {
    try {
      commands.get(cmd)(std::cin, std::cout, datasets);
    } catch (const std::exception &) {
      std::cout << "<INVALID COMMAND>" << '\n';
      std::cin.clear();
      std::cin.ignore(std::numeric_limits< std::streamsize >::max(), '\n');
    }
  }

  return 0;
}
