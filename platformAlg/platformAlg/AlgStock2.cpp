#include "StdAfx.h"
#include "AlgStock2.h"
#include "Log.h"
#include "CaculateStock2.h"
#include "Global.h"
#include <time.h>

CAlgStock2::~CAlgStock2()
{

}



bool CAlgStock2::createNewAlg()
{
	CLog log;
	char buf[100] = { 0 };
	static int temp = 0;

	static char* tempString[]  = {
		"Level1",
		"Level2",
		"Level3"
	};

	sprintf_s(buf, sizeof(buf), "%lld_%d_%s", time(NULL),  ++temp, tempString[m_id-1]);
	
	log.Init("Stock2_Log\\" + std::string(buf));

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

	log.logRecord("\n筛选结果=====================");
	if (m_output.empty())
		log.logRecord("没有找到匹配股票\n");
	else{
		log.logRecord("筛选成功[%d]支股票:\n", m_output.size());
		int i = 0;
		for each (auto var in m_output)
		{
			if(i % 4 == 0 && i != 0){
				log.logRecord("\n");
			}
			log.logRecord("[%d-%s], ", var.first.market, var.first.stockcode.c_str());
		
			i++;
		}
		log.logRecord("\n");
	}
	log.Flush();
	return true;
}




