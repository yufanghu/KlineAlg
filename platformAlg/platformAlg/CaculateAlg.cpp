#include "CaculateAlg.h"


CCaculateAlg::CCaculateAlg()
{
}


CCaculateAlg::~CCaculateAlg()
{
}


bool CCaculateAlg::get_avg(const std::vector<tagKline>& kLineData, int nStart,
	int nCount, double& dAvg)
{
//计算均线值
	double dSum = 0.0;
	int i;
	if (nStart - nCount < 0 || nCount <= 0)
	{
		//不合法数据
		return false;
	}
	for (i = 0; i != nCount; ++i)
	{
		dSum += kLineData[nStart-i].close;
	}
	if (i == nCount)
	{
		dAvg = dSum / nCount;
		return true;
	}

	return false;

}

bool CCaculateAlg::single_multi_step_one(const std::vector<tagKline>& kLineData, short avgFac,
	int& nPos,int& nKnb,int nMax, int nMin)
{
	/*
	从数据末尾向前筛选
	条件：满足大于等于5根K线，并且不超过15根k线收盘价 > 均线值（参数）
	记录数据K线数目：KNB, 从第1根收盘价高于均线算起，比如筛选结果为21根。
	不符合该股票数据筛选结束，返回空值
	*/
	int nCount = 0;
	nPos = kLineData.size();
	for (int i = kLineData.size() - 1; i >= 0; --i)
	{
		double dAvg;
		bool bRet = get_avg(kLineData, i, avgFac, dAvg);
		if (!bRet)
		{
			return false;
		}
		if ( kLineData[i].close > dAvg )
		{
			if (nCount <= nMax)
			{
				nCount++;
			}
			else
			{
				if (nCount > nMin)
				{
					nPos = i + 1;
					nKnb = nCount - 1;
					return true;
				}
				
			}
			
		}
		else
		{
			if (nCount > nMin )
			{
				nPos = i+1;
				nKnb = nCount;
				return true;
			}
			else
			{
				nCount = 0;
				continue;
			}
		}
		
	}
	
	return true;
}



bool CCaculateAlg::single_multi_step_two(const std::vector<tagKline>& kLineData, int& nPos)
{
	/*
	FOR(N=1;N<=8;N++)
	如果连续N+1….N+4 数据收盘价都低于N数据最高价。
	并且N<=8 ，记录N值。（此步骤定位了N数据所在K线）
	那么该过程结束，进入第3步数据筛选。
	否则：该股票数据筛选结束，返回空值
	*/
	if (nPos == kLineData.size())
	{
		return false;
	}
	int nCount = 0;
	int nEnd = (nPos + 8) > kLineData.size() - 1 ? kLineData.size() : nPos + 8;
	for (int i = nPos + 1; i <= nEnd; ++i)
	{
		if ( kLineData[i].close < kLineData[nPos].high)
		{
			if (++nCount == 4)
			{
				nPos = i - 4;
				return true;
			}
		}
		else
		{
			nCount = 0;
		}
	}
	return true;
}

bool CCaculateAlg::single_multi_step_three(const std::vector<tagKline>& kLineData, int& nPos,bool isMulti)
{
	int nEnd =  kLineData.size() - 2 ;
	for (int i = nPos + 5; i <= nEnd; ++i)
	{

		if ( (!isMulti && kLineData[i].close < kLineData[nPos].high)
			|| (isMulti && kLineData[i].close > kLineData[nPos].high ))
		{
			continue;
		}
		else
		{
			nPos = i - 1;
			return false;
		}
	}
	
	return true;
}

bool CCaculateAlg::is_fairing(const std::vector<tagKline>& kLineData, int& nPos, bool isFiring)
{
//起爆选项为true时返回判断，否则默认成功
	int size = kLineData.size();
	if (isFiring)
	{
		return kLineData[size - 1].close > kLineData[nPos].high ? true : false;
	}
	return true;
	
}

