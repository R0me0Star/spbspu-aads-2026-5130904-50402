#ifndef BSTREE_HPP
#define BSTREE_HPP

#include <algorithm>
#include <functional>
#include <stdexcept>
#include <string>
#include <utility>
#include "bstiterators.hpp"

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

    void clear(Node *node);
    Node *copyTree(const Node *node, Node *parentNode = nullptr);
    size_t calculateHeight(const Node *node) const;
    void replaceNodeInParent(Node *oldNode, Node *newNode);
    void removeNode(Node *node);

  public:
    BSTree();
    BSTree(const BSTree &other);
    BSTree(BSTree &&other) noexcept;
    BSTree &operator=(const BSTree &other);
    BSTree &operator=(BSTree &&other) noexcept;
    ~BSTree();

    void push(const Key &targetKey, const Value &targetValue);
    Value &get(const Key &targetKey);
    const Value &get(const Key &targetKey) const;
    Value drop(const Key &targetKey);

    size_t height() const;
    size_t height(const_iterator iterator) const;

    const_iterator rotateLeft(const_iterator iterator);
    const_iterator rotateRight(const_iterator iterator);
    const_iterator rotateLargeLeft(const_iterator iterator);
    const_iterator rotateLargeRight(const_iterator iterator);

    iterator begin();
    iterator end();
    const_iterator begin() const;
    const_iterator end() const;
  };

  template < class Key, class Value, class Compare >
  BSTree< Key, Value, Compare > intersect(const BSTree< Key, Value, Compare > &tree1,
                                          const BSTree< Key, Value, Compare > &tree2);

  template < class Key, class Value, class Compare >
  BSTree< Key, Value, Compare > complement(const BSTree< Key, Value, Compare > &tree1,
                                           const BSTree< Key, Value, Compare > &tree2);

  template < class Key, class Value, class Compare >
  BSTree< Key, Value, Compare > union_(const BSTree< Key, Value, Compare > &tree1,
                                       const BSTree< Key, Value, Compare > &tree2);

  template < class Key, class Value, class Compare >
  void BSTree< Key, Value, Compare >::clear(Node *node)
  {
    if (node && node != fakeLeaf) {
      clear(node->left);
      clear(node->right);
      delete node;
    }
  }

  template < class Key, class Value, class Compare >
  typename BSTree< Key, Value, Compare >::Node *BSTree< Key, Value, Compare >::copyTree(const Node *node,
                                                                                        Node *parentNode)
  {
    if (!node || node == fakeLeaf) {
      return (node == fakeLeaf) ? fakeLeaf : nullptr;
    }
    Node *newNode = new Node(node->key, node->value, parentNode);
    newNode->left = copyTree(node->left, newNode);
    newNode->right = copyTree(node->right, newNode);
    return newNode;
  }

  template < class Key, class Value, class Compare >
  size_t BSTree< Key, Value, Compare >::calculateHeight(const Node *node) const
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

  template < class Key, class Value, class Compare >
  void BSTree< Key, Value, Compare >::replaceNodeInParent(Node *oldNode, Node *newNode)
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

  template < class Key, class Value, class Compare >
  void BSTree< Key, Value, Compare >::removeNode(Node *node)
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

  template < class Key, class Value, class Compare >
  BSTree< Key, Value, Compare >::BSTree():
    root(nullptr),
    fakeLeaf(new Node(Key(), Value(), nullptr, true))
  {
    root = new Node(Key(), Value(), nullptr, true);
    root->left = fakeLeaf;
    root->right = fakeLeaf;
  }

  template < class Key, class Value, class Compare >
  BSTree< Key, Value, Compare >::BSTree(const BSTree &other):
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

  template < class Key, class Value, class Compare >
  BSTree< Key, Value, Compare >::BSTree(BSTree &&other) noexcept:
    root(other.root),
    fakeLeaf(other.fakeLeaf),
    comparator(std::move(other.comparator))
  {
    other.root = nullptr;
    other.fakeLeaf = nullptr;
  }

  template < class Key, class Value, class Compare >
  BSTree< Key, Value, Compare > &BSTree< Key, Value, Compare >::operator=(const BSTree &other)
  {
    if (this != &other) {
      BSTree temp(other);
      std::swap(root, temp.root);
      std::swap(fakeLeaf, temp.fakeLeaf);
      std::swap(comparator, temp.comparator);
    }
    return *this;
  }

  template < class Key, class Value, class Compare >
  BSTree< Key, Value, Compare > &BSTree< Key, Value, Compare >::operator=(BSTree &&other) noexcept
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

  template < class Key, class Value, class Compare >
  BSTree< Key, Value, Compare >::~BSTree()
  {
    if (root) {
      clear(root->left);
      delete root;
    }
    delete fakeLeaf;
  }

  template < class Key, class Value, class Compare >
  void BSTree< Key, Value, Compare >::push(const Key &targetKey, const Value &targetValue)
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

  template < class Key, class Value, class Compare >
  Value &BSTree< Key, Value, Compare >::get(const Key &targetKey)
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

  template < class Key, class Value, class Compare >
  const Value &BSTree< Key, Value, Compare >::get(const Key &targetKey) const
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

  template < class Key, class Value, class Compare >
  Value BSTree< Key, Value, Compare >::drop(const Key &targetKey)
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

  template < class Key, class Value, class Compare >
  size_t BSTree< Key, Value, Compare >::height() const
  {
    return calculateHeight(root->left);
  }

  template < class Key, class Value, class Compare >
  size_t BSTree< Key, Value, Compare >::height(const_iterator iterator) const
  {
    return calculateHeight(iterator.getNode());
  }

  template < class Key, class Value, class Compare >
  typename BSTree< Key, Value, Compare >::const_iterator
  BSTree< Key, Value, Compare >::rotateLeft(const_iterator iterator)
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

  template < class Key, class Value, class Compare >
  typename BSTree< Key, Value, Compare >::const_iterator
  BSTree< Key, Value, Compare >::rotateRight(const_iterator iterator)
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

  template < class Key, class Value, class Compare >
  typename BSTree< Key, Value, Compare >::const_iterator
  BSTree< Key, Value, Compare >::rotateLargeLeft(const_iterator iterator)
  {
    Node *const nodeA = const_cast< Node * >(iterator.getNode());
    if (!nodeA || nodeA == fakeLeaf || !nodeA->right || nodeA->right == fakeLeaf) {
      throw std::invalid_argument("Cannot perform large left rotation");
    }

    rotateRight(const_iterator(nodeA->right));
    return rotateLeft(iterator);
  }

  template < class Key, class Value, class Compare >
  typename BSTree< Key, Value, Compare >::const_iterator
  BSTree< Key, Value, Compare >::rotateLargeRight(const_iterator iterator)
  {
    Node *const nodeA = const_cast< Node * >(iterator.getNode());
    if (!nodeA || nodeA == fakeLeaf || !nodeA->left || nodeA->left == fakeLeaf) {
      throw std::invalid_argument("Cannot perform large right rotation");
    }

    rotateLeft(const_iterator(nodeA->left));
    return rotateRight(iterator);
  }

  template < class Key, class Value, class Compare >
  typename BSTree< Key, Value, Compare >::iterator BSTree< Key, Value, Compare >::begin()
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

  template < class Key, class Value, class Compare >
  typename BSTree< Key, Value, Compare >::iterator BSTree< Key, Value, Compare >::end()
  {
    return iterator(nullptr);
  }

  template < class Key, class Value, class Compare >
  typename BSTree< Key, Value, Compare >::const_iterator BSTree< Key, Value, Compare >::begin() const
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

  template < class Key, class Value, class Compare >
  typename BSTree< Key, Value, Compare >::const_iterator BSTree< Key, Value, Compare >::end() const
  {
    return const_iterator(nullptr);
  }

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

}

#endif
