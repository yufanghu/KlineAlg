#pragma once
#ifndef ALG_BASE
#define	ALG_BASE
#endif // !ALG_BASE
#include <map>
#include <vector>
#include "PlatFormAlgorithm.h"
class CAlgBase
{
public:
	CAlgBase();
	virtual ~CAlgBase();
	virtual bool createNewAlg() = 0;
};

class CAlg2:public CAlgBase
{
public:
	CAlg2(std::map<tagStockCodeInfo, std::vector<tagKline>> &inMap,
		std::map<tagStockCodeInfo, tagOutput> & output, TFirstFilter& tFirFilter):
	m_inMap(inMap),m_output(output),m_tFirFilter(tFirFilter),m_id(1){}
	CAlg2(std::map<tagStockCodeInfo, std::vector<tagKline>> &inMap,
		std::map<tagStockCodeInfo, tagOutput> & output, TSecondFilter& tFirFilter):
		m_inMap(inMap), m_output(output), m_tSecFilter(tFirFilter), m_id(2){};
	CAlg2(std::map<tagStockCodeInfo, std::vector<tagKline>> &inMap,
		std::map<tagStockCodeInfo, tagOutput> & output, TThirdFilter& tFirFilter):
		m_inMap(inMap), m_output(output), m_tTirFilter(tFirFilter), m_id(3){};
	~CAlg2();
	virtual bool createNewAlg();

private:
	std::map<tagStockCodeInfo, std::vector<tagKline>> m_inMap;
	std::map<tagStockCodeInfo, tagOutput>&  m_output;
	TFirstFilter m_tFirFilter;
	TSecondFilter m_tSecFilter;
	TThirdFilter m_tTirFilter;
	int m_id;
};

class CAlg3 :public CAlgBase
{
public:
	CAlg3(const std::map<tagStockCodeInfo, std::vector<tagKline>>& input, std::map<tagStockCodeInfo, tagOutput>& output,
	EPlatFormType platformType, short avgFac, bool bFiring) :
	m_input(input), m_output(output), m_platformType(platformType), m_avgFac(avgFac), m_bFiring(bFiring){}
	~CAlg3(){};
	virtual bool createNewAlg();

private:
	const std::map<tagStockCodeInfo, std::vector<tagKline>> m_input;
	std::map<tagStockCodeInfo, tagOutput>& m_output;
	EPlatFormType m_platformType;
	short m_avgFac;
	bool m_bFiring;
};

