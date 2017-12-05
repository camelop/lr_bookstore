#ifndef LR_STRINGTO
#define LR_STRINGTO
#include <string>
int stringToInt(const std::string& s) {
	int ret = 0;
	for (auto i : s) {
		ret = ret * 10 + i - '0';
	}
	return ret;
}
double stringToDouble(const std::string& s) {
	double ret = 0;
	bool after = false;
	double p = 0.1;
	for (auto i : s) {
		if (i == '.') {
			after = true;
			continue;
		}
		if (!after) {
			ret = ret * 10 + (double)(i - '0');
		}
		else {
			ret += p * (double)(i - '0');
			p /= 10;
		}
	}
	return ret;
}
string intToString(int x) {
	string ret = "";
	if (x <= 0) return "0";
	while (x > 0) {
		ret = (char)(x % 10 + '0') + ret;
		x /= 10;
	}
	return ret;
}
#endif