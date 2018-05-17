#include "StdAfx.h"
#include "PlatFormAlgorithm.h"
#include "CaculateAlg.h"
#include "Log.h"
#include "ThreadPoolExecutor.h"

CPlatFormAlgorithm::CPlatFormAlgorithm()
{
}

CPlatFormAlgorithm::~CPlatFormAlgorithm()
{
}

class CThreadPlatForm : public Runnable
{
public:
	CThreadPlatForm(const std::pair<tagStockCodeInfo, std::vector<tagKline>> input, std::map<tagStockCodeInfo, tagOutput>& output, short avgFac,
		EPlatFormType platformType, bool bFiring):
		m_input(input),
		m_output(&output),
		m_avgFac(avgFac),
		m_platformType(platformType),
		m_bFiring(bFiring)
	{
	}
	void Run()
	{
		CCaculateAlg calAlg;
		//遍历map开始筛选
		switch (m_platformType)
		{
			case
			::eSinglePlatForm:
				calAlg.single_plat(m_input, m_output, m_avgFac, m_bFiring);
				break;
				case
			::eDoublePlatForm:
				calAlg.double_plat(m_input, m_output, m_avgFac, m_bFiring);
				break;
			default:
				break;
		}
	}
private:
	const std::pair<tagStockCodeInfo, std::vector<tagKline>> m_input;
	std::map<tagStockCodeInfo, tagOutput>* m_output;
	short m_avgFac;
	EPlatFormType m_platformType;
	bool m_bFiring;
};


bool CPlatFormAlgorithm::select_entrance(const std::map<tagStockCodeInfo, std::vector<tagKline>>& input, std::map<tagStockCodeInfo, tagOutput>& output,
	                             EPlatFormType platformType, short avgFac, bool bFiring)
{
	CThreadPoolExecutor * pExecutor = new CThreadPoolExecutor();
	pExecutor->Init(1, 10, 50);
	
	std::map<tagStockCodeInfo, std::vector<tagKline>>::iterator iter;
	std::map<tagStockCodeInfo, std::vector<tagKline>> mapInput = input;
	

	for (iter = mapInput.begin(); iter != mapInput.end(); ++iter)
	{
		
		tagStockCodeInfo tagOne = iter->first;
		std::vector<tagKline> vecKline = iter->second;

		std::pair<tagStockCodeInfo, std::vector<tagKline>> pair_input(tagOne,vecKline);
		CThreadPlatForm *pPlat = new CThreadPlatForm(pair_input, output, avgFac, platformType, bFiring);
		while (!pExecutor->Execute(pPlat))
		{

		}
		//delete pPlat;

	}
	pExecutor->Terminate();
	delete pExecutor;
	LOG_FLUSH();
	return false;
}
