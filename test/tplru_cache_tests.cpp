#include "cache.hpp"
#include "tplru_cache_policy.hpp"
#include <gtest/gtest.h>

template <typename Key, typename Value>
using esl_tplru_cache_t = typename esl::Cache<Key, Value, esl::TPLRUCachePolicy<Key>>;

TEST(TPLRUCache, ESL_Simple_Test)
{
  esl_tplru_cache_t<int, int> fc(1, 4);
  EXPECT_EQ(fc.is_cached(0, 1), false);
  fc.put(0, 1, 10);
  EXPECT_EQ(fc.is_cached(0, 1), true);
  EXPECT_EQ(fc.get_cache_set_size(0), 1);
  fc.put(0, 2, 20);
  EXPECT_EQ(fc.is_cached(0, 2), true);
  EXPECT_EQ(fc.get_cache_set_size(0), 2);
  fc.put(0, 2, 30);
  EXPECT_EQ(fc.is_cached(0, 2), true);
  EXPECT_EQ(fc.get_cache_set_size(0), 2);
  EXPECT_EQ(fc.get(0, 2), 30);
  EXPECT_EQ(fc.is_success_remove(0, 2), true); 
  EXPECT_EQ(fc.is_success_remove(0, 2), false);
  fc.clear_cache();
  EXPECT_EQ(fc.get_cache_set_size(0), 0);
}

TEST(TPLRUCache, ESL_Missing_Value)
{
  esl_tplru_cache_t<int, int> fc(1, 2);

  fc.put(0, 1, 10);
  EXPECT_EQ(fc.get_cache_set_size(0), 1);
  EXPECT_EQ(fc.is_cached(0, 1), true);
  EXPECT_EQ(fc.get(0, 1), 10);
  EXPECT_THROW(fc.get(0, 2), std::range_error);
}

TEST(TPLRUCache, ESL_Replace_Tests)
{
  esl_tplru_cache_t<int, int> fc(1, 2);
  fc.put(0, 1, 10);
  EXPECT_EQ(fc.get_cache_set_size(0), 1);
  EXPECT_EQ(fc.is_cached(0, 1), true);
  fc.put(0, 2, 20);
  EXPECT_EQ(fc.is_cached(0, 1), true);
  fc.put(0, 3, 30);
  EXPECT_EQ(fc.get_cache_set_size(0), 2);
  EXPECT_EQ(fc.is_cached(0, 1), false);
  // EXPECT_THROW(fc.get(0, 1), std::range_error);
}

 TEST(TPLRUCache, ESL_Complex_Test)
 {
   esl_tplru_cache_t<int, int> fc(1, 4);
   fc.put(0, 2, 20);
   EXPECT_EQ(fc.get_cache_set_size(0), 1);
   EXPECT_EQ(fc.is_cached(0, 2), true);
   fc.put(0, 2, 20);
   fc.put(0, 3, 30);
   fc.put(0, 6, 20);
   fc.put(0, 1, 20);
   fc.put(0, 5, 20);
   EXPECT_EQ(fc.is_cached(0, 2), false);
   fc.put(0, 2, 20);
   EXPECT_EQ(fc.is_cached(0, 3), true);
   EXPECT_EQ(fc.is_cached(0, 1), true);
   fc.put(0, 4, 20);
   EXPECT_EQ(fc.is_cached(0, 1), true);
   fc.put(0, 5, 20);
   fc.put(0, 3, 20);
   EXPECT_EQ(fc.is_cached(0, 5), true);
   fc.put(0, 6, 20);
   //   EXPECT_EQ(fc.get(0, 2), 20);
   fc.put(0, 5, 20);
   fc.put(0, 2, 20);
   fc.put(0, 6, 20);
   fc.put(0, 6, 20);
   EXPECT_EQ(fc.is_cached(0, 1), false);
   EXPECT_EQ(fc.is_cached(0, 2), true);
   EXPECT_EQ(fc.is_cached(0, 3), true);
   EXPECT_EQ(fc.is_cached(0, 4), false);
   EXPECT_EQ(fc.is_cached(0, 5), true);
   EXPECT_EQ(fc.is_cached(0, 6), true);
}