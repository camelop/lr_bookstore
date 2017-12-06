/*
	Author : spectrometer
	TO DO : 无用内存回收，修缮Modify
*/

#ifndef LIST_H
#define LIST_H

#include "array.h"
#include <fstream>
#include <iostream>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <algorithm>
#include <iomanip>
#include <string>

using namespace std;

#ifndef BLOCK_SIZE
#define BLOCK_SIZE 2000
#endif

template<class KeyType, class ValueType>
class List{
public:
	struct Node{
		KeyType key;
		long offset;
		long position;
		long pointer[2];
		Node() {}
		Node(KeyType _key, long _offset, long _position, long prev, long succ)
			: key(_key), offset(_offset), position(_position){
				pointer[0] = prev;
				pointer[1] = succ;
		}
		void Show(){
			key.Show();
			printf("offset=%ld position=%ld pointer[0]=%ld pointer[1]=%ld\n", offset, position, pointer[0], pointer[1]);
		}
	};
	int block_cnt;
	char index_file_name[20], array_file_name[20];
	const long HEAD = 0, TAIL = sizeof(Node);
	long END_OF_FILE;
	typedef Array<KeyType, ValueType> ArrayType;
	typedef typename Array<KeyType, ValueType>::Node ArrayNode;

	Node DiskReadNode(long position){
		fstream file(index_file_name);
		file.seekg(position);
		Node node_to_read;
		file.read(reinterpret_cast<char *>(&node_to_read), sizeof(node_to_read));
		file.close();
		return node_to_read;
	}
	void DiskWriteNode(Node node_to_write, long position){
		fstream file(index_file_name);
		file.seekp(position);
		file.write(reinterpret_cast<char *>(&node_to_write), sizeof(node_to_write));
		file.close();
		if (position == END_OF_FILE) END_OF_FILE += sizeof(node_to_write);
	}
	Node CreatBlock(ArrayNode node_to_insert, long prev, long succ){
		block_cnt++;
		Node new_node(node_to_insert.key, (block_cnt - 1) * sizeof(ArrayType), END_OF_FILE, prev, succ);
		DiskWriteNode(new_node, END_OF_FILE);
		ArrayType block_array;
		block_array.Insert(node_to_insert.key, node_to_insert.value);
		block_array.WriteToDisk(array_file_name, new_node.offset);
		return new_node;
	}
	void Initialize(char *_file_name){
		END_OF_FILE = 0;
		block_cnt = 0;
		strcpy(index_file_name, _file_name);
		strcpy(array_file_name, _file_name);
		strcat(array_file_name, "2");
		ofstream file(index_file_name); file.close();
		file.open(array_file_name); file.close();
		KeyType null_key = KeyType();
		Node head(null_key, -1, HEAD, -1, TAIL);
		Node tail(null_key, -1, TAIL, HEAD, -1);
		DiskWriteNode(head, END_OF_FILE);
		DiskWriteNode(tail, END_OF_FILE);
	}
	pair<bool, ValueType> Search(KeyType key_to_find){
		Node now_node = DiskReadNode(HEAD);
		long now_position = now_node.pointer[1];
		ValueType null_value;
		for (; now_position != TAIL; now_position = now_node.pointer[1]){
			now_node = DiskReadNode(now_position);
			if (now_node.key >= key_to_find){
				ArrayType block_array;
				block_array.ReadFromDisk(array_file_name, now_node.offset);
				pair<bool, ArrayNode> info = block_array.Search(key_to_find);
				return make_pair(info.first, info.second.value);
			}
		}
		return make_pair(false, null_value);
	}
	Node Search_Node(KeyType key_to_find){
		Node now_node = DiskReadNode(HEAD);
		long now_position = now_node.pointer[1];
		for (; now_position != TAIL; now_position = now_node.pointer[1]){
			now_node = DiskReadNode(now_position);
			if (now_node.key >= key_to_find) return now_node;
		}
		return now_node;
	}
	void Insert(KeyType key_to_insert, ValueType value_to_insert){
		Node now_node = Search_Node(key_to_insert);
		if (now_node.position == HEAD){
			ArrayNode node_to_insert(key_to_insert, value_to_insert);
			Node new_node = CreatBlock(node_to_insert, HEAD, TAIL);
			Node succ_node = DiskReadNode(now_node.pointer[1]);
			now_node.pointer[1] = new_node.position;
			succ_node.pointer[0] = new_node.position;
			DiskWriteNode(now_node, now_node.position);
			DiskWriteNode(succ_node, succ_node.position);
			return;
		}
		ArrayType block_array;
		block_array.ReadFromDisk(array_file_name, now_node.offset);
		if (block_array.array_cnt + 1 <= BLOCK_SIZE){
			block_array.Insert(key_to_insert, value_to_insert);
			block_array.WriteToDisk(array_file_name, now_node.offset);
			now_node.key = block_array.array[block_array.array_cnt - 1].key;
			DiskWriteNode(now_node, now_node.position);
		}else{
			Node new_node = CreatBlock(block_array.array[block_array.array_cnt - 1], now_node.position, now_node.pointer[1]);
			block_array.array_cnt--;
			block_array.Insert(key_to_insert, value_to_insert);
			now_node.key = block_array.array[block_array.array_cnt - 1].key;
			Node succ_node = DiskReadNode(now_node.pointer[1]);
			now_node.pointer[1] = new_node.position;
			succ_node.pointer[0] = new_node.position;
			DiskWriteNode(now_node, now_node.position);
			DiskWriteNode(succ_node, succ_node.position);
		}
	}
	void Delete(KeyType key_to_delete){
		pair<bool, ValueType> info = Search(key_to_delete);
		if (info.first == false) return;
		Node now_node = Search_Node(key_to_delete);
		ArrayType block_array;
		block_array.ReadFromDisk(array_file_name, now_node.offset);
		if (block_array.array_cnt > 1){
			block_array.Delete(key_to_delete);
			block_array.WriteToDisk(array_file_name, now_node.offset);
			now_node.key = block_array.array[block_array.array_cnt - 1].key;;
			DiskWriteNode(now_node, now_node.position);
		}else{
			Node prev_node = DiskReadNode(now_node.pointer[0]);
			Node succ_node = DiskReadNode(now_node.pointer[1]);
			prev_node.pointer[1] = succ_node.position;
			succ_node.pointer[0] = prev_node.position;
			DiskWriteNode(prev_node, prev_node.position);
			DiskWriteNode(succ_node, succ_node.position);
		}
	}
	void Modify(KeyType key_to_modify, ValueType value_to_modify){
		Delete(key_to_modify);
		Insert(key_to_modify, value_to_modify);
	}
	void ShowAll(){
		Node now_node = DiskReadNode(HEAD);
		ArrayType block_array;
		long now_position = now_node.pointer[1];
		for (; now_position != TAIL; now_position = now_node.pointer[1]){
			now_node = DiskReadNode(now_position);
			block_array.ReadFromDisk(array_file_name, now_node.offset);
			block_array.ShowAll();
		} 
	}
	void ShowAll(KeyType key_to_show){
		Node now_node = Search_Node(key_to_show);
		ArrayType block_array;
		for (; now_node.position != TAIL; ){
			block_array.ReadFromDisk(array_file_name, now_node.offset);
			block_array.ShowAll(key_to_show);
			if (now_node.key != key_to_show) break;
			now_node = DiskReadNode(now_node.pointer[1]);
		}
	}
};

#endif