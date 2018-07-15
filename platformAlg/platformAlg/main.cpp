#include "StdAfx.h"
#include <fstream>
#include <iostream>
#include <stdio.h>     
#include <stdlib.h>   
#include "StockAlgorithm.h"
#include "Log.h"


void ReadKlineData(const char* path, std::map<tagStockCodeInfo, std::vector<tagKline>> & tempMap) {
	std::ifstream fin(path, std::ios::in);
	char line[512] = { 0 };
	tagStockCodeInfo stock(0, "");
	std::vector<tagKline> tempVector;
	while (fin.getline(line, sizeof(line))) {
		char * pp = strchr(line, '-');
		if(pp){
			if(!tempVector.empty())
			{
				tempMap[stock] = tempVector;
			}
			stock.stockcode = pp+1;
			char buf[20] = {0};
			memcpy(buf, line, pp - line);
			stock.market = atoi(buf);

			tempVector.shrink_to_fit();
			tempVector.clear();
		}
		else{
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
			
			tempVector.push_back(temp);
		}
		
	}
	tempMap[stock] = tempVector;
	//printf("读取%d行数据,开 高 低 收\n", klineVector.size());
}


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
	//printf("读取%d行数据,开 高 低 收\n", klineVector.size());
}

int main(int argc, char* argv)
{
	EnableAlgLog(true);
	std::map<tagStockCodeInfo, std::vector<tagKline>>  input;
	ReadKlineData("D:\\data1.txt", input);

	std::map<tagStockCodeInfo, tagOutput>  output;

	alg_platform(input, output, eSinglePlatForm, 10, false);

	TFirstFilter filter;
	filter.sA3Switch = eAB;
	filter.bA4Switch = true;
	filter.bA5Switch = true;
	filter.bA6Switch = true;
	filter.sCallbackRange = 90;
	filter.sRbcoe = 10;
	filter.tLineNum.iMaxka = 20;
	filter.tLineNum.iMinka = 6;
	filter.tLineNum.iMinkb = 1;
	filter.tLineNum.iMaxkb = 12;

	alg_stock2(input, output, filter);

	TThirdFilter second;
	second.tLineNum.iMaxka = 20;
	second.tLineNum.iMinka = 6;
	second.tLineNum.iMinkb = 1;
	second.tLineNum.iMaxkb = 12;
	second.sCallbackRange = 80;
	second.bA5Switch = false;

	alg_stock2(input, output, second);



	system("pause");
	return 0;
}