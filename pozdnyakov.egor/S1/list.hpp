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
      if (ptr) {
        ptr = ptr->next;
      }
      return *this;
    }

    LIter operator++(int)
    {
      const LIter tmp = *this;
      ++(*this);
      return tmp;
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
      if (ptr) {
        ptr = ptr->next;
      }
      return *this;
    }

    LCIter operator++(int)
    {
      const LCIter tmp = *this;
      ++(*this);
      return tmp;
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

    detail::BaseNode *getPrevious(const detail::BaseNode *node) const noexcept
    {
      detail::BaseNode *current = fakeNode;
      while (current->next != node) {
        current = current->next;
      }
      return current;
    }

    void makeLinear() noexcept
    {
      if (!empty()) {
        detail::BaseNode *tail = getPrevious(fakeNode);
        tail->next = nullptr;
      }
    }

    void makeCircular() noexcept
    {
      detail::BaseNode *tail = fakeNode;
      while (tail->next) {
        tail = tail->next;
      }
      tail->next = fakeNode;
    }

    template< class Compare >
    detail::BaseNode *mergeLists(detail::BaseNode *first1, detail::BaseNode *first2, Compare comp) noexcept
    {
      detail::BaseNode dummy;
      detail::BaseNode *current = &dummy;

      while (first1 && first2) {
        if (comp(static_cast< detail::Node< T > * >(first2)->data, static_cast< detail::Node< T > * >(first1)->data)) {
          current->next = first2;
          first2 = first2->next;
        } else {
          current->next = first1;
          first1 = first1->next;
        }
        current = current->next;
      }

      current->next = first1 ? first1 : first2;
      return dummy.next;
    }

    template< class Compare >
    detail::BaseNode *mergeSort(detail::BaseNode *head, Compare comp) noexcept
    {
      if (!head || !head->next) {
        return head;
      }

      detail::BaseNode *slow = head;
      detail::BaseNode *fast = head->next;

      while (fast && fast->next) {
        slow = slow->next;
        fast = fast->next->next;
      }

      detail::BaseNode *mid = slow->next;
      slow->next = nullptr;

      detail::BaseNode *left = mergeSort(head, comp);
      detail::BaseNode *right = mergeSort(mid, comp);

      return mergeLists(left, right, comp);
    }

  public:
    List()
    {
      fakeNode = new detail::BaseNode();
      fakeNode->next = fakeNode;
    }

    ~List() noexcept
    {
      clear();
      delete fakeNode;
    }

    List(const List &other)
    {
      fakeNode = new detail::BaseNode();
      fakeNode->next = fakeNode;
      if (other.empty()) {
        return;
      }

      try {
        detail::BaseNode *tail = fakeNode;
        for (auto it = other.cbegin(); it != other.cend(); ++it) {
          detail::Node< T > *newNode = new detail::Node< T >(*it);
          newNode->next = fakeNode;
          tail->next = newNode;
          tail = newNode;
        }
      } catch (...) {
        clear();
        delete fakeNode;
        throw;
      }
    }

    List(List &&other) noexcept:
      fakeNode(other.fakeNode)
    {
      other.fakeNode = nullptr;
    }

    List &operator=(const List &other)
    {
      if (this != &other) {
        List tmp(other);
        std::swap(fakeNode, tmp.fakeNode);
      }
      return *this;
    }

    List &operator=(List &&other) noexcept
    {
      if (this != &other) {
        clear();
        delete fakeNode;
        fakeNode = other.fakeNode;
        other.fakeNode = nullptr;
      }
      return *this;
    }

    void pushFront(const T &value)
    {
      if (!fakeNode) {
        fakeNode = new detail::BaseNode();
        fakeNode->next = fakeNode;
      }
      detail::Node< T > *newNode = new detail::Node< T >(value);
      newNode->next = fakeNode->next;
      fakeNode->next = newNode;
    }

    void pushFront(T &&value)
    {
      if (!fakeNode) {
        fakeNode = new detail::BaseNode();
        fakeNode->next = fakeNode;
      }
      detail::Node< T > *newNode = new detail::Node< T >(std::move(value));
      newNode->next = fakeNode->next;
      fakeNode->next = newNode;
    }

    void popFront() noexcept
    {
      if (!empty()) {
        detail::BaseNode *temp = fakeNode->next;
        fakeNode->next = temp->next;
        delete static_cast< detail::Node< T > * >(temp);
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
      return LIter< T >(fakeNode ? fakeNode->next : nullptr);
    }

    LIter< T > end()
    {
      return LIter< T >(fakeNode);
    }

    LCIter< T > cbegin() const
    {
      return LCIter< T >(fakeNode ? fakeNode->next : nullptr);
    }

    LCIter< T > cend() const
    {
      return LCIter< T >(fakeNode);
    }

    void splice(LCIter< T > pos, List &other) noexcept
    {
      if (other.empty() || this == &other) {
        return;
      }
      splice(pos, other, other.cbegin(), other.cend());
    }

    void splice(LCIter< T > pos, List &other, LCIter< T > it) noexcept
    {
      if (this == &other || it == other.cend()) {
        return;
      }
      LCIter< T > nextIt = it;
      ++nextIt;
      splice(pos, other, it, nextIt);
    }

    void splice(LCIter< T > pos, List &other, LCIter< T > first, LCIter< T > last) noexcept
    {
      if (first == last || (this == &other && pos == first)) {
        return;
      }

      detail::BaseNode *posPrev = getPrevious(pos.ptr);
      detail::BaseNode *firstPrev = other.getPrevious(first.ptr);
      detail::BaseNode *lastPrev = other.getPrevious(last.ptr);

      detail::BaseNode *savedPosNext = posPrev->next;
      firstPrev->next = const_cast< detail::BaseNode * >(last.ptr);

      lastPrev->next = savedPosNext;
      posPrev->next = const_cast< detail::BaseNode * >(first.ptr);
    }

    template< class Compare >
    void merge(List &other, Compare comp) noexcept
    {
      if (this == &other || other.empty()) {
        return;
      }

      makeLinear();
      other.makeLinear();

      detail::BaseNode *first1 = fakeNode->next;
      detail::BaseNode *first2 = other.fakeNode->next;

      fakeNode->next = mergeLists(first1, first2, comp);
      makeCircular();

      other.fakeNode->next = other.fakeNode;
    }

    void merge(List &other) noexcept
    {
      merge(other, [](const T &a, const T &b) {
        return a < b;
      });
    }

    template< class Compare >
    void sort(Compare comp) noexcept
    {
      if (empty() || fakeNode->next->next == fakeNode) {
        return;
      }

      makeLinear();
      fakeNode->next = mergeSort(fakeNode->next, comp);
      makeCircular();
    }

    void sort() noexcept
    {
      sort([](const T &a, const T &b) {
        return a < b;
      });
    }

    template< class Predicate >
    void partition(Predicate predicate) noexcept
    {
      if (empty()) {
        return;
      }

      makeLinear();

      detail::BaseNode dummyTrue;
      detail::BaseNode *trueTail = &dummyTrue;

      detail::BaseNode dummyFalse;
      detail::BaseNode *falseTail = &dummyFalse;

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

      if (dummyTrue.next) {
        fakeNode->next = dummyTrue.next;
        trueTail->next = dummyFalse.next;
        if (dummyFalse.next) {
          falseTail->next = fakeNode;
        } else {
          trueTail->next = fakeNode;
        }
      } else {
        fakeNode->next = dummyFalse.next;
        falseTail->next = fakeNode;
      }
    }
  };

}

#endif
