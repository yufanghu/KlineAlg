#include "AlgBase.h"
#include "stock2.h"
#include "Log.h"
#include <map>

CAlgBase::CAlgBase()
{
}


CAlgBase::~CAlgBase()
{
}

bool CAlg3::createNewAlg()
{

}

CAlg3::CAlg3(std::map<tagStockCodeInfo, std::vector<tagKline>> &inMap, std::map<tagStockCodeInfo, tagOutput> & output, TFirstFilter& tFirFilter, int id)
{
	CLog log;
	log.Init("platform2_log");
	CFilter2Alg calAlg;
	calAlg.SetLogObj(&log);
	
	switch (id)
	{
	case 1:
		calAlg.filter2Level1(inMap, output, tFirFilter);
	case 2:
	{
		TSecondFilter filter;
		filter.tLineNum = tFirFilter.tLineNum;
		filter.bA5Switch = tFirFilter.bA5Switch;
		filter.sCallbackRange = tFirFilter.sCallbackRange;
		calAlg.filter2Level2(inMap, output, filter);

	}
		break;
	case 3:
	{
		TThirdFilter sfilter;
		sfilter.tLineNum = tFirFilter.tLineNum;
		sfilter.bA5Switch = tFirFilter.bA5Switch;
		sfilter.sCallbackRange = tFirFilter.sCallbackRange;
		calAlg.filter2Level3(inMap, output, sfilter);
	}
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
	log.Flush(e_doc_3, "platform2_log");
}

CAlg3::CAlg3(std::map<tagStockCodeInfo, std::vector<tagKline>> &inMap, std::map<tagStockCodeInfo, tagOutput> & output, TFirstFilter& tFirFilter)
{
	m_step = eStep1;
}

CAlg3::CAlg3(std::map<tagStockCodeInfo, std::vector<tagKline>> &inMap, std::map<tagStockCodeInfo, tagOutput> & output, TSecondFilter& tFirFilter)
{
	m_step = eStep2;
}

CAlg3::CAlg3(std::map<tagStockCodeInfo, std::vector<tagKline>> &inMap, std::map<tagStockCodeInfo, tagOutput> & output, TThirdFilter& tFirFilter)
{
	m_step = eStep3;
}
