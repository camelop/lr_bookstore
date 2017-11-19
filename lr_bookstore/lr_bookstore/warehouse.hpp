#ifndef LR_WAREHOUSE
#define LR_WAREHOUSE
#include<iostream>
#include"lstring.hpp"
#include"database.hpp"

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
};

class Warehouse {
private:
	DB < lstring<20>, Book, lstring_hash<20> > isbn_db;
	DB < lstring<40>, lstring<20>, lstring_hash<40> > name_db;
	DB < lstring<40>, lstring<20>, lstring_hash<40> > author_db;
	DB < lstring<40>, lstring<20>, lstring_hash<40> > keyword_db;
	DB < lstring<10>, double, lstring_hash<10> > var_db;
	void spend(double money, const string& reason) {

	}
	void gain(double money, const string& reason) {

	}
public:
	Warehouse() :
		isbn_db("w_ISBN"),
		name_db("w_name"),
		author_db("w_author"),
		keyword_db("w_keyword"),
		var_db("w_v") {
		//TODO : load the values
	}
	bool modify(
		const lstring<20> &ISBN,
		const string& ins //string instruction
		) {
		//TODO

	}
	bool import(
		const lstring<20> &ISBN,
		int q, //quantity
		double cost //cost price in total
		) {
		Book nw;
		if (!isbn_db.find(ISBN, nw)) return false;
		spend(cost, ("BUY " + (string)ISBN));
		Book nnw = nw;
		nnw.quantity += q;
		return isbn_db.modify(ISBN, nw, nnw);
	}
	bool show(const string& ins) {
		//TODO
	}
	bool show_finance(int time) {
		//TODO

	}
	bool buy(
		const lstring<20> &ISBN, 
		int q //quantity
		) {
		//TODO

	}
};

#endif