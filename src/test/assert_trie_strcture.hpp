#ifndef TEST_COMMON_HPP_75170
#define TEST_COMMON_HPP_75170

#include <iostream>
#include <map>

#include <trie.hpp>

#pragma GCC diagnostic ignored "-Wparentheses"





template<typename T>
struct SAssert
{
	~SAssert()
	{
		if (value)
			delete value;

		for (auto ch : chs)
		{
			delete ch.second;
		}
	}

	T *value = nullptr;
	std::map<char, SAssert *> chs;
};

template<typename T, int N, int M>
bool assert_trie_structure(
	nvx::_TrieNode<T, N, M> const &trie,
	SAssert<T>              const &s,
	char                    const *err   = "Error",
	bool                          silent = false,
	char                          key    = '\0',
	nvx::_TrieNode<T, N, M> const *root  = nullptr
)
{
	auto print_trie = [&]()->void {
		std::cerr << *root << "\n\n";
		if (key)
			trie.print(std::cerr, key) << "\n\n" << std::flush;
	};

	if (!root)
		root = &trie;

	if ( (bool)s.value != trie.store ||
	     s.value && !(*s.value == trie.value) )
	{
		if (silent)
			return false;

		print_trie();
		if ((bool)s.value != trie.store && s.value)
			std::cerr << "Value absent, but expected: " << *s.value << std::endl;
		else if ((bool)s.value != trie.store && trie.store)
			std::cerr << "Value is present: " << trie.value <<
			        ", but expected null" << std::endl;
		else
			std::cerr << "Trie value doesn't match: " << trie.value <<
					", but expected: " << *s.value << std::endl;
		throw err;
	}

	for (int i = 0; i < N; ++i)
	{
		if (trie.chs[i] && s.chs.find(char(i+M)) == s.chs.end())
		{
			if (silent)
				return false;

			print_trie();
			fprintf(stderr, "Trie have %c key not null\n", char(i+M));
			throw err;
		}
	}

	for (auto b = s.chs.begin(), e = s.chs.end(); b != e; ++b)
	{
		if (!trie.chs[b->first-M])
		{
			if (silent)
				return false;

			print_trie();
			fprintf(stderr, "Trie hav't value for key %c\n", b->first);
			throw err;
		}
	}

	for (auto b = s.chs.begin(), e = s.chs.end(); b != e; ++b)
	{
		if (!assert_trie_structure(
			*trie.chs[b->first-M],
			*b->second, err,
			silent, b->first, root
		))
			return false;
	}

	return true;
}





#endif
