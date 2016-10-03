#include <string>
#include <algorithm>

using namespace std;

bool imatch(const char* s1, const char* s2) {
	string str1(s1);
	string str2(s2);
	
	transform(str1.begin(), str1.end(), str1.begin(), ::tolower);
	transform(str2.begin(), str2.end(), str2.begin(), ::tolower);
	
	return str1 == str2;
}
