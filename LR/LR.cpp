#include "LR.h"

set<char> Item::Vn;
set<char> Item::Vt;
set<char> Item::Symbol;

const char* LR::actionStatStr[] = {
"acc","s","r"
};

string Prod::displayStr() const {
string p = string(1, noTerminal) + "->" + right;
int i = 0;
for(const auto& c:additionalVt)
if(c != '#') p += string(1, i++==0?',':'|') + c;
if(additionalVt.find('#') != additionalVt.end())
p += string(1, i++==0?',':'|') + "#";
return p;
}

string Prod::replaceAll(const string &in, const string from, const string to) {
	string result = in;
	size_t pos = 0;
	while((pos = result.find(from, pos)) != string::npos) {
		result.replace(pos, from.length(), to);
		pos += to.length();
	}
	return result;
}

Prod::Prod(const string &in) {
noTerminal = in[0];
right = cut(in, 3, in.length());
}

void Item::add(const string &prod) {
if(prod.length() < 4) return;


char noTerminal;
if(Prod::cut(prod,1,3)=="->" && isupper(prod[0]))
	noTerminal = prod[0];
else return;

for(unsigned int i=0;i<prod.length();++i){
	char c = prod[i];
	if(isupper(c)){
		Vn.insert(c);
		Symbol.insert(c);
	}else if(c!='|' && !(c=='-' && prod[i+1]=='>' && ++i)){
		Vt.insert(c);
		Symbol.insert(c);
	}
}

for(unsigned int i=3;i<prod.length();++i){
	unsigned int j;
	for(j=i+1;j<prod.length() && prod[j]!='|';++j);
	Prod p = Prod(string(1,noTerminal)+"->"+Prod::cut(prod,i,j));
	if(find(prods.begin(),prods.end(),p)==prods.end())
		prods.push_back(p);
	i=j;
}


}

void LR::add(const string &s){
G.add(s);
}

void LR::loadStr(const string &in){
inStr.push_back('#');
status.push_back(0);
for(int i=in.length()-1;i>=0;--i)
inStr.push_back(in[i]);
}

void LR::parser(){
printf("\"parser\": [");


bool success=false;
int step=0;

while(!success){
	printf("%s{",step==0?"\n":"\n, ");
	printf("\"statusStack\": \"");

	for(auto s:status) {
		if(s < 10) printf(" %d ", s);
		else printf(" <span class='underline'>%d</span> ", s);
	}

	printf("\", \"parseStack\": \"");
	for(auto c:parse) printf("%c",c);

	printf("\", \"inStrStack\": \"");
	for(auto it=inStr.rbegin();it!=inStr.rend();++it) printf("%c",*it);

	printf("\", \"action\": ");

	int sTop=status.size()-1;
	int iTop=inStr.size()-1;

	auto key=make_pair(status[sTop],inStr[iTop]);

	if(ACTION.find(key)==ACTION.end()) break;

	auto act=ACTION[key];

	if(act.first==SHIFT){
		printf("\"SHIFT\"}");
		status.push_back(act.second);
		parse.push_back(inStr[iTop]);
		inStr.pop_back();
	}
	else if(act.first==REDUCE){
		Prod p=G.prods[act.second];
		printf("\"REDUCE %c->%s\"}",p.noTerminal,p.right.c_str());

		if(p.right!="@")
			for(unsigned i=0;i<p.right.size();++i){
				status.pop_back();
				parse.pop_back();
			}

		parse.push_back(p.noTerminal);
		status.push_back(GOTO[{status.back(),p.noTerminal}]);
	}
	else if(act.first==ACCEPT){
		success=true;
		printf("\"ACCEPT\"}");
	}

	++step;
}

if(!success)
	printf("\"ERROR\"}\n");

printf("]\n");


}

void LR::build(){
items();


for(unsigned int i=0;i<C.size();++i){
	const Item &item=C[i];

	for(const auto& prod:item.prods){
		unsigned long pos=prod.right.find('.');

		// Check if this is a complete item (dot at end, or dot before @)
		bool isComplete = (pos >= prod.right.length()-1) ||
			(prod.right.length() == 2 && prod.right == ".@");

		if(!isComplete){
			char X=prod.right[pos+1];
			if(G.Vt.count(X) && GOTO.count({i,X}))
				ACTION[{i,X}]={SHIFT,GOTO[{i,X}]};
		}
		else{
			if(prod.noTerminal!=EXTENSION_NOTERMINAL){
				string right=prod.right;
				right.erase(right.find('.'),1);
				// For epsilon items (.@), right becomes "@"

				for(const auto& X:prod.additionalVt){
					auto it=find(G.prods.begin(),G.prods.end(),
						Prod(prod.noTerminal,right,set<char>{}));
					if(it!=G.prods.end())
						ACTION[{i,X}]={REDUCE,(int)(it-G.prods.begin())};
				}
			}
			else{
				ACTION[{i,'#'}]={ACCEPT,0};
			}
		}
	}
}

if(G.Vt.count('@')){
	G.Vt.erase('@');
	G.Symbol.erase('@');
}

G.Vt.insert('#');
G.Symbol.insert('#');


}

