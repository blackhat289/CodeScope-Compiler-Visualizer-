#ifndef _REGEX_DFA_H
#define _REGEX_DFA_H

#include <iostream>
#include <vector>
#include <string>
#include <set>
#include <map>
#include <queue>
#include <stack>
#include <algorithm>
#include <sstream>
using namespace std;

// Epsilon symbol
#define EPS '\0'

struct Transition {
    int from;
    int to;
    char symbol; // EPS for epsilon
};

struct NFA {
    int numStates;
    int start;
    int accept;
    vector<Transition> transitions;
};

struct DFAState {
    set<int> nfaStates;
    bool isAccept;
    string label;
};

struct DFA {
    vector<DFAState> states;
    int start;
    set<int> acceptStates;
    map<pair<int,char>, int> transitions;
    set<char> alphabet;
};

struct MatchStep {
    string state;
    string remaining;
    char symbol;
    string nextState;
};

class RegexDFA {
private:
    string regex;
    string testStr;
    string processedRegex; // with explicit concat operators

    // NFA construction
    int stateCount;
    NFA nfa;
    
    // DFA
    DFA dfa;
    DFA minDfa;
    
    // Matching
    bool matchResult;
    vector<MatchStep> matchSteps;
    
    set<char> alphabet;

    // Regex processing
    string insertExplicitConcat(const string& r);
    string infixToPostfix(const string& r);
    int precedence(char c);
    bool isOperator(char c);
    
    // Thompson's Construction
    NFA createBasic(char symbol);
    NFA createUnion(NFA a, NFA b);
    NFA createConcat(NFA a, NFA b);
    NFA createStar(NFA a);
    NFA createPlus(NFA a);
    NFA createOptional(NFA a);
    void buildNFA();
    
    // Subset Construction
    set<int> epsilonClosure(const set<int>& states);
    set<int> move(const set<int>& states, char symbol);
    void buildDFA();
    
    // Minimization
    void minimizeDFA();
    
    // String matching
    void matchString();
    
    // Output helpers
    string stateSetToString(const set<int>& states);
    string nfaDot();
    string dfaDot(const DFA& d, const string& name);
    void outputJSON();

public:
    void run();
};

#endif
