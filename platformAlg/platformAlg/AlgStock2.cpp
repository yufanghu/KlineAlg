#include "AlgStock2.h"
#include "Log.h"
#include "CaculateStock2.h"

CAlgStock2::~CAlgStock2()
{

}



bool CAlgStock2::createNewAlg()
{
	CLog log;
	log.Init("Stock2_Log");

	CCaculateSotck2 calAlg;
	calAlg.SetLogObj(&log);

	m_output.clear();
	switch (m_id)
	{
	case 1:
	{
		calAlg.filter2Level1(m_inMap, m_output, m_tFirFilter);

	}
		break;
	case 2:
	{

		calAlg.filter2Level2(m_inMap, m_output, m_tSecFilter);

	}
		break;
	case 3:
	{

		calAlg.filter2Level3(m_inMap, m_output, m_tTirFilter);
	}
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
	return true;
}




