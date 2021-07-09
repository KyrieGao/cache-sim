[![Build Status](https://travis-ci.com/vpetrigo/caches.svg?branch=master)](https://travis-ci.com/vpetrigo/caches)
[![Build status](https://ci.appveyor.com/api/projects/status/kawd812e48065r7a?svg=true)](https://ci.appveyor.com/project/vpetrigo/caches)

# C++ Cache implementation

This project implements a simple cache with several page replacement policies:

  * Least Recently Used
  * First-In/First-Out
  * Least Frequently Used
  * Not Recently Used
  * ......

# Usage

Using this library is simple. It is necessary to include header with the cache implementation (`cache.hpp` file) 
and appropriate header with the cache policy if it is needed. If not then the non-special algorithm will be used (it removes
the last element which key is the last in the internal container).

Currently there is only five of them:

  * `fifo_cache_policy.hpp`
  * `lfu_cache_policy.hpp`
  * `lru_cache_policy.hpp`
  * `nru_cache_policy.hpp`
  * `tplru_cache_policy.hpp`
  * `......`

Example for the LRU policy:

```cpp
#include <string>
#include "cache.hpp"
#include "lru_cache_policy.hpp"

template <typename Key, typename Value>
using lru_cache_t = typename esl::Cache<Key, Value, LRUCachePolicy<Key>>;

void foo(...) {
  constexpr std::size_t max_set_num = 256;
  constexpr std::size_t max_way_num = 256;
  lru_cache_t<std::string, int> cache(max_set_num, max_way_num);

  cache.put(0, "Hello", 1);
  cache.put(0, "world", 2);

  std::cout << cache.get(0, "Hello") << cache.get(0, "world") << std::endl;
  // "12"
}
```

# Requirements

The only requirement is a compatible C++11 compiler.

This project was tested in the environments listed below:

  * MinGW64 ([MSYS2 project](https://msys2.github.io/))
    * Clang 3.8.0
    * GCC 5.3.0
  * MSVC (VS 2015)
  * FreeBSD
    * Clang 3.4.1

If you have any issues with the library building, let me know please.

# Contributing

Please fork this repository and contribute back using [pull requests](https://github.com/vpetrigo/caches/pulls). 
Features can be requested using [issues](https://github.com/vpetrigo/caches/issues). All code, comments, and 
critiques are greatly appreciated.
