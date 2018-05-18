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

namespace alg{

	//k线结构体
	struct tagKline {
		__int64 time;	//时间戳
		double open;	//开盘价
		double high;	//最高价
		double low;		//最低价
		double close;	//收盘价
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
		eDoublePlatForm		//双平台
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
class CPlatFormAlgorithm
{
public:
	CPlatFormAlgorithm();
	virtual ~CPlatFormAlgorithm();

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

};

#endif