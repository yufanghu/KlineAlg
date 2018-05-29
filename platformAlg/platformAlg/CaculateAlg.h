#ifndef _CACULATE_ALG_H
#define _CACULATE_ALG_H
/*************************************
创建日期	:2018-05-05
描述		: K线平台算法实现类
**************************************/
#include "PlatFormAlgorithm.h"

#define  DEADLINE_DATE "20181231"

class CCaculateAlg 
{
public:
	CCaculateAlg();
	~CCaculateAlg();
	CLog m_cLog;

	//单平台入口
	bool single_plat(const std::map<tagStockCodeInfo, std::vector<tagKline>> & input,
		std::map<tagStockCodeInfo, tagOutput> & output, short avgFac, bool bFiring = false);

	//双平台入口
	bool double_plat(const std::map<tagStockCodeInfo, std::vector<tagKline>> & input,
		std::map<tagStockCodeInfo, tagOutput> & output, short avgFac, bool bFiring = false);

	void SetLogObj(CLog * pLog){m_pLog = pLog;}
private:

	/**
	*  功能描述: 平台筛选第一步（单、双平台公用）
	*  @param nMax	 输入参数   最大根数量
	*  @param nMin 输入参数     最小根数量
	*/
	bool single_double_step_one(const std::vector<tagKline>& kLineData, short avgFac, int& nPos, int& nKnb,
					int nMax, int nMin);
	/**
	*  功能描述: 平台筛选第二步（单、双平台公用）
	*/
	bool single_double_step_two(const std::vector<tagKline>& kLineData,  int& nPos);

	/**
	*  功能描述: 单平台第三步
	*/
	bool single_plat_step_third(const std::vector<tagKline>& kLineData, int& nPos);

	/**
	*  功能描述: 双平台第三步、第四步、第五步
	*/
	bool double_step_third(const std::vector<tagKline>& kLineData, int& nPos);
	bool double_step_fourth(const std::vector<tagKline>& kLineData, int& nPos);
	bool double_step_fifth(const std::vector<tagKline>& kLineData, const int& nPos);

	/**
	*  功能描述: 判定是否起爆
	*/
	bool is_fairing(const std::vector<tagKline>& kLineData, int& nPos, bool isFiring);
	
	/**
	*  功能描述: 计算均线的参数
	*/
	bool get_avg(const std::vector<tagKline>& kLineData, int nStart,
		int nCount, double& dAvg);

	void PrintData(const std::vector<tagKline>& kLineData);
	void PrintHead(const tagStockCodeInfo & codeInfo, int nPos = -1);

	CLog * m_pLog;
};

#endif

