#ifndef LIST_HPP
#define LIST_HPP

#include <cstddef>
#include <iterator>
#include <utility>

namespace pozdnyakov
{

  namespace detail
  {
    struct BaseNode
    {
      BaseNode *next;

      BaseNode():
        next(nullptr)
      {}

      virtual ~BaseNode() = default;
    };

    template< class T >
    struct Node: BaseNode
    {
      T data;

      Node(const T &value):
        BaseNode(),
        data(value)
      {}

      Node(T &&value):
        BaseNode(),
        data(std::move(value))
      {}
    };
  }

  template< class T >
  class List;

  template< class T >
  class LCIter;

  template< class T >
  class LIter
  {
    friend class List< T >;
    friend class LCIter< T >;

  private:
    detail::BaseNode *ptr;

    explicit LIter(detail::BaseNode *p):
      ptr(p)
    {}

  public:
    using iterator_category = std::forward_iterator_tag;
    using value_type = T;
    using difference_type = std::ptrdiff_t;
    using pointer = T *;
    using reference = T &;

    LIter():
      ptr(nullptr)
    {}

    reference operator*() const
    {
      return static_cast< detail::Node< T > * >(ptr)->data;
    }

    pointer operator->() const
    {
      return &(static_cast< detail::Node< T > * >(ptr)->data);
    }

    LIter &operator++()
    {
      ptr = ptr->next;
      return *this;
    }

    LIter operator++(int)
    {
      LIter temp = *this;
      ptr = ptr->next;
      return temp;
    }

    bool operator==(const LIter &other) const
    {
      return ptr == other.ptr;
    }
    bool operator!=(const LIter &other) const
    {
      return ptr != other.ptr;
    }
  };

  template< class T >
  class LCIter
  {
    friend class List< T >;

  private:
    const detail::BaseNode *ptr;

    explicit LCIter(const detail::BaseNode *p):
      ptr(p)
    {}

  public:
    using iterator_category = std::forward_iterator_tag;
    using value_type = const T;
    using difference_type = std::ptrdiff_t;
    using pointer = const T *;
    using reference = const T &;

    LCIter():
      ptr(nullptr)
    {}
    LCIter(const LIter< T > &other):
      ptr(other.ptr)
    {}

    reference operator*() const
    {
      return static_cast< const detail::Node< T > * >(ptr)->data;
    }

    pointer operator->() const
    {
      return &(static_cast< const detail::Node< T > * >(ptr)->data);
    }

    LCIter &operator++()
    {
      ptr = ptr->next;
      return *this;
    }

    LCIter operator++(int)
    {
      LCIter temp = *this;
      ptr = ptr->next;
      return temp;
    }

    bool operator==(const LCIter &other) const
    {
      return ptr == other.ptr;
    }
    bool operator!=(const LCIter &other) const
    {
      return ptr != other.ptr;
    }
  };

  template< class T >
  class List
  {
  private:
    detail::BaseNode *fakeNode;

  public:
    List()
    {
      fakeNode = new detail::BaseNode();
      fakeNode->next = fakeNode;
    }

    ~List()
    {
      clear();
      delete fakeNode;
    }

    List(const List &other)
    {
      fakeNode = new detail::BaseNode();
      fakeNode->next = fakeNode;
      detail::BaseNode *tail = fakeNode;
      detail::BaseNode *curr = other.fakeNode->next;
      while (curr != other.fakeNode) {
        detail::Node< T > *newNode = new detail::Node< T >(static_cast< detail::Node< T > * >(curr)->data);
        tail->next = newNode;
        tail = newNode;
        curr = curr->next;
      }
      tail->next = fakeNode;
    }

    List &operator=(const List &other)
    {
      if (this != &other) {
        List temp(other);
        std::swap(fakeNode, temp.fakeNode);
      }
      return *this;
    }

    List(List &&other) noexcept:
      fakeNode(other.fakeNode)
    {
      other.fakeNode = new detail::BaseNode();
      other.fakeNode->next = other.fakeNode;
    }

    List &operator=(List &&other) noexcept
    {
      if (this != &other) {
        clear();
        delete fakeNode;
        fakeNode = other.fakeNode;
        other.fakeNode = new detail::BaseNode();
        other.fakeNode->next = other.fakeNode;
      }
      return *this;
    }

    void pushFront(const T &value)
    {
      detail::Node< T > *newNode = new detail::Node< T >(value);
      newNode->next = fakeNode->next;
      fakeNode->next = newNode;
    }

    void pushFront(T &&value)
    {
      detail::Node< T > *newNode = new detail::Node< T >(std::move(value));
      newNode->next = fakeNode->next;
      fakeNode->next = newNode;
    }

    void popFront()
    {
      if (empty())
        return;
      detail::BaseNode *toDelete = fakeNode->next;
      fakeNode->next = toDelete->next;
      delete static_cast< detail::Node< T > * >(toDelete);
    }

    template< class Predicate >
    void removeIf(Predicate predicate)
    {
      if (empty())
        return;

      detail::BaseNode dummyTrue;
      detail::BaseNode dummyFalse;
      detail::BaseNode *trueTail = &dummyTrue;
      detail::BaseNode *falseTail = &dummyFalse;

      detail::BaseNode *tail = fakeNode;
      while (tail->next != fakeNode) {
        tail = tail->next;
      }
      tail->next = nullptr;

      detail::BaseNode *current = fakeNode->next;
      while (current) {
        detail::BaseNode *nextNode = current->next;
        if (predicate(static_cast< detail::Node< T > * >(current)->data)) {
          trueTail->next = current;
          trueTail = current;
        } else {
          falseTail->next = current;
          falseTail = current;
        }
        current = nextNode;
      }

      trueTail->next = nullptr;
      falseTail->next = nullptr;

      if (dummyFalse.next) {
        fakeNode->next = dummyFalse.next;
        falseTail->next = fakeNode;
      } else {
        fakeNode->next = fakeNode;
      }

      detail::BaseNode *toDelete = dummyTrue.next;
      while (toDelete) {
        detail::BaseNode *nextNode = toDelete->next;
        delete static_cast< detail::Node< T > * >(toDelete);
        toDelete = nextNode;
      }
    }

    void clear() noexcept
    {
      while (!empty()) {
        popFront();
      }
    }

    bool empty() const noexcept
    {
      return fakeNode == nullptr || fakeNode->next == fakeNode;
    }

    T &front()
    {
      return static_cast< detail::Node< T > * >(fakeNode->next)->data;
    }

    const T &front() const
    {
      return static_cast< const detail::Node< T > * >(fakeNode->next)->data;
    }

    LIter< T > begin()
    {
      return LIter< T >(fakeNode->next);
    }

    LIter< T > end()
    {
      return LIter< T >(fakeNode);
    }

    LCIter< T > begin() const
    {
      return LCIter< T >(fakeNode->next);
    }

    LCIter< T > end() const
    {
      return LCIter< T >(fakeNode);
    }
  };

}

#endif
