#pragma once
#include "AlgBase.h"
#include "StockAlgorithm.h"

class CAlgStock1 :public CAlgBase
{
public:
	CAlgStock1(const std::map<tagStockCodeInfo, std::vector<tagKline>> & inMap, std::map<tagStockCodeInfo, outKline> & outMap);
	
	~CAlgStock1();
	
	virtual bool createNewAlg();

private:
	std::map<tagStockCodeInfo, std::vector<tagKline>> m_inMap;
	std::map<tagStockCodeInfo, outKline>&  m_output;
};