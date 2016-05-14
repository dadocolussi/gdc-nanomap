//
// Copyright (c) 2016 Dado Colussi
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//


// This tells Catch to provide a main() - only do this in one cpp file
#define CATCH_CONFIG_MAIN


#include <gdc/nanomap.hpp>

#include "catch.hpp"


class myvalue
{
public:
	
	
	myvalue(int value = 0) : _value(value)
	{
	}
	
	
	myvalue(const myvalue& v) = default;
	myvalue(myvalue&& v) = default;
	
	
	~myvalue()
	{
		_value = 42;
	}
	
	
	int value() const
	{
		return _value;
	}
	
	
	myvalue& operator=(const myvalue& v)
	{
		_value = v._value;
		return *this;
	}
	
	
	myvalue& operator=(myvalue&& v)
	{
		_value = v._value;
		return *this;
	}
	
	
	operator int() const
	{
		return _value;
	}
	
	
	friend bool operator==(const myvalue& lhs, const myvalue& rhs);
	friend bool operator==(const myvalue& lhs, const int rhs);
	friend bool operator==(const int rhs, const myvalue& lhs);
	
	
private:
	
	
	// Marked volatile to prevent compilers from optimizing away the destructor.
	volatile int _value;
	
	
};


bool operator==(const myvalue& lhs, const myvalue& rhs)
{
	return lhs._value == rhs._value;
}


bool operator==(const myvalue& lhs, const int rhs)
{
	return lhs._value == rhs;
}


bool operator==(const int lhs, const myvalue& rhs)
{
	return lhs == rhs._value;
}


