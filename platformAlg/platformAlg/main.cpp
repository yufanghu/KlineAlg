#include "StdAfx.h"
#include <fstream>
#include <iostream>
#include <stdio.h>     
#include <stdlib.h>   
#include "PlatFormAlgorithm.h"


void ReadKlineData(const char* path, std::vector<tagKline> & klineVector) {
	std::ifstream fin(path, std::ios::in);
	char line[512] = { 0 };
	while (fin.getline(line, sizeof(line))) {
		const char* token = ",";
		char * nest_pos;
		char* pos = strtok_s(line, token, &nest_pos);
		tagKline temp;
		int index = 0;
		while (pos != NULL) {
			//printf("%s,", pos);
			if (0 == index)
				temp.time = _atoi64(pos);
			else if (1 == index)
				temp.open = atof(pos);
			else if (2 == index)
				temp.high = atof(pos);
			else if (3 == index)
				temp.low = atof(pos);
			else if(4 == index)
				temp.close = atof(pos);
			pos = strtok_s(NULL, token, &nest_pos);
			index++;
		}
		//printf("\n", pos);
		klineVector.push_back(temp);
	}
	//printf("��ȡ%d������,�� �� �� ��\n", klineVector.size());
}

int main(int argc, char* argv)
{
	std::vector<tagKline>  klineVector;
	ReadKlineData("data.txt", klineVector);

	std::map<tagStockCodeInfo, std::vector<tagKline>>  input;
	tagStockCodeInfo tagOne((short)111, std::string("601881"));

	input[tagOne] = klineVector;
	std::map<tagStockCodeInfo, tagOutput>  output;


	CPlatFormAlgorithm plat;
	plat.select_entrance(input, output, eSinglePlatForm,20,false);

	system("pause");
	return 0;
}