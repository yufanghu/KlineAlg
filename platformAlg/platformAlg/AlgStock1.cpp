#include "AlgStock1.h"
#include "Log.h"
#include "CaculateStock2.h"

CAlgStock1::~CAlgStock1()
{

}

CAlgStock1::CAlgStock1(const std::map<tagStockCodeInfo, std::vector<tagKline>> & inMap, std::map<tagStockCodeInfo, outKline> & outMap) : 
m_inMap(inMap), m_output(outMap)
{

}


bool CAlgStock1::createNewAlg()
{
	return true;
}




