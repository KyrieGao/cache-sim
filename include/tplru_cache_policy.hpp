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

  void Insert(const Key &key) override
  {
    fifo_queue.emplace_front(key);
  }
  // handle request to the key-element in a cache
  void Touch(const Key &key) override
  {
    // nothing to do here in the FIFO strategy
  }
  // handle element deletion from a cache
  void Erase(const Key &key) override
  {
    fifo_queue.pop_back();
  }

  // return a key of a replacement candidate
  const Key &ReplCandidate() const override
  {
    return fifo_queue.back();
  }

private:
  std::list<Key> fifo_queue;
};
} // namespace esl

#endif // TPLRU_CACHE_POLICY_HPP
