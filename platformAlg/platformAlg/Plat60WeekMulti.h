#pragma once
#include "StockAlgorithm.h"
#include "Log.h"
#include "AlgBase.h"
#include <vector>
#include <map>
#include <string>
using namespace std;
using namespace  alg;

class CPlat60WeekMulti :public CAlgBase
{
public:
	CPlat60WeekMulti(const std::map<tagStockCodeInfo, std::vector<tagKline>>& inMap, std::map<tagStockCodeInfo, tagOutput> & outMap);
	~CPlat60WeekMulti();
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
	void PrintHead(const tagStockCodeInfo & codeInfo, int nPos);

private:
	std::map<tagStockCodeInfo, std::vector<tagKline>> m_inMap;
	std::map<tagStockCodeInfo, tagOutput> & m_outMap;
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
	std::vector<tagQuota>  m_quota;
	std::map<long long, double> m_mapAvg; //记录60周均线值
	int minVal;;
};

