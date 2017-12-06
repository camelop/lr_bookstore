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
            return token;
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
inline string Split(string str){
	string result = str.substr(1);
	result.pop_back();
	return result;
}
inline void BookNameListShow(string _book_name){
	string book_name = Split(_book_name);
	const char *name = book_name.data();
	InfoIndex book_name_to_show(name, "");
	book_name_list.ShowAll(book_name_to_show);
}
inline void BookAuthorListShow(string _book_author){
	string book_author = Split(_book_author);
	const char *author = book_author.data();
	InfoIndex book_author_to_show(author, "");
	book_author_list.ShowAll(book_author_to_show);
}
inline void BookKeywordsListShow(string _book_keywords){
	string book_keywords = Split(_book_keywords);
	const char *keywords = book_keywords.data();
	InfoIndex book_keywords_to_show(keywords, "");
	book_keywords_list.ShowAll(book_keywords_to_show);
}
inline bool equal(const string &str1, const string &str2){
	//cout << "COMPARE" << endl;
	//cout << str1 << " " << str2 << endl;
	return str1.compare(str2) == 0;
}
inline bool IsLegalUserID(string st){
	if (st.empty()) return false;
	if (st.size() > 30) return false;
	for (int i = 0; i < st.size(); i++)
		if (!isdigit(st[i]) && !islower(st[i]) && !isupper(st[i]) && st[i] != '_') return false;
	return true;
}
inline bool IsLegalUserPasswd(string st){
	return IsLegalUserID(st);
}
inline bool IsNonNegativeInteger(string st){
	if (st.empty()) return true;
	for (int i = 0; i < st.size(); i++)
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
pair<bool, double> CheckIfLegalDouble(string st){
	if (st.size() == 1 && !isdigit(st[0])) return make_pair(false, 0);
	string tmpst = "0" + st, rest = ""; 
	string integer = Strtok(tmpst, '.', rest), decimal = Strtok(rest, '.', rest), _rest = Strtok(rest, '.', rest);
	if (integer.empty() || !_rest.empty()) return make_pair(false, 0);
	if (decimal.empty()){
		if (!((integer.size() == st.size() + 1) || (integer.size() == st.size() && st[st.size() - 1] == '.'))) 
			return make_pair(false, 0);
		if (!IsNonNegativeInteger(integer)) return make_pair(false, 0);
		return make_pair(true, 1.0 * atoi(integer.data()));
	}else{
		if (integer.size() + decimal.size() != st.size()) return make_pair(false, 0);
		if (!IsNonNegativeInteger(integer) || !IsNonNegativeInteger(decimal)) return make_pair(false, 0);
		return make_pair(true, atoi(integer.data()) + atoi(decimal.data()) * pow(0.1, decimal.size()));
	}
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
			//cout << "command : " << command << endl;
			for (tokens[tokens_cnt] = Strtok(command, ' ', rest); !tokens[tokens_cnt].empty(); ){
				++tokens_cnt;
				tokens[tokens_cnt] = Strtok(rest, ' ', rest);
			}
			//cout << tokens_cnt << endl;
			//cout << tokens[0] << endl;
			/*
			cout << "|"; for (int i = 0; i < tokens_cnt; i++) cout << tokens[i] << "|"; cout << endl;
			for (int i = 0; i < tokens_cnt; i++) cout << tokens[i] << "|";
			cout << endl;
            */
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
						string tmp;
						string identifier = Strtok(tokens[i], '=', tmp), rest = Strtok(tmp, '=', tmp), rrest = Strtok(tmp, '=', tmp);
						if (rest.empty() || !rrest.empty()) throw true;
						if (identifier.size() + rest.size() + 1 != tokens[i].size()) throw true;
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
					now_selection = book_to_modify.ISBN;
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
							now_selection = tokens[1]; 
						}else if (equal(tokens[0], SHOW) && !equal(tokens[1], FINANCE)){
							//show -ISBN=[ISBN]/-name=[NAME]/-author=[AUTHOR]/-keyword=[keyword]
							if (now_authority < 1) throw true;
							string tmp; 
							string identifier = Strtok(tokens[1], '=', tmp), rest = Strtok(tmp, '=', tmp), rrest = Strtok(tmp, '=', tmp);
							if (rest.empty() || !rrest.empty()) throw true;
							if (identifier.size() + rest.size() + 1 != tokens[1].size()) throw true;
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