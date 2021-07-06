#ifndef CACHE_HPP
#define CACHE_HPP

#include "cache_policy.hpp"

#include <cstddef>
#include <functional>
#include <limits>
#include <memory>
#include <mutex>
#include <utility>
#include <vector>
#include <unordered_map>

namespace esl
{
// Base class for caching algorithms
template <typename Key, typename Value, typename Policy = NoCachePolicy<Key>>
class fixed_sized_cache
{
public:
  using iterator = typename std::unordered_map<Key, Value>::iterator;
  using const_iterator =
      typename std::unordered_map<Key, Value>::const_iterator;
  using operation_guard = typename std::lock_guard<std::mutex>;
  using Callback =
      typename std::function<void(const Key &key, const Value &value)>;

  explicit fixed_sized_cache(
      size_t max_size, const Policy &policy = Policy(),
      Callback OnErase = [](const Key &, const Value &) {})
      : cache_policy(policy), max_cache_size(max_size),
        OnEraseCallback(OnErase)
  {
    if (max_cache_size == 0)
    {
      max_cache_size = std::numeric_limits<size_t>::max();
    }
  }

  ~fixed_sized_cache()
  {
    Clear();
  }

  void Put(const Key &key, const Value &value)
  {
    operation_guard lock{safe_op};
    auto elem_it = FindElem(key);

    if (elem_it == cache_items_map.end())
    {
      // add new element to the cache
      if (cache_items_map.size() + 1 > max_cache_size)
      {
        auto disp_candicate_key = cache_policy.repl_candicate();

        Erase(disp_candicate_key);
      }

      Insert(key, value);
    }
    else
    {
      // update previous value
      Update(key, value);
    }
  }

  const Value &Get(const Key &key) const
  {
    operation_guard lock{safe_op};
    auto elem_it = FindElem(key);

    if (elem_it == cache_items_map.end())
    {
      throw std::range_error{"No such element in the cache"};
    }
    cache_policy.touch(key);

    return elem_it->second;
  }

  bool Cached(const Key &key) const
  {
    operation_guard lock{safe_op};
    return FindElem(key) != cache_items_map.end();
  }

  size_t Size() const
  {
    operation_guard lock{safe_op};

    return cache_items_map.size();
  }

  bool Remove(const Key &key)
  {
    operation_guard{safe_op};

    if (cache_items_map.find(key) == cache_items_map.cend())
    {
      return false;
    }

    Erase(key);

    return true;
  }

  void Clear()
  {
    operation_guard lock{safe_op};

    for (auto it = begin(); it != end(); ++it)
    {
      cache_policy.erase(it->first);
      OnEraseCallback(it->first, it->second);
    }
    cache_items_map.clear();
  }

  typename std::unordered_map<Key, Value>::const_iterator begin() const
  {
    return cache_items_map.begin();
  }

  typename std::unordered_map<Key, Value>::const_iterator end() const
  {
    return cache_items_map.end();
  }

protected:
  void Insert(const Key &key, const Value &value)
  {
    cache_policy.insert(key);
    cache_items_map.emplace(std::make_pair(key, value));
  }

  void Erase(const Key &key)
  {
    cache_policy.erase(key);

    auto elem_it = FindElem(key);
    OnEraseCallback(key, elem_it->second);
    cache_items_map.erase(elem_it);
  }

  void Update(const Key &key, const Value &value)
  {
    cache_policy.touch(key);
    cache_items_map[key] = value;
  }

  const_iterator FindElem(const Key &key) const
  {
    return cache_items_map.find(key);
  }

private:
  std::unordered_map<Key, Value> cache_items_map;
  mutable Policy cache_policy;
  mutable std::mutex safe_op;
  size_t max_cache_size;
  Callback OnEraseCallback;
};

// CacheSet: One cache set with some ways
template <typename Tag, typename Data, typename Policy = NoCachePolicy<Tag>>
class CacheSet
{
public:
  using const_iterator = typename std::unordered_map<Tag, Data>::const_iterator;
  CacheSet () = default;
  CacheSet(size_t max_way_num, const Policy &policy = Policy())
      : max_way_num_(max_way_num),
        replace_policy_(policy)
  {}

