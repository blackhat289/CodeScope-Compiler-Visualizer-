#include "LL1.h"

Prod::Prod(const string &in) {
prod = in;
isValid = false;
split();
}

bool Prod::split() {
if(prod.length() < 4) return false;
if(cut(1, 3) == "->" && isupper(prod[0]))
noTerminal = prod[0];
else return false;


for(unsigned int i=0; i<prod.length(); ++i) {
	char c = prod[i];
	if(isupper(c)) Vn.insert(c);
	else if(c!='|' && !(c=='-' && prod[i+1] == '>' && ++i)) Vt.insert(c);
}

for(unsigned int i=3; i<prod.length(); ++i) {
	unsigned int j;
	for(j=i+1; j<prod.length() && prod[j] != '|'; ++j);
	selection.insert(cut(i, j));
	i = j;
}

return isValid = true;


}

bool LL1::addProd(const Prod &prod) {
if(!prod.isValid) return false;


auto it = find(G.begin(), G.end(), prod.noTerminal);
if(it != G.end()) {
	it->selection.insert(prod.selection.begin(), prod.selection.end());
	for(auto s: prod.selection)
		it->prod += ("|" + s);
} else {
	G.push_back(prod);
}

VN.insert(prod.Vn.begin(), prod.Vn.end());
VT.insert(prod.Vt.begin(), prod.Vt.end());
return true;


}

set<char> LL1::first(const string &s) {
Prod prod = Prod(s);


if(prod.isValid) {
	if(FIRST[prod.noTerminal].size() != 0) return FIRST[prod.noTerminal];

	for(auto sel:prod.selection) {
		set<char> f = first(sel);
		FIRST[prod.noTerminal].insert(f.begin(), f.end());
	}
	return FIRST[prod.noTerminal];
}

else if(s.length() == 0)
	return set<char>({'@'});

else if(s.length() == 1){
	if(VT.find(s[0]) != VT.end())
		return set<char>({s[0]});
	else {
		if(FIRST[s[0]].size() != 0) return FIRST[s[0]];
		auto it = find(G.begin(), G.end(), s[0]);
		if(it != G.end()) {
			set<char> f = first(it->prod);
			FIRST[s[0]].insert(f.begin(), f.end());
		}
		return FIRST[s[0]];
	}
}

else {
	set<char> result;
	for(unsigned int i=0; i<s.length(); ++i) {
		set<char> f = first(string(1, s[i]));
		if(f.find('@') != f.end() && i != s.length()-1) {
			f.erase('@');
			result.insert(f.begin(), f.end());
		} else {
			result.insert(f.begin(), f.end());
			break;
		}
	}
	return result;
}


}

void LL1::follow() {
FOLLOW[G[0].noTerminal].insert('#');


for(auto pp: G) {
	unsigned int size = 0;
	while(size != FOLLOW[pp.noTerminal].size()) {
		size = FOLLOW[pp.noTerminal].size();

		for(auto prod: G) {
			char X = prod.noTerminal;

			for(auto p: G)
				for(auto s: p.selection) {
					auto pos = s.find(X);
					if(pos != string::npos) {
						set<char> f = first(string(s.begin()+pos+1, s.end()));
						FOLLOW[X].insert(f.begin(), f.end());

						if(f.find('@') != f.end()) {
							FOLLOW[X].erase('@');
							set<char> fw = FOLLOW[p.noTerminal];
							FOLLOW[X].insert(fw.begin(), fw.end());
						}
					}
				}
		}
	}
}


}

void LL1::parseTable() {
for(auto prod: G) {
for(auto sel:prod.selection) {
set<char> f = first(sel);


		for(auto t: f)
			if(t == '@')
				for(auto term: FOLLOW[prod.noTerminal])
					M[{prod.noTerminal, term}] = sel;
			else
				M[{prod.noTerminal, t}] = sel;
	}
}


}

void LL1::build() {
for(auto prod: G) first(prod.prod);
follow();
parseTable();

if(VT.find('@') != VT.end())
	VT.erase('@');

VT.insert('#');


}

void LL1::loadIndata(const string &s) {
indata.push_back('#');
for(int i=s.length()-1;i>=0;--i)
indata.push_back(s[i]);
}

void LL1::parser() {
parse.push_back('#');
parse.push_back(G[0].noTerminal);

printf(",\"Parser\": [");
int i = 0, step = 0;
string prod = "";

while(!parse.empty()) {
	printf("%s{ \"step\": %d, ", i++==0?" ":", ", step++);
	printf("\"parseStack\": \"");

	for(char c: parse) printf("%c", c);

	printf("\", \"indataStack\": \"");
	for(auto it=indata.rbegin(); it!=indata.rend(); ++it) printf("%c", *it);

	printf("\", \"production\": \"%s\"}", prod.c_str());

	char X = parse.back(); parse.pop_back();
	char cur = indata.back();
	prod = "";

	if(VT.count(X)) {
		if(X != cur) break;
		indata.pop_back();
		prod = "match " + string(1,X);
	}
	else if(X != '@') {
		prod = M[{X,cur}];
		if(prod.empty()) break;

		if(prod != "@")
			for(int j=prod.size()-1;j>=0;--j)
				parse.push_back(prod[j]);

		prod = string(1,X) + "->" + prod;
	}
}

printf("]\n");


}

void LL1::info() {
// Output FIRST and FOLLOW sets
printf("\"FIRST\": [");
int i = 0;
for(auto prod: G) {
	if(i++ > 0) printf(",");
	printf("{\"noTerminal\": \"%c\", \"Terminal\": [", prod.noTerminal);
	int j = 0;
	for(auto f: FIRST[prod.noTerminal]) {
		if(j++ > 0) printf(",");
		printf("\"%c\"", f);
	}
	printf("]}");
}
printf("], \"FOLLOW\": [");
i = 0;
for(auto prod: G) {
	if(i++ > 0) printf(",");
	printf("{\"noTerminal\": \"%c\", \"Terminal\": [", prod.noTerminal);
	int j = 0;
	for(auto f: FOLLOW[prod.noTerminal]) {
		if(j++ > 0) printf(",");
		printf("\"%c\"", f);
	}
	printf("]}");
}
printf("]");
}

void LL1::tableInfo() {
// Output parsing table
printf(",\"Table\": {\"Header\": [");
int i = 0;
for(auto vt: VT) {
	if(i++ > 0) printf(",");
	printf("\"%c\"", vt);
}
printf("], \"Body\": [");
i = 0;
for(auto prod: G) {
	if(i++ > 0) printf(",");
	printf("{\"noTerminal\": \"%c\", \"production\": [", prod.noTerminal);
	int j = 0;
	for(auto vt: VT) {
		if(j++ > 0) printf(",");
		string entry = M[{prod.noTerminal, vt}];
		printf("\"%s\"", entry.empty() ? "" : entry.c_str());
	}
	printf("]}");
}
printf("]}");
}

void LL1::run() {
puts("{");
string in;

while(cin >> in && in != "#")
	addProd(Prod(in));

cin >> in;
if(in.empty() || in == "#") return;

loadIndata(in);
build();
info();
tableInfo();
parser();
puts("}");


}

int main() {
LL1 parser;
parser.run();
return 0;
}
