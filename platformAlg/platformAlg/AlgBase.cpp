#include "AlgBase.h"
#include "stock2.h"
#include "CaculateAlg.h"
#include "Log.h"
#include <map>

CAlgBase::CAlgBase()
{
}


CAlgBase::~CAlgBase()
{
}

bool CAlg2::createNewAlg()
{
	CLog log;
	log.Init("platform2_log");
	CFilter2Alg calAlg;
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
	log.Flush(e_doc_3, "platform2_log");
	return true;
}



CAlg2::~CAlg2()
{

}


bool CAlg3::createNewAlg()
{
	CLog log;
	log.Init("platform3_log");
	CCaculateAlg calAlg;
	calAlg.SetLogObj(&log);
	bool bRet = false;
	//for (int nCal = 0; nCal != input.size(); ++nCal)
	{
		//遍历map开始筛选
		switch (m_platformType)
		{
			case
				::eSinglePlatForm:
					bRet = calAlg.single_plat(m_input, m_output, m_avgFac, m_bFiring);
					if (bRet)
					{
						log.Flush(e_doc_3, "platform3_log");
						return false;
					}

					break;
					case
						::eDoublePlatForm:
							bRet = calAlg.double_plat(m_input, m_output, m_avgFac, m_bFiring);
							if (bRet)
							{
								log.Flush(e_doc_3, "platform3_log");
								return false;
							}

							break;
					default:
						break;
		}

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
	log.Flush(e_doc_3, "platform3_log");
	return false;
}