void LR::run(){
string in;


while(cin>>in && in!="#")
	add(in);

printf("{");
showGrammar();
printf(",");

build();
showTable();
printf(",");

drawGraph();
printf(",\n");

cin>>in;
if(in.empty() || in=="#") return;

loadStr(in);
parser();

printf("}\n");


}

set<char> LR::first(const string &s) {
	set<char> result;
	if(s.empty() || s == "@") {
		result.insert('@');
		return result;
	}
	for(unsigned int i = 0; i < s.length(); ++i) {
		char c = s[i];
		if(G.Vt.count(c) || c == '#' || c == '@') {
			if(c != '@') result.insert(c);
			if(c == '@') {
				if(i == s.length() - 1) result.insert('@');
				continue;
			}
			break;
		}
		if(G.Vn.count(c)) {
			if(FIRST.find(c) == FIRST.end()) {
				// compute FIRST(c)
				for(const auto& p : G.prods) {
					if(p.noTerminal == c) {
						set<char> f = first(p.right);
						FIRST[c].insert(f.begin(), f.end());
					}
				}
			}
			bool hasEpsilon = false;
			for(char fc : FIRST[c]) {
				if(fc == '@') hasEpsilon = true;
				else result.insert(fc);
			}
			if(!hasEpsilon) break;
			if(i == s.length() - 1) result.insert('@');
		}
	}
	return result;
}

Item LR::closure(Item I) {
	bool changed = true;
	while(changed) {
		changed = false;
		vector<Prod> newProds;
		for(const auto& prod : I.prods) {
			unsigned long pos = prod.right.find('.');
			if(pos >= prod.right.length() - 1) continue;
			char B = prod.right[pos + 1];
			if(B == '@') continue; // epsilon - nothing after dot
			if(!G.Vn.count(B)) continue;

			// Compute lookahead: FIRST(beta a) where beta is after B, a is lookahead
			string beta = Prod::cut(prod.right, pos + 2, prod.right.length());

			for(const auto& gp : G.prods) {
				if(gp.noTerminal != B) continue;
				string newRight = "." + gp.right;

				// Compute lookahead
				set<char> lookahead;
				for(char la : prod.additionalVt) {
					set<char> f = first(beta + string(1, la));
					for(char fc : f) {
						if(fc != '@') lookahead.insert(fc);
					}
					if(f.count('@')) lookahead.insert(la);
				}

				// Check if this production already exists
				Prod np(B, newRight, lookahead);
				bool found = false;
				for(auto& ep : I.prods) {
					if(ep.noTerminal == np.noTerminal && ep.right == np.right) {
						// Merge lookaheads
						size_t oldSize = ep.additionalVt.size();
						ep.additionalVt.insert(lookahead.begin(), lookahead.end());
						if(ep.additionalVt.size() > oldSize) changed = true;
						found = true;
						break;
					}
				}
				if(!found) {
					for(auto& ep : newProds) {
						if(ep.noTerminal == np.noTerminal && ep.right == np.right) {
							size_t oldSize = ep.additionalVt.size();
							ep.additionalVt.insert(lookahead.begin(), lookahead.end());
							if(ep.additionalVt.size() > oldSize) changed = true;
							found = true;
							break;
						}
					}
				}
				if(!found) {
					newProds.push_back(np);
					changed = true;
				}
			}
		}
		for(auto& np : newProds)
			I.prods.push_back(np);
	}
	return I;
}

Item LR::Goto(const Item& I, char X) {
	Item J;
	for(const auto& prod : I.prods) {
		unsigned long pos = prod.right.find('.');
		if(pos >= prod.right.length() - 1) continue;
		if(prod.right[pos + 1] == '@') continue; // epsilon
		if(prod.right[pos + 1] != X) continue;
		string newRight = Prod::cut(prod.right, 0, pos) +
						  string(1, X) + "." +
						  Prod::cut(prod.right, pos + 2, prod.right.length());
		Prod np(prod.noTerminal, newRight, prod.additionalVt);
		J.prods.push_back(np);
	}
	return closure(J);
}

