#ifndef _PLATFORM_SELECT_H
#define _PLATFORM_SELECT_H
/*************************************
作者		:Jarvis
创建日期	:2018-04-29
修改日期	:2018-05-01
描述		: K线筛选算法接口文件
**************************************/

#include <vector>
#include <map>
#include <string>

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

	

#pragma endregion 公共部分

#pragma region 算法一

	enum ESign
	{ 
		Normal = 1, NoPos, Abnormal
	};

	struct klinePoint
	{
		__int64 uDate;   //
		double   dPrice;  //
		ESign    eRet;    // 表示是否正常返回值   1 正常，算法进行此点均线N值的计算；前端画此点，正常处理N值。  3 异常 算法不进行此点均线N值的计算，N赋值为0；前端不画此点 ，N赋值0
	};

	enum eTendKind { rise = 1, down, fault }; // //趋势分类：rise--上升趋势  down--下降趋势(输出的结构各个变量对应下降趋势各个点及其值)  fault--无趋势，所有点和N都无值

	//当ifRise=rise时，输出的结构各个变量对应上升趋势各个点及其值
	struct outRiseKline
	{
		eTendKind tendency;
		klinePoint A;        //ifRise=rise，在K线界面标示该点，并在该点旁边注明“A”
		klinePoint B;        //ifRise=rise，在K线界面标示该点，并在该点旁边注明“B”
		klinePoint C;        //ifRise=rise，在K线界面标示该点，并在该点旁边注明“C”
		klinePoint B1;       //ifRise=rise，在K线界面标示该点，并在该点旁边注明“B1”
		klinePoint V1;       //ifRise=rise，在K线界面标示该点，并在该点旁边注明“V1”
		klinePoint V2;       //ifRise=rise，在K线界面标示该点，并在该点旁边注明“V2”
		klinePoint V3;       //ifRise=rise，在K线界面标示该点，并在该点旁边注明“V3”
		klinePoint M1;      // ifRise=rise，在K线界面标示该点，并在该点旁边注明“M1”

		int nV1;            //ifRise=rise，在K线界面弹框显示计算结果   nV1=  nV2=   nV3=   nM1=  nN0=
		int nV2;
		int nV3;
		int nM1;
		int nN0;
	};

	//当ifRise=down时，输出的结构各个变量对应下降趋势各个点及其值
	struct outDownKline
	{
		eTendKind tendency;
		klinePoint A;        //ifRise=down，在K线界面标示该点，并在该点旁边注明“A”
		klinePoint B;        //ifRise=down，在K线界面标示该点，并在该点旁边注明“B”
		klinePoint C;        //ifRise=down，在K线界面标示该点，并在该点旁边注明“C”
		klinePoint B1;       //ifRise=down，在K线界面标示该点，并在该点旁边注明“B1”
		klinePoint M1;       //ifRise=down，在K线界面标示该点，并在该点旁边注明“M1”
		klinePoint M2;       //ifRise=down，在K线界面标示该点，并在该点旁边注明“M2”
		klinePoint M3;       //ifRise=down，在K线界面标示该点，并在该点旁边注明“M3”
		klinePoint V1;      // ifRise=down，在K线界面标示该点，并在该点旁边注明“V1”

		int nM1;            //ifRise=down，在K线界面弹框显示计算结果   nM1=  nM2=   nM3=   nV1=  nN0=
		int nM2;
		int nM3;
		int nV1;
		int nN0;
	};

	//当ifRise=fault，输出的8个点的枚举值为3，输出的5个N值均为0

	struct outKline
	{
		eTendKind tendency;
		union
		{
			outRiseKline riseKline;
			outDownKline downKline;
			outDownKline noKline;
		};
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

		TA2()
			:iMaxkb(0),
			iMaxka(0),
			iMinkb(0),
			iMinka(0)
		{

		};
	};

	struct Alg2Filter{
		Alg2Filter(){}
		virtual ~Alg2Filter(){}
	};
	//筛选参数
	struct TFirstFilter : public Alg2Filter
	{
		TA2		tLineNum;
		short	sA3Switch;        //A3步骤参数   0 -> off  1 -> AA   2 -> AB
		bool	bA4Switch;        //A4步骤开关 
		bool	bA5Switch;        //A5步骤开关
		bool	bA6Switch;        //A6步骤开关	
		short	sCallbackRange;   //双底调整幅度  整数形式：1-99  
		short	sRbcoe;           //阳k线比例  整数形式：1-99
		TFirstFilter() 
			:sA3Switch(0), 
			bA4Switch(false), 
			bA5Switch(false), 
			sCallbackRange(1),
			bA6Switch(false),
			sRbcoe(1)
		{
		};
	};

	//二级筛选
	struct TSecondFilter  : public Alg2Filter
	{
		TA2    tLineNum;
		bool   bA5Switch;
		short  sCallbackRange;//双底调整幅度  整数形式：1-99
		TSecondFilter() 
			:bA5Switch(false), 
			sCallbackRange(1)
		{
		};
	};


	//三级筛选
	struct TThirdFilter  : public Alg2Filter
	{
		TA2   tLineNum;
		bool   bA5Switch;
		short sCallbackRange;//双底调整幅度  整数形式：1-99
		TThirdFilter() 
			:bA5Switch(false), 
			sCallbackRange(1)
		{
		};
	};

	//算法二的部分最小最大默认边界（用户不传的情况）
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

#pragma endregion 算法二

#pragma region 算法三
	//平台枚举
	enum EPlatFormType{
		eSinglePlatForm,    //单平台
		eDoublePlatForm,    //双平台
	};

#pragma endregion 算法三


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
@param output 输出参数  key-股票信息， value-
@param filter 输入参数  TFilter TFirstFilter一级筛选 TSecondFilter二级筛选 TThirdFilter三级筛选
*/
/************************************************************************/
bool  alg_stock2(const std::map<tagStockCodeInfo, std::vector<tagKline>> &inMap,
	std::map<tagStockCodeInfo, tagOutput> & output, TFirstFilter& tFirFilter);


bool  alg_stock2(const std::map<tagStockCodeInfo, std::vector<tagKline>> &inMap,
	std::map<tagStockCodeInfo, tagOutput> & output, TSecondFilter& filter);


bool  alg_stock2(const std::map<tagStockCodeInfo, std::vector<tagKline>> &inMap,
	std::map<tagStockCodeInfo, tagOutput> & output, TThirdFilter& filter);

/************************************************************************/
/*
算法1 入口
@param inMap  输入参数  key-股票信息， value-k线集合
@param output 输出参数  key-股票信息， value-
*/
/************************************************************************/
bool alg_stock1(const std::map<tagStockCodeInfo, std::vector<tagKline>>& inMap, std::map<tagStockCodeInfo, outKline> & outMap);

/************************************************************************/
/*
关闭/开启日志
@param bEnable  输入参数  true-开启日志， false-关闭日志
*/
/************************************************************************/

void EnableAlgLog(bool bEnable = true);

#endif


