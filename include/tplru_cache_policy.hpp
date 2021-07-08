#ifndef TPLRU_CACHE_POLICY_HPP
#define TPLRU_CACHE_POLICY_HPP

#include "cache_policy.hpp"
#include <iostream>
#include <vector>
#include <utility>

#define SET_T(a) (a = true)
#define SET_F(a) (a = false)


namespace esl
{
template <typename Key>
class TPLRUCachePolicy : public CachePolicy<Key>
{
public:
  using tplru_it_erator = typename std::vector<std::pair<Key, bool>>::iterator;

  TPLRUCachePolicy() = default; 
 
  ~TPLRUCachePolicy() = default;
  // Allocate
  void insert(const Key &key) override
  {
    int index = find_elem(key);
    if (index != -1)
    {
      tplru_vec_[index].second = true;
      update(index);
    }
    else
    {
      if (tplru_vec_.size() < way_num_)
      {
        tplru_vec_.push_back(std::make_pair<Key, bool>((Key)key, (bool)true));
        update(tplru_vec_.size()-1);
      }
      else
      {
        for (int i = 0; i < way_num_; i++)
        {
          if (!tplru_vec_[i].second)
          {
            tplru_vec_[i].first = key;
            tplru_vec_[i].second = true;
            update(i);
          }
        }
      }
      // for (auto it = tplru_vec_.begin(); it != tplru_vec_.end(); it++)
      // {
      //   std::cout << "it->value1=" << it->first << std::endl;
      // }
    }
  }

