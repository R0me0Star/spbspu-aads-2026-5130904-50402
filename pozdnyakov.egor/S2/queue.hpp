#ifndef QUEUE_HPP
#define QUEUE_HPP

#include <iterator>
#include <utility>
#include "list.hpp"

namespace pozdnyakov
{

  template < class T >
  class Queue
  {
  private:
    List< T > container{};
    std::size_t size_{0};

  public:
    Queue():
      container(),
      size_(0)
    {}

    void push(const T &val)
    {
      if (empty()) {
        container.pushFront(val);
      } else {
        LIter< T > tailIter = container.begin();
        std::advance(tailIter, size_ - 1);
        container.insertAfter(tailIter, val);
      }
      ++size_;
    }

    void push(T &&val)
    {
      if (empty()) {
        container.pushFront(std::move(val));
      } else {
        LIter< T > tailIter = container.begin();
        std::advance(tailIter, size_ - 1);
        container.insertAfter(tailIter, std::move(val));
      }
      ++size_;
    }

    void pop()
    {
      if (!empty()) {
        container.popFront();
        --size_;
      }
    }

    T &front()
    {
      return container.front();
    }

    const T &front() const
    {
      return container.front();
    }

    bool empty() const noexcept
    {
      return size_ == 0;
    }
  };

}

#endif
