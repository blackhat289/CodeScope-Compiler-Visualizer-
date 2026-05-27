#include "LR.h"

int main() {
LR parser;


string input;

while(cin >> input && input != "#")
	parser.add(input);

cin >> input;
if(input.empty() || input == "#") return -1;

parser.loadStr(input);
parser.build();
parser.generateDot();

return 0;


}
