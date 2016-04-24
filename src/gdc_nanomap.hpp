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


#ifndef __gdc__nanomap__
#define __gdc__nanomap__


#include <type_traits>
#include <stdexcept>
#include <functional>
#include <utility>
#include <limits>
#include <tuple>
#include <cstdlib>
#include <cstddef>


namespace gdc
{
	
	
	template<typename key_type, typename mapped_type, typename key_equal> class nanomap;
	
	
	template<bool is_const, typename key_type, typename mapped_type, typename key_equal>
	class nanomap_iterator
	{
	public:
		
		typedef nanomap<key_type, mapped_type, key_equal> map_type;
		typedef typename map_type::reference reference;
		typedef typename map_type::const_reference const_reference;
		typedef typename map_type::size_type size_type;
		typedef typename std::pair<const key_type&, reference> value_type;
		typedef typename std::conditional<is_const, const_reference, reference>::type ref;
		typedef nanomap_iterator<false, key_type, mapped_type, key_equal> iterator;
		typedef nanomap_iterator<true, key_type, mapped_type, key_equal> const_iterator;
		friend map_type;
		friend const_iterator;
		friend iterator;
		
		
		nanomap_iterator(map_type* map, size_type index) : _map(map), _index(index)
		{
		}
		
		
		nanomap_iterator(const map_type* map, size_type index) : _map(const_cast<map_type*>(map)), _index(index)
		{
		}
		
		
		// Note: this allows const_iterator(const iterator&).
		template<typename IT = nanomap_iterator>
		nanomap_iterator(const IT& it) : _map(const_cast<map_type*>(it._map)), _index(it._index)
		{
			static_assert(
				std::is_same<IT, nanomap_iterator>::value ||
				std::is_same<IT, iterator>::value, "Invalid iterator type");
		}
		
		
		// Note: this allows const_iterator(const iterator&&).
		template<typename IT = nanomap_iterator>
		nanomap_iterator(IT&& it) : _map(it._map), _index(it._index)
		{
			if (it._map != nullptr)
			{
				it._index = it._map->max_size();
			}
			
			it._map = nullptr;
		}
		
		
		nanomap_iterator() : nanomap_iterator(nullptr, 0)
		{
		}
		
		
		~nanomap_iterator() = default;
		
		
		template<typename IT = nanomap_iterator>
		nanomap_iterator& operator=(const nanomap_iterator& it)
		{
			static_assert(
				std::is_same<IT, nanomap_iterator>::value ||
				std::is_same<IT, iterator>::value, "Invalid type");
			_map = it._map;
			_index = it._index;
			return *this;
		}
		
		
		nanomap_iterator& operator=(nanomap_iterator&& it)
		{
			_map = it._map;
			_index = it._index;
			
			if (it._map != nullptr)
			{
				it._index = it._map->max_size();
				it._map = nullptr;
			}
			
			return *this;
		}
		
		
		nanomap_iterator& operator++()
		{
			if (++_index >= _map->size())
			{
				_index = _map->max_size();
			}
			
			return *this;
		}
		
		
		nanomap_iterator& operator++(int)
		{
			nanomap_iterator tmp(*this);
			operator++();
			return tmp;
		}
		
		
		bool operator==(const iterator& rhs) const
		{
			return _map != nullptr && _map == rhs._map && _index == rhs._index;
		}
		
		
		bool operator==(const const_iterator& rhs) const
		{
			return _map != nullptr && _map == rhs._map && _index == rhs._index;
		}
		
		
		bool operator!=(const iterator& rhs) const
		{
			return _map == nullptr || rhs._map == nullptr || _map != rhs._map || _index != rhs._index;
		}
		
		
		bool operator!=(const const_iterator& rhs) const
		{
			return _map == nullptr || rhs._map == nullptr || _map != rhs._map || _index != rhs._index;
		}
		
		
		value_type operator*()
		{
			return value_type(_map->_keys[_index], _map->_values[_index]);
		}
		
		
		const key_type& key() const
		{
			return _map->_keys[_index];
		}
		
		
		ref& value()
		{
			return _map->_values[_index];
		}
		
		
	private:
		
		map_type* _map;
		size_type _index;
		
	};
	
	
	template<typename key_type, typename mapped_type, typename key_equal = std::equal_to<key_type>>
	class nanomap
	{
	public:
		
