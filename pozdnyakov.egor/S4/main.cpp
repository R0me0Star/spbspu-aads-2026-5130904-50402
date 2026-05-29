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

int main()
{
  std::cout << "S4 init" << std::endl;
  return 0;
}
