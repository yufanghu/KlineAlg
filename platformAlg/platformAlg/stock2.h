#ifndef FILTER2_H
#define FILTER2_H
#include "PlatFormAlgorithm.h"


class CFilter2Alg
{
public:
	CFilter2Alg();
	~CFilter2Alg();
	bool filter2Level1(const std::map<tagStockCodeInfo, std::vector<tagKline>> &inMap,
		std::map<tagStockCodeInfo, tagOutput> & output,
		TFirstFilter& tFirFilter);
	bool filter2Level2(const std::map<tagStockCodeInfo, std::vector<tagKline>> &inMap,
		TSecondFilter& tSedFilter);
	bool filter2Level3(const std::map<tagStockCodeInfo, std::vector<tagKline>> &inMap,
		TThirdFilter& tThdFilter);
	void SetLogObj(CLog * pLog){ m_pLog = pLog; }

private:
	//获取阳线个数
	int GetSunLineNum(std::vector<tagKline>::const_iterator itvDataBegin,
		std::vector<tagKline>::const_iterator itvDataEnd);
	bool GetLowClose(const std::vector<tagKline>& itvDataBegin,  int size,
		 tagKline& tagResult, bool bLow = false);
	bool GetLow( std::vector<tagKline>::const_iterator itvDataBegin,
		std::vector<tagKline>::const_iterator itvDataEnd, tagKline tResult)

private:
	CLog * m_pLog;

}


#endif