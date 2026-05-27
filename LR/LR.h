#ifndef _LR_H
#define _LR_H

#include <iostream>
#include <ctype.h>
#include <vector>
#include <string>
#include <algorithm>
#include <map>
#include <set>
using namespace std;

#define EXTENSION_NOTERMINAL '^'

class Item; 

class Prod {
friend class Item;
friend class LR;
friend bool operator==(const Item&, const Item&); 

private:
char noTerminal;
string right;
set<char> additionalVt;


friend bool operator == (const Prod &a, const Prod &b) {
	return a.noTerminal == b.noTerminal && a.right == b.right;
}

friend bool operator == (const Prod &a, char c) {
	return a.noTerminal == c;
}


public:
static string cut(const string &in, int i, int j) {
return string(in.begin() + i, in.begin() + j);
}


static string replaceAll(const string &in, const string from, const string to);

string displayStr() const;

Prod(const string &in);

Prod(const char &noTerminal, const string& right, const set<char>& additionalVt)
	: noTerminal(noTerminal), right(right), additionalVt(additionalVt) {}


};

class Item {
friend class LR;

private:
vector<Prod> prods;


static set<char> Vn;
static set<char> Vt;
static set<char> Symbol;

friend bool operator==(const Item &a, const Item &b) {
	if(a.prods.size() != b.prods.size()) return false;

	for(const auto& p : a.prods) {
		auto it = find(b.prods.begin(), b.prods.end(), p);
		if(it == b.prods.end()) return false;

		// 🔥 SAFE COMPARISON
		if(p.additionalVt.size() != it->additionalVt.size()) return false;

		for(char c : p.additionalVt) {
			if(it->additionalVt.find(c) == it->additionalVt.end())
				return false;
		}
	}
	return true;
}


public:
void add(const string &prod);
void display() const;
};

class LR {
private:
Item G;


enum actionStat {
	ACCEPT = 0,
	SHIFT,
	REDUCE,
};

static const char *actionStatStr[];

vector<Item> C;
map<pair<int, char>, int> GOTO;
map<pair<int, char>, pair<actionStat, int> > ACTION;

map<char, set<char> > FIRST;
map<char, set<char> > FOLLOW;

set<char> first(const string &s);
void follow();

vector<char> inStr;
vector<int> status;
vector<char> parse;

Item closure(Item I);
Item Goto(const Item& I, char X);
void items();

void showStrStack();
void showStatusStack();
void showParseStack();


public:
void add(const string &s);
void build();
void showTable();
void debug();
void loadStr(const string &in);
void parser();
void showGrammar();
void drawGraph();
void generateDot();
void run();
};

#endif
