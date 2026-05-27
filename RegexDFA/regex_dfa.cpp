#include "regex_dfa.h"

// ==================== Regex Processing ====================

int RegexDFA::precedence(char c) {
    switch(c) {
        case '*': case '+': case '?': return 3;
        case '.': return 2; // concat
        case '|': return 1;
        default: return 0;
    }
}

bool RegexDFA::isOperator(char c) {
    return c == '*' || c == '+' || c == '?' || c == '.' || c == '|';
}

string RegexDFA::insertExplicitConcat(const string& r) {
    string result;
    for (unsigned i = 0; i < r.length(); i++) {
        char c = r[i];
        result += c;
        if (i + 1 < r.length()) {
            char next = r[i + 1];
            // Insert '.' between: (a)(b), (a*)(b), (a)((), (*)(a), (?)(a), (+)(a), (a)not-operator
            if (c != '(' && c != '|' && next != ')' && next != '|' &&
                next != '*' && next != '+' && next != '?') {
                result += '.';
            }
        }
    }
    return result;
}

string RegexDFA::infixToPostfix(const string& r) {
    string postfix;
    stack<char> ops;
    
    for (char c : r) {
        if (c == '(') {
            ops.push(c);
        } else if (c == ')') {
            while (!ops.empty() && ops.top() != '(') {
                postfix += ops.top();
                ops.pop();
            }
            if (!ops.empty()) ops.pop(); // remove '('
        } else if (isOperator(c)) {
            while (!ops.empty() && ops.top() != '(' && precedence(ops.top()) >= precedence(c)) {
                postfix += ops.top();
                ops.pop();
            }
            ops.push(c);
        } else {
            postfix += c;
            alphabet.insert(c);
        }
    }
    while (!ops.empty()) {
        postfix += ops.top();
        ops.pop();
    }
    return postfix;
}

// ==================== Thompson's Construction ====================

NFA RegexDFA::createBasic(char symbol) {
    NFA n;
    n.numStates = 2;
    n.start = stateCount++;
    n.accept = stateCount++;
    n.transitions.push_back({n.start, n.accept, symbol});
    return n;
}

NFA RegexDFA::createUnion(NFA a, NFA b) {
    NFA n;
    n.start = stateCount++;
    n.accept = stateCount++;
    n.numStates = a.numStates + b.numStates + 2;
    
    // New start -> old starts (epsilon)
    n.transitions.push_back({n.start, a.start, EPS});
    n.transitions.push_back({n.start, b.start, EPS});
    
    // Old accepts -> new accept (epsilon)
    n.transitions.push_back({a.accept, n.accept, EPS});
    n.transitions.push_back({b.accept, n.accept, EPS});
    
    // Copy transitions
    n.transitions.insert(n.transitions.end(), a.transitions.begin(), a.transitions.end());
    n.transitions.insert(n.transitions.end(), b.transitions.begin(), b.transitions.end());
    
    return n;
}

NFA RegexDFA::createConcat(NFA a, NFA b) {
    NFA n;
    n.start = a.start;
    n.accept = b.accept;
    n.numStates = a.numStates + b.numStates;
    
    // Connect a's accept to b's start (epsilon)
    n.transitions.push_back({a.accept, b.start, EPS});
    
    // Copy transitions
    n.transitions.insert(n.transitions.end(), a.transitions.begin(), a.transitions.end());
    n.transitions.insert(n.transitions.end(), b.transitions.begin(), b.transitions.end());
    
    return n;
}

NFA RegexDFA::createStar(NFA a) {
    NFA n;
    n.start = stateCount++;
    n.accept = stateCount++;
    n.numStates = a.numStates + 2;
    
    n.transitions.push_back({n.start, a.start, EPS});
    n.transitions.push_back({n.start, n.accept, EPS});     // skip (zero)
    n.transitions.push_back({a.accept, a.start, EPS});      // loop
    n.transitions.push_back({a.accept, n.accept, EPS});     // exit
    
    n.transitions.insert(n.transitions.end(), a.transitions.begin(), a.transitions.end());
    return n;
}

