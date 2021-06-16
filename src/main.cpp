#include <iostream>

#include <nvx/random.hpp>

#include "test/all.hpp"


using namespace std;





// objs
NVX_DRE;





// main
int main( int argc, char *argv[] )
{
	auto tests = {
		make_pair(&insert_get_test,  "insert_get_test"),
		make_pair(&trie_random_test, "trie_random_test"),
	};

	int success = 0;
	int fail = 0;
	for (auto test : tests)
	{
		printf("========================================\n");
		printf("Testing %s...\n", test.second);
		fflush(stdout);

		if ((*test.first)())
		{
			++success;
			printf("Success\n");
		}
		else
		{
			++fail;
			printf("Fail\n");
		}

		printf("\n");
		fflush(stdout);
	}

	printf("Success: %i\n", success);
	printf("Fail:    %i\n", fail);
	printf("Total:   %i\n", success + fail);



	return 0;
}





// end
