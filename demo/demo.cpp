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


#include <iostream>
#include <algorithm>
#include <cstdlib>
#include <vector>
#include <chrono>
#include <unordered_map>

#include <gdc/nanomap.hpp>


typedef std::unordered_map<int, int> um;
typedef gdc::nanomap<int, int> nm;


void
run_um(um& needles, const std::vector<int>& haystack)
{
	for (auto hit = haystack.cbegin(); hit != haystack.cend(); ++hit)
	{
		auto nit = needles.find(*hit);
		
		if (nit != needles.end())
		{
			++(nit->second);
		}
	}
}


void
run_nm(nm& needles, const std::vector<int>& haystack)
{
	for (auto hit = haystack.cbegin(); hit != haystack.cend(); ++hit)
	{
		auto nit = needles.find(*hit);
		
		if (nit != needles.end())
		{
			++(nit.value());
		}
	}
}


int
main(int argc, char** argv)
{
	std::vector<int> input
	{
		1, 11, 111, 1111, 11111, 111111, 1111111, 11111111,
		2, 22, 222, 2222, 22222, 222222, 2222222, 22222222
	};
	std::vector<int> haystack(10 * 1000 * 1000);
	std::srand(9203854);
	std::generate(haystack.begin(), haystack.end(), []() { return std::rand(); });
	um um_needles;
	gdc::nanomap_factory<int, int, 16> factory;
	nm& nm_needles = factory.get();
	
	for (auto i : input)
	{
		um_needles[i] = i;
		nm_needles[i] = i;
	}
	
	// Warmup
	run_um(um_needles, haystack);
	run_um(um_needles, haystack);
	// Final run
	auto t0 = std::chrono::high_resolution_clock::now();
	run_um(um_needles, haystack);
	auto t1 = std::chrono::high_resolution_clock::now();
	auto duration_um = std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0);
	std::cout << "unordered_map duration: " << duration_um.count() << " ms" << std::endl;
	
	run_nm(nm_needles, haystack);
	run_nm(nm_needles, haystack);
	t0 = std::chrono::high_resolution_clock::now();
	run_nm(nm_needles, haystack);
	t1 = std::chrono::high_resolution_clock::now();
	auto duration_nm = std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0);
	std::cout << "gdc::nanomap duration: " << duration_nm.count() << " ms" << std::endl;
}
