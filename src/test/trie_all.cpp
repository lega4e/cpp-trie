#include <trie.hpp>

#include <assert.hpp>

#include "trie_random.hpp"


using namespace nvx;
using namespace std;





template<typename T, int N, int M>
bool is_equal(Trie<T, N, M> const &trie, std::map<string, T> const &vals)
{
	T *val;
	for (auto b = vals.begin(), e = vals.end(); b != e; ++b)
	{
		val = trie.get(b->first.c_str());
		if (!val || !(*val == b->second))
			return false;
	}

	return true;
}

bool trie_random_test()
{
	std::map<string, int> vals;

	TrieOperation<int> op;
	Trie<int, 26, 'a'> trie;
	for (int i = 0; i < 10000; ++i)
	{
		op = trie_random_operation(vals.begin(), vals.end(), 1, 10, 0, 100);

		try
		{
			switch (op.type)
			{
			case '+':
			  {
				trie.insert(op.key.c_str(), op.val);
				auto res = vals.insert({op.key, op.val});
				if (!res.second)
					res.first->second = op.val;
				break;
			  }
			case '-':
				assert_eq(
					trie.erase(op.key.c_str()),
					(bool)vals.erase(op.key),
					"trie.erase != vals.erase"
				);
				break;

			case '=':
			  {
				int *val = trie.get(op.key.c_str());
				auto it = vals.find(op.key);

				if ((bool)val != (it != vals.end()))
					throw "trie.isfind() != vals.isfind()";

				if (val)
					assert_eq(*val, it->second, "trie.find() != vals.find()");
				break;
			  }
			}

			/*
			 * cerr << op.type << " " << op.key << " " << op.val << endl;
			 * print(cerr << "map: ", vals.begin(), vals.end()) << '\n';
			 * cerr << trie << endl << endl;
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
			print(cerr << "map: ", vals.begin(), vals.end()) << '\n';
			cerr << trie << endl;
			return false;
		}
	}

	return true;
}





// END
