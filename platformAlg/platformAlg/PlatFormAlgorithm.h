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


#define PERIODKAMIN     6
#define PERIODKAMINB1   2
#define PERIODKAMINC2   2
#define PERIODKAMAX     9
#define PERIODKAMAXB1   8
#define PERIODKAMAXC1   8
#define PERIODKBMIN     6
#define PERIODKBMINB1   5
#define PERIODKBMINC1   5
#define PERIODKBMAX     8
#define PERIODKBMAXB1   8
#define PERIODKBMAXC1   8


namespace alg{

#pragma region 公共部分
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

#pragma endregion 公共部分
	

#pragma region 算法一
	//平台枚举
	enum EPlatFormType{
		eSinglePlatForm,    //单平台
		eDoublePlatForm,    //双平台
	};
#pragma endregion 算法一


#pragma region 算法二


	//筛选出的类型
	enum EFilterType{
		eOff,  //关闭
		eAA,
		eAB,
		eAAP
	};

	struct TA2
	{
		short iMinka;  //最小KA
		short iMaxka;  //最大KA
		short iMinkb;  //最小KB
		short iMaxkb;  //最大KB

		TA2():iMaxkb(0),iMaxka(0), iMinkb(0), iMinka(0){};
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
		TFirstFilter() :sA3Switch(0), bA4Switch(false), bA5Switch(false), 
			sCallbackRange(1), bA6Switch(false), sRbcoe(1){};
	};

	//二级筛选
	struct TSecondFilter
	{
		TA2   tLineNum;
		bool   bA5Switch;
		short sCallbackRange;//双底调整幅度  整数形式：1-99
		TSecondFilter() :bA5Switch(false), sCallbackRange(1){};
	};

	//三级筛选
	struct TThirdFilter
	{
		TA2   tLineNum;
		bool   bA5Switch;
		short sCallbackRange;//双底调整幅度  整数形式：1-99
		TThirdFilter() :bA5Switch(false), sCallbackRange(1){};
	};


#pragma endregion 算法二


	
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
bool alg_platform(const std::map<tagStockCodeInfo, std::vector<tagKline>> & input,
	std::map<tagStockCodeInfo, tagOutput> & output, EPlatFormType platformType, short avgFac, bool bFiring =false);


/************************************************************************/
/* 
算法2
@param inMap  输入参数  key-股票信息， value-k线集合
@param output 输出参数  key-股票信息， value-存储股票类型 off   AA AB  AAP
@param filter 输入参数  TFilter 用户选项默认均关闭
@param eType  输入参数	EPlatFormType 进阶筛选三个平台
*/
/************************************************************************/
bool  alg_stock2(std::map<tagStockCodeInfo, std::vector<tagKline>> &inMap,
	std::map<tagStockCodeInfo, tagOutput> & output, TFirstFilter& tFirFilter);

bool  alg_stock2(std::map<tagStockCodeInfo, std::vector<tagKline>> &inMap,
	std::map<tagStockCodeInfo, tagOutput> & output, TSecondFilter& filter);

bool  alg_stock2(std::map<tagStockCodeInfo, std::vector<tagKline>> &inMap,
	std::map<tagStockCodeInfo, tagOutput> & output, TThirdFilter& filter);

#endif


