#include <algorithm>
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

      TreeNode(const Key &k, const Value &v, TreeNode *p = nullptr):
        key(k),
        value(v),
        parent(p),
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
        Node *p = current->parent;
        while (p && current == p->right) {
          current = p;
          p = p->parent;
        }
        current = p;
      }
      return *this;
    }

    BSTIterator operator++(int)
    {
      BSTIterator tmp = *this;
      ++(*this);
      return tmp;
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
        const Node *p = current->parent;
        while (p && current == p->right) {
          current = p;
          p = p->parent;
        }
        current = p;
      }
      return *this;
    }

    BSTConstIterator operator++(int)
    {
      BSTConstIterator tmp = *this;
      ++(*this);
      return tmp;
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
    Compare comp;

    void clear(Node *node)
    {
      if (node) {
        clear(node->left);
        clear(node->right);
        delete node;
      }
    }

    Node *copyTree(const Node *node, Node *parent = nullptr)
    {
      if (!node) {
        return nullptr;
      }
      Node *newNode = new Node(node->key, node->value, parent);
      newNode->left = copyTree(node->left, newNode);
      newNode->right = copyTree(node->right, newNode);
      return newNode;
    }

    size_t calculateHeight(const Node *node) const
    {
      if (!node) {
        return 0;
      }
      size_t leftHeight = calculateHeight(node->left);
      size_t rightHeight = calculateHeight(node->right);

      if (leftHeight > rightHeight) {
        return 1 + leftHeight;
      } else {
        return 1 + rightHeight;
      }
    }

    void replaceNodeInParent(Node *u, Node *v)
    {
      if (!u->parent) {
        root = v;
      } else if (u == u->parent->left) {
        u->parent->left = v;
      } else {
        u->parent->right = v;
      }

      if (v) {
        v->parent = u->parent;
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
      comp(other.comp)
    {
      root = copyTree(other.root);
    }

    BSTree(BSTree &&other) noexcept:
      root(other.root),
      comp(std::move(other.comp))
    {
      other.root = nullptr;
    }

    BSTree &operator=(const BSTree &other)
    {
      if (this != &other) {
        BSTree tmp(other);
        std::swap(root, tmp.root);
        std::swap(comp, tmp.comp);
      }
      return *this;
    }

    BSTree &operator=(BSTree &&other) noexcept
    {
      if (this != &other) {
        clear(root);
        root = other.root;
        comp = std::move(other.comp);
        other.root = nullptr;
      }
      return *this;
    }

    ~BSTree()
    {
      clear(root);
    }

    void push(Key k, Value v)
    {
      if (!root) {
        root = new Node(k, v);
        return;
      }

      Node *current = root;
      Node *parent = nullptr;

      while (current) {
        parent = current;
        if (comp(k, current->key)) {
          current = current->left;
        } else if (comp(current->key, k)) {
          current = current->right;
        } else {
          current->value = v;
          return;
        }
      }

      Node *newNode = new Node(k, v, parent);
      if (comp(k, parent->key)) {
        parent->left = newNode;
      } else {
        parent->right = newNode;
      }
    }

    Value get(Key k) const
    {
      Node *current = root;
      while (current) {
        if (comp(k, current->key)) {
          current = current->left;
        } else if (comp(current->key, k)) {
          current = current->right;
        } else {
          return current->value;
        }
      }
      throw std::out_of_range("Key not found");
    }

    Value drop(Key k)
    {
      Node *current = root;
      while (current) {
        if (comp(k, current->key)) {
          current = current->left;
        } else if (comp(current->key, k)) {
          current = current->right;
        } else {
          break;
        }
      }

      if (!current) {
        throw std::out_of_range("Key not found");
      }

      Value retVal = current->value;
      removeNode(current);
      return retVal;
    }

    size_t height() const
    {
      return calculateHeight(root);
    }

    size_t height(const_iterator it) const
    {
      return calculateHeight(it.getNode());
    }

    const_iterator rotateLeft(const_iterator it)
    {
      Node *a = const_cast< Node * >(it.getNode());
      if (!a || !a->right) {
        throw std::invalid_argument("Node or right child is null");
      }

      Node *b = a->right;

      a->right = b->left;
      if (b->left) {
        b->left->parent = a;
      }

      b->parent = a->parent;
      if (!a->parent) {
        root = b;
      } else if (a == a->parent->left) {
        a->parent->left = b;
      } else {
        a->parent->right = b;
      }

      b->left = a;
      a->parent = b;

      return const_iterator(b);
    }

    const_iterator rotateRight(const_iterator it)
    {
      Node *a = const_cast< Node * >(it.getNode());
      if (!a || !a->left) {
        throw std::invalid_argument("Node or left child is null");
      }

      Node *b = a->left;

      a->left = b->right;
      if (b->right) {
        b->right->parent = a;
      }

      b->parent = a->parent;
      if (!a->parent) {
        root = b;
      } else if (a == a->parent->left) {
        a->parent->left = b;
      } else {
        a->parent->right = b;
      }

      b->right = a;
      a->parent = b;

      return const_iterator(b);
    }

    const_iterator rotateLargeLeft(const_iterator it)
    {
      Node *a = const_cast< Node * >(it.getNode());
      if (!a || !a->right) {
        throw std::invalid_argument("Right child is null");
      }

      rotateRight(const_iterator(a->right));
      return rotateLeft(it);
    }

    const_iterator rotateLargeRight(const_iterator it)
    {
      Node *a = const_cast< Node * >(it.getNode());
      if (!a || !a->left) {
        throw std::invalid_argument("Left child is null");
      }

      rotateLeft(const_iterator(a->left));
      return rotateRight(it);
    }

    iterator begin()
    {
      Node *curr = root;
      if (!curr) {
        return iterator(nullptr);
      }
      while (curr->left) {
        curr = curr->left;
      }
      return iterator(curr);
    }

    iterator end()
    {
      return iterator(nullptr);
    }

    const_iterator begin() const
    {
      const Node *curr = root;
      if (!curr) {
        return const_iterator(nullptr);
      }
      while (curr->left) {
        curr = curr->left;
      }
      return const_iterator(curr);
    }

    const_iterator end() const
    {
      return const_iterator(nullptr);
    }
  };

  template < class Key, class Value, class Compare >
  BSTree< Key, Value, Compare > unite(const BSTree< Key, Value, Compare > &t1, const BSTree< Key, Value, Compare > &t2)
  {
    BSTree< Key, Value, Compare > result;
    auto it1 = t1.begin();
    auto it2 = t2.begin();
    Compare comp;

    while (it1 != t1.end() && it2 != t2.end()) {
      if (comp((*it1).first, (*it2).first)) {
        result.push((*it1).first, (*it1).second);
        ++it1;
      } else if (comp((*it2).first, (*it1).first)) {
        result.push((*it2).first, (*it2).second);
        ++it2;
      } else {
        result.push((*it1).first, (*it1).second);
        ++it1;
        ++it2;
      }
    }

    while (it1 != t1.end()) {
      result.push((*it1).first, (*it1).second);
      ++it1;
    }

    while (it2 != t2.end()) {
      result.push((*it2).first, (*it2).second);
      ++it2;
    }

    return result;
  }

  template < class Key, class Value, class Compare >
  BSTree< Key, Value, Compare > intersect(const BSTree< Key, Value, Compare > &t1,
                                          const BSTree< Key, Value, Compare > &t2)
  {
    BSTree< Key, Value, Compare > result;
    auto it1 = t1.begin();
    auto it2 = t2.begin();
    Compare comp;

    while (it1 != t1.end() && it2 != t2.end()) {
      if (comp((*it1).first, (*it2).first)) {
        ++it1;
      } else if (comp((*it2).first, (*it1).first)) {
        ++it2;
      } else {
        result.push((*it1).first, (*it1).second);
        ++it1;
        ++it2;
      }
    }

    return result;
  }

  template < class Key, class Value, class Compare >
  BSTree< Key, Value, Compare > complement(const BSTree< Key, Value, Compare > &t1,
                                           const BSTree< Key, Value, Compare > &t2)
  {
    BSTree< Key, Value, Compare > result;
    auto it1 = t1.begin();
    auto it2 = t2.begin();
    Compare comp;

    while (it1 != t1.end() && it2 != t2.end()) {
      if (comp((*it1).first, (*it2).first)) {
        result.push((*it1).first, (*it1).second);
        ++it1;
      } else if (comp((*it2).first, (*it1).first)) {
        ++it2;
      } else {
        ++it1;
        ++it2;
      }
    }

    while (it1 != t1.end()) {
      result.push((*it1).first, (*it1).second);
      ++it1;
    }

    return result;
  }

}

int main()
{
  std::cout << "S4 init" << "\n";
  return 0;
}