void LR::items() {
	// Add augmented grammar: E' -> .E
	Item I0;
	char startSymbol = G.prods[0].noTerminal;
	string augRight = "." + string(1, startSymbol);
	set<char> startLookahead;
	startLookahead.insert('#');
	Prod augProd(EXTENSION_NOTERMINAL, augRight, startLookahead);
	I0.prods.push_back(augProd);
	I0 = closure(I0);
	C.push_back(I0);

	bool changed = true;
	while(changed) {
		changed = false;
		unsigned int cSize = C.size();
		for(unsigned int i = 0; i < cSize; ++i) {
			for(char X : G.Symbol) {
				Item gotoResult = Goto(C[i], X);
				if(gotoResult.prods.empty()) continue;

				// Check if this item set already exists
				bool found = false;
				for(unsigned int j = 0; j < C.size(); ++j) {
					if(C[j] == gotoResult) {
						GOTO[{i, X}] = j;
						found = true;
						break;
					}
				}
				if(!found) {
					C.push_back(gotoResult);
					GOTO[{i, X}] = C.size() - 1;
					changed = true;
				}
			}
		}
	}
}

void LR::showGrammar() {
	printf("\"Grammar\": [\n");
	for(unsigned int i = 0; i < G.prods.size(); ++i) {
		if(i == 0) printf(" ");
		else printf(", ");
		printf("\"%c->%s\"", G.prods[i].noTerminal, G.prods[i].right.c_str());
	}
	printf("\n]\n");
}

void LR::showTable() {
	// Collect Vt and Vn in sorted order
	vector<char> vtList(G.Vt.begin(), G.Vt.end());
	vector<char> vnList(G.Vn.begin(), G.Vn.end());

	printf("\"parseTable\": {\n");

	// Print Vt
	printf("\"Vt\": [");
	for(unsigned int i = 0; i < vtList.size(); ++i) {
		if(i > 0) printf(", ");
		printf("\"%c\"", vtList[i]);
	}
	printf("],\n");

	// Print Vn
	printf("\"Vn\": [");
	for(unsigned int i = 0; i < vnList.size(); ++i) {
		if(i > 0) printf(",");
		printf(" \"%c\"", vnList[i]);
	}
	printf("],\n");

	// Print Body
	printf("\"Body\": [\n");
	for(unsigned int i = 0; i < C.size(); ++i) {
		if(i == 0) printf(" ");
		else printf(", ");
		printf("[");

		// ACTION columns (Vt)
		for(unsigned int j = 0; j < vtList.size(); ++j) {
			if(j > 0) printf(", ");
			auto it = ACTION.find({i, vtList[j]});
			if(it != ACTION.end()) {
				if(it->second.first == ACCEPT)
					printf("\"acc\"");
				else
					printf("\"%s%d\"", actionStatStr[it->second.first], it->second.second);
			} else {
				printf("\"\"");
			}
		}

		// GOTO columns (Vn)
		for(unsigned int j = 0; j < vnList.size(); ++j) {
			printf(", ");
			auto it = GOTO.find({i, vnList[j]});
			if(it != GOTO.end()) {
				printf("%d", it->second);
			} else {
				printf("\"\"");
			}
		}

		printf("]\n");
	}
	printf("]}\n");
}

void LR::showStatusStack() {
	for(unsigned int i = 0; i < status.size(); ++i) {
		if(status[i] < 10)
			printf(" %d ", status[i]);
		else
			printf(" <span class='underline'>%d</span> ", status[i]);
	}
}

