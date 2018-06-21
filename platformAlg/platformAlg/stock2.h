#pragma once

#include <iostream>
#include <map>
#include <vector>
#include <stdint.h>
#include <string>  
#include <algorithm> 
#include <iomanip>
#include <fstream>
#include <time.inl>

using namespace std;

namespace stock_second
{
	struct tBaseInfo
	{
		short       sSetcode;   //
		std::string sTockcode;  //
	};

	bool operator<(const tBaseInfo& a1, const tBaseInfo& a2);


	typedef struct DATAPP
	{
		uint64_t uDate;     //
		double   dOpenPrice; //
		double   dHighPrice; //
		double   dLowPrice;  //
		double   dColsePrice; //
		bool operator == (const DATAPP uDes){return this->uDate == uDes.uDate;}
	}DataT;

	struct TA2
	{
		short iMinka;  //最小KA
		short iMaxka;  //最大KA
		short iMinkb;  //最小KB
		short iMaxkb;  //最大KB
	};

	//一级筛选
	struct TFirstFilter
	{
		TA2   tLineNum;
		short  sA3Switch;       //A3步骤参数   0 -> off  1 -> AA   2 -> AB
		bool  bA4Switch;        // A4步骤开关 
		bool  bA5Switch;        // A5步骤开关
		short sCallbackRange;   //双底调整幅度  整数形式：1-99  
		bool  bA6Switch;        // A6步骤开关	
		short sRbcoe;           //阳k线比例  整数形式：1-99
	};

	//二级筛选
	struct TSecondFilter
	{
		TA2   tLineNum;
		bool   bA5Switch;
		short sCallbackRange;//双底调整幅度  整数形式：1-99
	};

	//三级筛选
	struct TThirdFilter
	{
		TA2   tLineNum;
		bool   bA5Switch;
		short sCallbackRange;//双底调整幅度  整数形式：1-99
	};

	//返回数据
	struct tOutData   //预留前端需要的数据
	{

	};


	class CStock
	{
	public:
		CStock(){};
		virtual ~CStock(){};

		//std::map<tBaseInfo, TData>  mInputDate;
		//因为所有股票的TData都是一样的，因此我们只需要传一次就好，
		//所以定义函数的时候，弄两个入参，一个是股票列表std::vector<tBaseInfo> infoVec，另一个就是TData，函数声明如下所示：

		//删除级别开关，做第一级筛选，就调第一个接口，做第二级筛选，就调第二个接口，做第三级筛选，就调第三个接口
		//第一级筛选接口
		std::map<tBaseInfo, tOutData> DoAlgorithm(std::map<tBaseInfo, std::vector<DATAPP>> &inMap, 
			                                      TFirstFilter&);  //

		//第二级筛选接口
		std::map<tBaseInfo, tOutData> DoAlgorithm(std::map<tBaseInfo, std::vector<DATAPP>> &inMap,
			                                      TSecondFilter&);

		//第三级筛选接口
		std::map<tBaseInfo, tOutData> DoAlgorithm(std::map<tBaseInfo,std::vector<DATAPP>> &inMap, 
			                                      TThirdFilter&);

	};

	void logforpara2(std::map<tBaseInfo, std::vector<DataT>>& inMap);
}