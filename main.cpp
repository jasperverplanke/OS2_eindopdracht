#include "ate.h"
#include <string>
#include <stdio.h>

int main() {
	std::string inFileName = "you_and_i.pcm";
	std::string outFileName = "i_and_you.pcm";
	ate myAte(5, 10, 10, inFileName, outFileName);
	//myAte.execute(6, 10, 10, filename, filename);
}