#include <iostream>
#include <stdio.h>
#include <stdlib.h>
using namespace std;
//byte[0]=ab;

char * strcpy(char * strDest, const char * strSrc)

{
	if ((NULL == strDest) || (NULL == strSrc))
//[1]
		throw "Invalid argument(s)";
//[2]
	char * strDestCopy = strDest;
//[3]
	while ((*strDest++ = *strSrc++) != '\0');
//[4]
	return strDestCopy;
}

void transformT2(int bits, int buff[7]) { //十进制转为二进制


	buff[0] = (bits & 0x01) == 0x01 ? 1 : 0;
	buff[1] = (bits & 0x02) == 0x02 ? 1 : 0;
	buff[2] = (bits & 0x04) == 0x04 ? 1 : 0;
	buff[3] = (bits & 0x08) == 0x08 ? 1 : 0;
	buff[4] = (bits & 0x10) == 0x10 ? 1 : 0;
	buff[5] = (bits & 0x20) == 0x20 ? 1 : 0;
	buff[6] = (bits & 0x40) == 0x40 ? 1 : 0;
	buff[7] = (bits & 0x80) == 0x80 ? 1 : 0;

}

int main() {
	int byte = 0xab;
	int buff7[7];


	cout << byte << endl;
	transformT2(byte, buff7);
	for (int i = 0; i < 8; i++) {
		cout << buff7[7-i] << endl;
	};
	return 0;

}