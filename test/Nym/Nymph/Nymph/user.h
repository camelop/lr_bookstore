/*
	Author : spectrometer
*/
#ifndef USER_H
#define USER_H

#define BOSS 7
#define MANAGER 3
#define CUSTOMER 1
#define NONE 0

#include <iostream>
#include <fstream>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <algorithm>
#include <iomanip>

using namespace std;

class User{
public:
	char user_id[30], user_password[30], user_name[30];
	short authority;

	User(int _authority = 1) : authority(_authority){
		memset(user_id, 0, sizeof(user_id));
        memset(user_password, 0, sizeof(user_password));
        memset(user_name, 0, sizeof(user_name));
	}
	User(char *_user_id, char *_user_password, char *_user_name, int _authority = 1) : authority(_authority){
		strcpy(user_id, _user_id);
		strcpy(user_password, _user_password);
		strcpy(user_name, _user_name);
	}
	void SetUserID(char *_id){ strcpy(user_id, _id); }
	void SetUserPassword(char *_password){ strcpy(user_password, _password); }
	void SetUserName(char *name){ strcpy(user_name, name); }
	void Show(){
		printf("user_show : \n");
		printf("id=%s psw=%s nm=%s authority=%d\n", user_id, user_password, user_name, authority);
	}
};

ostream & operator << (ostream & os, const User & user){
	os << user.user_id << '\t' << user.user_name << '\t' << user.user_password << '\t'
	<< user.authority;
	return os;
}

#endif