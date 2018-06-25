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
		//����map��ʼɸѡ
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
		log.logRecord("û���ҵ�ƥ���Ʊ\n");
	else{
		log.logRecord("ɸѡ�ɹ�:");
		for each (auto var in output)
		{
			log.logRecord("[%d-%s],", var.first.market, var.first.stockcode.c_str());
		}
		log.logRecord("\n");
	}
	log.Flush(e_doc_3);
	return false;
}


//�ĵ��㷨2
bool  DoAlgorithm(std::map<tagStockCodeInfo, std::vector<tagKline>> &inMap, std::map<tagStockCodeInfo, tagOutput> & output, TFilter& filter, EStock2Type type)
{
	CLog log;
	log.Init();
	CFilter2Alg calAlg;
	calAlg.SetLogObj(&log);
	bool bRet = false;

	switch (type)
	{
	case eStock2First:
		bRet = calAlg.filter2Level1(inMap, output, filter);
		break;
	case  eStock2Second:
		bRet = calAlg.filter2Level2(inMap, output, filter);
		break;
	case eStock3Three:
		bRet = calAlg.filter2Level3(inMap, output, filter);
		break;

	default:
		break;
	}

	if (output.empty())
		log.logRecord("û���ҵ�ƥ���Ʊ\n");
	else{
		log.logRecord("ɸѡ�ɹ�:");
		for each (auto var in output)
		{
			log.logRecord("[%d-%s],", var.first.market, var.first.stockcode.c_str());
		}
		log.logRecord("\n");
	}
	log.Flush(e_doc_3);
	return bRet;


}

