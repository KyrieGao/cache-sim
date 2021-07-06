#ifndef TPLRU_CACHE_POLICY_HPP
#define TPLRU_CACHE_POLICY_HPP

#include "cache_policy.hpp"
#include <list>

namespace esl
{
template <typename Key>
class TPLRUCachePolicy : public CachePolicy<Key>
{
public:
  TPLRUCachePolicy() = default;
  ~TPLRUCachePolicy() = default;

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
  const Key &repl_candicate() const override
  {
    return fifo_queue.back();
  }

private:
  std::list<Key> fifo_queue;
};
} // namespace esl

#endif // TPLRU_CACHE_POLICY_HPP
