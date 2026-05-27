#ifndef LL1_H
#define LL1_H

#include <iostream>
#include <ctype.h>
#include <vector>
#include <string>
#include <algorithm>
#include <map>
#include <set>
using namespace std;

class Prod {
friend class LL1;
private:
string prod;
char noTerminal;
set<string> selection;
set<char> Vn;
set<char> Vt;


	string cut(int i, int j) {
		return string(prod.begin() + i, prod.begin() + j);
	}

	friend bool operator == (const Prod &a, const char &c) {
		return a.noTerminal == c;
	}

	bool isValid;

public:
	Prod(const string &in);
	bool split();


};

class LL1 {
private:
vector<Prod> G;
set<char> VN;
set<char> VT;
map<char, set<char> > FIRST;
map<char, set<char> > FOLLOW;
map<pair<char, char>, string> M;


	set<char> first(const string &s);
	void follow();
	vector<char> parse;
	vector<char> indata;
	void parseTable();

public:
	bool addProd(const Prod & prod);
	void info();
	void tableInfo();
	void build();
	void showIndataStack();
	void showParseStack();
	void loadIndata(const string &s);
	void parser();
	void error(int step);
	void run();


};

#endif
