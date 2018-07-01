#pragma once
#include "AlgBase.h"
#include "StockAlgorithm.h"
class CAlgPlatForm : public CAlgBase
{
public:
	CAlgPlatForm(const std::map<tagStockCodeInfo, std::vector<tagKline>>& input, std::map<tagStockCodeInfo, tagOutput>& output,
		EPlatFormType platformType, short avgFac, bool bFiring);
	
	~CAlgPlatForm();

	virtual bool createNewAlg() override;

private:
	
	const std::map<tagStockCodeInfo, std::vector<tagKline>> m_input;
	
	std::map<tagStockCodeInfo, tagOutput> & m_output;

	EPlatFormType m_platformType;
	
	short m_avgFac;
	
	bool m_bFiring;
};