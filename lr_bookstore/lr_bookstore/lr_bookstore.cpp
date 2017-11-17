#include <iostream>
#include "database.hpp"
#include "lstring.hpp"
#include "user.hpp"
#include "interface.hpp"

using namespace std;
extern map<string, optype> opcode;


int main() {
#define TEST_INTERFACE
# ifdef TEST_DB
	DB<lstring<20>, lstring<20>, lstring_hash> db("db1");
	db.save("v1", "hello");
	db.save("v2", "world");
	lstring<20> output;
	db.modify("v2", "world", "new_world");
	db.find("v2", output);
	cout << output.data;
# endif
# ifdef TEST_USER
	UserSystem userSystem;
	cout << userSystem.login("root", "sjtu") << endl;
	userSystem.logout();
	cout << userSystem.login("root", "sjtu") << endl;
	userSystem.useradd("lxy", "yxl", 3, "liu");
	userSystem.login("lxy", "lxy");
	userSystem.login("lxy", "yxl");
#endif
#ifdef TEST_INTERFACE
	initOpcode();
	Interface interface;
	string mes;
	cout << interface.currentUser() << "@";
	while (getline(cin, mes)) {
		cout.flush();
		interface.receive(mes);
		cout << interface.currentUser() << "@";
	}
#endif
}