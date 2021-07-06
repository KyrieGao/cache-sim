#ifndef NRU_CACHE_POLICY_HPP
#define NRU_CACHE_POLICY_HPP

#include "cache_policy.hpp"
#include <vector>
#include <utility>

namespace esl
{
template <typename Key>
class NRUCachePolicy : public CachePolicy<Key>
{
public:
  using nru_it_erator = typename std::vector<std::pair<Key, bool>>::iterator;

  NRUCachePolicy() {
    nru_it_ = nru_vec_.end();
  } 
 
  ~NRUCachePolicy() = default;

  // Allocate
  void insert(const Key &key) override
  {
    if (nru_it_ == nru_vec_.end()) {
      nru_vec_.push_back(std::make_pair<Key, bool>((Key)key, (bool)true));
      nru_it_ = nru_vec_.end();
    } else {
      nru_it_->first = key;
      nru_it_->second = true;
      nru_it_++;
      if (nru_it_ == nru_vec_.end()) {
        nru_it_ = nru_vec_.begin();
      }
    }
  }

  // Find and update
  void touch(const Key &key) override
  {
    nru_it_erator it;
    for (it = nru_vec_.begin(); it != nru_vec_.end(); it++) {
      if (key == it->first) {
        it->second = true;
        break;
      }
    }
  }

  void erase(const Key &key) override
  {
    nru_it_erator it;
    for (it = nru_vec_.begin(); it != nru_vec_.end(); it++) {
      if (it->first == key) {
        it->second = false;
        break;
      }
    }
  }

  // Return a key of a displacement candidate
  const Key &repl_candicate() const override
  {
    size_t max_way_num = nru_vec_.size();
    nru_it_erator nru_it = nru_it_;
    nru_it == nru_vec_.end() ? nru_vec_.begin() : nru_it;
    int i = 0;
    while (i < max_way_num) 
    {
      if (nru_it->second) 
      {
        nru_it->second = false;
        nru_it++;
        nru_it == nru_vec_.end() ? nru_vec_.begin() : nru_it;
        i++;
      } 
      else 
      {
        break;
      }      
    }
    return nru_it->first;
  }

private:
  std::vector<std::pair<Key, bool>> nru_vec_;
  nru_it_erator nru_it_;
};
} // namespace esl

#endif // NRU_CACHE_POLICY_HPP
