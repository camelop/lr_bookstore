/*
	Author : spectrometer
*/

#ifndef ARRAY_H
#define ARRAY_H

#include <iostream>
#include <fstream>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <algorithm>
#include <iomanip>

using namespace std;

#ifndef BLOCK_SIZE
#define BLOCK_SIZE 2000
#endif

template<class KeyType, class ValueType>
class Array{
public:
	struct Node{
		KeyType key;
		ValueType value;
		Node() {
			key = KeyType();
			value = ValueType();
		}
		Node(KeyType _key, ValueType _value) : key(_key), value(_value) {}
	};

	int array_cnt;
	Node array[BLOCK_SIZE];
	
	Array(){
		memset(array, 0, sizeof(array));
		array_cnt = 0;
	}
	void ShowAll(){
		for (int i = 0; i < array_cnt; i++)
			cout << array[i].value;
	}
	void ShowAll(KeyType key_to_show){
		for (int i = 0; i < array_cnt; i++)
			if (!(array[i].key != key_to_show)) 
				cout << array[i].value;
	}
	void ReadFromDisk(char *file_name, long position){
		fstream file(file_name);
		file.seekg(position);
		file.read(reinterpret_cast<char *>(&array_cnt), sizeof(array_cnt));
		for (int i = 0; i < array_cnt; i++)
			file.read(reinterpret_cast<char *>(&array[i]), sizeof(array[i]));
		file.close();
	}
	void WriteToDisk(char *file_name, long position){
		fstream file(file_name);
		file.seekp(position);
		file.write(reinterpret_cast<char *>(&array_cnt), sizeof(array_cnt));
		for (int i = 0; i < array_cnt; i++)
			file.write(reinterpret_cast<char *>(&array[i]), sizeof(array[i]));
		file.close();
	}
	int Find(KeyType key_to_find){
		if (array[0].key >= key_to_find) return 0;
		int L = 0, R = array_cnt - 1;
		for (int mid = (L + R) >> 1; L + 1 < R; mid = (L + R) >> 1){
			if (array[mid].key >= key_to_find) R = mid;
			else L = mid;
		}
		return R;
	}
	pair<bool, Node> Search(KeyType key_to_find){
		int pos = Find(key_to_find);
		return make_pair(key_to_find == array[pos].key, array[pos]);
	}
	void Insert(KeyType key_to_insert, ValueType value_to_insert){
		if (array_cnt == 0 || key_to_insert > array[array_cnt - 1].key){
			array[array_cnt] = Node(key_to_insert, value_to_insert);
			array_cnt++;
			return;
		}
		int pos = Find(key_to_insert);
		for (int i = array_cnt; i >= pos + 1; i--) {
			array[i].key = array[i - 1].key;
			array[i].value = array[i - 1].value;
		}
		array[pos] = Node(key_to_insert, value_to_insert);
		array_cnt++;
	}
	void Delete(KeyType key_to_delete){
		int pos = Find(key_to_delete);
		for (int i = pos; i < array_cnt - 1; i++){
			array[i].key = array[i + 1].key;
			array[i].value = array[i + 1].value;
		}
		array_cnt--;
	}
};

#endif