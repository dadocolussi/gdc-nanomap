**gdc-nanomap** is a CPU-cache-friendly unordered map implementation in C++
for small data sets. **gdc-nanomap** stores keys in one contiguous block of
memory, and values in another contiguous block of memory, without need for
dynamic memory allocation. A key lookup traverses the key array linearly.
Once a key is found, the index of the mapped value is known. Your CPU likes
the key array. See here: http://www.youtube.com/watch?v=WDIkqP4JbkE&t=44m40s

**gdc-nanomap** has been tested on Linux using GCC 5.3.1 and on
Mac OS X using clang 7.3.0.


```c++
// nanomap implementation is in a single header file.
#include <gdc/nanomap.hpp>

// We use nanomap_factory to create a nanomap with capacity for
// 32 key/value pairs. The map and the key and value arrays are
// members of the factory.  
gdc::nanomap_factory<int, int, 32> factory;

// Get a reference to the map in the factory. The factory
// automatically initializes the map to use the factory's
// key and value arrays. It's also possible to initialize
// a nanomap with custom key and value arrays.
// See gdc::nanomap::init().
gdc::nanomap<int, int>& map = factory.get();

// Put some values in the map.
map[1] = 2;
map[8] = 9;

for (auto it = map.cbegin(); it != map.cend(); ++it)
	std::cout << it.key() << "=" << it.value() << std::endl;

// The above program prints:
// 1=2
// 8=9
```

