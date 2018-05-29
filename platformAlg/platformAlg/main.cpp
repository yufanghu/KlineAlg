#include "StdAfx.h"
#include <fstream>
#include <iostream>
#include <stdio.h>     
#include <stdlib.h>   
#include "PlatFormAlgorithm.h"
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
	CPlatFormAlgorithm plat;
	{
		std::map<tagStockCodeInfo, std::vector<tagKline>>  input;
		ReadKlineData("1526900149_0_log_data.txt", input);

		std::map<tagStockCodeInfo, tagOutput>  output;
		
		plat.select_entrance(input, output, eDoublePlatForm,15,false);
	}



	//system("pause");
	return 0;
}