bool CCaculateAlg::single_plat(const std::map<tagStockCodeInfo, std::vector<tagKline>> & input, std::map<tagStockCodeInfo,
						tagOutput> & output, EPlatFormType platformType, short avgFac, bool bFiring /*= false*/)
{
	std::map<tagStockCodeInfo, std::vector<tagKline>>::iterator iter ;
	std::map<tagStockCodeInfo, std::vector<tagKline>> mapInput = input;
	int nPos = 0;  //满足条件的K线位置
	bool bRet = false;
	int nKnb = 0;
	for (iter = mapInput.begin(); iter != mapInput.end(); ++iter)
	{
		tagStockCodeInfo tagOne = iter->first;
		//K线数据
		std::vector<tagKline> vecKline = iter->second;
		if (vecKline.size() - 5 < avgFac)
		{
			//均线参数不足，满足大于等于5根K线，并且不超过15根k线收盘价>均线值（参数）
			return false;
		}
		

		bRet = single_multi_step_one(vecKline, avgFac, nPos, nKnb,15, 5);
		if (!bRet)
		{
			//第一步失败
			continue;
		}
		bRet = single_multi_step_two(vecKline, nPos);
		if (!bRet)
		{
			//第二步失败
			continue;
		}
		if (nPos + 4 == vecKline.size() || nPos + 4 == vecKline.size() -1 )
		{
			output[tagOne] = {};
			continue;
		}

		bRet = single_multi_step_three(vecKline, nPos);
		if (!bRet)
		{
			continue;
		}
		bRet = is_fairing(vecKline, nPos, bFiring);
		if (bRet)
		{
			//筛选成功
			output[tagOne] = {};
		}
	}

	return false;
}

bool CCaculateAlg::multi_step_fourth(const std::vector<tagKline>& kLineData, int& nPos)
{
	int nEnd = kLineData.size();
	//KNB - M <4 结束筛选
	if (nEnd - nPos < 4)
	{
		return false;
	}
	int nCount = 0;
	for (int i = nPos; i < nEnd; ++i)
	{
		if (kLineData[i].close < kLineData[nPos].high)
		{
			if (++nCount == 4)
			{
				nPos = i - 4;
				return true;
			}
		}
		else
		{
			nCount = 0;
		}
	}
	return true;
}

bool multi_step_fifth(const std::vector<tagKline>& kLineData, int& nPos)
{
	int nEnd = kLineData.size() - 2;
	for (int i = nPos + 5; i <= nEnd; ++i)
	{

		if ( kLineData[i].close < kLineData[nPos].high)
		{
			continue;
		}
		else
		{
			return false;
		}
	}

	return true;
}
bool CCaculateAlg::multi_plat(const std::map<tagStockCodeInfo, std::vector<tagKline>> & input,
	std::map<tagStockCodeInfo, tagOutput> & output, EPlatFormType platformType,
	short avgFac, bool bFiring /*= false*/)
{
	std::map<tagStockCodeInfo, std::vector<tagKline>>::iterator iter;
	std::map<tagStockCodeInfo, std::vector<tagKline>> mapInput = input;
	int nPos = 0;  //满足条件的K线位置
	bool bRet = false;
	int nKnb = 0;
	for (iter = mapInput.begin(); iter != mapInput.end(); ++iter)
	{
		tagStockCodeInfo tagOne = iter->first;
		//K线数据
		std::vector<tagKline> vecKline = iter->second;
		if (vecKline.size() - 10 < avgFac)
		{
			//均线参数不足，满足大于等于10根K线，并且不超过25根k线收盘价>均线值（参数）
			return false;
		}

		bRet = single_multi_step_one(vecKline, avgFac, nPos, nKnb, 25, 10);
		if (!bRet)
		{
			//第一步失败
			continue;
		}
		bRet = single_multi_step_two(vecKline, nPos);
		if (!bRet)
		{
			//第二步失败
			continue;
		}
		//待确认 若N+5为最后一根是否算成功
		//只有N+4 失败
		if (nPos + 4 == vecKline.size())
		{
			continue;
		}
		bRet = single_multi_step_three(vecKline, nPos);
		if (!bRet)
		{
			//第三步失败
			continue;
		}
		bRet = multi_step_fourth(vecKline, nPos);
		if (!bRet)
		{
			//第四步失败
			continue;
		}
		bRet = multi_step_fifth(vecKline, nPos);
		if (!bRet)
		{
			//第五步失败
			continue;
		}

		bRet = is_fairing(vecKline, nPos, bFiring);
		if (bRet)
		{
			//筛选成功
			output[tagOne] = {};
		}

	return false;
}


