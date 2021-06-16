#ifndef TRIE_COMMON
#define TRIE_COMMON

#include <map>

#include <assert.hpp>

#include "trie_random.hpp"





template<typename Trie, typename T>
bool is_equal(Trie const &trie, std::map<std::string, T> const &vals)
{
	if (trie.size() != (int)vals.size())
		return false;

	T *val;
	for (auto b = vals.begin(), e = vals.end(); b != e; ++b)
	{
		val = trie.get(b->first.c_str());
		if (!val || !(*val == b->second))
			return false;
	}

	return true;
}

template<typename Trie, typename T>
bool _trie_random_test_core(
	bool ins = true,
	bool era = true,
	bool get = true
)
{
	std::map<std::string, T> vals;

	TrieOperation<T> op;
	Trie trie;
	T *pval;

	for (int i = 0; i < 10000; ++i)
	{
		op = trie_random_operation(vals.begin(), vals.end(), 1, 20, 0, 100);

		try
		{
			switch (op.type)
			{
			case '+':
			  {
				if (!ins)
					continue;

				trie.insert(op.key.c_str(), op.val);
				auto res = vals.insert({op.key, op.val});
				if (!res.second)
					res.first->second = op.val;
				break;
			  }
			case '-':
				if (!era)
					continue;

				nvx::assert_eq(
					trie.erase(op.key.c_str()),
					(bool)vals.erase(op.key),
					"trie.erase != vals.erase"
				);
				break;

			case '=':
				if (!get)
					continue;

			  {
				pval = trie.get(op.key.c_str());
				auto it = vals.find(op.key);

				if ((bool)pval != (it != vals.end()))
					throw "trie.isfind() != vals.isfind()";

				if (pval)
					nvx::assert_eq(*pval, it->second, "trie.find() != vals.find()");

				break;
			  }
			}

			/*
			 * std::cerr << op.type << " " << op.key << " " << op.val << std::endl;
			 * nvx::print(std::cerr << "map: ", vals.begin(), vals.end()) << '\n';
			 * std::cerr << trie << std::endl << std::endl;
			 */

			if (!is_equal(trie, vals))
			{
				fprintf(stderr, "after %c\n", op.type);
				throw "map != trie";
			}
		}
		catch(char const *err)
		{
			fprintf(stderr, "%s\n", err);
			nvx::print(std::cerr << "map: ", vals.begin(), vals.end()) << '\n';
			std::cerr << trie << std::endl;
			return false;
		}
	}

	return true;

}





#endif
