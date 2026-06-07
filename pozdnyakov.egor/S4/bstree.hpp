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
      return 1 + std::max(leftHeight, rightHeight);
    }

    void replaceNodeInParent(Node *oldNode, Node *newNode)
    {
      if (oldNode->parent) {
        if (oldNode == oldNode->parent->left) {
          oldNode->parent->left = newNode;
        } else {
          oldNode->parent->right = newNode;
        }
      } else {
        root = newNode;
      }
      if (newNode) {
        newNode->parent = oldNode->parent;
      }
    }

    void removeNode(Node *node)
    {
      if (node->left && node->right) {
        Node *successor = node->right;
        while (successor->left) {
          successor = successor->left;
        }
        node->key = successor->key;
        node->value = successor->value;
        removeNode(successor);
      } else if (node->left) {
        replaceNodeInParent(node, node->left);
        delete node;
      } else if (node->right) {
        replaceNodeInParent(node, node->right);
        delete node;
      } else {
        replaceNodeInParent(node, nullptr);
        delete node;
      }
    }

    Node *findNode(Node *node, const Key &key) const
    {
      while (node) {
        if (comparator(key, node->key)) {
          node = node->left;
        } else if (comparator(node->key, key)) {
          node = node->right;
        } else {
          return node;
        }
      }
      return nullptr;
    }

  public:
    BSTree():
      root(nullptr),
      comparator()
    {}

    BSTree(const BSTree &other):
      root(nullptr),
      comparator(other.comparator)
    {
      root = copyTree(other.root);
    }

    ~BSTree()
    {
      clear(root);
    }

    BSTree &operator=(const BSTree &other)
    {
      if (this != &other) {
        clear(root);
        root = copyTree(other.root);
        comparator = other.comparator;
      }
      return *this;
    }

    void push(const Key &key, const Value &value)
    {
      if (!root) {
        root = new Node(key, value);
        return;
      }
      Node *current = root;
      Node *parent = nullptr;
      while (current) {
        parent = current;
        if (comparator(key, current->key)) {
          current = current->left;
        } else if (comparator(current->key, key)) {
          current = current->right;
        } else {
          current->value = value;
          return;
        }
      }
      Node *newNode = new Node(key, value, parent);
      if (comparator(key, parent->key)) {
        parent->left = newNode;
      } else {
        parent->right = newNode;
      }
    }

    Value &get(const Key &key)
    {
      Node *node = findNode(root, key);
      if (!node) {
        throw std::out_of_range("Key not found");
      }
      return node->value;
    }

    const Value &get(const Key &key) const
    {
      Node *node = findNode(root, key);
      if (!node) {
        throw std::out_of_range("Key not found");
      }
      return node->value;
    }

    bool contains(const Key &key) const
    {
      return findNode(root, key) != nullptr;
    }

    void remove(const Key &key)
    {
      Node *node = findNode(root, key);
      if (node) {
        removeNode(node);
      }
    }

    bool empty() const
    {
      return root == nullptr;
    }

    size_t height() const
    {
      return calculateHeight(root);
    }

    iterator begin()
    {
      Node *current = root;
      while (current && current->left) {
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
      return cbegin();
    }

    const_iterator end() const
    {
      return cend();
    }

    const_iterator cbegin() const
    {
      Node *current = root;
      while (current && current->left) {
        current = current->left;
      }
      return const_iterator(current);
    }

    const_iterator cend() const
    {
      return const_iterator(nullptr);
    }
  };

  template < class Key, class Value, class Compare >
  BSTree< Key, Value, Compare > intersect(const BSTree< Key, Value, Compare > &tree1,
                                          const BSTree< Key, Value, Compare > &tree2)
  {
    BSTree< Key, Value, Compare > result;
    for (auto it = tree1.begin(); it != tree1.end(); ++it) {
      const Key &key = (*it).first;
      if (tree2.contains(key)) {
        result.push(key, (*it).second);
      }
    }
    return result;
  }

  template < class Key, class Value, class Compare >
  BSTree< Key, Value, Compare > union_(const BSTree< Key, Value, Compare > &tree1,
                                       const BSTree< Key, Value, Compare > &tree2)
  {
    BSTree< Key, Value, Compare > result;
    for (auto it = tree1.begin(); it != tree1.end(); ++it) {
      result.push((*it).first, (*it).second);
    }
    for (auto it = tree2.begin(); it != tree2.end(); ++it) {
      const Key &key = (*it).first;
      if (!tree1.contains(key)) {
        result.push(key, (*it).second);
      }
    }
    return result;
  }

  template < class Key, class Value, class Compare >
  BSTree< Key, Value, Compare > complement(const BSTree< Key, Value, Compare > &tree1,
                                           const BSTree< Key, Value, Compare > &tree2)
  {
    BSTree< Key, Value, Compare > result;
    for (auto it = tree1.begin(); it != tree1.end(); ++it) {
      const Key &key = (*it).first;
      if (!tree2.contains(key)) {
        result.push(key, (*it).second);
      }
    }
    return result;
  }

}

#endif
