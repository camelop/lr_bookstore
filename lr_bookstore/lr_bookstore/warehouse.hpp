#ifndef LR_WAREHOUSE
#define LR_WAREHOUSE
#include<iostream>
#include<iomanip>
#include<map>
#include"lstring.hpp"
#include"database.hpp"
#include"stringTo.hpp"

class Warehouse;

class Book {
	friend class Warehouse;
private:
	lstring<20> ISBN;
	lstring<40> name, author;
	lstring<40> keywords;
	double price;
	int quantity;
public:
	Book():price(0),quantity(0){}
	bool operator ==(const Book& rhs) const {
		if (ISBN != rhs.ISBN) return false;
		if (name != rhs.name) return false;
		if (author != rhs.author) return false;
		if (keywords != rhs.keywords) return false;
		if ((price - rhs.price > 0.01) ||
			(price - rhs.price < -0.01)) return false;
		if (quantity != rhs.quantity) return false;
		return true;
	}
	friend ostream& operator << (ostream& os, const Book& rhs) {
		if (rhs.quantity == 0) return os;
		os << rhs.ISBN << '\t' << rhs.name << '\t' << rhs.author << '\t' << rhs.keywords << '\t';
		os << fixed << setprecision(2) << rhs.price;
		os << '\t' << rhs.quantity << "±¾" << '\n';
		return os;
	}
};

int int_hash(int x) { return x; }

