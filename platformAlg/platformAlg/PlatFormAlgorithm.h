#ifndef _PLATFORM_SELECT_H
#define _PLATFORM_SELECT_H
/*************************************
作者		:Jarvis
创建日期	:2018-04-29
修改日期	:2018-05-01
描述		: K线【单/双】平台算法
**************************************/

#include <vector>
#include <map>
#include <string>
#include "Log.h"

namespace alg{

	//k线结构体
	struct tagKline {
		__int64 time;	//时间戳
		double open;	//开盘价
		double high;	//最高价
		double low;		//最低价
		double close;	//收盘价
		bool operator == (const tagKline uDes){ return this->time == uDes.time; }
	};


	//k线周期  --暂时没有用到
	enum EKlinePeriod {
		eInvalidPeriod = -1, //无效周期 占位
		ePeriod5Min = 1,
		ePeriod15Min = 2,
		ePeriod30Min = 3,
		ePeriod60Min = 4,
		ePeriodDay = 5,
		ePeriodWeek = 6,
		ePeriodMouth = 7,
		ePeriodYear = 8,
		ePeriod1Min = 35   // 1分钟线
	};

	//平台枚举
	enum EPlatFormType{
		eSinglePlatForm,   //单平台
		eDoublePlatForm,	//双平台
		eFitler2First1,		//文档筛选2 一级筛选
		eFitler2First2,    //文档晒选2 二级筛选
		eFitler2First3,    //文档筛选2 三级筛选
	};



	struct TA2
	{
		short iMinka;  //最小KA
		short iMaxka;  //最大KA
		short iMinkb;  //最小KB
		short iMaxkb;  //最大KB

		TA2(){
			iMinka = 6;
			iMaxka = 9;
			iMinkb = 6;
			iMaxkb = 8;
		}
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
		TFirstFilter(){
			sA3Switch = 0;
			bA4Switch = false;
			bA5Switch = false;
			bA6Switch = false;
			sCallbackRange = 1;
			sRbcoe = 1;
		}
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



	//输出结果信息 可扩展
	struct tagOutput {
		//std::vector<tagKline> klineVect;
		//其他信息
	};

	//股票基本信息
	struct tagStockCodeInfo {
		short market;		//交易所|市场
		std::string stockcode;	//合约代码

		tagStockCodeInfo(short  market, const std::string & code) :
			market(market), stockcode(code) {}

		bool operator<(const tagStockCodeInfo  & another)const {
			if (this->market == another.market) {
				return this->stockcode < another.stockcode;
			}
			else
				return this->market < another.market;
		}

		bool operator==(const tagStockCodeInfo  & another)const {
			if (this->market == another.market && this->stockcode == another.stockcode)
				return true;
			else
				return false;
		}

	};
}
using namespace alg;

/**
*  功能描述: 算法对外调用接口
*  @param input	 输入参数  key-股票信息， value-k线集合
*  @param output 输出参数  key-股票信息， value-暂时保留空
*  @param EPlatFormType 平台类型
*  @param avgFac 均线参数
*  @param bFiring 是否起爆
*  @返回 true-成功 false-失败
*/
//平台筛选接口
bool select_entrance(const std::map<tagStockCodeInfo, std::vector<tagKline>> & input,
	std::map<tagStockCodeInfo, tagOutput> & output, EPlatFormType platformType, short avgFac, bool bFiring =false);



//删除级别开关，做第一级筛选，就调第一个接口，做第二级筛选，就调第二个接口，做第三级筛选，就调第三个接口
////第一级筛选接口
//bool DoAlgorithm(std::map<tagStockCodeInfo, std::vector<tagKline>> &inMap,
//	std::map<tagStockCodeInfo, tagOutput> & output,
//	TFirstFilter&);  //
//
////第二级筛选接口
//bool DoAlgorithm(std::map<tagStockCodeInfo, std::vector<tagKline>> &inMap,
//	std::map<tagStockCodeInfo, tagOutput> & output,
//	TSecondFilter&);
//
////第三级筛选接口
//bool DoAlgorithm(std::map<tagStockCodeInfo, std::vector<tagKline>> &inMap,
//	std::map<tagStockCodeInfo, tagOutput> & output,
//	TThirdFilter&);


template<typename T> 
bool bool DoAlgorithm(std::map<tagStockCodeInfo, std::vector<tagKline>> &inMap,
	std::map<tagStockCodeInfo, tagOutput> & output,
	T&, int );

#endif