  // Find and update
  void touch(const Key &key) override
  {
    for (int i = 0; i < tplru_vec_.size(); i++)
    {
      if (tplru_vec_[i].first == key)
      {
        tplru_vec_[i].first = key;
        tplru_vec_[i].second = true;
        if (true) {
          update(i);
          // for (int i = 0; i < way_num_ - 1; i++)
          // {
          //   std::cout << "refer_bit_[" << i << "]=" << refer_bit_[i] << std::endl;
          // }
        }
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
    switch(way_num_) 
    {
      case 1:  {return tplru_vec_[repl_candicate_index(1)].first; break;}
      case 2:  {return tplru_vec_[repl_candicate_index(2)].first; break;}
      case 4:  {return tplru_vec_[repl_candicate_index(4)].first; break;}
      case 8:  {return tplru_vec_[repl_candicate_index(8)].first; break;}
      case 16: {return tplru_vec_[repl_candicate_index(16)].first; break;}
      default: {throw std::range_error{"No such way num in the cache"}; break;}
    }
  }

  void set_way_num(const size_t way_num) override {
    way_num_ = way_num;
    for (int i = 0; i < way_num_ - 1; i++) 
    {
      bool tmp = false;
      refer_bit_.push_back(tmp);
    }
  }

protected:
  const size_t repl_candicate_index(const size_t way_num)
  {
    switch(way_num)
    {
      case 1:
      {
        return 0;
        break;
      }
      case 2: 
      {
        return (refer_bit_[0] ? 1 : 0);
        break;
      }
      case 4:
      {
        if (!refer_bit_[0] && !refer_bit_[1]) return 0; 
        if (!refer_bit_[0] &&  refer_bit_[1]) return 1; 
        if ( refer_bit_[0] && !refer_bit_[2]) return 2; 
        if ( refer_bit_[0] &&  refer_bit_[2]) return 3; 
        break;
      }
      case 8: 
      {
        if (!refer_bit_[0] && !refer_bit_[1] && !refer_bit_[3]) return 0; 
        if (!refer_bit_[0] && !refer_bit_[1] &&  refer_bit_[3]) return 1; 
        if (!refer_bit_[0] &&  refer_bit_[1] && !refer_bit_[4]) return 2; 
        if (!refer_bit_[0] &&  refer_bit_[1] &&  refer_bit_[4]) return 3; 
        if ( refer_bit_[0] && !refer_bit_[2] && !refer_bit_[5]) return 4; 
        if ( refer_bit_[0] && !refer_bit_[2] &&  refer_bit_[5]) return 5; 
        if ( refer_bit_[0] &&  refer_bit_[2] && !refer_bit_[6]) return 6; 
        if ( refer_bit_[0] &&  refer_bit_[2] &&  refer_bit_[6]) return 7;
        break;
      }
      case 16:
      {
        if (!refer_bit_[0] && !refer_bit_[1] && !refer_bit_[3] && !refer_bit_[7])  return 0; 
        if (!refer_bit_[0] && !refer_bit_[1] && !refer_bit_[3] &&  refer_bit_[7])  return 1; 
        if (!refer_bit_[0] && !refer_bit_[1] &&  refer_bit_[3] && !refer_bit_[8])  return 2; 
        if (!refer_bit_[0] && !refer_bit_[1] &&  refer_bit_[3] &&  refer_bit_[8])  return 3; 
        if (!refer_bit_[0] &&  refer_bit_[1] && !refer_bit_[4] && !refer_bit_[9])  return 4; 
        if (!refer_bit_[0] &&  refer_bit_[1] && !refer_bit_[4] &&  refer_bit_[9])  return 5; 
        if (!refer_bit_[0] &&  refer_bit_[1] &&  refer_bit_[4] && !refer_bit_[10]) return 6; 
        if (!refer_bit_[0] &&  refer_bit_[1] &&  refer_bit_[4] &&  refer_bit_[10]) return 7; 
        if ( refer_bit_[0] && !refer_bit_[2] && !refer_bit_[5] && !refer_bit_[11]) return 8; 
        if ( refer_bit_[0] && !refer_bit_[2] && !refer_bit_[5] &&  refer_bit_[11]) return 9; 
        if ( refer_bit_[0] && !refer_bit_[2] &&  refer_bit_[5] && !refer_bit_[12]) return 10; 
        if ( refer_bit_[0] && !refer_bit_[2] &&  refer_bit_[5] &&  refer_bit_[12]) return 11; 
        if ( refer_bit_[0] &&  refer_bit_[2] && !refer_bit_[6] && !refer_bit_[13]) return 12; 
        if ( refer_bit_[0] &&  refer_bit_[2] && !refer_bit_[6] &&  refer_bit_[13]) return 13; 
        if ( refer_bit_[0] &&  refer_bit_[2] &&  refer_bit_[6] && !refer_bit_[14]) return 14;
        if ( refer_bit_[0] &&  refer_bit_[2] &&  refer_bit_[6] &&  refer_bit_[14]) return 15;
        break;
      }
    }
    return -1;
  }

  void update(const size_t i) {
    switch(way_num_)
    {
      case 1: {break;}
      case 2:
      {
        switch(i) {
          case 0: {SET_T(refer_bit_[0]); break;}
          case 1: {SET_F(refer_bit_[0]); break;}
        }
        break;
      }
      case 4:
      {
        switch(i){
          case 0: {SET_T(refer_bit_[0]); SET_T(refer_bit_[1]); break;}
          case 1: {SET_T(refer_bit_[0]); SET_F(refer_bit_[1]); break;}
          case 2: {SET_F(refer_bit_[0]); SET_T(refer_bit_[2]); break;}
          case 3: {SET_F(refer_bit_[0]); SET_F(refer_bit_[2]); break;}
        }
        break;
      }
      case 8:
      {
        switch (i) {
          case 0: case 1: {SET_T(refer_bit_[0]); SET_T(refer_bit_[1]); SET_T(refer_bit_[3]); break;}
          case 2: case 3: {SET_T(refer_bit_[0]); SET_T(refer_bit_[1]); SET_T(refer_bit_[4]); break;}
          case 4: case 5: {SET_T(refer_bit_[0]); SET_T(refer_bit_[2]); SET_T(refer_bit_[5]); break;}
          case 6: case 7: {SET_T(refer_bit_[0]); SET_T(refer_bit_[2]); SET_T(refer_bit_[6]); break;}
        }
        break;
      }
      case 16:
      {
        switch(i) {
          case 0: case 1:   {SET_T(refer_bit_[0]); SET_T(refer_bit_[1]); SET_T(refer_bit_[3]); SET_T(refer_bit_[7] ); break;}
          case 2: case 3:   {SET_T(refer_bit_[0]); SET_T(refer_bit_[1]); SET_T(refer_bit_[4]); SET_T(refer_bit_[8] ); break;}
          case 4: case 5:   {SET_T(refer_bit_[0]); SET_T(refer_bit_[2]); SET_T(refer_bit_[5]); SET_T(refer_bit_[9] ); break;}
          case 6: case 7:   {SET_T(refer_bit_[0]); SET_T(refer_bit_[2]); SET_T(refer_bit_[6]); SET_T(refer_bit_[10]); break;}
          case 8: case 9:   {SET_T(refer_bit_[0]); SET_T(refer_bit_[1]); SET_T(refer_bit_[3]); SET_T(refer_bit_[11]); break;}
          case 10: case 11: {SET_T(refer_bit_[0]); SET_T(refer_bit_[1]); SET_T(refer_bit_[4]); SET_T(refer_bit_[12]); break;}
          case 12: case 13: {SET_T(refer_bit_[0]); SET_T(refer_bit_[2]); SET_T(refer_bit_[5]); SET_T(refer_bit_[13]); break;}
          case 14: case 15: {SET_T(refer_bit_[0]); SET_T(refer_bit_[2]); SET_T(refer_bit_[6]); SET_T(refer_bit_[14]); break;}
        }
        break;
      }
      break;
    }
  }

  int find_elem(const Key &key) 
  {
    for (int i = 0; i < tplru_vec_.size(); i++)
    {
      if (tplru_vec_[i].first == key) {
        return i;
      }
    }
    return -1;
  }

private:
  std::vector<std::pair<Key, bool>> tplru_vec_;
  size_t way_num_;
  std::vector<bool> refer_bit_;
};
} // namespace esl

#endif // TPLRU_CACHE_POLICY_HPP