		typedef std::uint16_t size_type;
		typedef mapped_type& reference;
		typedef const mapped_type& const_reference;
		typedef std::pair<const key_type, mapped_type> value_type;
		typedef nanomap_iterator<false, key_type, mapped_type, key_equal> iterator;
		typedef nanomap_iterator<true, key_type, mapped_type, key_equal> const_iterator;
		friend iterator;
		friend const_iterator;
		
		
		nanomap() : _capacity(0), _size(0), _keys(nullptr), _values(nullptr)
		{
		}
		
		
		nanomap(key_type* keys, mapped_type* values, size_type capacity, size_type size = 0) :
			_capacity(capacity), _size(size), _keys(keys), _values(values)
		{
		}
		
		
		nanomap(const nanomap&) = delete;
		nanomap(nanomap&&) = delete;
		
		
		~nanomap()
		{
			clear();
		}
		
		
		void init(key_type* keys, mapped_type* values, size_type capacity, size_type size = 0)
		{
			_capacity = capacity;
			_size = size;
			_keys = keys;
			_values = values;
		}
		
		
		bool empty() const
		{
			return _size == 0;
		}
		
		
		size_type size() const
		{
			return _size;
		}
		
		
		size_type max_size() const
		{
			return _capacity;
		}
		
		
		void clear()
		{
			for (size_type i = 0; i < _size; ++i)
			{
				_keys[i].~key_type();
			}
			
			for (size_type i = 0; i < _size; ++i)
			{
				_values[i].~mapped_type();
			}
			
			_size = 0;
		}
		
		
		std::pair<iterator, bool> insert(const value_type& item)
		{
			if (_size >= max_size())
			{
				return std::pair<iterator, bool>(end(), false);
			}
			
			new (&_keys[_size]) key_type(item.first);
			new (&_values[_size]) mapped_type(item.second);
			return std::pair<iterator, bool>(iterator(this, _size++), true);
		}
		
		
		template<typename ... Args>
		std::pair<iterator, bool> emplace(const key_type& key, Args&& ... value_args)
		{
			if (_size >= max_size())
			{
				return std::pair<iterator, bool>(end(), false);
			}
			
			new (&_keys[_size]) key_type(key);
			new (&_values[_size]) mapped_type(value_args ...);
			return std::pair<iterator, bool>(iterator(this, _size++), true);
		}
		
		
		iterator erase(const_iterator it)
		{
			if (it._index == max_size())
			{
				std::abort();
			}
			
			_keys[it._index].~key_type();
			_values[it._index].~mapped_type();

			if (it._index < --_size)
			{
				new (&_keys[it._index]) key_type(std::move(_keys[_size]));
				new (&_values[it._index]) mapped_type(std::move(_values[_size]));
				_keys[_size].~key_type();
				_values[_size].~mapped_type();
			}
			else
			{
				it._index = end()._index;
			}

			return iterator(it._map, it._index);
		}
		
		
		mapped_type& operator[](const key_type& key)
		{
			iterator it = find(key);
			
			if (it == end())
			{
				if (_size >= _capacity)
				{
					throw std::overflow_error("nanomap is full");
				}
				
				size_type i = _size++;
				new (&_keys[i]) key_type(key);
				new (&_values[i]) mapped_type();
				return _values[i];
			}
			
			return _values[it._index];
		}
		
		
		mapped_type& operator[](key_type&& key)
		{
			iterator it = find(key);
			
			if (it == end())
			{
				if (_size >= _capacity)
				{
					throw std::overflow_error("nanomap is full");
				}
				
				size_type i = _size++;
				new (&_keys[i]) key_type(std::move(key));
				new (&_values[i]) mapped_type();
				return _values[i];
			}
			
			return _values[it._index];
		}
		
		
		mapped_type& at(const key_type& key)
		{
			const_iterator it(_find(key));
			
			if (it == end())
			{
				throw std::out_of_range("item not found");
			}
			
			return _values[it._index];
		}

		
		const mapped_type& at(const key_type& key) const
		{
			mapped_type& item = at(key);
			return item;
		}

		
		size_type count() const
		{
			return _size;
		}
		
		
		const_iterator find(const key_type& key) const
		{
			return _find(key);
		}
		
		
		iterator find(const key_type& key)
		{
			const_iterator it(_find(key));
			return iterator(it._map, it._index);
		}
		
		
		const_iterator begin() const
		{
			return empty() ? end() : const_iterator(this, 0);
		}
		
		
		const_iterator end() const
		{
			return const_iterator(this, max_size());
		}
		
		
		iterator begin()
		{
			return empty() ? end() : iterator(this, 0);
		}
		
		
		iterator end()
		{
			return iterator(this, max_size());
		}
		
		
		const_iterator cbegin() const
		{
			return empty() ? end() : const_iterator(this, 0);
		}
		
		
		const_iterator cend() const
		{
			return const_iterator(this, max_size());
		}
		
		
	private:
		
		std::uint32_t _capacity:16;
		std::uint32_t _size:16;
		key_type* _keys;
		mapped_type* _values;
		
		
		const_iterator _find(const key_type& key) const
		{
			key_equal eq;
			
			for (size_type i = 0; i < size(); ++i)
			{
				if (eq(_keys[i], key))
				{
					return const_iterator(this, i);
				}
			}
			
			return cend();
		}
		
		
	};
	
	
	template<typename key_type, typename mapped_type, std::uint16_t _capacity, typename key_equal = std::equal_to<key_type>>
	class nanomap_factory
	{
	public:
		
		typedef nanomap<key_type, mapped_type, key_equal> map_type;
		typedef typename map_type::size_type size_type;
		
		
		nanomap_factory() : _map(_keys, _values, _capacity)
		{
		}
		
		
		nanomap_factory(const nanomap_factory& f) : _map(_keys, _values, _capacity, f._map.size())
		{
			for (size_type i = 0; i < _capacity; ++i)
			{
				new (&_keys[i]) key_type(f._keys[i]);
			}
			
			for (size_type i = 0; i < _capacity; ++i)
			{
				new (&_values[i]) mapped_type(f._values[i]);
			}
		}
		
		
		nanomap_factory(nanomap_factory&& f) : _map(_keys, _values, _capacity, f._map.size())
		{
			for (size_type i = 0; i < _capacity; ++i)
			{
				new (&_keys[i]) key_type(std::move(f._keys[i]));
			}
			
			for (size_type i = 0; i < _capacity; ++i)
			{
				new (&_values[i]) mapped_type(std::move(f._values[i]));
			}
			
			f._map.clear();
		}
		
		
		constexpr size_type capacity() const
		{
			return _capacity;
		}
		
		
		map_type& get()
		{
			return _map;
		}
		
		
	private:
		
		map_type _map;
		key_type _keys[_capacity];
		mapped_type _values[_capacity];
		
		
	};
	
	
}


#endif
