#ifndef LRU_CACHE_POLICY_HPP
#define LRU_CACHE_POLICY_HPP

#include "cache_policy.hpp"
#include <list>
#include <unordered_map>

namespace esl
{
template <typename Key>
class LRUCachePolicy : public CachePolicy<Key>
{
public:
  using lru_iterator = typename std::list<Key>::iterator;

  LRUCachePolicy() = default;
  ~LRUCachePolicy() = default;

  void insert(const Key &key) override
  {
    lru_queue.emplace_front(key);
    key_finder[key] = lru_queue.begin();
  }

  void touch(const Key &key) override
  {
    // move the touched element at the beginning of the lru_queue
    lru_queue.splice(lru_queue.begin(), lru_queue, key_finder[key]);
  }

  void erase(const Key &) override
  {
    // remove the least recently used element
    key_finder.erase(lru_queue.back());
    lru_queue.pop_back();
  }

  // return a key of a displacement candidate
  const Key &repl_candicate() override
  {
    return lru_queue.back();
  }

private:
  std::list<Key> lru_queue;
  std::unordered_map<Key, lru_iterator> key_finder;
};
} // namespace esl

#endif // LRU_CACHE_POLICY_HPP
