/*
	Author : spectrometer
*/

#include <iostream>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <algorithm>
#include <iomanip>
#include "book.h"
#include "user.h"
#include "list.h"
#include "type.h"

using namespace std;

char SU[] = "su";
char LOGOUT[] = "logout";
char USERADD[] = "useradd";
char REGISTER[] = "register";
char DELETE[] = "delete";
char PASSWD[] = "passwd";
char SELECT[] = "select";
char MODIFY[] = "modify";
char IMPORT[] = "import";
char SHOW[] = "show";
char BUY[] = "buy";
char EXIT[] = "exit";
char ISISBN[] = "-ISBN";
char ISNAME[] = "-name";
char ISAUTHOR[] = "-author";
char ISKEYWORD[] = "-keyword";
char ISPRICE[] = "-price";
char FINANCE[] = "finance";

List<SingleIndex, User> user_list;
List<SingleIndex, Book> ISBN_list;
List<InfoIndex, Book> book_name_list;
List<InfoIndex, Book> book_author_list;
List<InfoIndex, Book> book_keywords_list;

int now_authority = NONE;
char now_selection[20] = "";
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
inline void ISBNListShow(char *ISBN){
	SingleIndex ISBN_to_show(ISBN);
	ISBN_list.ShowAll(ISBN_to_show);
}
inline char *Split(char *str){
	char *st = new char [1000];
	return strncpy(st, str + 1, strlen(str) - 2);
}
inline void BookNameListShow(char *_book_name){
	char *book_name = Split(_book_name);
	InfoIndex book_name_to_show(book_name, "");
	book_name_list.ShowAll(book_name_to_show);
}
inline void BookAuthorListShow(char *_book_author){
	char *book_author = Split(_book_author);
	InfoIndex book_author_to_show(book_author, "");
	book_author_list.ShowAll(book_author_to_show);
}
inline void BookKeywordsListShow(char *_book_keywords){
	char *book_keywords = Split(_book_keywords);
	InfoIndex book_keywords_to_show(book_keywords, "");
	book_keywords_list.ShowAll(book_keywords_to_show);
}
inline bool equal(char *a, char *b){ return strcmp(a, b) == 0; }
inline bool IsLegalUserID(char *st){
	int len = strlen(st);
	if (len > 30) return false;
	for (int i = 0; i < len; i++)
		if (!isdigit(st[i]) && !islower(st[i]) && !isupper(st[i]) && st[i] != '_') return false;
	return true;
}
inline bool IsLegalUserPasswd(char *st){
	return IsLegalUserID(st);
}
inline bool IsNonNegativeInteger(char *st){
	if (!st) return true;
	int len = strlen(st);
	for (int i = 0; i < len; i++)
		if (!isdigit(st[i])) return false;
	return true;
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
	file.open("finance");
	file.close();
	User root("root", "sjtu", "root", BOSS);
	SingleIndex root_to_insert(root.user_id);
	user_list.Insert(root_to_insert, root);
}
inline void LoadInfoFromDisk(){
	fstream file("info_storage");
	file.seekg(0);
	file.read(reinterpret_cast<char *> (&user_list), sizeof(user_list));
	file.read(reinterpret_cast<char *> (&ISBN_list), sizeof(ISBN_list));
	file.read(reinterpret_cast<char *> (&book_name_list), sizeof(book_name_list));
	file.read(reinterpret_cast<char *> (&book_author_list), sizeof(book_author_list));
	file.read(reinterpret_cast<char *> (&book_keywords_list), sizeof(book_keywords_list));
	file.close();
	file.open("finance");
	file.seekg(0);
	file.read(reinterpret_cast<char *> (&total_income), sizeof(total_income));
	file.read(reinterpret_cast<char *> (&total_expense), sizeof(total_expense));
	file.read(reinterpret_cast<char *> (&now_trade_cnt), sizeof(now_trade_cnt));
	file.close();	
}
inline void UpdateInfoToDisk(){
	fstream file("info_storage");
	file.seekp(0);
	file.write(reinterpret_cast<char *> (&user_list), sizeof(user_list));
	file.write(reinterpret_cast<char *> (&ISBN_list), sizeof(ISBN_list));
	file.write(reinterpret_cast<char *> (&book_name_list), sizeof(book_name_list));
	file.write(reinterpret_cast<char *> (&book_author_list), sizeof(book_author_list));
	file.write(reinterpret_cast<char *> (&book_keywords_list), sizeof(book_keywords_list));
	file.close();
	file.open("finance");
	file.seekp(0);
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
	file.seekp(ios :: end);
	file.write(reinterpret_cast<char *> (&money), sizeof(money));
	file.close();
}
inline void ShowFinance(int time){
	fstream file("finance");
	file.seekg(ios :: end);
	double ans_income = 0, ans_expense = 0, trade;
	for (int i = 0; i < time; i++){
		file.seekg(-sizeof(double), ios :: cur);
		file.read(reinterpret_cast<char *> (&trade), sizeof(trade));
		printf("%.2lf\n", trade);
		if (trade < 0) ans_expense += trade;
		else ans_income += trade;
	}
	cout << "+ " << setiosflags(ios :: fixed) << setprecision(2) << ans_income << " - " << ans_expense << endl;
}
pair<bool, double> CheckIfLegalDouble(char *st){
	if (strlen(st) == 1 && !isdigit(st[0])) return make_pair(false, 0);
	char tmpst[100] = "0"; strcat(tmpst, st);
	char *integer = strtok(tmpst, "."), *decimal = strtok(NULL, "."), *_rest = strtok(NULL, ".");
	if (integer == NULL || _rest != NULL) return make_pair(false, 0);
	if (decimal == NULL){
		if (!((strlen(integer) == strlen(st) + 1) || (strlen(integer) == strlen(st) && st[strlen(st) - 1] == '.'))) 
			return make_pair(false, 0);
		if (!IsNonNegativeInteger(integer)) return make_pair(false, 0);
		return make_pair(true, 1.0 * atoi(integer));
	}else{
		if (strlen(integer) + strlen(decimal) != strlen(st)) return make_pair(false, 0);
		if (!IsNonNegativeInteger(integer) || !IsNonNegativeInteger(decimal)) return make_pair(false, 0);
		return make_pair(true, atoi(integer) + atoi(decimal) * pow(0.1, strlen(decimal)));
	}
}
int main(){
	if (IsFirstTime()) Initialize();
	else LoadInfoFromDisk();
	if (IsTerminalMode()){
		fstream file("command.txt");
		now_authority = BOSS;
		for (; ;){
			char command[1000], *tokens[10];
			int tokens_cnt = 0;
			file.getline(command, 1000, '\n');
			for (tokens[0] = strtok(command, " "); tokens[tokens_cnt] != NULL; ){
				tokens_cnt++;
				tokens[tokens_cnt] = strtok(NULL, " ");
			}
			if (command == NULL) break;
            try{
				if (equal(tokens[0], MODIFY)){
					if (now_authority < 3) throw true;
					if (equal(now_selection, "")) throw true;
					SingleIndex ISBN_to_modify(now_selection);
					pair<bool, Book> info = ISBN_list.Search(ISBN_to_modify);
					Book book_to_modify;
					if (info.first == false){
						book_to_modify.SetISBN(now_selection);
					}else book_to_modify = info.second;
					for (int i = 1; i < tokens_cnt; i++){
						if (tokens[i][0] != '-') throw true;
						char tmpst[1000]; strcpy(tmpst, tokens[i]);
						char *identifier = strtok(tmpst, "="), *rest = strtok(NULL, "="), *rrest = strtok(NULL, "=");
						if (rest == NULL || rrest != NULL) throw true;
						if (strlen(identifier) + strlen(rest) + 1 != strlen(tokens[i])) throw true;
						if (equal(identifier, ISISBN)){
							SingleIndex ISBN_to_modify(rest);
							pair<bool, Book> info = ISBN_list.Search(ISBN_to_modify);
							if (info.first == true) throw true;
							book_to_modify.SetISBN(rest);
						}else if (equal(identifier, ISNAME)){
							rest = Split(rest);
							book_to_modify.SetBookName(rest);
						}else if (equal(identifier, ISAUTHOR)){
							rest = Split(rest);
							book_to_modify.SetBookAuthor(rest);
						}else if (equal(identifier, ISKEYWORD)){
							rest = Split(rest);
							book_to_modify.SetBookKeywords(rest);
						}else if (equal(identifier, ISPRICE)){
							pair<bool, double> package = CheckIfLegalDouble(rest);
							if (package.first == false) throw true;
							else book_to_modify.SetBookPrice(package.second);
						}else throw true;
					}
					BookModify(book_to_modify);
					strcpy(now_selection, book_to_modify.ISBN);
				}else{
					if (tokens_cnt == 1){
						if (equal(tokens[0], LOGOUT)){
							//logout
							if (now_authority < 1) throw true;
							now_authority = 0;
						}else if (equal(tokens[0], EXIT)){
							//exit
							break;
						}else if (equal(tokens[0], SHOW)){
							//show
							if (now_authority < 1) throw true;
							ISBN_list.ShowAll();
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
							strcpy(now_selection, tokens[1]); 
						}else if (equal(tokens[0], SHOW) && !equal(tokens[1], FINANCE)){
							//show -ISBN=[ISBN]/-name=[NAME]/-author=[AUTHOR]/-keyword=[keyword]
							if (now_authority < 1) throw true;
							char tmp[1000]; strcpy(tmp, tokens[1]);
							char *identifier = strtok(tmp, "="), *rest = strtok(NULL, "="), *rrest = strtok(NULL, "=");
							if (rest == NULL || rrest != NULL) throw true;
							if (strlen(identifier) + strlen(rest) + 1 != strlen(tokens[1])) throw true;
							if (equal(identifier, ISISBN)) ISBNListShow(rest);
							else if (equal(identifier, ISNAME)) BookNameListShow(rest);
							else if (equal(identifier, ISAUTHOR)) BookAuthorListShow(rest);
							else if (equal(identifier, ISKEYWORD)) BookKeywordsListShow(rest);
							else throw true;
						}else if (equal(tokens[0], SHOW) && equal(tokens[1], FINANCE)){
							cout << "+ " << setiosflags(ios :: fixed) << setprecision(2) << total_income << " - " << total_expense << endl;
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
							book_to_import.AddBookStock(atoi(tokens[1]));
							BookModify(book_to_import);
							total_expense += package.second;
							WriteTradeToDisk(package.second);
						}else if (equal(tokens[0], SHOW)){
							//show finance [time]
							if (now_authority < 7) throw true;
							if (!equal(tokens[1], FINANCE)) throw true;
							if (!IsNonNegativeInteger(tokens[2])) throw true;
							if (atoi(tokens[2]) > now_trade_cnt) throw true;
							ShowFinance(atoi(tokens[2]));
						}else if (equal(tokens[0], BUY)){
							//buy [ISBN] quantity
							if (now_authority < 1) throw true;
							if (!IsNonNegativeInteger(tokens[2])) throw true;
							int delta = atoi(tokens[2]);
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
							User user_to_register(tokens[1], tokens[2], tokens[3]);
							user_list.Insert(user_id_to_register, user_to_register);
						}else throw true;
					}else if (tokens_cnt == 5){
						if (equal(tokens[0], USERADD)){
							//useradd [user-id] [passwd] [7/3/1] [name]
							if (now_authority < 3) throw true;
							if (!IsLegalUserID(tokens[1])) throw true;
							if (!IsLegalUserPasswd(tokens[2])) throw true;
							if (!equal(tokens[3], "7") && !equal(tokens[3], "3") && !equal(tokens[3], "1")) throw true;
							if (atoi(tokens[3]) >= now_authority) throw true;
							SingleIndex	user_id_to_add(tokens[1]);
							pair<bool, User> info = user_list.Search(user_id_to_add);
							if (info.first == true) throw true;
							User user_to_add(tokens[1], tokens[2], tokens[4], atoi(tokens[3]));
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
	}else{
		//TO DO: user-friendly mode
	}

	UpdateInfoToDisk();
	return 0;
}