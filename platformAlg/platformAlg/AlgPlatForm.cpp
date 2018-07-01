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


	//����map��ʼɸѡ
	switch (m_platformType)
	{
		case eSinglePlatForm: //��ƽ̨
				bRet = calAlg.single_plat(m_input, m_output, m_avgFac, m_bFiring);
				break;
		case eDoublePlatForm: //˫ƽ̨
				bRet = calAlg.double_plat(m_input, m_output, m_avgFac, m_bFiring);
			break;
		default:
			break;
	}


	if (m_output.empty())
		log.logRecord("û���ҵ�ƥ���Ʊ\n");
	else{
		log.logRecord("ɸѡ�ɹ�:");
		for each (auto var in m_output)
		{
			log.logRecord("[%d-%s],", var.first.market, var.first.stockcode.c_str());
		}
		log.logRecord("\n");
	}

	log.Flush();

	return bRet;
}