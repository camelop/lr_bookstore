#ifndef LR_USER
#define LR_USER
#include "lstring.hpp"
#include "database.hpp"

class UserSystem;

class User {
	friend class UserSystem;
private:
	lstring<30> id;
	lstring<30> passwd;
	lstring<24> name;
	int access;
public:
	bool operator == (const User& rhs) const {
		if (id != rhs.id) return false;
		if (passwd != rhs.passwd) return false;
		if (name != rhs.name) return false;
		if (access != rhs.access) return false;
		return true;
	}
};

class UserSystem {
private:
	DB<lstring<30>, User, lstring_hash<30> > db;
	User cur;
public:
	UserSystem():db("user") {
		User temp;
		if (!db.find("root", temp)) {
			User root;
			root.id = "root";
			root.passwd = "sjtu";
			root.name = "root";
			root.access = 7;
			db.save("root", root);
		}
		if (!db.find("unreg", temp)) {
			User unreg;
			unreg.id = "unreg";
			unreg.passwd = "";
			unreg.name = "unreg";
			unreg.access = 0;
			db.save("unreg", unreg);
		}
		db.find("unreg", cur);
	}

	bool forceLogin(const lstring<30> &id) {
		User nw;
		if (!db.find(id, nw)) {
			return false;
		}
		else {
			cur = nw;
			cerr << "Login: " << id.data << endl;
			return true;
		}
	}

	bool login(const lstring<30> &id, const lstring<30> &passwd) {
		User nw; 
		if (!db.find(id, nw)) {
			return false;
		}
		else {
			if (cur.access <= nw.access && nw.passwd != passwd) 
				return false;
			if (nw.access < 1) return false;
			cur = nw;
			cerr << "Login: " << id.data << endl;
			return true;
		}
	}

	void logout() {
		forceLogin("unreg");
	}
	
	bool useradd(const lstring<30> &id,
		const lstring<30> &passwd,
		int access,
		const lstring<24> &name) {
		if (access > 1 && access >= cur.access) return false;
		User nw;
		if (db.find(id, nw)) {
			if (nw.access != -1) {
				return false;
			}
			else {
				//MODIFY the result of findings
				User nnw;
				nnw.id = id;
				nnw.passwd = passwd;
				nnw.access = access;
				nnw.name = name;
				db.modify(id, nw, nnw);
				return true;
			}
		}
		nw.id = id;
		nw.passwd = passwd;
		nw.access = access;
		nw.name = name;
		db.save(id, nw);
		return true;
	}

	bool erase(const lstring<30> &id) {
		User nw;
		if (id == "root") return false;
		if (id == cur.id) return false;
		if (!db.find(id, nw)) return false;
		User nnw = nw;
		nnw.access = -1; // deleted
		return db.modify(id, nw, nnw);
	}

	bool passwd(const lstring<30> &id,
		const lstring<30> &old_passwd,
		const lstring<30> &new_passwd) {
		User nw;
		if (!db.find(id, nw)) {
			return false;
		}
		if (cur.access < nw.access) return false;
		if (cur.access != 7 && old_passwd != nw.passwd) return false;
		User nnw = nw;
		nnw.passwd = new_passwd;
		return db.modify(id, nw, nnw);
	}

	int access() const{
		return cur.access;
	}

	string cur_user() const {
		return cur.id;
	}

	void display() {
		cout << "Nothing to show" << endl;
	}
};
#endif