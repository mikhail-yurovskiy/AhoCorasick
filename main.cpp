#include "AhoCorasick.h"
#include <iostream>

using namespace std;
int main()
{
	AhoCorasick<'a','z'> ac;
	ac.add_pattern( "abc"s );
	ac.add_pattern( "bcdc"s );
	ac.add_pattern( "cccb"s );
	ac.add_pattern( "bcdd"s );
	ac.add_pattern( "bbbc"s );
	ac.find_patterns( "abcdcbcddbbbcccbbbcccbb"s, []( auto const& pattern, auto pos ) { cout << pos + 1 << " " << pattern << endl; } );
    return 0;
}
