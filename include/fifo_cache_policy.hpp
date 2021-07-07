#ifndef FIFO_CACHE_POLICY_HPP
#define FIFO_CACHE_POLICY_HPP

#include "cache_policy.hpp"
#include <list>

namespace esl
{
template <typename Key>
class FIFOCachePolicy : public CachePolicy<Key>
{
public:
  FIFOCachePolicy() = default;
  ~FIFOCachePolicy() = default;

  void insert(const Key &key) override
  {
    fifo_queue.emplace_front(key);
  }
  // handle request to the key-element in a cache
  void touch(const Key &key) override
  {
    // nothing to do here in the FIFO strategy
  }
  // handle element deletion from a cache
  void erase(const Key &key) override
  {
    fifo_queue.pop_back();
  }

  // return a key of a replacement candidate
  const Key &repl_candicate() override
  {
    return fifo_queue.back();
  }


  void set_way_num(const size_t way_num) override{
    way_num_ = way_num;
  }
private:
  std::list<Key> fifo_queue;
  size_t way_num_;
};
} // namespace esl

#endif // FIFO_CACHE_POLICY_HPP
