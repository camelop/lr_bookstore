/*
	Author : spectrometer
*/ 
#ifndef BOOK_H
#define BOOK_H

#include <iostream>
#include <fstream>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <string>
#include <algorithm>
#include <iomanip>

using namespace std;

class Book{
public:
	char ISBN[20], book_name[60], book_author[60], keywords[20][60];
	double book_price;
	int book_stock, keywords_cnt;

	Book() {
		memset(ISBN, 0, sizeof(ISBN));
		memset(book_name, 0, sizeof(book_name));
		memset(book_author, 0, sizeof(book_author));
		memset(keywords, 0, sizeof(keywords));
		book_price = 0;
		book_stock = keywords_cnt = 0;
	}
	Book(const Book &x){
		book_price = x.book_price;
		book_stock = x.book_stock;
		keywords_cnt = x.keywords_cnt;
		strcpy(ISBN, x.ISBN);
		strcpy(book_name, x.book_name);
		strcpy(book_author, x.book_author);
		for (int i = 0; i < keywords_cnt; i++)
			strcpy(keywords[i], x.keywords[i]);
	}
	Book & operator = (const Book &x){
		book_price = x.book_price;
		book_stock = x.book_stock;
		keywords_cnt = x.keywords_cnt;
		strcpy(ISBN, x.ISBN);
		strcpy(book_name, x.book_name);
		strcpy(book_author, x.book_author);
		for (int i = 0; i < keywords_cnt; i++)
			strcpy(keywords[i], x.keywords[i]);
	}
	void SetISBN(string __ISBN){
		const char *_ISBN = __ISBN.data();
		strcpy(ISBN, _ISBN); 
	}
	void SetBookName(string __name){ 
		const char *_name = __name.data();
		strcpy(book_name, _name); 
	}
	void SetBookAuthor(string __author){ 
		const char *_author = __author.data();
		strcpy(book_author, _author); 
	}
	void SetBookPrice(double price){ book_price = price; }
	void SetBookStock(int stock){ book_stock = stock; }
	string Strtok(string str, const char delimiter, string &rest){
	    if (str.empty()){
	        rest.clear();
	        return str;
	    }else{
	        int pos1 = 0, pos2 = 0;
	        for (; pos1 < str.size() && str[pos1] == delimiter; pos1++);
	        string token = str.substr(pos1);
	        if (token.empty()){
	            rest.clear();
	            return str;
	        }else{
	            pos2 = token.find(delimiter);
	            if (pos2 == -1){
	                rest.clear();
	                return token;
	            }
	            string result = token.substr(0, pos2);
	            rest = token.substr(pos2 + 1);
	            return result;
	        }
	    }
	}
	void SetBookKeywords(string __keywords){
		keywords_cnt = 0;
		string rest = "", ptr = Strtok(__keywords, '|', rest);
		const char *_keywords = ptr.data();
		strcpy(keywords[0], _keywords);
		for (; strlen(keywords[keywords_cnt]); ){
			keywords_cnt++;
			ptr = Strtok(rest, '|', rest);
			if (ptr.empty()) break;
			const char *_keywords_ = ptr.data();
			strcpy(keywords[keywords_cnt], _keywords_);
		}
	}
	void AddBookStock(int delta_stock) { book_stock += delta_stock; }
};

ostream & operator << (ostream & os, const Book & book){
	os << book.ISBN << '\t' << book.book_name << '\t' << book.book_author << '\t';
	for (int i = 0; i < book.keywords_cnt - 1; i++) os << book.keywords[i] << "|";
	os << book.keywords[book.keywords_cnt - 1] << "\t";
	os<< setiosflags(ios::fixed) << setprecision(2) << book.book_price << '\t' << book.book_stock << "±¾\n";
	return os;
}

#endif
