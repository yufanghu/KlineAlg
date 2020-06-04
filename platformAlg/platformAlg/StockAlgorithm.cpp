
#include "StockAlgorithm.h"
#include "AlgStock2.h"
#include "AlgStock1.h"
#include "AlgPlatForm.h"
#include "Plat60Week.h"
#include "Plat60WeekMulti.h"
#include "Log.h"

bool alg_platform(const std::map<tagStockCodeInfo, std::vector<tagKline>>& input, std::map<tagStockCodeInfo, tagOutput>& output,
	EPlatFormType platformType, short avgFac, bool bFiring)
{
	std::map<tagStockCodeInfo, std::vector<tagKline>> inTmp = const_cast<std::map<tagStockCodeInfo, std::vector<tagKline>>&>(input);
	std::map<tagStockCodeInfo, std::vector<tagKline>>::iterator  iter;
	//四舍五入
	for (iter = inTmp.begin(); iter != inTmp.end(); ++iter)
	{
		for (int j = 0; j != iter->second.size(); ++j)
		{
			iter->second[j].open = iter->second[j].open + 0.0051*(int(iter->second[j].open / fabs(iter->second[j].open)));
			iter->second[j].high = iter->second[j].high + 0.0051*(int(iter->second[j].high / fabs(iter->second[j].high)));
			iter->second[j].low = iter->second[j].low + 0.0051*(int(iter->second[j].low / fabs(iter->second[j].low)));
			iter->second[j].close = iter->second[j].close + 0.0051*(int(iter->second[j].close / fabs(iter->second[j].close)));
		}
	}
	CAlgBase *pBase = new CAlgPlatForm(inTmp, output, platformType, avgFac, bFiring);

	pBase->createNewAlg();

	delete pBase;
	return true;
}


bool alg_stock2(const std::map<tagStockCodeInfo, std::vector<tagKline>> &inMap, std::map<tagStockCodeInfo, tagOutput> & output, TFirstFilter& tFirFilter)
{
	std::map<tagStockCodeInfo, std::vector<tagKline>> inTmp = const_cast<std::map<tagStockCodeInfo, std::vector<tagKline>>&>(inMap);
	std::map<tagStockCodeInfo, std::vector<tagKline>>::iterator  iter;
	//四舍五入
	for (iter = inTmp.begin(); iter != inTmp.end(); ++iter)
	{
		for (int j = 0; j != iter->second.size(); ++j)
		{
			iter->second[j].open = iter->second[j].open + 0.0051*(int(iter->second[j].open / fabs(iter->second[j].open)));
			iter->second[j].high = iter->second[j].high + 0.0051*(int(iter->second[j].high / fabs(iter->second[j].high)));
			iter->second[j].low = iter->second[j].low + 0.0051*(int(iter->second[j].low / fabs(iter->second[j].low)));
			iter->second[j].close = iter->second[j].close + 0.0051*(int(iter->second[j].close / fabs(iter->second[j].close)));
		}
	}
	CAlgBase *pBase = new CAlgStock2(inTmp, output, tFirFilter);
	pBase->createNewAlg();
	delete pBase;
	return true;

}


bool alg_stock2(const std::map<tagStockCodeInfo, std::vector<tagKline>> &inMap, std::map<tagStockCodeInfo, tagOutput> & output, TSecondFilter& filter)
{
	std::map<tagStockCodeInfo, std::vector<tagKline>> inTmp = const_cast<std::map<tagStockCodeInfo, std::vector<tagKline>>&>(inMap);
	std::map<tagStockCodeInfo, std::vector<tagKline>>::iterator  iter;
	//四舍五入
	for (iter = inTmp.begin(); iter != inTmp.end(); ++iter)
	{
		for (int j = 0; j != iter->second.size(); ++j)
		{
			iter->second[j].open = iter->second[j].open + 0.0051*(int(iter->second[j].open / fabs(iter->second[j].open)));
			iter->second[j].high = iter->second[j].high + 0.0051*(int(iter->second[j].high / fabs(iter->second[j].high)));
			iter->second[j].low = iter->second[j].low + 0.0051*(int(iter->second[j].low / fabs(iter->second[j].low)));
			iter->second[j].close = iter->second[j].close + 0.0051*(int(iter->second[j].close / fabs(iter->second[j].close)));
		}
	}

	CAlgBase *pBase = new CAlgStock2(inTmp, output, filter);
	pBase->createNewAlg();

	delete pBase;
	return true;
}


bool alg_stock2(const std::map<tagStockCodeInfo, std::vector<tagKline>> &inMap, std::map<tagStockCodeInfo, tagOutput> & output, TThirdFilter& filter)
{
	std::map<tagStockCodeInfo, std::vector<tagKline>> inTmp = const_cast<std::map<tagStockCodeInfo, std::vector<tagKline>>&>(inMap);
	std::map<tagStockCodeInfo, std::vector<tagKline>>::iterator  iter;
	//四舍五入
	for (iter = inTmp.begin(); iter != inTmp.end(); ++iter)
	{
		for (int j = 0; j != iter->second.size(); ++j)
		{
			iter->second[j].open = iter->second[j].open + 0.0051*(int(iter->second[j].open / fabs(iter->second[j].open)));
			iter->second[j].high = iter->second[j].high + 0.0051*(int(iter->second[j].high / fabs(iter->second[j].high)));
			iter->second[j].low = iter->second[j].low + 0.0051*(int(iter->second[j].low / fabs(iter->second[j].low)));
			iter->second[j].close = iter->second[j].close + 0.0051*(int(iter->second[j].close / fabs(iter->second[j].close)));
		}
	}
	CAlgBase *pBase = new CAlgStock2(inTmp, output, filter);
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

void alg_stock4(const std::vector<tagKline>& inStockKlines, std::vector<tagQuota> & quota)
{
	CAlgBase *pBase = new CPlat60Week(inStockKlines, quota);
	pBase->createNewAlg();
	delete pBase;
}

void alg_stock4_multi(const std::map<tagStockCodeInfo, std::vector<tagKline>>& inMap, std::map<tagStockCodeInfo, tagOutput> & outMap)
{
	CAlgBase *pBase = new CPlat60WeekMulti(inMap, outMap);
	pBase->createNewAlg();
	delete pBase;
}

void EnableAlgLog(bool bEnable)
{
	CLog::m_bEnableLog = bEnable;
}