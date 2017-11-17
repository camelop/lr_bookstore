#ifndef LR_LSTRING
#define LR_LSTRING
#include <cstring>
#include <string>
template<int cap>
class lstring {
public:
	char data[cap];
	lstring(char* x) {
		memset(data, 0, cap);
		memcpy(data, x, cap);
		bool care = false;
		for (int i = 0; i < cap; ++i) {
			if (data[i] == '\0') care = true;
			if (!care) continue;
			data[i] = '\0';
		}
	}
	lstring(const string& x) {
		memset(data, 0, cap);
		memcpy(data, x.c_str(), cap);
		bool care = false;
		for (int i = 0; i < cap; ++i) {
			if (data[i] == '\0') care = true;
			if (!care) continue;
			data[i] = '\0';
		}
	}
	lstring() {
		memset(data, 0, cap);
	}
	lstring& operator = (const lstring& rhs) {
		const char* x = rhs.data;
		memset(data, 0, cap);
		memcpy(data, x, cap);
		bool care = false;
		for (int i = 0; i < cap; ++i) {
			if (data[i] == '\0') care = true;
			if (!care) continue;
			data[i] = '\0';
		}
		return (*this);
	}

	bool operator ==(const lstring& rhs) const {
		return (strcmp(data, rhs.data) == 0);
	}
	bool operator !=(const lstring& rhs) const {
		return !(strcmp(data, rhs.data) == 0);
	}

	operator string() const{
		return data;
	}
};

template<int cap>
int lstring_hash(lstring<cap> x) {
	int ret = 0;
	for (int i = 0; i < cap; ++i) {
		ret += (unsigned int)x.data[i];
	}
	return ret;
}
#endif