class Warehouse {
private:
	DB < lstring<20>, Book, lstring_hash<20> > isbn_db;
	DB < lstring<40>, lstring<20>, lstring_hash<40> > name_db;
	DB < lstring<40>, lstring<20>, lstring_hash<40> > author_db;
	DB < lstring<40>, lstring<20>, lstring_hash<40> > keyword_db;
	DB < lstring<10>, int, lstring_hash<10> > var_db;
	DB < int, double, int_hash > log_db;
	void spend(double money, const string& reason) {
		cerr << "Spend " << money << " because " << reason << endl;
		log_db.save(log_size, -money);
		var_db.modify("log_size", log_size, log_size + 1);
		++log_size;
	}
	void gain(double money, const string& reason) {
		cerr << "Gain " << money << " because " << reason << endl;
		log_db.save(log_size, money);
		var_db.modify("log_size", log_size, log_size + 1);
		++log_size;
	}
	int log_size;
	map<string, string> parameters(const string& s) {
		map<string, string> ret;
		bool inQuo = false;
		bool isKey = false;
		bool isValue = false;
		string mem = "";
		string key = "";
		for (auto i : s + " ") {
			if (i == '"') {
				inQuo = !inQuo;
				continue;
			}
			if (inQuo) {
				mem += i;
			}
			else {
				switch (i) {
				case ' ':
				case '\t':
				case '\n':
				case '\r':
					if (isValue) {
						ret[key] = mem;
					}
					mem = "";
					isKey = false;
					isValue = false;
					break;
				case '-':
					mem = "";
					isKey = true;
					break;
				case '=':
					key = mem;
					mem = "";
					isValue = true;
					break;
				default:
					mem += i;
				}
			}
		}
		return ret;
	}
public:
	Warehouse() :
		isbn_db("w_ISBN"),
		name_db("w_name"),
		author_db("w_author"),
		keyword_db("w_keyword"),
		log_db("w_log"),
		var_db("w_v"){
		//TODO : load the values
		if (!var_db.find("log_size", log_size)) {
			log_size = 0;
			var_db.save("log_size", 0);
		}
	}
	bool modify(
		const lstring<20> &ISBN,
		const string& ins //string instruction
		) {
		if (ISBN == "") return false;
		Book nw;
		if (!isbn_db.find(ISBN, nw)) {
			map<string, string> para = parameters(ins);
			if (para.count("ISBN")) {
				nw.ISBN = para["ISBN"];
			}
			else {
				nw.ISBN = ISBN;
			}
			if (para.count("name")) {
				nw.name = para["name"];
			}
			else return false;
			if (para.count("author")) {
				nw.author = para["author"];
			}
			else return false;
			if (para.count("keyword")) {
				nw.keywords = para["keyword"];
			}
			else return false;
			if (para.count("price")) {
				nw.price = stringToDouble(para["price"]);
			}
			else return false;
			isbn_db.save(nw.ISBN, nw);
			name_db.save(nw.name, nw.ISBN);
			author_db.save(nw.author, nw.ISBN);
			string singleKey = "";
			for (auto i : para["keyword"] + "|") {
				if (i == '|' && singleKey !="") {
					keyword_db.save(singleKey, nw.ISBN);
					singleKey = "";
				}
				else singleKey += i;
			}
			return true;
		}
		else {
			map<string, string> para = parameters(ins);

			// if cannot change ISBN, stop it in advance
			Book toReplace;
			lstring<20> nwISBN;
			if (para.count("ISBN")) {
				nwISBN = para["ISBN"];
				if (isbn_db.find(nwISBN, toReplace)) {
					if (toReplace.quantity > 0) return false;
				}
			}

			Book nnw = nw;
			if (para.count("price")) {
				nnw.price = stringToDouble(para["price"]);
			}
			if (para.count("name")) {
				nnw.name = para["name"];
				name_db.modify(nw.name, ISBN, "");
				name_db.save(nw.name, ISBN);
			}
			if (para.count("author")) {
				nnw.author = para["author"];
				author_db.modify(nw.author, ISBN, "");
				author_db.save(nw.author, ISBN);
			}
			if (para.count("keyword")) {
				nnw.keywords = para["keyword"];
				string singleKey;
				for (auto i : string(nw.keywords) + "|") {
					if (i == '|' && singleKey != "") {
						keyword_db.modify(singleKey, ISBN, "");
						singleKey = "";
					}
					else {
						singleKey += i;
					}
				}
				for (auto i : para["keyword"] + "|") {
					if (i == '|' && singleKey != "") {
						keyword_db.save(singleKey, ISBN);
						singleKey = "";
					}
					else {
						singleKey += i;
					}
				}
			}
			isbn_db.modify(ISBN, nw, nnw);
			nw = nnw;

			// and another condition..
			if (para.count("ISBN")) {
				// so much to change
				double doNotSave = false;
				if (isbn_db.find(nwISBN, toReplace)) {
					doNotSave = true;
					// delete the old
					name_db.modify(toReplace.name, toReplace.ISBN, "");
					author_db.modify(toReplace.author, toReplace.ISBN, "");
					string singleKey = "";
					for (auto i : string(toReplace.keywords) + "|") {
						if (i == '|' && singleKey != "") {
							keyword_db.modify(singleKey, toReplace.ISBN, "");
							singleKey = "";
						}
						else {
							singleKey += i;
						}
					}
				}
				name_db.modify(nw.name, ISBN, nwISBN);
				author_db.modify(nw.author, ISBN, nwISBN);
				string singleKey = "";
				for (auto i : string(nw.keywords) + "|") {
					if (i == '|' && singleKey != "") {
						keyword_db.modify(singleKey, ISBN, nwISBN);
						singleKey = "";
					}
					else {
						singleKey += i;
					}
				}
				nnw.quantity = 0;
				isbn_db.modify(ISBN, nw, nnw);
				if (doNotSave) {
					isbn_db.modify(nwISBN, toReplace, nw);
				}
				else {
					isbn_db.save(nwISBN, nw);
				}
			}
			return true;
			// and other
		}

	}
	bool import(
		const lstring<20> &ISBN,
		int q, //quantity
		double cost //cost price in total
		) {
		Book nw;
		if (!isbn_db.find(ISBN, nw)) return false;
		spend(cost, ("IMPORT " + (string)ISBN));
		Book nnw = nw;
		nnw.quantity += q;
		return isbn_db.modify(ISBN, nw, nnw);
	}
	bool show(const string& ins) {
		map<string, string> para = parameters(ins);
		if (para.count("ISBN")) {
			string& ISBN = para["ISBN"];
			for (auto i : isbn_db.findall(ISBN)) cout << i;
			cout.flush();
			return true;
		}
		if (para.count("name")) {
			string& name = para["name"];
			for (auto i : name_db.findall(name)) {
				for (auto j : isbn_db.findall(i)) cout << j;
			}
			cout.flush();
			return true;
		}
		if (para.count("author")) {
			string& author = para["author"];
			for (auto i : author_db.findall(author)) {
				for (auto j : isbn_db.findall(i)) cout << j;
			}
			cout.flush();
			return true;
		}
		if (para.count("keyword")) {
			string& keyword = para["keyword"];
			for (auto i : keyword_db.findall(keyword)) {
				for (auto j : isbn_db.findall(i)) cout << j;
			}
			cout.flush();
			return true;
		}
		for (auto i : isbn_db.all()) cout << i;
		cout.flush();
		return true;
	}
	bool show_finance(int time) {
		// TODO
		cout << "I don't wanna write anymore..." << endl;
		return true;
	}
	bool buy(
		const lstring<20> &ISBN, 
		int q //quantity
		) {
		Book nw;
		if (!isbn_db.find(ISBN, nw)) return false;
		if (nw.quantity < q) return false;
		gain(nw.price * q, "BUY " + (string)(ISBN)+" * " + intToString(q));
		Book nnw = nw;
		nnw.quantity -= q;
		return isbn_db.modify(ISBN, nw, nnw);
	}
};

#endif