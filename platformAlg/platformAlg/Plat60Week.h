#pragma once
#include "StockAlgorithm.h"
#include "Log.h"
#include "AlgBase.h"
#include <vector>
#include <map>
using namespace  alg;

class CPlat60Week :public CAlgBase
{
public:
	CPlat60Week(const std::vector<tagKline>& inStockKlines, std::vector<tagQuota> & quota);
	~CPlat60Week(void);
	virtual bool createNewAlg();

	void SetLogObj(CLog * pLog){ m_pLog = pLog; }
private:
	/**
	*  功能描述: 计算均线的参数
	*/
	bool get_avg();

	bool loopStepA(int start, int& end);
	int getPlatGrade(const int start, const int end, alg::EPlatLevel& level);
	bool loopStepB(int start, int& end);
	bool loopStepBOne(int start, int& end);
	bool loopStepBTwo(int start, int&end);
	bool loopStepBThree(int start, int&end);
	bool loopEntrance();
	void PrintData(const std::vector<tagKline>& kLineData);

	/************************************************************************/
	/* 获取最大包容周期，从beginD 到endD                                                                     */
	/************************************************************************/
	int  MaxPeriod(__int64 beginD, __int64 endD, int& pos);

	/************************************************************************/
	/* 返回值：-1无效，0可以继续向下找，1当前为最大包容值                                                                     */
	/************************************************************************/
	int getMaxAvg(int pos, int weeks);

private:
	CLog * m_pLog;
	double m_H2;
	double m_HB1;
	double m_H1;
	int m_posHB1;
	double m_CA;
	long long m_CATime;
	long long m_stepBhb1;
	bool m_type;
	std::vector<tagKline> m_inStockKlines;
	std::vector<tagQuota>&  m_quota;
	std::map<long long, double> m_mapAvg; //记录60周均线值
};

