#include "StdAfx.h"
#include "PlatFormAlgorithm.h"
#include "CaculateAlg.h"
#include "stock2.h"
#include "Log.h"



bool alg_platform(const std::map<tagStockCodeInfo, std::vector<tagKline>>& input, std::map<tagStockCodeInfo, tagOutput>& output,
	                             EPlatFormType platformType, short avgFac, bool bFiring)
{
	CAlgBase *pBase = new CAlg3(input, output, platformType, avgFac, bFiring);
	pBase->createNewAlg();
	return true;

	
}



bool alg_stock2(std::map<tagStockCodeInfo, std::vector<tagKline>> &inMap, std::map<tagStockCodeInfo, tagOutput> & output, TFirstFilter& tFirFilter)
{
	CAlgBase *pBase = new CAlg2(inMap,output, tFirFilter);
	pBase->createNewAlg();
	return true;

}

bool alg_stock2(std::map<tagStockCodeInfo, std::vector<tagKline>> &inMap, std::map<tagStockCodeInfo, tagOutput> & output, TSecondFilter& filter)
{
	CAlgBase *pBase = new CAlg2(inMap, output, filter);
	pBase->createNewAlg();
	return true;
}

bool alg_stock2(std::map<tagStockCodeInfo, std::vector<tagKline>> &inMap, std::map<tagStockCodeInfo, tagOutput> & output, TThirdFilter& filter)
{
	CAlgBase *pBase = new CAlg2(inMap, output, filter);
	pBase->createNewAlg();
	return true;
}

