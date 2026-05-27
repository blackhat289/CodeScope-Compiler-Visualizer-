CXX = g++
CXXFLAGS = -O3 -std=c++11

SRC_MAIN = LR.cpp
SRC_DFA = LR_DFA.cpp

OUT_MAIN = lr_parser
OUT_DFA = lr_dfa_generator

all: $(OUT_MAIN) $(OUT_DFA)

$(OUT_MAIN): $(SRC_MAIN)
	$(CXX) $(CXXFLAGS) $< -o $@

$(OUT_DFA): $(SRC_DFA) $(SRC_MAIN)
	$(CXX) $(CXXFLAGS) -D_GENERATE_DOT_ $^ -o $@

clean:
	rm -f $(OUT_MAIN) $(OUT_DFA)

test: $(OUT_MAIN) $(OUT_DFA)
	for file in Test/*.in; do 
		./$(OUT_MAIN) < $$file > "$${file%.in}.out"; 
		./$(OUT_DFA) < $$file | dot -Tpdf -o "$${file%.in}.pdf"; 
	done
