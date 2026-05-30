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
      }
      else {
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
      return 0; // TODO
    }

    size_t height(const_iterator it) const
    {
      return 0; // TODO
    }

    const_iterator rotateLeft(const_iterator it)
    {
      return it; // TODO
    }

    const_iterator rotateRight(const_iterator it)
    {
      return it; // TODO
    }

    const_iterator rotateLargeLeft(const_iterator it)
    {
      return it; // TODO
    }

    const_iterator rotateLargeRight(const_iterator it)
    {
      return it; // TODO
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

}

int main()
{
  std::cout << "S4 init" << std::endl;
  return 0;
}
