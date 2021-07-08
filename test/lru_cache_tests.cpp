#include "cache.hpp"
#include "lru_cache_policy.hpp"
#include <gtest/gtest.h>

template <typename Key, typename Value>
using esl_lru_cache_t = typename esl::Cache<Key, Value, esl::LRUCachePolicy<Key>>;

TEST(CacheTest, Simpleput)
{
  esl_lru_cache_t<std::string, int> cache(1, 1);

  cache.put(0, "test", 666);

  EXPECT_EQ(cache.get(0, "test"), 666);
}

TEST(CacheTest, MissingValue)
{
  esl_lru_cache_t<std::string, int> cache(1, 1);

  EXPECT_THROW(cache.get(0, "test"), std::range_error);
}

TEST(CacheTest, KeepsAllValuesWithinCapacity)
{
  constexpr int CACHE_CAP = 50;
  const int TEST_RECORDS = 100;
  esl_lru_cache_t<int, int> cache(1, CACHE_CAP);

  for (int i = 0; i < TEST_RECORDS; ++i)
  {
    cache.put(0, i, i);
  }

  for (int i = 0; i < TEST_RECORDS - CACHE_CAP; ++i)
  {
    EXPECT_THROW(cache.get(0, i), std::range_error);
  }

  for (int i = TEST_RECORDS - CACHE_CAP; i < TEST_RECORDS; ++i)
  {
    EXPECT_EQ(i, cache.get(0, i));
  }
}

TEST(LRUCache, is_success_remove_Test)
{
  constexpr std::size_t TEST_SIZE = 10;
  esl_lru_cache_t<std::string, int> fc(1, TEST_SIZE);

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
