// stock_main.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <iostream>
#include <map>
#include <vector>
#include <algorithm>
#include <stdint.h>
#include <string>
#include <iomanip>  

#include "stock2.h"

using namespace std;
using namespace stock_second;

stock_second::DATAPP t[] = {
		{ 1505404800, 5.29, 5.75, 5.25, 5.41 },
		{ 1506009600, 5.42, 5.48, 5.11, 5.34 },
		{ 1506614400, 5.33, 5.48, 5.08, 5.48 },
		{ 1507824000, 5.55, 5.58, 4.81, 4.93 },
		{ 1508428800, 4.94, 5.05, 4.59, 4.81 },
		{ 1509033600, 4.82, 5.02, 4.76, 4.88 },
		{ 1509638400, 4.89, 4.91, 4.49, 4.57 },
		{ 1510243200, 4.55, 4.64, 4.41, 4.5 },
		{ 1510848000, 4.51, 4.72, 4.41, 4.43 },
		{ 1511452800, 4.41, 4.41, 4.2, 4.27 },
		{ 1512057600, 4.26, 4.66, 4.21, 4.5 },
		{ 1512662400, 4.49, 4.55, 4.28, 4.39 },
		{ 1513267200, 4.39, 4.48, 4.32, 4.38 },
		{ 1513872000, 4.38, 4.43, 4.23, 4.39 },
		{ 1514476800, 4.38, 4.43, 4.21, 4.34 },
		{ 1515081600, 4.35, 4.6, 4.32, 4.55 },
		{ 1515686400, 4.56, 4.9, 4.52, 4.55 },
		{ 1516291200, 4.71, 4.84, 4.49, 4.67 },
		{ 1516896000, 4.63, 4.79, 4.57, 4.78 },
		{ 1517500800, 4.79, 4.86, 4.27, 4.57 },
		{ 1518105600, 4.46, 4.96, 4.42, 4.46 },
		{ 1518537600, 4.45, 4.65, 4.45, 4.55 },
		{ 1519315200, 4.59, 5.18, 4.58, 5.16 },
		{ 1519920000, 5.26, 5.47, 5.14, 5.23 },
		{ 1520524800, 5.27, 5.31, 4.98, 5.08 },
		{ 1521129600, 5.09, 5.24, 4.76, 4.78 },
		{ 1521734400, 4.8, 4.84, 4.38, 4.44 },
		{ 1522339200, 4.4, 4.65, 4.28, 4.54 },
		{ 1522771200, 4.54, 4.57, 4.41, 4.49 },
		{ 1523289600, 4.47, 4.64, 4.44, 4.61 }
						};

stock_second::DATAPP t1[] = {
	{1518019200,15.500000,15.790000,15.100000,15.370000},
	{1518105600,14.900000,15.060000,14.000000,14.900000},
	{1518364800,14.900000,15.480000,14.830000,15.320000}
};





int _tmain(int argc, _TCHAR* argv[])
{
	/*std::map<tBaseInfo, std::vector<DATAPP>> inMap;

	tBaseInfo info1, info2;
	info1.sSetcode = 1;
	info1.sTockcode = "000001";

	info2.sSetcode = 1;
	info2.sTockcode = "000002";
	std::vector<DATAPP> vec;

	inMap.insert(std::make_pair(info1, vec));

	inMap.insert(std::make_pair(info2, vec));

	int size = inMap.size();
	if (size == 2){
		cout << size  << endl;
	}*/


	std::map<tBaseInfo, std::vector<DATAPP>> inMap;
	stock_second::tBaseInfo info;
	info.sSetcode  = 111;
	info.sTockcode = "aa";
	vector<DataT> vdata(t, t+30);
	//vdata.push_back(data);
	inMap.insert(make_pair(info, vdata));
	CStock stock; 

	TFirstFilter firFilter;
	TSecondFilter secFileter;
	TThirdFilter  thirdFileter;
	
	std::map<tBaseInfo, tOutData> ret = stock.DoAlgorithm(inMap, thirdFileter);  //;


	return 0;
}

