#include "StdAfx.h"
#include "PlatFormAlgorithm.h"
#include "CaculateAlg.h"
#include "Log.h"

CPlatFormAlgorithm::CPlatFormAlgorithm()
{
	int i = 0;
	1 / i;
}

CPlatFormAlgorithm::~CPlatFormAlgorithm()
{
}

bool CPlatFormAlgorithm::select_entrance(const std::map<tagStockCodeInfo, std::vector<tagKline>>& input, std::map<tagStockCodeInfo, tagOutput>& output,
	                             EPlatFormType platformType, short avgFac, bool bFiring)
{
	CLog log;
	log.Init();
	CCaculateAlg calAlg; 
	calAlg.SetLogObj(&log);
	//for (int nCal = 0; nCal != input.size(); ++nCal)
	{
		//遍历map开始筛选
		switch (platformType)
		{
			case
				::eSinglePlatForm:
				calAlg.single_plat(input, output, avgFac, bFiring);
				break;
				case
				::eDoublePlatForm:
				calAlg.double_plat(input, output, avgFac, bFiring);
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
	log.Flush();
	return false;
}
