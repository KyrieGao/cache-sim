#include "cache.hpp"
#include "fifo_cache_policy.hpp"
#include <gtest/gtest.h>

template <typename Key, typename Value>
using esl_fifo_cache_t = typename esl::Cache<Key, Value, esl::FIFOCachePolicy<Key>>;

TEST(FIFOCache, ESL_Simple_Test)
{
  esl_fifo_cache_t<int, int> fc(1, 4);
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

TEST(FIFOCache, ESL_Missing_Value)
{
  esl_fifo_cache_t<int, int> fc(1, 2);

  fc.put(0, 1, 10);
  EXPECT_EQ(fc.get_cache_set_size(0), 1);
  EXPECT_EQ(fc.is_cached(0, 1), true);
  EXPECT_EQ(fc.get(0, 1), 10);
  EXPECT_THROW(fc.get(0, 2), std::range_error);
}