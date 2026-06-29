#ifndef QUEUE_HPP
#define QUEUE_HPP

#include "../common/list.hpp"
#include <iterator>
#include <utility>
#include <cstddef>

namespace pozdnyakov
{
  template < class T >
  class Queue
  {
  private:
    List< T > container_{};
    std::size_t size_{0};

  public:
    Queue():
      container_(),
      size_(0)
    {}

    void push(const T &val)
    {
      if (empty()) {
        container_.pushFront(val);
      } else {
        LIter< T > tailIter = container_.begin();
        std::advance(tailIter, size_ - 1);
        container_.insertAfter(tailIter, val);
      }
      ++size_;
    }

    void push(T &&val)
    {
      if (empty()) {
        container_.pushFront(std::move(val));
      } else {
        LIter< T > tailIter = container_.begin();
        std::advance(tailIter, size_ - 1);
        container_.insertAfter(tailIter, std::move(val));
      }
      ++size_;
    }

    void pop()
    {
      if (!empty()) {
        container_.popFront();
        --size_;
      }
    }

    T &front()
    {
      return container_.front();
    }

    const T &front() const
    {
      return container_.front();
    }

    bool empty() const noexcept
    {
      return size_ == 0;
    }
  };
}

#endif
