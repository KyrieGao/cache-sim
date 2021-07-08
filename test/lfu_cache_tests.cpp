#include "cache.hpp"
#include "lfu_cache_policy.hpp"
#include <gtest/gtest.h>

template <typename Key, typename Value>
using esl_lfu_cache_t = typename esl::Cache<Key, Value, esl::LFUCachePolicy<Key>>;


TEST(LFUCache, Simple_Test)
{
  constexpr size_t FIRST_FREQ = 10;
  constexpr size_t SECOND_FREQ = 9;
  constexpr size_t THIRD_FREQ = 8;
  esl_lfu_cache_t<std::string, int> cache(1, 3);

  cache.put(0, "A", 1);
  cache.put(0, "B", 2);
  cache.put(0, "C", 3);

  for (size_t i = 0; i < FIRST_FREQ; ++i)
  {
    EXPECT_EQ(cache.get(0, "B"), 2);
  }

  for (size_t i = 0; i < SECOND_FREQ; ++i)
  {
    EXPECT_EQ(cache.get(0, "C"), 3);
  }

  for (size_t i = 0; i < THIRD_FREQ; ++i)
  {
    EXPECT_EQ(cache.get(0, "A"), 1);
  }

  cache.put(0, "D", 4);

  EXPECT_EQ(cache.get(0, "B"), 2);
  EXPECT_EQ(cache.get(0, "C"), 3);
  EXPECT_EQ(cache.get(0, "D"), 4);
  EXPECT_THROW(cache.get(0, "A"), std::range_error);
}

TEST(LFUCache, Single_Slot)
{
  constexpr size_t TEST_SIZE = 5;
  esl_lfu_cache_t<int, int> cache(1, 1);

  cache.put(0, 1, 10);

  for (size_t i = 0; i < TEST_SIZE; ++i)
  {
    cache.put(0, 1, i);
  }

  EXPECT_EQ(cache.get(0, 1), 4);

  cache.put(0, 2, 20);

  EXPECT_THROW(cache.get(0, 1), std::range_error);
  EXPECT_EQ(cache.get(0, 2), 20);
}

TEST(LFUCache, FrequencyIssue)
{
  constexpr size_t TEST_SIZE = 50;
  esl_lfu_cache_t<int, int> cache(1, 3);

  cache.put(0, 1, 10);
  cache.put(0, 2, 1);
  cache.put(0, 3, 2);

  // cache value with key '1' will have the counter 50
  for (size_t i = 0; i < TEST_SIZE; ++i)
  {
    EXPECT_NO_THROW(cache.get(0, 1));
  }

  cache.put(0, 4, 3);
  cache.put(0, 5, 4);

  EXPECT_EQ(cache.get(0, 1), 10);
  EXPECT_EQ(cache.get(0, 2), 1);
  EXPECT_EQ(cache.get(0, 5), 4);
  EXPECT_THROW(cache.get(0, 3), std::range_error);
  EXPECT_THROW(cache.get(0, 4), std::range_error);

  cache.put(0, 6, 5);
  cache.put(0, 7, 6);

  EXPECT_EQ(cache.get(0, 1), 10);
  EXPECT_EQ(cache.get(0, 5), 4);
  EXPECT_EQ(cache.get(0, 7), 6);
  EXPECT_THROW(cache.get(0, 3), std::range_error);
  EXPECT_THROW(cache.get(0, 6), std::range_error);
}

TEST(LFUCache, is_success_remove_Test)
{
  constexpr std::size_t TEST_SIZE = 10;
  esl_lfu_cache_t<std::string, int> fc(1, TEST_SIZE);

  for (std::size_t i = 0; i < TEST_SIZE; ++i)
  {
    fc.put(0, std::to_string(i), i);
  }

  EXPECT_EQ(fc.get_cache_set_size(0), TEST_SIZE);

  for (std::size_t i = 0; i < TEST_SIZE; ++i)
  {
    EXPECT_TRUE(fc.is_success_remove(0, std::to_string(i)));
  }

  EXPECT_EQ(fc.get_cache_set_size(0), 0);

  for (std::size_t i = 0; i < TEST_SIZE; ++i)
  {
    EXPECT_FALSE(fc.is_success_remove(0, std::to_string(i)));
  }
}
