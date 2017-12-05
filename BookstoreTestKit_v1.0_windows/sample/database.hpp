#ifndef LR_DATABASE
#define LR_DATABASE

#include<string>
#include<fstream>
#include<vector>
#include<algorithm>
using namespace std;

template<typename K, typename V, int(*hashK)(K)>
class DB {

private:
	string src;

	bool exist(const string& des) {
		fstream test;
		test.open(des, fstream::in);
		if (!test) return false; else return true;
	}

	string toString(int x) {
		if (x < 0) return "other";
		if (x == 0) return "0";
		string ret = "";
		while (x > 0) {
			ret = (char)('0' + x % 10) + ret;
			x /= 10;
		}
		return ret;
	}

public:
	// find one match
	bool find(const K& key, V& ans) {
		string des = src + "_" + toString(hashK(key)&255)+ ".bin";
		if (!exist(des)) {
			return false;
		}
		else {
			fstream drawer(des, fstream::in | fstream::binary);
			int s;
			drawer.read(
				reinterpret_cast<char*>(&s),
				sizeof(int)
				);
			for (int i = 0; i < s; ++i) {
				K nw;
				drawer.read(
					reinterpret_cast<char*>(&nw),
					sizeof(K)
					);
				if (nw == key) {
					drawer.read(
						reinterpret_cast<char*>(&ans),
						sizeof(V)
						);
					drawer.close();
					return true;
				}
				else {
					drawer.seekg(sizeof(V), ios::cur);
				}
			}
			drawer.close();
			return false;
		}
	}
	
	bool modify(const K& key, const V& old_value,
				const V& new_value) {
		V ans;
		string des = src + "_" + toString(hashK(key) & 255) + ".bin";
		if (!exist(des)) {
			return false;
		}
		else {
			fstream drawer(des, fstream::in | fstream::out | fstream::binary);
			int s;
			drawer.read(
				reinterpret_cast<char*>(&s),
				sizeof(int)
				);
			for (int i = 0; i < s; ++i) {
				K nw;
				drawer.read(
					reinterpret_cast<char*>(&nw),
					sizeof(K)
					);
				if (nw == key) {
					drawer.read(
						reinterpret_cast<char*>(&ans),
						sizeof(V)
						);
					if (ans == old_value) {
						streamoff pos = drawer.tellg();
						pos -= sizeof(V);
						drawer.seekp(pos);
						drawer.write(
							reinterpret_cast<const char*>(&new_value),
							sizeof(V)
							);
						drawer.close();
						return true;
					}
				}
				else {
					drawer.seekg(sizeof(V), ios::cur);
				}
			}
			drawer.close();
			return false;
		}
	}

	vector<V> findall(const K& key) {
		vector<V> ret;
		V ans;
		string des = src + "_" + toString(hashK(key) & 255) + ".bin";
		if (!exist(des)) {
			return ret;
		}
		else {
			fstream drawer(des, fstream::in | fstream::binary);
			int s;
			drawer.read(
				reinterpret_cast<char*>(&s),
				sizeof(int)
				);
			for (int i = 0; i < s; ++i) {
				K nw;
				drawer.read(
					reinterpret_cast<char*>(&nw),
					sizeof(K)
					);
				if (nw == key) {
					drawer.read(
						reinterpret_cast<char*>(&ans),
						sizeof(V)
						);
					ret.push_back(ans);
				}
				else {
					drawer.seekg(sizeof(V), ios::cur);
				}
			}
			drawer.close();
			sort(ret.begin(), ret.end());
			return ret;
		}
	}

	vector<V> all() {
		vector<V> ret;
		V ans;
		for (int hash_result = 0; hash_result < 256; ++hash_result) {
			string des = src + "_" + toString(hash_result) + ".bin";
			if (!exist(des)) {
				continue;
			}
			else {
				fstream drawer(des, fstream::in | fstream::binary);
				int s;
				drawer.read(
					reinterpret_cast<char*>(&s),
					sizeof(int)
					);
				for (int i = 0; i < s; ++i) {
					K nw;
					drawer.seekg(sizeof(K), ios::cur);
					drawer.read(
						reinterpret_cast<char*>(&ans),
						sizeof(V)
						);
					ret.push_back(ans);
				}
				drawer.close();
			}
		}
		sort(ret.begin(), ret.end());
		return ret;
	}
	
	void show() {
		cerr << "* SHOW DATABASE " << src << endl; 

		K key;
		V ans;
		for (int hash_result = 0; hash_result < 256; ++hash_result) {
			string des = src + "_" + toString(hash_result) + ".bin";
			if (!exist(des)) {
				continue;
			}
			else {
				fstream drawer(des, fstream::in | fstream::binary);
				int s;
				drawer.read(
					reinterpret_cast<char*>(&s),
					sizeof(int)
					);
				for (int i = 0; i < s; ++i) {
					drawer.read(
						reinterpret_cast<char*>(&key),
						sizeof(K)
						);
					drawer.read(
						reinterpret_cast<char*>(&ans),
						sizeof(V)
						);
					cerr << "KEY: " << key << "\tVALUE: " << ans << endl;
				}
				drawer.close();
			}
		}

		cerr << endl;
	}

	void save(const K& key, const V& value) {
		for (auto i : findall(key)) {
			if (value == i) return;
		}
		string des = src + "_" + toString(hashK(key) & 255) + ".bin";
		fstream drawer;
		int s = 1;
		if (!exist(des)) {
			drawer.open(des, fstream::trunc | fstream::out | fstream::binary);
			drawer.write(
				reinterpret_cast<const char*>(&s),
				sizeof(int)
				);
		}
		else {
			drawer.open(des, fstream::in | fstream::out | fstream::binary);
			drawer.read(
				reinterpret_cast<char*>(&s),
				sizeof(int)
				);
			++s;
			drawer.seekp(0, ios::beg);
			drawer.write(
				reinterpret_cast<const char*>(&s),
				sizeof(int)
				);
		}
		int end_loc = (sizeof(int) + (s-1) * (sizeof(K) + sizeof(V)));
		drawer.seekp(end_loc, ios::beg);
		drawer.write(
			reinterpret_cast<const char*>(&key),
			sizeof(K)
			);
		drawer.write(
			reinterpret_cast<const char*>(&value),
			sizeof(V)
			);
		drawer.close();
	}

	DB(const string& des):
		src(des)
	{
	}
};
#endif