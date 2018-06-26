#include "StdAfx.h"
#include "PlatFormAlgorithm.h"
#include "CaculateAlg.h"
#include "stock2.h"
#include "Log.h"



bool select_entrance(const std::map<tagStockCodeInfo, std::vector<tagKline>>& input, std::map<tagStockCodeInfo, tagOutput>& output,
	                             EPlatFormType platformType, short avgFac, bool bFiring)
{
	CLog log;
	log.Init("platform3_log");
	CCaculateAlg calAlg; 
	calAlg.SetLogObj(&log);
	bool bRet= false;
	//for (int nCal = 0; nCal != input.size(); ++nCal)
	{
		//遍历map开始筛选
		switch (platformType)
		{
			case
				::eSinglePlatForm:
					bRet = calAlg.single_plat(input, output, avgFac, bFiring);
					if (bRet)
					{
						log.Flush(e_doc_3,"platform3_log");
						return false;
					}
					
				break;
				case
				::eDoublePlatForm:
					bRet = calAlg.double_plat(input, output, avgFac, bFiring);
					if (bRet)
					{
						log.Flush(e_doc_3,"platform3_log");
						return false;
					}
					
				break;
			default:
				break;
		}

	}

	if (output.empty())
		log.logRecord("没有找到匹配股票\n");
	else{
		log.logRecord("筛选成功:");
		for each (auto var in output)
		{
			log.logRecord("[%d-%s],", var.first.market, var.first.stockcode.c_str());
		}
		log.logRecord("\n");
	}
	log.Flush(e_doc_3,"platform3_log");
	return false;
}



bool alg_stock2(std::map<tagStockCodeInfo, std::vector<tagKline>> &inMap, std::map<tagStockCodeInfo, tagOutput> & output, TFirstFilter& tFirFilter)
{
	CAlgBase *pBase = new CAlg3(inMap,output, tFirFilter, 1);
	pBase->createNewAlg();
	return true;

}

bool alg_stock2(std::map<tagStockCodeInfo, std::vector<tagKline>> &inMap, std::map<tagStockCodeInfo, tagOutput> & output, TSecondFilter& filter)
{
	CAlgBase *pBase = new CAlg3(inMap, output, tFirFilter, 1);
	pBase->createNewAlg();
	return true;
}

bool alg_stock2(std::map<tagStockCodeInfo, std::vector<tagKline>> &inMap, std::map<tagStockCodeInfo, tagOutput> & output, TThirdFilter& filter)
{
	CAlgBase *pBase = new CAlg3(inMap, output, tFirFilter, 1);
	pBase->createNewAlg();
	return true;
}

