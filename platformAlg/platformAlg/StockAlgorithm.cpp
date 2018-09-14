#include "StdAfx.h"
#include "StockAlgorithm.h"
#include "AlgStock2.h"
#include "AlgStock1.h"
#include "AlgPlatForm.h"
#include "Log.h"
#include "Global.h"
bool alg_platform(const std::map<tagStockCodeInfo, std::vector<tagKline>>& input, std::map<tagStockCodeInfo, tagOutput>& output,
	                             EPlatFormType platformType, short avgFac, bool bFiring)
{
	CAlgBase *pBase = new CAlgPlatForm(input, output, platformType, avgFac, bFiring);
	
	pBase->createNewAlg();

	delete pBase;
	return true;
}


bool alg_stock2(const std::map<tagStockCodeInfo, std::vector<tagKline>> &inMap, std::map<tagStockCodeInfo, tagOutput> & output, TFirstFilter& tFirFilter)
{
	CAlgBase *pBase = new CAlgStock2(inMap, output, tFirFilter);
	pBase->createNewAlg();
	delete pBase;
	return true;

}


bool alg_stock2(const std::map<tagStockCodeInfo, std::vector<tagKline>> &inMap, std::map<tagStockCodeInfo, tagOutput> & output, TSecondFilter& filter)
{
	CAlgBase *pBase = new CAlgStock2(inMap, output, filter);
	
	pBase->createNewAlg();
	
	delete pBase;
	return true;
}


bool alg_stock2(const std::map<tagStockCodeInfo, std::vector<tagKline>> &inMap, std::map<tagStockCodeInfo, tagOutput> & output, TThirdFilter& filter)
{
	CAlgBase *pBase = new CAlgStock2(inMap, output, filter);
	pBase->createNewAlg();
	delete pBase;
	return true;
}


bool alg_stock1(const std::map<tagStockCodeInfo, std::vector<tagKline>>& inMap, std::map<tagStockCodeInfo, outKline> & outMap)
{
	CAlgBase *pBase = new CAlgStock1(inMap, outMap);
	pBase->createNewAlg();
	delete pBase;
	return true;
}

void EnableAlgLog(bool bEnable)
{
	//g_bEnableLog = bEnable;
	CLog::g_bEnableLog = bEnable;
}