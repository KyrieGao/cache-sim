#ifndef TPLRU_CACHE_POLICY_HPP
#define TPLRU_CACHE_POLICY_HPP

#include "cache_policy.hpp"
#include <iostream>
#include <vector>
#include <utility>

namespace esl
{
template <typename Key>
class TPLRUCachePolicy : public CachePolicy<Key>
{
public:
  using tplru_it_erator = typename std::vector<std::pair<Key, bool>>::iterator;

  TPLRUCachePolicy() {
    tplru_it_ = tplru_vec_.end();
  } 
 
  ~TPLRUCachePolicy() = default;

  // Allocate
  void insert(const Key &key) override
  {
    if (tplru_it_ == tplru_vec_.end()) {
      tplru_vec_.push_back(std::make_pair<Key, bool>((Key)key, (bool)true));
      tplru_it_ = tplru_vec_.end();
    } else {
      tplru_it_->first = key;
      tplru_it_->second = true;
      tplru_it_++;
      tplru_it_ = (tplru_it_ == tplru_vec_.end()) ? tplru_vec_.begin() : tplru_it_;
    }
  }

  // Find and update
  void touch(const Key &key) override
  {
    tplru_it_erator it;
    for (it = tplru_vec_.begin(); it != tplru_vec_.end(); it++) {
      if (key == it->first) {
        it->second = true;
        break;
      }
    }
  }

  void erase(const Key &key) override
  {
    tplru_it_erator it;
    for (it = tplru_vec_.begin(); it != tplru_vec_.end(); it++) {
      if (it->first == key) {
        it->second = false;
        break;
      }
    }
  }

  // Return a key of a displacement candidate
  const Key &repl_candicate() override
  {
    size_t max_way_num = tplru_vec_.size();
    tplru_it_ = (tplru_it_ == tplru_vec_.end()) ? tplru_vec_.begin() : tplru_it_;
    int i = 0;
    while (i <= max_way_num) 
    {
      if (tplru_it_->second) 
      {
        tplru_it_->second = false;
        tplru_it_++;
        tplru_it_ = (tplru_it_ == tplru_vec_.end()) ? tplru_vec_.begin() : tplru_it_;
        i++;
      } 
      else 
      {
        break;
      }      
    }
    return tplru_it_->first;
  }

private:
  std::vector<std::pair<Key, bool>> tplru_vec_;
  tplru_it_erator tplru_it_;
};
} // namespace esl

#endif // TPLRU_CACHE_POLICY_HPP