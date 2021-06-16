#include <assert.hpp>
#include <trie.hpp>

#include "assert_trie_strcture.hpp"

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

		assert_trie_structure(
			*_TriePrivateAccess::root(trie),
			SAssert<int> { nullptr, {
				{ 't', new SAssert<int> { nullptr, {
					{ 'r', new SAssert<int> { new int(6), {
						{ 'a', new SAssert<int> { new int(1), { {
							{ 's', new SAssert<int> { new int(3) } }
						} } } }
					} } },
				} } },
				{ 'x', new SAssert<int> { nullptr, {
					{ 'a', new SAssert<int> { nullptr, {
						{ 's', new SAssert<int> { new int(4) } }
					} } },
					{ 't', new SAssert<int> { nullptr, {
						{ 's', new SAssert<int> { new int(5) } }
					} } }
				} } }
			} }
		);
	}
	catch(char const *err)
	{
		fprintf(stderr, "%s\n", err);
		return false;
	}

	return true;
}





// END
