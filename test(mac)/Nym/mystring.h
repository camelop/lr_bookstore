/*
	Author : spectrometer
*/ 
#ifndef STRING_H
#define STRING_H

#include <iostream>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <algorithm>
#include <iomanip>
#include <string>

using namespace std;

inline void ProcessWhiteSpace(string &str){
	int i = str.size() - 1;
	for (; i >= 0 && (str[i] == ' ' || str[i] == '\t') ; i--);
	int j = 0;
	for (; j < str.size() && (str[j] == ' ' || str[j] == '\t'); j++);
	str = str.substr(j, i - j + 1);
}
inline bool equal(const string &str1, const string &str2){
	return str1.compare(str2) == 0;
}
string Strtok(string str, const string &delimiter, string &rest){
    if (str.empty()){
        rest.clear();
        return str;
    }else{
        int pos1 = 0, pos2 = 0;
        for (; pos1 < str.size() && delimiter.find(str[pos1]) != -1; pos1++);
        string token = str.substr(pos1);
        if (token.empty()){
            rest.clear();
            return token;
        }else{
            pos2 = token.find_first_of(delimiter);
            if (pos2 == -1){
                rest.clear();
                return token;
            }
            string result = token.substr(0, pos2);
            for (; pos2 < token.size() && delimiter.find(token[pos2]) != -1; pos2++);
            rest = token.substr(pos2);
            return result;
        }
    }
}
inline bool IsLegalUserID(string st){
	if (st.empty()) return false;
	if (st.size() > 30) return false;
	for (int i = 0; i < st.size(); i++)
		if (!isdigit(st[i]) && !islower(st[i]) && !isupper(st[i]) && st[i] != '_') return false;
	return true;
}
inline bool IsLegalUserPasswd(string st){
	return IsLegalUserID(st);
}
inline bool IsNonNegativeInteger(string st){
	if (st.empty()) return true;
	for (int i = 0; i < st.size(); i++)
		if (!isdigit(st[i])) return false;
	return true;
}
pair<bool, double> CheckIfLegalDouble(string st){
	if (st.size() == 1 && !isdigit(st[0])) return make_pair(false, 0);
	string tmpst = "0" + st, rest = ""; 
	string integer = Strtok(tmpst, ".", rest), decimal = Strtok(rest, ".", rest), _rest = Strtok(rest, ".", rest);
	if (integer.empty() || !_rest.empty()) return make_pair(false, 0);
	if (decimal.empty()){
		if (!((integer.size() == st.size() + 1) || (integer.size() == st.size() && st[st.size() - 1] == '.'))) 
			return make_pair(false, 0);
		if (!IsNonNegativeInteger(integer)) return make_pair(false, 0);
		return make_pair(true, 1.0 * atoi(integer.data()));
	}else{
		if (integer.size() + decimal.size() != st.size()) return make_pair(false, 0);
		if (!IsNonNegativeInteger(integer) || !IsNonNegativeInteger(decimal)) return make_pair(false, 0);
		return make_pair(true, atoi(integer.data()) + atoi(decimal.data()) * pow(0.1, decimal.size()));
	}
}
inline pair<bool, string> Split(string &str){
	if (str[0] == '"'){
		string result = str.substr(1);
		int pos = result.find_first_of('"');
		str = result.substr(pos + 1);
		result = result.substr(0, pos);
		return make_pair(true, result);
	}else{
		string result = Strtok(str, " \t", str);
		return make_pair(false, result);	
	}
}
#endif