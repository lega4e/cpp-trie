#ifndef TEST_COMMON_HPP_75170
#define TEST_COMMON_HPP_75170

#include <map>

#include <trie.hpp>





template<typename T>
struct SAssert
{
	SAssert() {}
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
bool assert_structure(
	_TrieNode  const &trie,
	SAssert<T> const &s,
	char       const *err = "Error",
	bool             silent = false
)
{
	
}





#endif
