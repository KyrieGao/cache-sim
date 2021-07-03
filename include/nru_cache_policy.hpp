#ifndef NRU_CACHE_POLICY_HPP
#define NRU_CACHE_POLICY_HPP

#include "cache_policy.hpp"
#include <vector>
#include <unordered_map>

namespace esl
{
template <typename Key>
class NRUCachePolicy : public CachePolicy<Key>
{
public:
  using nru_iterator = typename std::vector<Key>::iterator;

  NRUCachePolicy() : max_way_num_(16), index_(0)
  {
    std::unique_ptr<Key[]> cache_set(new Key[max_way_num_]);
    std::unique_ptr<bool[]> value(new bool[max_way_num_]);
    std::unique_ptr<bool[]> flag(new bool[max_way_num_]);
    cache_set_ = std::move(cache_set);
    value_ = std::move(value);
    flag_ = std::move(flag);
  }
  ~NRUCachePolicy() = default;

  // Allocate
  void Insert(const Key &key) override
  {
    cache_set_[index_] = key;
    flag_[index_] = true;
    index_ = (index_ + 1) % max_way_num_;
    /*
      if (!value_[index_]) {
        value_[index_] = true;
        flag_[index_] = false;
        index_ = (index_ + 1) % max_way_num_;
      } else {
        while (true) {
          if (flag_[index_] == true) {
            flag_[index_] = false;
            index_ = (index_ + 1) % max_way_num_;
          } else {
            cache_set_[index_] = key;
            flag_[index_] = true;
            index_ = (index_ + 1) % max_way_num_;
            break;
          }
        }
      }
      */
  }

  // find and update
  void Touch(const Key &key) override
  {
    size_t temp = index_;
    while (true)
    {
      if (value_[index_] == true && cache_set_[index_] == key)
      {
        flag_[index_] = true;
        index_ = (index_ + 1) % max_way_num_;
        break;
      }
      else
      {
        flag_[index_] == false;
        index_ = (index_ + 1) % max_way_num_;
        if (temp == index_)
        {
          break;
        }
      }
    }
  }

  void Erase(const Key &) override
  {
    // remove the least recently used element
  }

  // return a key of a displacement candidate
  const Key &ReplCandidate() const override
  {
    return;
  }

private:
  size_t max_way_num_;
  std::unique_ptr<Key[]> cache_set_;
  std::unique_ptr<bool[]> value_;
  std::unique_ptr<bool[]> flag_;
  size_t index_;
};
} // namespace esl

#endif // NRU_CACHE_POLICY_HPP