void LR::drawGraph() {
	// Build grammar label
	string grammarLabel = "Grammar\\n";
	for(const auto& p : G.prods)
		grammarLabel += string(1, p.noTerminal) + "->" + p.right + "\\n";

	// ALL graph (detailed, box nodes with labels)
	printf("\"Graph\": {\"All\": \"digraph all{node [shape=box style=filled];");
	printf("Grammar[style=rounded label=\\\"%s\\\" ];\\n", grammarLabel.c_str());

	for(unsigned int i = 0; i < C.size(); ++i) {
		printf("I%d[label=\\\"I%d\\n", i, i);
		for(const auto& prod : C[i].prods) {
			string dispRight = prod.right;
			// Remove dot for display
			string cleanRight = Prod::replaceAll(dispRight, ".", "");
			// Re-insert dot
			unsigned long dotPos = prod.right.find('.');
			string displayRight;
			if(dotPos == 0) displayRight = "." + cleanRight;
			else if(dotPos >= prod.right.length() - 1) displayRight = cleanRight + ".";
			else displayRight = Prod::cut(cleanRight, 0, dotPos) + "." + Prod::cut(cleanRight, dotPos, cleanRight.length());

			printf("%c->%s,", prod.noTerminal, displayRight.c_str());
			// Print lookaheads
			bool firstLA = true;
			for(char la : prod.additionalVt) {
				if(la == '#') continue;
				if(!firstLA) printf("|");
				printf("%c", la);
				firstLA = false;
			}
			if(prod.additionalVt.count('#')) {
				if(!firstLA) printf("|");
				printf("#");
			}
			printf("\\n");
		}
		printf("\\\" ];\\n");
	}

	// Print edges
	for(const auto& g : GOTO) {
		printf("I%d -> I%d[label=\\\"%c\\\"];", g.first.first, g.second, g.first.second);
	}
	printf("Grammar -> I0[style=invis];}\"");

	// SIMPLE graph (circle nodes with tooltips)
	printf("\n, \"Simple\": \"digraph simple {node [shape = circle style=filled];");
	printf("Grammar[shape=box style=rounded label=\\\"%s\\\" ];\\n", grammarLabel.c_str());

	for(unsigned int i = 0; i < C.size(); ++i) {
		printf("I%d[tooltip=\\\"I%d\\n", i, i);
		for(const auto& prod : C[i].prods) {
			string dispRight = prod.right;
			string cleanRight = Prod::replaceAll(dispRight, ".", "");
			unsigned long dotPos = prod.right.find('.');
			string displayRight;
			if(dotPos == 0) displayRight = "." + cleanRight;
			else if(dotPos >= prod.right.length() - 1) displayRight = cleanRight + ".";
			else displayRight = Prod::cut(cleanRight, 0, dotPos) + "." + Prod::cut(cleanRight, dotPos, cleanRight.length());

			printf("%c->%s,", prod.noTerminal, displayRight.c_str());
			bool firstLA = true;
			for(char la : prod.additionalVt) {
				if(la == '#') continue;
				if(!firstLA) printf("|");
				printf("%c", la);
				firstLA = false;
			}
			if(prod.additionalVt.count('#')) {
				if(!firstLA) printf("|");
				printf("#");
			}
			printf("\\n");
		}
		printf("\\\" ];\\n");
	}

	for(const auto& g : GOTO) {
		printf("I%d -> I%d[label=\\\"%c\\\"];", g.first.first, g.second, g.first.second);
	}
	printf("Grammar -> I0[style=invis];}\"}\n");
}

void LR::generateDot() {
	build();

	string grammarLabel = "Grammar\\n";
	for(const auto& p : G.prods)
		grammarLabel += string(1, p.noTerminal) + "->" + p.right + "\\n";

	printf("digraph LR1 {\n");
	printf("node [shape=box style=filled];\n");
	printf("Grammar[style=rounded label=\"%s\" ];\n", grammarLabel.c_str());

	for(unsigned int i = 0; i < C.size(); ++i) {
		printf("I%d[label=\"I%d\\n", i, i);
		for(const auto& prod : C[i].prods) {
			string dispRight = prod.right;
			string cleanRight = Prod::replaceAll(dispRight, ".", "");
			unsigned long dotPos = prod.right.find('.');
			string displayRight;
			if(dotPos == 0) displayRight = "." + cleanRight;
			else if(dotPos >= prod.right.length() - 1) displayRight = cleanRight + ".";
			else displayRight = Prod::cut(cleanRight, 0, dotPos) + "." + Prod::cut(cleanRight, dotPos, cleanRight.length());

			printf("%c->%s,", prod.noTerminal, displayRight.c_str());
			bool firstLA = true;
			for(char la : prod.additionalVt) {
				if(la == '#') continue;
				if(!firstLA) printf("|");
				printf("%c", la);
				firstLA = false;
			}
			if(prod.additionalVt.count('#')) {
				if(!firstLA) printf("|");
				printf("#");
			}
			printf("\\n");
		}
		printf("\" ];\n");
	}

	for(const auto& g : GOTO) {
		printf("I%d -> I%d[label=\"%c\"];\n", g.first.first, g.second, g.first.second);
	}
	printf("Grammar -> I0[style=invis];\n");
	printf("}\n");
}

#ifndef _GENERATE_DOT_
int main(){
LR parser;
parser.run();
return 0;
}
#endif
