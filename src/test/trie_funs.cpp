#include "trie_common.hpp"

#include <trie.hpp>
#include <ctrie.hpp>





bool trie_random_test()
{
	return _trie_random_test_core<nvx::Trie<int, 26, 'a'>, int>();
}

bool ctrie_random_test()
{
	return _trie_random_test_core<nvx::CTrie<int>, int>(true, true, true);
}





// END