  ~CacheSet()
  {
    cache_set_map_.clear();
  }

  void invalid()
  {
    cache_set_map_.clear();
  }

  bool is_cached(const Tag &tag) const
  {
    return find_elem(tag) != cache_set_map_.end();
  }

  const Data &get(const Tag &tag) const
  {
    auto elem_it = find_elem(tag);
    if (elem_it == cache_set_map_.end())
    {
      throw std::range_error{"No such element in the cache"};
    }
    replace_policy_.touch(tag);
    return elem_it->second;
  }

  void put(const Tag &tag, const Data &data)
  {
    auto elem_it = find_elem(tag);
    // if not found
    if (elem_it == cache_set_map_.end())
    {
      if (cache_set_map_.size() >= max_way_num_)
      {
        auto disp_candicate_tag = replace_policy_.repl_candicate();
        erase(disp_candicate_tag);
      }
      insert(tag, data);
    }
    else 
    {
      update(tag, data);
    }
  }

  size_t get_size() const
  {
    return cache_set_map_.size();
  }

  bool is_success_remove(const Tag &tag)
  {
    if (cache_set_map_.find(tag) == cache_set_map_.cend())
    {
      return false;
    }
    erase(tag);
    return true;
  }

  void clear_cache() 
  {
    clear(); 
  }

protected:
  void clear() 
  {
    for (const_iterator it = cache_set_map_.begin(); it != cache_set_map_.end(); it++) {
      replace_policy_.erase(it->first);
    }
    cache_set_map_.clear();
  }

  void insert(const Tag &tag, const Data &data)
  {
    replace_policy_.insert(tag);
    cache_set_map_.emplace(std::make_pair(tag, data));
  }

  void erase(const Tag &tag)
  {
    replace_policy_.erase(tag);

    auto elem_it = find_elem(tag);
    cache_set_map_.erase(elem_it);
  }

  void update(const Tag &tag, const Data &data)
  {
    replace_policy_.touch(tag);
    cache_set_map_[tag] = data;
  }

  const_iterator find_elem(const Tag &tag) const
  {
    return cache_set_map_.find(tag);
  }
private:
  size_t max_way_num_;
  mutable Policy replace_policy_;
  std::unordered_map<Tag, Data> cache_set_map_;
};

// Cache: All cache set implement
// 初始化
// 查找并返回状态
// 更新，包括hit的更新和miss的更新（调用替换策略）
// 回填，allocate（调用替换策略）
// invalid函数
// clean，牵扯到写策略，TODO
template <typename Tag, typename Data, typename Policy = NoCachePolicy<Tag>>
class Cache
{
public:
  // using const_iterator = typename std::vector<CacheSet<Tag, Data, Policy>>::const_iterator;
  explicit Cache(size_t max_set_num, size_t max_way_num, const Policy &policy = Policy())
      : max_set_num_(max_set_num),
        max_way_num_(max_way_num),
        replace_policy_(policy)
  {
    CacheSet<Tag, Data, Policy> cache_set(max_way_num_, replace_policy_);
    for (int i = 0; i < max_set_num_; i++) {
      cache_.push_back(cache_set);
    }
  }

  ~Cache()
  {
  }

  bool is_cached(const size_t index, const Tag &tag) const
  {
    return cache_[index].is_cached(tag);
  }

  const Data &get(const size_t index, const Tag &tag) const
  {
    return cache_[index].get(tag);
  }
  
  void put(const size_t index, const Tag &tag, const Data &data)
  {
    cache_[index].put(tag, data);
  }

  void clear_cache() 
  {
    for (int i = 0; i < max_set_num_; i++) {
      cache_[i].clear_cache();
    }
    cache_.clear();
  }

  bool is_success_remove(const size_t index, const Tag &tag) 
  {
    return cache_[index].is_success_remove(tag);
  }

  size_t get_cache_set_size (const size_t index)
  {
    return cache_[index].get_size();
  }
protected:

private:
  size_t max_set_num_;
  size_t max_way_num_;
  mutable Policy replace_policy_;
  std::vector<CacheSet<Tag, Data, Policy>> cache_;
};
} // namespace esl
#endif // CACHE_HPP
