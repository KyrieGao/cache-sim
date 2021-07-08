#include "cache.hpp"
#include "cache_policy.hpp"

#include <gtest/gtest.h>
#include <stdexcept>

TEST(NoPolicyCache, Add_one_element)
{
  esl::Cache<std::string, int> cache(1, 4);
  cache.put(0, "Hello", 1);
  ASSERT_EQ(cache.get(0, "Hello"), 1);
}

TEST(NoPolicyCache, Add_delete_add_one_element)
{
  esl::Cache<std::string, int> cache(1, 1);
  cache.put(0, "Hello", 1);
  cache.put(0, "World", 2);
  ASSERT_THROW(cache.get(0, "Hello"), std::range_error);
  ASSERT_EQ(cache.get(0, "World"), 2);
}

TEST(NoPolicyCache, Add_many_elements)
{
  constexpr std::size_t cache_size = 1024;
  esl::Cache<std::string, int> cache(1, cache_size);
  for (int i = 0; i < cache_size; ++i)
  {
    cache.put(0, std::to_string(i), i);
  }
  ASSERT_EQ(cache.get_cache_set_size(0), cache_size);

  for (std::size_t i = 0; i < cache_size; ++i)
  {
    ASSERT_EQ(cache.get(0, std::to_string(i)), i);
  }
}

TEST(NoPolicyCache, Small_cache_many_elements)
{
  constexpr std::size_t cache_size = 1;
  esl::Cache<std::string, int> cache(1, cache_size);

  for (std::size_t i = 0; i < cache_size; ++i)
  {
    std::string temp_key = std::to_string(i);
    cache.put(0, temp_key, i);
    ASSERT_EQ(cache.get(0, temp_key), i);
  }

  ASSERT_EQ(cache.get_cache_set_size(0), cache_size);
}

TEST(NoPolicyCache, Remove_Test)
{
  constexpr std::size_t TEST_SIZE = 10;
  esl::Cache<std::string, int> fc(1, TEST_SIZE);

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
