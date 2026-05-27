CXX = g++
CXXFLAGS = -O3 -std=c++11

SRC = LL1.cpp
HEADERS = LL1.h
OUT = grammar_parser

all: $(OUT)

$(OUT): $(SRC) $(HEADERS)
	$(CXX) $(CXXFLAGS) $(SRC) -o $(OUT)
	strip $(OUT)

clean:
	rm -f $(OUT)

test: $(OUT)
	for file in Test/*.in; do 
		$(OUT) < $$file > "$${file%.in}.out"; 
	done