NFA RegexDFA::createPlus(NFA a) {
    NFA n;
    n.start = stateCount++;
    n.accept = stateCount++;
    n.numStates = a.numStates + 2;
    
    n.transitions.push_back({n.start, a.start, EPS});
    n.transitions.push_back({a.accept, a.start, EPS});      // loop
    n.transitions.push_back({a.accept, n.accept, EPS});     // exit
    
    n.transitions.insert(n.transitions.end(), a.transitions.begin(), a.transitions.end());
    return n;
}

NFA RegexDFA::createOptional(NFA a) {
    NFA n;
    n.start = stateCount++;
    n.accept = stateCount++;
    n.numStates = a.numStates + 2;
    
    n.transitions.push_back({n.start, a.start, EPS});
    n.transitions.push_back({n.start, n.accept, EPS});     // skip
    n.transitions.push_back({a.accept, n.accept, EPS});     // exit
    
    n.transitions.insert(n.transitions.end(), a.transitions.begin(), a.transitions.end());
    return n;
}

void RegexDFA::buildNFA() {
    stateCount = 0;
    processedRegex = insertExplicitConcat(regex);
    string postfix = infixToPostfix(processedRegex);
    
    stack<NFA> nfaStack;
    
    for (char c : postfix) {
        if (c == '.') {
            NFA b = nfaStack.top(); nfaStack.pop();
            NFA a = nfaStack.top(); nfaStack.pop();
            nfaStack.push(createConcat(a, b));
        } else if (c == '|') {
            NFA b = nfaStack.top(); nfaStack.pop();
            NFA a = nfaStack.top(); nfaStack.pop();
            nfaStack.push(createUnion(a, b));
        } else if (c == '*') {
            NFA a = nfaStack.top(); nfaStack.pop();
            nfaStack.push(createStar(a));
        } else if (c == '+') {
            NFA a = nfaStack.top(); nfaStack.pop();
            nfaStack.push(createPlus(a));
        } else if (c == '?') {
            NFA a = nfaStack.top(); nfaStack.pop();
            nfaStack.push(createOptional(a));
        } else {
            nfaStack.push(createBasic(c));
        }
    }
    
    if (!nfaStack.empty()) {
        nfa = nfaStack.top();
    }
}

// ==================== Subset Construction ====================

set<int> RegexDFA::epsilonClosure(const set<int>& states) {
    set<int> closure = states;
    queue<int> worklist;
    for (int s : states) worklist.push(s);
    
    while (!worklist.empty()) {
        int s = worklist.front(); worklist.pop();
        for (const auto& t : nfa.transitions) {
            if (t.from == s && t.symbol == EPS && closure.find(t.to) == closure.end()) {
                closure.insert(t.to);
                worklist.push(t.to);
            }
        }
    }
    return closure;
}

set<int> RegexDFA::move(const set<int>& states, char symbol) {
    set<int> result;
    for (const auto& t : nfa.transitions) {
        if (states.count(t.from) && t.symbol == symbol) {
            result.insert(t.to);
        }
    }
    return result;
}

string RegexDFA::stateSetToString(const set<int>& states) {
    string s = "{";
    bool first = true;
    for (int st : states) {
        if (!first) s += ",";
        s += to_string(st);
        first = false;
    }
    s += "}";
    return s;
}

void RegexDFA::buildDFA() {
    dfa.alphabet = alphabet;
    
    // Start state = epsilon-closure({nfa.start})
    set<int> startSet = epsilonClosure({nfa.start});
    
    DFAState startState;
    startState.nfaStates = startSet;
    startState.isAccept = startSet.count(nfa.accept) > 0;
    startState.label = "q0";
    
    dfa.states.push_back(startState);
    dfa.start = 0;
    if (startState.isAccept) dfa.acceptStates.insert(0);
    
    queue<int> worklist;
    worklist.push(0);
    map<set<int>, int> stateMap;
    stateMap[startSet] = 0;
    
    int nextId = 1;
    
    while (!worklist.empty()) {
        int current = worklist.front(); worklist.pop();
        
        for (char c : alphabet) {
            set<int> moveResult = move(dfa.states[current].nfaStates, c);
            if (moveResult.empty()) continue;
            
            set<int> newSet = epsilonClosure(moveResult);
            if (newSet.empty()) continue;
            
            if (stateMap.find(newSet) == stateMap.end()) {
                DFAState newState;
                newState.nfaStates = newSet;
                newState.isAccept = newSet.count(nfa.accept) > 0;
                newState.label = "q" + to_string(nextId);
                
                stateMap[newSet] = nextId;
                dfa.states.push_back(newState);
                if (newState.isAccept) dfa.acceptStates.insert(nextId);
                worklist.push(nextId);
                nextId++;
            }
            
            dfa.transitions[{current, c}] = stateMap[newSet];
        }
    }
}

