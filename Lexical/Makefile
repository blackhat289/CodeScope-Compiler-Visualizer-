CXX = g++
CXXFLAGS = -O3 -std=c++11

SRC = lexical.cpp
HEADERS = lexical.h
OUT = lexer

all: $(OUT)

$(OUT): $(SRC) $(HEADERS)
	$(CXX) $(CXXFLAGS) $(SRC) -o $(OUT)
	strip $(OUT)

clean:
	rm -f $(OUT)
