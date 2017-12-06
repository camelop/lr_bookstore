/*
	Author : spectrometer
*/

#include <iostream>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <algorithm>
#include <iomanip>
#include <string>
#include "book.h"
#include "user.h"
#include "list.h"
#include "type.h"
#include "mystring.h"

using namespace std;

string SU = "su";
string LOGOUT = "logout";
string USERADD = "useradd";
string REGISTER = "register";
string DELETE = "delete";
string PASSWD = "passwd";
string SELECT = "select";
string MODIFY = "modify";
string IMPORT = "import";
string SHOW = "show";
string BUY = "buy";
string EXIT = "exit";
string ISISBN = "-ISBN";
string ISNAME = "-name";
string ISAUTHOR = "-author";
string ISKEYWORD = "-keyword";
string ISPRICE = "-price";
string FINANCE = "finance";

List<SingleIndex, User> user_list;
List<SingleIndex, Book> ISBN_list;
List<InfoIndex, Book> book_name_list;
List<InfoIndex, Book> book_author_list;
List<InfoIndex, Book> book_keywords_list;

int now_authority = NONE;
string now_selection = "";
int now_show_all_cnt = 0, now_trade_cnt = 0;
double total_income = 0, total_expense = 0;

inline void UserListModify(User user_to_modify){
	SingleIndex user_id_to_modify(user_to_modify.user_id);
	user_list.Modify(user_id_to_modify, user_to_modify);
}
inline void ISBNListModify(Book book_to_modify){
	SingleIndex ISBN_to_modify(book_to_modify.ISBN);
	ISBN_list.Modify(ISBN_to_modify, book_to_modify);
}
inline void BookNameListModify(Book book_to_modify){
	InfoIndex book_name_to_modify(book_to_modify.book_name, book_to_modify.ISBN);
	book_name_list.Modify(book_name_to_modify, book_to_modify);
}
inline void BookAuthorListModify(Book book_to_modify){
	InfoIndex book_author_to_modify(book_to_modify.book_author, book_to_modify.ISBN);
	book_author_list.Modify(book_author_to_modify, book_to_modify);
}
inline void BookKeywordsListModify(Book book_to_modify){
	for (int i = 0; i < book_to_modify.keywords_cnt; i++){
		InfoIndex book_keywords_to_modify(book_to_modify.keywords[i], book_to_modify.ISBN);
		book_keywords_list.Modify(book_keywords_to_modify, book_to_modify);
	}
}
inline void BookModify(Book book_to_modify){
	ISBNListModify(book_to_modify);
	BookNameListModify(book_to_modify);
	BookAuthorListModify(book_to_modify);
	BookKeywordsListModify(book_to_modify);
}
inline void ISBNListShow(string _ISBN){
	const char *ISBN = _ISBN.data();
	SingleIndex ISBN_to_show(ISBN);
	ISBN_list.ShowAll(ISBN_to_show);
}
inline void BookNameListShow(string book_name){
	const char *name = book_name.data();
	InfoIndex book_name_to_show(name, "");
	book_name_list.ShowAll(book_name_to_show);
}
inline void BookAuthorListShow(string book_author){
	const char *author = book_author.data();
	InfoIndex book_author_to_show(author, "");
	book_author_list.ShowAll(book_author_to_show);
}
inline void BookKeywordsListShow(string book_keywords){
	const char *keywords = book_keywords.data();
	InfoIndex book_keywords_to_show(keywords, "");
	book_keywords_list.ShowAll(book_keywords_to_show);
}

