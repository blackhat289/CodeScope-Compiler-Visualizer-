CXX = g++
CXXFLAGS = -O3 -std=c++11

SRC = regex_dfa.cpp
HEADERS = regex_dfa.h
OUT = regex_dfa

all: $(OUT)

$(OUT): $(SRC) $(HEADERS)
	$(CXX) $(CXXFLAGS) $(SRC) -o $(OUT)

clean:
	rm -f $(OUT)
