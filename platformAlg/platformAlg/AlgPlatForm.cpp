#include "AlgPlatForm.h"
#include "CaculatePlatform.h"
#include "StockAlgorithm.h"

CAlgPlatForm::CAlgPlatForm(const std::map<tagStockCodeInfo, std::vector<tagKline>>& input, std::map<tagStockCodeInfo, tagOutput>& output,
	EPlatFormType platformType, short avgFac, bool bFiring) : m_input(input), 
	m_output(output), 
	m_platformType(platformType), 
	m_avgFac(avgFac), 
	m_bFiring(bFiring)
{

}

CAlgPlatForm::~CAlgPlatForm()
{

}



bool CAlgPlatForm::createNewAlg()
{
	CLog log;
	log.Init("PlatForm3_Log");

	CCaculateAlg calAlg;
	calAlg.SetLogObj(&log);
	
	bool bRet = false;


	//遍历map开始筛选
	switch (m_platformType)
	{
		case eSinglePlatForm: //单平台
				bRet = calAlg.single_plat(m_input, m_output, m_avgFac, m_bFiring);
				break;
		case eDoublePlatForm: //双平台
				bRet = calAlg.double_plat(m_input, m_output, m_avgFac, m_bFiring);
			break;
		default:
			break;
	}


	if (m_output.empty())
		log.logRecord("没有找到匹配股票\n");
	else{
		log.logRecord("筛选成功:");
		for each (auto var in m_output)
		{
			log.logRecord("[%d-%s],", var.first.market, var.first.stockcode.c_str());
		}
		log.logRecord("\n");
	}

	log.Flush();

	return bRet;
}