SCENARIO("nanomap", "[nanomap]")
{
	typedef int key_type;
	typedef myvalue value_type;
	constexpr std::uint16_t capacity = 32;
	typedef gdc::nanomap_factory<key_type, value_type, capacity> factory_type;
	typedef typename factory_type::map_type map_type;
	
	
	GIVEN("an empty nanomap")
	{
		factory_type factory;
		auto& that = factory.get();
		
		THEN("empty() returns true")
		{
			CHECK(that.empty());
		}
		
		THEN("size() returns 0")
		{
			CHECK(that.size() == 0);
		}
		
		THEN("max_size() returns capacity")
		{
			CHECK(that.max_size() == capacity);
		}
		
		THEN("iterators are equal")
		{
			CHECK(that.begin() == that.begin());
			CHECK(that.end() == that.end());
			CHECK(that.begin() == that.cbegin());
			CHECK(that.end() == that.cend());
			CHECK(that.begin() == that.end());
			CHECK(that.begin() == that.cend());
			CHECK(that.cbegin() == that.end());
			CHECK(that.cbegin() == that.cend());
			CHECK_FALSE(that.begin() != that.begin());
			CHECK_FALSE(that.end() != that.end());
			CHECK_FALSE(that.begin() != that.cbegin());
			CHECK_FALSE(that.end() != that.cend());
			CHECK_FALSE(that.begin() != that.end());
			CHECK_FALSE(that.begin() != that.cend());
			CHECK_FALSE(that.cbegin() != that.end());
			CHECK_FALSE(that.cbegin() != that.cend());
		}
		
		WHEN("inserting 2 items as rvalues and lvalues")
		{
			int k = 1;
			int v = 2;
			auto i1 = that.insert(map_type::value_type(k, v)); // lvalues
			auto i2 = that.insert(map_type::value_type(2, 3)); // rvalues
			
			THEN("first insert is successful")
			{
				CHECK(i1.first == that.begin());
				CHECK(i1.second);
			}
			
			THEN("first insert is successful")
			{
				CHECK(i1.second);
			}
			
			THEN("first second is successful")
			{
				CHECK(i2.second);
			}
			
			THEN("empty() returns false")
			{
				CHECK_FALSE(that.empty());
			}

			THEN("size() returns 2")
			{
				CHECK(that.size() == 2);
			}
			
			THEN("max_size() returns capacity")
			{
				CHECK(that.max_size() == capacity);
			}

			THEN("find() for the rvalue item returns an iterator for the item")
			{
				auto it = that.find(k);
				CHECK((*it).first == k);
				CHECK((*it).second == v);
			}
			
			THEN("find() for the lvalue item returns an iterator for the item")
			{
				auto it = that.find(2);
				CHECK((*it).first == 2);
				CHECK((*it).second == 3);
			}
			
			THEN("at() for the rvalue item returns the item")
			{
				CHECK(that.at(k) == v);
			}
			
			THEN("at() for the lvalue item returns the item")
			{
				CHECK(that.at(2) == 3);
			}
			
			THEN("operator[] with key reference for rvalue item returns the item")
			{
				CHECK(that[std::ref(k)] == v);
			}
			
			THEN("operator[] with moved key for rvalue item returns the item")
			{
				CHECK(that[std::move(k)] == v);
			}
			
			THEN("operator[] for lvalue item returns the item")
			{
				CHECK(that[2] == 3);
			}
			
			THEN("find() some other key as rvalue returns end()")
			{
				int missing = 3;
				CHECK(that.find(missing) == that.end());
			}
			
			THEN("find() some other key as lvalue returns end()")
			{
				CHECK(that.find(3) == that.end());
			}
			
			THEN("at() some other key as rvalue throws std::out_of_range")
			{
				int missing = 3;
				CHECK_THROWS_AS(that.at(missing), std::out_of_range);
			}
			
			THEN("at() some other key as lvalue throws std::out_of_range")
			{
				CHECK_THROWS_AS(that.at(3), std::out_of_range);
			}
			
			THEN("begin() == end() is false")
			{
				CHECK_FALSE(that.begin() == that.end());
			}
			
			THEN("begin() != end() is true")
			{
				CHECK(that.begin() != that.end());
			}
		}
		
		WHEN("emplace() a value")
		{
			std::pair<map_type::iterator, bool> pos = that.emplace(1, 2);
			
			THEN("emplace is successful")
			{
				CHECK(pos.second);
			}
			
			THEN("the returned iterator points to the emplaced item")
			{
				auto it = pos.first;
				CHECK((*it).first == 1);
				CHECK((*it).second == 2);
			}
		}
	}
	
	
	GIVEN("a nanomap with 1 item")
	{
		factory_type factory;
		auto& that = factory.get();
		that.insert(map_type::value_type(1, 2));
	
		WHEN("invoking clear()")
		{
			value_type& v = that[1];
			that.clear();
			
			THEN("empty() returns true")
			{
				CHECK(that.empty());
			}
			
			THEN("size() returns 0")
			{
				CHECK(that.size() == 0);
			}
			
			THEN("max_size() returns capacity")
			{
				CHECK(that.max_size() == capacity);
			}
			
			THEN("destructor is called for the deleted value")
			{
				CHECK(v == 42);
			}
		}
		
		WHEN("erasing the only item")
		{
			CHECK(that.erase(that.begin()) == that.end());
			
			THEN("empty() returns true")
			{
				CHECK(that.empty());
			}
		}
		
		WHEN("iterating all items")
		{
			int n = 0;
			
			for (auto it = that.begin(); it != that.end(); ++it)
			{
				++n;
			}
			
			THEN("only the single item is returned")
			{
				CHECK(n == 1);
			}
		}
	}
	
	
	GIVEN("a nanomap that has been initialized with init()")
	{
		map_type that;
		key_type keys[capacity];
		value_type values[capacity];
		that.init(keys, values, capacity);
		
		WHEN("putting an item in the map")
		{
			that.insert(map_type::value_type(1, 2));
			
			THEN("empty() returns false")
			{
				CHECK_FALSE(that.empty());
			}
			
			THEN("size() returns 1")
			{
				CHECK(that.size() == 1);
			}
			
			THEN("max_size() returns capacity")
			{
				CHECK(that.max_size() == capacity);
			}
			
			THEN("the item is in the collection")
			{
				CHECK(that[1] == 2);
			}
			
			THEN("clear() invokes destructor on the value in the map")
			{
				that.clear();
				CHECK(values[0] == 42);
			}
		}
	}
	
	
	GIVEN("a full nanomap")
	{
		factory_type factory;
		auto& that = factory.get();

		for (int i = 0; i < that.max_size(); ++i)
		{
			that.insert(map_type::value_type(i, i));
		}
	
		THEN("insert returns pair with false")
		{
			CHECK_FALSE(that.insert(map_type::value_type(key_type(that.max_size()), value_type(that.max_size()))).second);
		}
		
		THEN("insert using operator throws std::overflow_error")
		{
			value_type v(42);
			CHECK_THROWS_AS(that[capacity] = std::ref(v), std::overflow_error);
		}
		
		THEN("insert using operator with move semantics throws std::overflow_error")
		{
			value_type v(42);
			CHECK_THROWS_AS(that[capacity] = std::move(v), std::overflow_error);
		}
		
		WHEN("iterating from begin() to end()")
		{
			int sum = 0;
			
			for (auto it = that.begin(); it != that.end(); ++it)
			{
				sum += it.value();
			}
			
			THEN("all items are visited")
			{
				int n = that.max_size();
				int nn = n * (n - 1);
				int nnn = nn / 2;
				CHECK(sum == nnn);
			}
		}
		
		WHEN("iterating with for(:) syntax")
		{
			int sum = 0;
			
			for (auto i : that)
			{
				sum += i.second.value();
			}
			
			THEN("all items are visited")
			{
				int n = that.max_size();
				int nn = n * (n - 1);
				int nnn = nn / 2;
				CHECK(sum == nnn);
			}
		}
		
		WHEN("erasing the first item")
		{
			auto it = that.erase(that.begin());
			
			THEN("empty() return false")
			{
				CHECK_FALSE(that.empty());
			}
			
			THEN("size() return capacity - 1")
			{
				CHECK(that.size() == capacity - 1);
			}
			
			THEN("returned iterator now has the last inserted value")
			{
				CHECK((*it).second == capacity - 1);
			}
		}
		
		WHEN("erasing all items one by one")
		{
			auto it = that.begin();
			
			for (int i = that.size(); i > 1; --i)
			{
				it = that.erase(it);
			}
			
			THEN("erase() on the last item returns end()")
			{
				CHECK(that.erase(it) == that.end());
			}
		}
	}
	
	
	GIVEN("a nanomap with 2 items")
	{
		factory_type factory;
		factory.get()[1] = 2;
		factory.get()[3] = 4;
		
		WHEN("iterating all items")
		{
			auto it = factory.get().cbegin();
			
			THEN("only the two items are returned")
			{
				CHECK(it.key() == 1);
				CHECK(it.value() == 2);
				++it;
				CHECK(it.key() == 3);
				CHECK(it.value() == 4);
			}
		}
		
		WHEN("taking a copy of the factory")
		{
			factory_type copy(factory);
			
			THEN("the original map retains its data and properties")
			{
				CHECK(factory.get().max_size() == capacity);
				CHECK(factory.get().size() == 2);
				CHECK(factory.get()[1] == 2);
				CHECK(factory.get()[3] == 4);
			}
			
			THEN("the copy is identical to the original")
			{
				CHECK(copy.get().max_size() == capacity);
				CHECK(copy.get().size() == 2);
				CHECK(copy.get()[1] == 2);
				CHECK(copy.get()[3] == 4);
			}
		}
		
		WHEN("moving the factory")
		{
			factory_type copy(std::move(factory));
			
			THEN("the original map loses its data and properties")
			{
				CHECK(factory.get().max_size() == capacity);
				CHECK(factory.get().empty());
			}
			
			THEN("the copy has the original")
			{
				CHECK(copy.get().max_size() == capacity);
				CHECK(copy.get().size() == 2);
				CHECK(copy.get()[1] == 2);
				CHECK(copy.get()[3] == 4);
			}
		}
	}
	
}
