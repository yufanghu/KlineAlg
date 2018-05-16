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
	
	for (int nCal = 0; nCal != input.size(); ++nCal)
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
	LOG_FLUSH();
	return false;
}
