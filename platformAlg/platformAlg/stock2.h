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
	bool GetLowOrHighClose(const std::vector<tagKline>& itvDataBegin, int start, int end,
		tagKline& tagResult, int& nPos, bool bLow = false);
	bool GetLow(std::vector<tagKline>::const_iterator itvDataBegin,
		std::vector<tagKline>::const_iterator itvDataEnd, tagKline tResult);

	/*
	进阶筛选（A1）
	a、搜索范围内收盘价最低数据A
	b、搜索A数据日期前收盘价最高B1数据
	c.搜索A数据日期后收盘价最高B2数据
	*/
	bool filterStepA1(const std::vector<tagKline>& vecKline, tagKline& tLowA, int& nApos,
		tagKline& tHighB1, int& nB1Pos,
		tagKline& tHighB2, int& nB2Pos);

	bool filterStepA2();

private:
	CLog * m_pLog;

}


#endif