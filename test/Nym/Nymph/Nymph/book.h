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
	void SetISBN(char *_ISBN){ strcpy(ISBN, _ISBN); }
	void SetBookName(char *_name){ strcpy(book_name, _name); }
	void SetBookAuthor(char *_author){ strcpy(book_author, _author); }
	void SetBookPrice(double price){ book_price = price; }
	void SetBookStock(int stock){ book_stock = stock; }
	void SetBookKeywords(char *_keywords){
		keywords_cnt = 0;
		char *ptr = strtok(_keywords, "|");
		strcpy(keywords[0], ptr);
		for (; keywords[keywords_cnt] != NULL; ){
			keywords_cnt++;
			ptr = strtok(NULL, "|");
			if (ptr == NULL) break;
			strcpy(keywords[keywords_cnt], ptr);
		}
	}
	void AddBookStock(int delta_stock) { book_stock += delta_stock; }
};

ostream & operator << (ostream & os, const Book & book){
	os << book.ISBN << '\t' << book.book_name << '\t' << book.book_author << '\t';
	for (int i = 0; i < book.keywords_cnt - 1; i++) os << book.keywords[i] << "|";
	os << book.keywords[book.keywords_cnt - 1] << "\t";
	os<< setiosflags(ios::fixed) << setprecision(2) << book.book_price << '\t' << book.book_stock << "本\n";
	return os;
}

#endif