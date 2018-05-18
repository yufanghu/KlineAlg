#include "StdAfx.h"
#include "PlatFormAlgorithm.h"
#include "CaculateAlg.h"
#include "Log.h"

CPlatFormAlgorithm::CPlatFormAlgorithm()
{
}

CPlatFormAlgorithm::~CPlatFormAlgorithm()
{
}

bool CPlatFormAlgorithm::select_entrance(const std::map<tagStockCodeInfo, std::vector<tagKline>>& input, std::map<tagStockCodeInfo, tagOutput>& output,
	                             EPlatFormType platformType, short avgFac, bool bFiring)
{
	CCaculateAlg calAlg; 
	std::map<tagStockCodeInfo, std::vector<tagKline>>::iterator iter;
	std::map<tagStockCodeInfo, std::vector<tagKline>> mapInput = input;
	for (iter = mapInput.begin(); iter != mapInput.end(); ++iter)
	{
		tagStockCodeInfo tagOne = iter->first;
		std::vector<tagKline> vecKline = iter->second;
		std::pair<tagStockCodeInfo, std::vector<tagKline>> pair_input(tagOne, vecKline);
		//遍历map开始筛选
		switch (platformType)
		{
			case
				::eSinglePlatForm:
					calAlg.single_plat(pair_input, output, avgFac, bFiring);
				break;
				case
				::eDoublePlatForm:
					calAlg.double_plat(pair_input, output, avgFac, bFiring);
				break;
			default:
				break;
		}

	}
	LOG_FLUSH();
	return false;
}