// ==================== DFA Minimization (Hopcroft's) ====================

void RegexDFA::minimizeDFA() {
    minDfa.alphabet = alphabet;
    
    if (dfa.states.empty()) return;
    
    int n = dfa.states.size();
    
    // Initial partition: accept vs non-accept
    set<int> acceptSet, nonAcceptSet;
    for (int i = 0; i < n; i++) {
        if (dfa.acceptStates.count(i))
            acceptSet.insert(i);
        else
            nonAcceptSet.insert(i);
    }
    
    vector<set<int>> partitions;
    if (!nonAcceptSet.empty()) partitions.push_back(nonAcceptSet);
    if (!acceptSet.empty()) partitions.push_back(acceptSet);
    
    // Refine partitions
    bool changed = true;
    while (changed) {
        changed = false;
        vector<set<int>> newPartitions;
        
        for (const auto& group : partitions) {
            if (group.size() <= 1) {
                newPartitions.push_back(group);
                continue;
            }
            
            // Try to split this group
            map<vector<int>, set<int>> signatureMap;
            
            for (int state : group) {
                vector<int> signature;
                for (char c : alphabet) {
                    auto it = dfa.transitions.find({state, c});
                    if (it == dfa.transitions.end()) {
                        signature.push_back(-1);
                    } else {
                        // Find which partition the target belongs to
                        int targetPartition = -1;
                        for (unsigned p = 0; p < partitions.size(); p++) {
                            if (partitions[p].count(it->second)) {
                                targetPartition = p;
                                break;
                            }
                        }
                        signature.push_back(targetPartition);
                    }
                }
                signatureMap[signature].insert(state);
            }
            
            for (const auto& entry : signatureMap) {
                newPartitions.push_back(entry.second);
            }
            
            if (signatureMap.size() > 1) changed = true;
        }
        
        partitions = newPartitions;
    }
    
    // Build minimized DFA from partitions
    map<int, int> stateToPartition;
    for (unsigned i = 0; i < partitions.size(); i++) {
        for (int state : partitions[i]) {
            stateToPartition[state] = i;
        }
    }
    
    for (unsigned i = 0; i < partitions.size(); i++) {
        DFAState s;
        s.label = "m" + to_string(i);
        s.isAccept = false;
        for (int state : partitions[i]) {
            s.nfaStates.insert(state);
            if (dfa.acceptStates.count(state)) s.isAccept = true;
        }
        minDfa.states.push_back(s);
        if (s.isAccept) minDfa.acceptStates.insert(i);
    }
    
    minDfa.start = stateToPartition[dfa.start];
    
    // Build transitions
    for (unsigned i = 0; i < partitions.size(); i++) {
        int representative = *partitions[i].begin();
        for (char c : alphabet) {
            auto it = dfa.transitions.find({representative, c});
            if (it != dfa.transitions.end()) {
                int targetPartition = stateToPartition[it->second];
                minDfa.transitions[{(int)i, c}] = targetPartition;
            }
        }
    }
}

// ==================== String Matching ====================

void RegexDFA::matchString() {
    matchSteps.clear();
    
    const DFA& d = minDfa.states.empty() ? dfa : minDfa;
    
    int current = d.start;
    string remaining = testStr;
    matchResult = false;
    
    for (unsigned i = 0; i < testStr.length(); i++) {
        char c = testStr[i];
        MatchStep step;
        step.state = d.states[current].label;
        step.remaining = remaining;
        step.symbol = c;
        
        auto it = d.transitions.find({current, c});
        if (it == d.transitions.end()) {
            step.nextState = "DEAD";
            matchSteps.push_back(step);
            matchResult = false;
            return;
        }
        
        current = it->second;
        step.nextState = d.states[current].label;
        matchSteps.push_back(step);
        remaining = remaining.substr(1);
    }
    
    matchResult = d.acceptStates.count(current) > 0;
}

