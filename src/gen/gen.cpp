#include <algorithm>
#include <iostream>
#include <vector>

#include <nvx/random.hpp>


using namespace nvx;
using namespace std;





// objects
NVX_DRE;





// main
int main( int argc, char *argv[] )
{
	vector<string> words;
	string word;
	while(cin >> word)
		words.push_back(word);
	shuffle( words.begin(), words.end(), dre );

	double prob = 0.8;
	disD one(0.0, 1.0);
	for(auto b = words.begin(), e = words.end(); b != e; ++b)
	{
		cout << "+ " << *b << " " << disI(0, 1000)(dre) << '\n';
		cout << "? " << words[ disI(0, words.size()-1)(dre) ] << '\n';
		if(one(dre) < prob)
		{
			cout << (one(dre) > 0.3 ? "? " : "- ");
			cout << words[ disI(0, words.size()-1)(dre) ] << '\n';
		}
	}


	return 0;
}





// end
