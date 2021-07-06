#ifndef CACHE_POLICY_HPP
#define CACHE_POLICY_HPP

#include <unordered_set>

namespace esl
{
template <typename Key>
class CachePolicy
{
public:
  virtual ~CachePolicy()
  {
  }
  // handle element insertion in a cache
  virtual void insert(const Key &key) = 0;
  // handle request to the key-element in a cache
  virtual void touch(const Key &key) = 0;
  // handle element deletion from a cache
  virtual void erase(const Key &key) = 0;

  // return a key of a replacement candidate
  virtual const Key &repl_candicate() const = 0;
};

template <typename Key>
class NoCachePolicy : public CachePolicy<Key>
{
public:
  NoCachePolicy() = default;
  ~NoCachePolicy() override = default;

  void insert(const Key &key) override
  {
    key_storage.emplace(key);
  }

  void touch(const Key &key) override
  {
    // do not do anything
  }

  void erase(const Key &key) override
  {
    key_storage.erase(key);
  }

  // return a key of a displacement candidate
  const Key &repl_candicate() const override
  {
    return *key_storage.cbegin();
  }

private:
  std::unordered_set<Key> key_storage;
};
} // namespace esl

#endif // CACHE_POLICY_HPP
