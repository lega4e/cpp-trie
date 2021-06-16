#include <trie.hpp>

#include <assert.hpp>

using namespace nvx;







bool insert_get_test()
{
	Trie<int, 26, 'a'> trie;

	trie.insert("tra",  1);
	trie.insert("tr",   2);
	trie.insert("tras", 3);
	trie.insert("xas",  4);
	trie.insert("xts",  5);
	trie.insert("tr",   6);

	try
	{
		assert_eq(trie.size(), 5, "Size != 5");
		assert_eq(*trie.get("tra"),  1);
		assert_eq(*trie.get("tr"),   6);
		assert_eq(*trie.get("tras"), 3);
		assert_eq(*trie.get("xas"),  4);
		assert_eq(*trie.get("xts"),  5);
	}
	catch(char const *err)
	{
		fprintf(stderr, "%s\n", err);
		return false;
	}

	return true;
}





// END
