#ifndef LEXICAL_H
#define LEXICAL_H

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <ctype.h>
#include <cstdio>
using namespace std;

struct Trie {
Trie *next[26];
bool isEnd;
int loc;
Trie() {
isEnd = false;
for(int i=0; i<26; ++i) next[i] = NULL;
}
};

class Key {
private:
Trie *root;
public:
Key() {
root = new Trie();
}
void add(const string &str, int loc);
void add(vector<string> strs);
int find(const string &str);
void free(Trie *p);
~Key() {
free(root);
}
};

enum Type {
ERROR = 0,
KEY,
DELIMITER,
ARITHMETICOPTR,
RELATIONOPTR,
NUMBER,
ID,
CHAR,
STRING,
COMMENT
};

struct Symbol {
Type type;
string optr;
Symbol(Type type,string optr) : type(type), optr(optr) {};
friend bool operator==(const Symbol &a,const Symbol &b) {
return a.optr == b.optr;
}
};

class Lexical {
private:
Key keys;
vector<pair<string, Type> > optrs;
vector<pair<string, Type> > indetifiers;
vector<pair<string, Type> > constants;
vector<pair<string, Type> > strings;
vector<pair<char, Type> > chars;
unsigned int row, column;
bool isFirst;
string in;
static const char* typeStr[];

	string cut(int i, int j);

public:
	Lexical();
	bool isKey(const string &str);
	int getKeyPointer(const string &str);
	bool isOptr(const string &str);
	int getOptrPointer(const string &str);
	Type getOptrType(const string &str);
	bool isId(const string &str);
	int getIDPointer(const string &str);
	bool isNum(const string &str);
	int getNumPointer(const string &str);
	bool isString(const string &str);
	int getStringPointer(const string &str);
	bool isChar(const string &str);
	int getCharPointer(const string &str);
	bool getIn();
	void analysis();
	void run();


};

const char *Lexical::typeStr[] = {
"ERROR",
"KEY",
"DELIMITER",
"ARITHMETICOPTR",
"RELATIONOPTR",
"NUMBER",
"IDENTIFIER",
"CHAR",
"STRING",
"COMMENT"
};

#endif
