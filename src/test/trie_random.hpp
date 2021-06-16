#ifndef TEST_TRIE_RANDOM_47890175
#define TEST_TRIE_RANDOM_47890175

#include <map>

#include <random_value.hpp>

#include <nvx/random.hpp>





template<typename T>
struct TrieOperation
{
	/*
	 * type:
	 * '+' insert
	 * '-' erase
	 * '=' get
	 */

	char type;

	std::string key;
	T val;
};

inline std::string random_key(int minlen = 1, int maxlen = 3)
{
	int const len = nvx::rnd(minlen, maxlen);
	std::string key;
	key.resize(len);

	for (int i = 0; i < len; ++i)
		key[i] = nvx::rnd('a', 'z');

	return key;
}

/*
 * Iterator of map (or pairs)
 */
template<typename Iterator, typename...Args>
auto trie_random_operation(
	Iterator b,
	Iterator e,
	int keylenmin = 1,
	int keylenmax = 5,
	Args...args
)
{
	typedef decltype(Iterator()->second) value_t;

	using namespace nvx;

	disD dis;

	TrieOperation<value_t> op;
	if (dis(dre) < 0.33 || b == e)
	{
		op.type = '+'; // insert
		op.val = nvx::random_value<value_t>(args...);
		op.key = random_key(keylenmin, keylenmax);
	}
	else if (dis(dre) < 0.5)
	{
		op.type = '='; // get 
		op.key = dis(dre) < 0.8 ?
			choice_it(b, e)->first :
			random_key(keylenmin, keylenmax);
	}
	else
	{
		op.type = '-'; // erase
		op.key = dis(dre) < 0.8 ?
			choice_it(b, e)->first :
			random_key(keylenmin, keylenmax);
	}

	return op;
}





#endif