inline bool IsFirstTime(){
	ifstream file("initialize");
	if (!file){
		ofstream _file("initialize");
		file.close();
		_file.close();
		return true;
	}else{
		file.close();
		return false;
	}
}
inline void Initialize(){
	user_list.Initialize("user_list");
	ISBN_list.Initialize("ISBN_list");
	book_name_list.Initialize("book_name_list");
	book_author_list.Initialize("book_author_list");
	book_keywords_list.Initialize("book_keywords_list");
	ofstream file;
	file.open("finance"); file.close();
	file.open("info_storage"); file.close();
	User root("root", "sjtu", "root", BOSS);
	SingleIndex root_to_insert(root.user_id);
	user_list.Insert(root_to_insert, root);
}
inline void LoadInfoFromDisk(){
	fstream file("info_storage", ios :: in);
	file.seekg(0, ios :: beg);
	file.read(reinterpret_cast<char *> (&user_list), sizeof(user_list));
	file.read(reinterpret_cast<char *> (&ISBN_list), sizeof(ISBN_list));
	file.read(reinterpret_cast<char *> (&book_name_list), sizeof(book_name_list));
	file.read(reinterpret_cast<char *> (&book_author_list), sizeof(book_author_list));
	file.read(reinterpret_cast<char *> (&book_keywords_list), sizeof(book_keywords_list));
	file.close();
	file.open("initialize", ios :: in);
	file.seekg(0, ios :: beg);
	file.read(reinterpret_cast<char *> (&total_income), sizeof(total_income));
	file.read(reinterpret_cast<char *> (&total_expense), sizeof(total_expense));
	file.read(reinterpret_cast<char *> (&now_trade_cnt), sizeof(now_trade_cnt));
	file.close();	
}
inline void UpdateInfoToDisk(){
	fstream file("info_storage", ios :: out);
	file.seekp(0, ios :: beg);
	file.write(reinterpret_cast<char *> (&user_list), sizeof(user_list));
	file.write(reinterpret_cast<char *> (&ISBN_list), sizeof(ISBN_list));
	file.write(reinterpret_cast<char *> (&book_name_list), sizeof(book_name_list));
	file.write(reinterpret_cast<char *> (&book_author_list), sizeof(book_author_list));
	file.write(reinterpret_cast<char *> (&book_keywords_list), sizeof(book_keywords_list));
	file.close();
	file.open("initialize", ios :: out);
	file.seekp(0, ios :: beg);
	file.write(reinterpret_cast<char *> (&total_income), sizeof(total_income));
	file.write(reinterpret_cast<char *> (&total_expense), sizeof(total_expense));
	file.write(reinterpret_cast<char *> (&now_trade_cnt), sizeof(now_trade_cnt));
	file.close();	
}
inline bool IsTerminalMode(){
	fstream file;
	file.open("command.txt", ios :: in);
	if (!file)  return false;
	else{
		file.close();
		return true;
	}
}
inline void WriteTradeToDisk(double money){
	now_trade_cnt++;
	fstream file("finance");
	file.seekp(0, ios :: end);
	file.write(reinterpret_cast<char *> (&money), sizeof(money));
	file.close();
}
inline void ShowFinance(int time){
	fstream file("finance");
	double ans_income = 0, ans_expense = 0, trade;
	file.seekg(-sizeof(double) * time, ios :: end);
	for (int i = 0; i < time; i++){
		file.read(reinterpret_cast<char *> (&trade), sizeof(trade));
		if (trade < 0) ans_expense += trade;
		else ans_income += trade;
	}
	file.close();
	cout << "+ " << setiosflags(ios :: fixed) << setprecision(2) << ans_income << " - " << abs(ans_expense) << endl;
}

