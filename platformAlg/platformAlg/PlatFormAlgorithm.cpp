#include "StdAfx.h"
#include "PlatFormAlgorithm.h"
#include "CaculateAlg.h"
#include "stock2.h"
#include "Log.h"




bool select_entrance(const std::map<tagStockCodeInfo, std::vector<tagKline>>& input, std::map<tagStockCodeInfo, tagOutput>& output,
	                             EPlatFormType platformType, short avgFac, bool bFiring)
{
	CLog log;
	log.Init();
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
						log.Flush(e_doc_3);
						return false;
					}
					
				break;
				case
				::eDoublePlatForm:
					bRet = calAlg.double_plat(input, output, avgFac, bFiring);
					if (bRet)
					{
						log.Flush(e_doc_3);
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
	log.Flush(e_doc_3);
	return false;
}


//文档算法2
template<typename T>
bool bool DoAlgorithm(std::map<tagStockCodeInfo, std::vector<tagKline>> &inMap, std::map<tagStockCodeInfo, tagOutput> & output, 
	T& filter, int nType)
{
	CLog log;
	log.Init();
	CFilter2Alg calAlg;
	calAlg.SetLogObj(&log);
	bool bRet = false;

	switch (nType)
	{
	case eFitler2First1:
		bRet = calAlg.filter2Level1(inMap, output, filter);
		break;
	case  eFitler2First2:
		bRet = calAlg.filter2Level2(inMap, output, filter);
		break;
	case eFitler2First3:
		bRet = calAlg.filter2Level3(inMap, output, filter);
		break;

	default:
		break;
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
	log.Flush(e_doc_3);
	return bRet;


}