// ==================== DOT Output ====================

string RegexDFA::nfaDot() {
    ostringstream dot;
    dot << "digraph nfa {rankdir=LR;node[shape=circle style=filled fillcolor=\\\"#e8eaf6\\\"];";
    
    // Accept state is double circle
    dot << "node[shape=doublecircle fillcolor=\\\"#c8e6c9\\\"];" << nfa.accept << ";";
    dot << "node[shape=circle fillcolor=\\\"#e8eaf6\\\"];";
    
    // Invisible start arrow
    dot << "start[shape=point];start->" << nfa.start << ";";
    
    for (const auto& t : nfa.transitions) {
        dot << t.from << "->" << t.to << "[label=\\\"";
        if (t.symbol == EPS) dot << "\xce\xb5"; // ε epsilon symbol
        else dot << t.symbol;
        dot << "\\\"";
        if (t.symbol == EPS) dot << " style=dashed color=\\\"#9e9e9e\\\"";
        dot << "];";
    }
    dot << "}";
    return dot.str();
}

string RegexDFA::dfaDot(const DFA& d, const string& name) {
    ostringstream dot;
    dot << "digraph " << name << "{rankdir=LR;node[shape=circle style=filled fillcolor=\\\"#e3f2fd\\\"];";
    
    // Accept states
    dot << "node[shape=doublecircle fillcolor=\\\"#c8e6c9\\\"];";
    for (int s : d.acceptStates) dot << d.states[s].label << ";";
    dot << "node[shape=circle fillcolor=\\\"#e3f2fd\\\"];";
    
    // Start arrow
    dot << "start[shape=point];start->" << d.states[d.start].label << ";";
    
    // Merge transitions with same source and dest
    map<pair<string,string>, vector<char>> edgeLabels;
    for (const auto& t : d.transitions) {
        string from = d.states[t.first.first].label;
        string to = d.states[t.second].label;
        edgeLabels[{from, to}].push_back(t.first.second);
    }
    
    for (const auto& e : edgeLabels) {
        dot << e.first.first << "->" << e.first.second << "[label=\\\"";
        for (unsigned i = 0; i < e.second.size(); i++) {
            if (i > 0) dot << ",";
            dot << e.second[i];
        }
        dot << "\\\"];";
    }
    
    dot << "}";
    return dot.str();
}

// ==================== JSON Output ====================

