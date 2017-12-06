/*
	Author : spectrometer
*/

#ifndef TYPE_H
#define TYPE_H

#include <iostream>
#include <fstream>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <algorithm>
#include <iomanip>

using namespace std;

struct SingleIndex{
	char key[60];
	SingleIndex() {
		memset(key, 0, sizeof(key));
	}
	SingleIndex(char *_key){
		strcpy(key, _key);
	}
	void Show(){
		printf("keyshow : \n");
		printf("key = %s\n", key);
	}
};
bool operator < (const SingleIndex &a, const SingleIndex &b){ 
	return strcmp(a.key, b.key) < 0;
}
bool operator == (const SingleIndex &a, const SingleIndex &b){
	return strcmp(a.key, b.key) == 0;
}
bool operator > (const SingleIndex &a, const SingleIndex &b){
	return strcmp(a.key, b.key) > 0;
}
bool operator <= (const SingleIndex &a, const SingleIndex &b){
	return a < b || a == b;
}
bool operator >= (const SingleIndex &a, const SingleIndex &b){
	return a > b || a == b;
}
bool operator != (const SingleIndex &a, const SingleIndex &b){
	return strcmp(a.key, b.key) != 0;
}

struct InfoIndex{
	char key1[60], key2[60];
	InfoIndex() {
		memset(key1, 0, sizeof(key1));
		memset(key2, 0, sizeof(key2));
	}
	InfoIndex(char *_key1, char *_key2){
		strcpy(key1, _key1);
		strcpy(key2, _key2);
	}
	void Show(){
		printf("keyshow : \n");
		printf("key1 = %s key2 = %s\n", key1, key2);
	}
};
bool operator < (const InfoIndex &a, const InfoIndex &b){ 
	int cmp1 = strcmp(a.key1, b.key1), cmp2 = strcmp(a.key2, b.key2);
	return (cmp1 < 0) || (cmp1 == 0 && cmp2 < 0);
}
bool operator == (const InfoIndex &a, const InfoIndex &b){
	int cmp1 = strcmp(a.key1, b.key1), cmp2 = strcmp(a.key2, b.key2);
	return (cmp1 == 0) && (cmp2 == 0);
}
bool operator > (const InfoIndex &a, const InfoIndex &b){
	int cmp1 = strcmp(a.key1, b.key1), cmp2 = strcmp(a.key2, b.key2);
	return (cmp1 > 0) || (cmp1 == 0 && cmp2 > 0);
}
bool operator <= (const InfoIndex &a, const InfoIndex &b){
	return a < b || a == b;
}
bool operator >= (const InfoIndex &a, const InfoIndex &b){
	return a > b || a == b;
}
bool operator != (const InfoIndex &a, const InfoIndex &b){
	return strcmp(a.key1, b.key1) != 0;
}

#endif