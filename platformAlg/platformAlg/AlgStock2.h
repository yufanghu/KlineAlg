#pragma once
#include "AlgBase.h"
#include "StockAlgorithm.h"
class CAlgStock2 :public CAlgBase
{
public:
	CAlgStock2(const std::map<tagStockCodeInfo, std::vector<tagKline>> &inMap,
		std::map<tagStockCodeInfo, tagOutput> & output, TFirstFilter& tFirFilter) :
		m_inMap(inMap), m_output(output), m_tFirFilter(tFirFilter), m_id(1){}

	CAlgStock2(const std::map<tagStockCodeInfo, std::vector<tagKline>> &inMap,
		std::map<tagStockCodeInfo, tagOutput> & output, TSecondFilter& tFirFilter) :
		m_inMap(inMap), m_output(output), m_tSecFilter(tFirFilter), m_id(2){};

	CAlgStock2(const std::map<tagStockCodeInfo, std::vector<tagKline>> &inMap,
		std::map<tagStockCodeInfo, tagOutput> & output, TThirdFilter& tFirFilter) :
		m_inMap(inMap), m_output(output), m_tTirFilter(tFirFilter), m_id(3){};

	~CAlgStock2();

	virtual bool createNewAlg();

private:
	std::map<tagStockCodeInfo, std::vector<tagKline>> m_inMap;
	std::map<tagStockCodeInfo, tagOutput>&  m_output;
	TFirstFilter m_tFirFilter;
	TSecondFilter m_tSecFilter;
	TThirdFilter m_tTirFilter;
	int m_id;
};