int main(){
	if (IsFirstTime()) Initialize();
	else LoadInfoFromDisk();
	if (IsTerminalMode()){
		fstream file("command.txt");
		now_authority = BOSS;
		for (; ;){
			string command, tokens[100], rest = "";
			int tokens_cnt = 0;
			getline(file, command); 
			ProcessWhiteSpace(command);
			tokens[0] = Strtok(command, " \t", rest);    
            try{
				if (equal(tokens[0], MODIFY)){
					if (now_authority < 3) throw true;
					if (equal(now_selection, "")) throw true;
					SingleIndex ISBN_to_modify(now_selection);
					pair<bool, Book> info = ISBN_list.Search(ISBN_to_modify);
					//printf("info.first %d\n", info.first);
					Book book_to_modify;
					if (info.first == false){
						book_to_modify.SetISBN(now_selection);
					}else book_to_modify = info.second;
					string mrest = rest;
					for (int i = 1; !mrest.empty(); i++){
						string tmp = mrest, identifier = Strtok(mrest, "=", mrest); 
						//cout << "identifier:" << identifier << endl;
						//cout << "mrest:" << mrest << endl; 
						if (identifier[0] != '-' || mrest.empty()) throw true;
						if (identifier.size() + mrest.size() + 1 != tmp.size()) throw true;
						pair<bool, string> checker = Split(mrest);
						string rest = checker.second;
						//cout << "mrest:" << mrest << endl; 
						ProcessWhiteSpace(mrest);
						//cout << "mrest:" << mrest << endl;
						//cout << "rest:" << rest << endl;
						//cout << "identifier:" << identifier << "rest:" << rest << endl;
						if (equal(identifier, ISISBN)){
							//puts("OK");
							SingleIndex ISBN_to_modify(rest);
							pair<bool, Book> info = ISBN_list.Search(ISBN_to_modify);
							//puts("OK");
							if (info.first == true) throw true;
							book_to_modify.SetISBN(rest);
						}else if (equal(identifier, ISNAME)){
							if (!checker.first) throw true;
							book_to_modify.SetBookName(rest);
						}else if (equal(identifier, ISAUTHOR)){
							if (!checker.first) throw true;
							book_to_modify.SetBookAuthor(rest);
						}else if (equal(identifier, ISKEYWORD)){
							if (!checker.first) throw true;
							book_to_modify.SetBookKeywords(rest);
						}else if (equal(identifier, ISPRICE)){
							pair<bool, double> package = CheckIfLegalDouble(rest);
							if (package.first == false) throw true;
							else book_to_modify.SetBookPrice(package.second);
						}else throw true;
					}
					//puts("!!!");
					BookModify(book_to_modify);
					//puts("*****");
					now_selection = book_to_modify.ISBN;
				}else if (equal(tokens[0], SHOW)){
					if (rest.empty()){
						//show
						if (now_authority < 1) throw true;
						ISBN_list.ShowAll();
					}else if (rest.find('=') == -1){
						//show finance
						if (!equal(rest, FINANCE)) throw true;
						if (now_authority < 7) throw true;
						cout << "+ " << setiosflags(ios :: fixed) << setprecision(2) << total_income << " - " << total_expense << endl;
					}else{
						//show -ISBN=[ISBN]/-name=[NAME]/-author=[AUTHOR]/-keyword=[keyword]
						string tmp = rest, identifier = Strtok(rest, "=", rest);
						if (identifier[0] != '-' || rest.empty()) throw true;
						if (identifier.size() + rest.size() + 1 != tmp.size()) throw true;
						if (now_authority < 1) throw true;
						pair<bool, string> checker = Split(rest);
						string message = checker.second;
						if (equal(identifier, ISISBN)) ISBNListShow(message);
						else if (equal(identifier, ISNAME)) BookNameListShow(message);
						else if (equal(identifier, ISAUTHOR)) BookAuthorListShow(message);
						else if (equal(identifier, ISKEYWORD)) BookKeywordsListShow(message);
						else throw true;
					}
				}else{
					for (; !tokens[tokens_cnt].empty(); ){
						++tokens_cnt;
						tokens[tokens_cnt] = Strtok(rest, " \t", rest);
					}
					if (tokens_cnt == 1){
						if (equal(tokens[0], LOGOUT)){
							//logout
							if (now_authority < 1) throw true;
							now_authority = 0;
						}else if (equal(tokens[0], EXIT)){
							//exit
							break;
						}else throw true;
					}else if (tokens_cnt == 2){
						if (equal(tokens[0], SU)){
							//su [user_id]
							SingleIndex user_id_to_login(tokens[1]);
							if (!IsLegalUserID(tokens[1])) throw true;
							pair<bool, User> info = user_list.Search(user_id_to_login);
							if (info.first == false) throw true;
							User user_to_login = info.second;
							if (now_authority <= user_to_login.authority) throw true;
							now_authority = user_to_login.authority;
						}else if (equal(tokens[0], SELECT)){
							//select [ISBN]
							if (now_authority < 3) throw true;
							now_selection = tokens[1]; 
						}else if (equal(tokens[0], DELETE)){
							if (now_authority < 7) throw true;
							if (!IsLegalUserID(tokens[1])) throw true;
							SingleIndex user_id_to_delete(tokens[1]);
							pair<bool, User> info = user_list.Search(user_id_to_delete);
							if (info.first == false) throw true;
							user_list.Delete(user_id_to_delete);
						}else throw true;
					}else if (tokens_cnt == 3){
						if (equal(tokens[0], SU)){
							//su [user-id] [passwd]
							if (!IsLegalUserID(tokens[1])) throw true;
							if (!IsLegalUserPasswd(tokens[2])) throw true;
							SingleIndex user_id_to_login(tokens[1]);
							pair<bool, User> info = user_list.Search(user_id_to_login);
							if (info.first == false) throw true;
							User user_to_login = info.second;
							if (!equal(user_to_login.user_password, tokens[2])) throw true;
							now_authority = user_to_login.authority;
						}else if (equal(tokens[0], PASSWD)){
							//password [user-id] [new-passwd]
							if (now_authority != 7) throw true;
							if (!IsLegalUserID(tokens[1])) throw true;
							if (!IsLegalUserPasswd(tokens[2])) throw true;
							SingleIndex user_id_to_modify(tokens[1]);
							pair<bool, User> info = user_list.Search(user_id_to_modify);
							User user_to_modify = info.second;
							user_to_modify.SetUserPassword(tokens[2]);
							UserListModify(user_to_modify);
						}else if (equal(tokens[0], IMPORT)){
							//import [quantity] [cost-price(in total)]
							if (now_authority < 3) throw true;
							if (equal(now_selection, "")) throw true;
							if (!IsNonNegativeInteger(tokens[1])) throw true;
							pair<bool, double> package = CheckIfLegalDouble(tokens[2]);
							if (package.first == false) throw true;
							SingleIndex ISBN_to_import(now_selection);
							pair<bool, Book> info = ISBN_list.Search(ISBN_to_import);
							Book book_to_import = info.second;
							book_to_import.AddBookStock(atoi(tokens[1].data()));
							BookModify(book_to_import);
							total_expense += package.second;
							WriteTradeToDisk(-package.second);
						}else if (equal(tokens[0], SHOW)){
							//show finance [time]
							if (now_authority < 7) throw true;
							if (!equal(tokens[1], FINANCE)) throw true;
							if (!IsNonNegativeInteger(tokens[2])) throw true;
							if (atoi(tokens[2].data()) > now_trade_cnt) throw true;
							ShowFinance(atoi(tokens[2].data()));
						}else if (equal(tokens[0], BUY)){
							//buy [ISBN] quantity
							if (now_authority < 1) throw true;
							if (!IsNonNegativeInteger(tokens[2])) throw true;
							int delta = atoi(tokens[2].data());
							SingleIndex ISBN_to_buy(tokens[1]);
							pair<bool, Book> info = ISBN_list.Search(ISBN_to_buy);
							if (info.first == false) throw true;
							Book book_to_buy = info.second;
							if (book_to_buy.book_stock < delta) throw true;
							book_to_buy.book_stock -= delta;
							BookModify(book_to_buy);
							total_income += book_to_buy.book_price * delta;
							WriteTradeToDisk(book_to_buy.book_price * delta);
						}else throw true;
					}else if (tokens_cnt == 4){
						if (equal(tokens[0], PASSWD)){
							//passwd [user-id] [old-passwd] [new-passwd]
							if (now_authority < 1) throw true;
							if (!IsLegalUserID(tokens[1])) throw true;
							if (!IsLegalUserPasswd(tokens[2])) throw true;
							if (!IsLegalUserPasswd(tokens[3])) throw true;
							SingleIndex user_id_to_modify(tokens[1]);
							pair<bool, User> info = user_list.Search(user_id_to_modify);
							if (info.first == false) throw true;
							User user_to_modify = info.second;
							if (!equal(user_to_modify.user_password, tokens[2])) throw true;
							user_to_modify.SetUserPassword(tokens[3]);
							UserListModify(user_to_modify);
						}else if (equal(tokens[0], REGISTER)){
							//register [user-id] [passwd] [name]
							if (!IsLegalUserID(tokens[1])) throw true;
							if (!IsLegalUserPasswd(tokens[2])) throw true;
							SingleIndex user_id_to_register(tokens[1]);
							pair<bool, User> info = user_list.Search(user_id_to_register);
							if (info.first == true) throw true;
							User user_to_register(tokens[1].data(), tokens[2].data(), tokens[3].data());
							user_list.Insert(user_id_to_register, user_to_register);
						}else throw true;
					}else if (tokens_cnt == 5){
						if (equal(tokens[0], USERADD)){
							//useradd [user-id] [passwd] [7/3/1] [name]
							if (now_authority < 3) throw true;
							if (!IsLegalUserID(tokens[1])) throw true;
							if (!IsLegalUserPasswd(tokens[2])) throw true;
							if (!equal(tokens[3], "7") && !equal(tokens[3], "3") && !equal(tokens[3], "1")) throw true;
							if (atoi(tokens[3].data()) >= now_authority) throw true;
							SingleIndex	user_id_to_add(tokens[1]);
							pair<bool, User> info = user_list.Search(user_id_to_add);
							if (info.first == true) throw true;
							User user_to_add(tokens[1].data(), tokens[2].data(), tokens[4].data(), atoi(tokens[3].data()));
							user_list.Insert(user_id_to_add, user_to_add);
						}else throw true;
					}else throw true;
				}
			}
			catch(bool IsInvalid){
				cout << "Invalid" << endl;
			}
			/*
			cout << "------User_List------" << endl;
			user_list.ShowAll();
			cout << "------Book List------" << endl;
			ISBN_list.ShowAll();
			cout << "------END------" << endl;
			*/
		}
	}
	UpdateInfoToDisk();
	return 0;
}