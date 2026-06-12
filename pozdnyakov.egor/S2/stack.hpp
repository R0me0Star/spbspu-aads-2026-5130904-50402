#ifndef STACK_HPP
#define STACK_HPP

#include <utility>
#include "list.hpp"

namespace pozdnyakov
{

  template < class T >
  class Stack
  {
  private:
    List< T > container{};

  public:
    Stack():
      container()
    {}

    void push(const T &val)
    {
      container.pushFront(val);
    }

    void push(T &&val)
    {
      container.pushFront(std::move(val));
    }

    void pop()
    {
      if (!empty()) {
        container.popFront();
      }
    }

    T &top()
    {
      return container.front();
    }

    const T &top() const
    {
      return container.front();
    }

    bool empty() const noexcept
    {
      return container.empty();
    }

    void clear() noexcept
    {
      container.clear();
    }
  };

}

#endif