void RegexDFA::outputJSON() {
    // Escape regex for JSON
    string escapedRegex;
    for (char c : regex) {
        if (c == '"' || c == '\\') escapedRegex += '\\';
        escapedRegex += c;
    }
    
    printf("{");
    
    // Regex
    printf("\"regex\": \"%s\"", escapedRegex.c_str());
    
    // NFA
    printf(", \"nfa\": {");
    printf("\"states\": %d", stateCount);
    printf(", \"start\": %d", nfa.start);
    printf(", \"accept\": %d", nfa.accept);
    printf(", \"transitions\": [");
    for (unsigned i = 0; i < nfa.transitions.size(); i++) {
        if (i > 0) printf(",");
        const auto& t = nfa.transitions[i];
        printf("{\"from\":%d,\"to\":%d,\"symbol\":\"", t.from, t.to);
        if (t.symbol == EPS) printf("ε");
        else {
            if (t.symbol == '"' || t.symbol == '\\') printf("\\");
            printf("%c", t.symbol);
        }
        printf("\"}");
    }
    printf("]");
    printf(", \"dot\": \"%s\"", nfaDot().c_str());
    printf("}");
    
    // DFA
    printf(", \"dfa\": {");
    printf("\"states\": [");
    for (unsigned i = 0; i < dfa.states.size(); i++) {
        if (i > 0) printf(",");
        printf("{\"label\":\"%s\",\"nfaStates\":\"%s\",\"isAccept\":%s}",
            dfa.states[i].label.c_str(),
            stateSetToString(dfa.states[i].nfaStates).c_str(),
            dfa.states[i].isAccept ? "true" : "false");
    }
    printf("]");
    printf(", \"start\": \"%s\"", dfa.states[dfa.start].label.c_str());
    printf(", \"accept\": [");
    {
        bool first = true;
        for (int s : dfa.acceptStates) {
            if (!first) printf(",");
            printf("\"%s\"", dfa.states[s].label.c_str());
            first = false;
        }
    }
    printf("]");
    // Transition table
    printf(", \"table\": [");
    {
        vector<char> alphaVec(alphabet.begin(), alphabet.end());
        // Header
        printf("[\"State\"");
        for (char c : alphaVec) printf(",\"%c\"", c);
        printf("]");
        // Rows
        for (unsigned i = 0; i < dfa.states.size(); i++) {
            printf(",[\"");
            if (dfa.acceptStates.count(i)) printf("*");
            printf("%s\"", dfa.states[i].label.c_str());
            for (char c : alphaVec) {
                auto it = dfa.transitions.find({(int)i, c});
                if (it != dfa.transitions.end())
                    printf(",\"%s\"", dfa.states[it->second].label.c_str());
                else
                    printf(",\"-\"");
            }
            printf("]");
        }
    }
    printf("]");
    printf(", \"dot\": \"%s\"", dfaDot(dfa, "dfa").c_str());
    printf("}");
    
    // Minimized DFA
    printf(", \"minDfa\": {");
    printf("\"states\": [");
    for (unsigned i = 0; i < minDfa.states.size(); i++) {
        if (i > 0) printf(",");
        printf("{\"label\":\"%s\",\"dfaStates\":\"%s\",\"isAccept\":%s}",
            minDfa.states[i].label.c_str(),
            stateSetToString(minDfa.states[i].nfaStates).c_str(),
            minDfa.states[i].isAccept ? "true" : "false");
    }
    printf("]");
    printf(", \"start\": \"%s\"", minDfa.states.empty() ? "" : minDfa.states[minDfa.start].label.c_str());
    printf(", \"accept\": [");
    {
        bool first = true;
        for (int s : minDfa.acceptStates) {
            if (!first) printf(",");
            printf("\"%s\"", minDfa.states[s].label.c_str());
            first = false;
        }
    }
    printf("]");
    // Transition table
    printf(", \"table\": [");
    {
        vector<char> alphaVec(alphabet.begin(), alphabet.end());
        printf("[\"State\"");
        for (char c : alphaVec) printf(",\"%c\"", c);
        printf("]");
        for (unsigned i = 0; i < minDfa.states.size(); i++) {
            printf(",[\"");
            if (minDfa.acceptStates.count(i)) printf("*");
            printf("%s\"", minDfa.states[i].label.c_str());
            for (char c : alphaVec) {
                auto it = minDfa.transitions.find({(int)i, c});
                if (it != minDfa.transitions.end())
                    printf(",\"%s\"", minDfa.states[it->second].label.c_str());
                else
                    printf(",\"-\"");
            }
            printf("]");
        }
    }
    printf("]");
    printf(", \"dot\": \"%s\"", dfaDot(minDfa, "mindfa").c_str());
    printf("}");
    
    // Match result
    printf(", \"match\": {");
    printf("\"result\": %s", matchResult ? "true" : "false");
    printf(", \"steps\": [");
    for (unsigned i = 0; i < matchSteps.size(); i++) {
        if (i > 0) printf(",");
        // Escape remaining string
        string escaped;
        for (char c : matchSteps[i].remaining) {
            if (c == '"' || c == '\\') escaped += '\\';
            escaped += c;
        }
        printf("{\"state\":\"%s\",\"remaining\":\"%s\",\"symbol\":\"%c\",\"nextState\":\"%s\"}",
            matchSteps[i].state.c_str(),
            escaped.c_str(),
            matchSteps[i].symbol,
            matchSteps[i].nextState.c_str());
    }
    printf("]");
    printf("}");
    
    printf("}\n");
}

// ==================== Main ====================

void RegexDFA::run() {
    // Read regex
    getline(cin, regex);
    
    // Read separator
    string sep;
    getline(cin, sep);
    
    // Read test string
    getline(cin, testStr);
    
    if (regex.empty()) return;
    
    buildNFA();
    buildDFA();
    minimizeDFA();
    
    if (!testStr.empty() && testStr != "#") {
        matchString();
    }
    
    outputJSON();
}

int main() {
    RegexDFA engine;
    engine.run();
    return 0;
}
