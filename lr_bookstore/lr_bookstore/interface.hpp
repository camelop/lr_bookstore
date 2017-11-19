#ifndef LR_INTERFACE
#define LR_INTERFACE
#include<vector>
#include<map>
#include<iostream>
#include<fstream>
#include "database.hpp"
#include "user.hpp"
#include "warehouse.hpp"
#include "stringTo.hpp"

using namespace std;

//define opcode
enum optype {
	opload,opexit,
	opsu,oplogout,opuseradd,opregister,opdelete,oppasswd,
	opselect,opmodify,opimport,opshow,opbuy,
	opreport,oplog
};
map<string, optype> opcode;

void initOpcode() {
	opcode["load"] = opload;
	opcode["exit"] = opexit;
	opcode["su"] = opsu;
	opcode["logout"] = oplogout;
	opcode["useradd"] = opuseradd;
	opcode["register"] = opregister;
	opcode["delete"] = opdelete;
	opcode["passwd"] = oppasswd;
	opcode["select"] = opselect;
	opcode["modify"] = opmodify;
	opcode["import"] = opimport;
	opcode["show"] = opshow;
	opcode["buy"] = opbuy;
	opcode["report"] = opreport;
	opcode["log"] = oplog;
}


class Interface {
private:
#ifdef LR_DEBUG
public:
#endif
	string selected;
	UserSystem us;
	Warehouse wh;
	vector<string> parse(const string& message) {
		string temp = "";
		vector<string> ret;
		for (auto i : message) {
			if (i == ' ' || i == '\n' || i == '\t') {
				if (temp != "") {
					ret.push_back(temp);
					temp = "";
				}
			}
			else {
				temp += i;
			}
		}
		if (temp != "") ret.push_back(temp);
		return ret;
	}
public:
	Interface() {

	}

#define invalid { \
	cout<<"Invalid"<<endl; \
	return; \
}
	string currentUser() const {
		return us.cur_user();
	}

	void receive(const string& message) {
		vector<string> words = parse(message);
		if (words.empty()) return;
		if (opcode.count(words[0]) == 0) invalid;
		switch (opcode[words[0]]) {

		case opload:
			if (us.access() < 7) invalid;
			if (words.size() != 2) invalid;
			{
				ifstream fin(words[1]);
				if (!fin) invalid;
				string f_message;
				while (getline(fin,f_message)) {
					receive(f_message);
				}
			}
			break;

		case opexit:
			if (words.size() != 1) invalid;
			exit(0);

		case opsu:
			if (words.size() > 3 || words.size() < 2) invalid;
			{
				string& user_id = words[1];
				string passwd = (words.size() > 2) ? (words[2]) : ("");
				if (!us.login(user_id, passwd)) invalid;
			}
			break;

		case oplogout:
			if (us.access() < 1) invalid;
			if (words.size() != 1) invalid;
			us.logout();
			break;

		case opuseradd:
			if (us.access() < 3) invalid;
			if (words.size() != 5) invalid;
			{
				string& user_id = words[1];
				string& passwd = words[2];
				int status = words[3][0] - '0';
				string& name = words[4];
				if (!us.useradd(user_id, passwd, status, name)) invalid;
			}
			break;

		case opregister:
			if (words.size() != 4) invalid;
			{
				string& user_id = words[1];
				string& passwd = words[2];
				string& name = words[3];
				if (!us.useradd(user_id, passwd, 1, name)) invalid;
			}
			break;

		case opdelete:
			if (us.access() < 7) invalid;
			if (words.size() != 2) invalid;
			if (!us.erase(words[1])) invalid;
			break;

		case oppasswd:
			if (us.access() < 1) invalid;
			if (words.size() < 3 || words.size() > 4) invalid;
			{
				string& user_id = words[1];
				string old_pw = (words.size() == 3) ? ("") : (words[2]);
				string new_pw = (words.size() == 3) ? (words[2]) : (words[3]);
				if (!us.passwd(user_id, old_pw, new_pw)) invalid;
			}
			break;

		case opselect:
			if (us.access() < 3) invalid;
			if (words.size() != 2) invalid;
			{
				selected = words[1];
			}
			break;

		case opmodify:
			if (us.access() < 3) invalid;
			if (words.size() < 2) invalid;
			if (!wh.modify(selected, message)) invalid;
			break;

		case opimport:
			if (us.access() < 3) invalid;
			if (words.size() != 3) invalid;
			{
				if (selected == "") invalid;
				if (!wh.import(selected, stringToInt(words[1]), stringToDouble(words[2])))
					invalid;
			}
			break;

		case opshow:
			if (words.size()>1 && words[1] == "finance") {
				if (us.access() < 7) invalid;
				if (words.size() > 3) invalid;
				if (words.size() == 2) {
					wh.show_finance(-1);
				}
				else {
					wh.show_finance(stringToInt(words[2]));
				}
			}
			else {
				if (us.access() < 1) invalid;
				if (!wh.show(message)) invalid;
			}
			break;

		case opbuy:
			if (us.access() < 1) invalid;
			if (words.size() != 3) invalid;
			if (!wh.buy(words[1], stringToInt(words[2]))) invalid;
			break;

		case opreport:
			cout << "invictus maneo" << endl;
			break;

		case oplog:
			cout << "A piece of wood here" << endl;
			break;

		default:
			invalid;
		}
	}
#undef invalid

};
#endif