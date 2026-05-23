#ifndef HASH_TABLE_HPP
#define HASH_TABLE_HPP

#include <stdexcept>
#include <utility>
#include "list.hpp"
#include "vector.hpp"

namespace pozdnyakov
{
  template < class Key >
  struct DefaultEqual
  {
    bool operator()(const Key &lhs, const Key &rhs) const
    {
      return lhs == rhs;
    }
  };

  template < class Key, class Value, class Hash, class Equal = DefaultEqual< Key > >
  class HashTable
  {
  private:
    Vector< List< std::pair< Key, Value > > > buckets_;
    std::size_t size_;
    Hash hash_obj_;
    Equal equal_obj_;

  public:
    class iterator
    {
      friend class HashTable;

    private:
      Vector< List< std::pair< Key, Value > > > *buckets_ptr_;
      std::size_t bucket_idx_;
      LIter< std::pair< Key, Value > > list_it_;

      void skipEmpty()
      {
        if (!buckets_ptr_)
          return;
        while (bucket_idx_ < buckets_ptr_->size()) {
          if (list_it_ != (*buckets_ptr_)[bucket_idx_].end()) {
            return;
          }
          ++bucket_idx_;
          if (bucket_idx_ < buckets_ptr_->size()) {
            list_it_ = (*buckets_ptr_)[bucket_idx_].begin();
          }
        }
        bucket_idx_ = buckets_ptr_->size();
      }

      iterator(Vector< List< std::pair< Key, Value > > > *b_ptr, std::size_t idx, LIter< std::pair< Key, Value > > it):
        buckets_ptr_(b_ptr),
        bucket_idx_(idx),
        list_it_(it)
      {
        skipEmpty();
      }

    public:
      using iterator_category = std::forward_iterator_tag;
      using value_type = std::pair< Key, Value >;
      using difference_type = std::ptrdiff_t;
      using pointer = std::pair< Key, Value > *;
      using reference = std::pair< Key, Value > &;

      iterator():
        buckets_ptr_(nullptr),
        bucket_idx_(0),
        list_it_()
      {}

      reference operator*() const
      {
        return *list_it_;
      }
      pointer operator->() const
      {
        return &(*list_it_);
      }

      iterator &operator++()
      {
        ++list_it_;
        skipEmpty();
        return *this;
      }

      iterator operator++(int)
      {
        iterator temp = *this;
        ++(*this);
        return temp;
      }

      bool operator==(const iterator &other) const
      {
        if (bucket_idx_ == buckets_ptr_->size() && other.bucket_idx_ == other.buckets_ptr_->size())
          return true;
        return bucket_idx_ == other.bucket_idx_ && list_it_ == other.list_it_;
      }

      bool operator!=(const iterator &other) const
      {
        return !(*this == other);
      }
    };

    HashTable():
      size_(0)
    {}

    void add(Key k, Value v)
    {
      if (buckets_.size() == 0) {
        rehash(16);
      }

      std::size_t idx = hash_obj_(k) % buckets_.size();

      for (auto it = buckets_[idx].begin(); it != buckets_[idx].end(); ++it) {
        if (equal_obj_((*it).first, k)) {
          (*it).second = std::move(v);
          return;
        }
      }

      buckets_[idx].pushFront(std::make_pair(std::move(k), std::move(v)));
      ++size_;
    }

    Value drop(Key k)
    {
      if (buckets_.size() == 0) {
        throw std::runtime_error("Hash table is empty");
      }

      std::size_t idx = hash_obj_(k) % buckets_.size();

      List< std::pair< Key, Value > > newList;
      Value droppedVal;
      bool found = false;

      for (auto it = buckets_[idx].begin(); it != buckets_[idx].end(); ++it) {
        if (equal_obj_((*it).first, k)) {
          droppedVal = std::move((*it).second);
          found = true;
        } else {
          newList.pushFront(*it);
        }
      }

      if (!found) {
        throw std::runtime_error("Key not found");
      }

      buckets_[idx] = std::move(newList);
      --size_;
      return droppedVal;
    }

    bool has(Key k)
    {
      if (buckets_.size() == 0)
        return false;
      std::size_t idx = hash_obj_(k) % buckets_.size();
      for (auto it = buckets_[idx].begin(); it != buckets_[idx].end(); ++it) {
        if (equal_obj_((*it).first, k)) {
          return true;
        }
      }
      return false;
    }

    Value &at(Key k)
    {
      if (buckets_.size() == 0)
        throw std::runtime_error("Key not found");
      std::size_t idx = hash_obj_(k) % buckets_.size();
      for (auto it = buckets_[idx].begin(); it != buckets_[idx].end(); ++it) {
        if (equal_obj_((*it).first, k)) {
          return (*it).second;
        }
      }
      throw std::runtime_error("Key not found");
    }

    void rehash(std::size_t slots)
    {
      if (slots == 0)
        slots = 1;
      Vector< List< std::pair< Key, Value > > > new_buckets;
      new_buckets.resize(slots);

      for (std::size_t i = 0; i < buckets_.size(); ++i) {
        for (auto it = buckets_[i].begin(); it != buckets_[i].end(); ++it) {
          std::size_t idx = hash_obj_((*it).first) % slots;
          new_buckets[idx].pushFront(std::move(*it));
        }
      }

      buckets_ = std::move(new_buckets);
    }

    std::size_t size() const
    {
      return size_;
    }
    bool empty() const
    {
      return size_ == 0;
    }

    iterator begin()
    {
      if (buckets_.size() == 0)
        return end();
      return iterator(&buckets_, 0, buckets_[0].begin());
    }

    iterator end()
    {
      return iterator(&buckets_, buckets_.size(), LIter< std::pair< Key, Value > >());
    }
  };
}

#endif
