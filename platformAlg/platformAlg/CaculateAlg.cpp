#include "CaculateAlg.h"
#include "Log.h"
#include "time.h"  
#include "string.h"  


CCaculateAlg::CCaculateAlg()
{
}


CCaculateAlg::~CCaculateAlg()
{
}
typedef struct times
{
	int Year;
	int Mon;
	int Day;
	int Hour;
	int Min;
	int Second;
}Times;

char* stamp_to_standard(int stampTime, char* s)
{
	time_t tick = (time_t)stampTime;
	struct tm tm;
	//char s[100];
	Times standard;

	//tick = time(NULL);
	tm = *localtime(&tick);
	strftime(s, 32, "%Y-%m-%d", &tm);

	return s;
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
		LOG("均线参数不足");
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
	int nCount = 1;
	nPos = kLineData.size();
	for (int i = kLineData.size() - 1; i >= 0; --i)
	{
		
		double dAvg;
		
		bool bRet = get_avg(kLineData, i, avgFac, dAvg);


		if (!bRet && i < nMin)
		{
			return false;
		}
		if ( kLineData[i].close > dAvg )
		{
			if (nCount <= nMax)
			{
				
#ifdef _DEBUG
				char ss[32];
				stamp_to_standard(kLineData[i].time, ss);
				printf("single step 1 count %d %s close %f avg %f \n",nCount, ss, kLineData[i].close, dAvg);
#endif // _DEBUG
				nCount++;
			}
			else
			{
				nPos = i + 1;
				nKnb = nCount - 1 ;
#ifdef _DEBUG
				char ss[32];
				stamp_to_standard(kLineData[nPos].time, ss);
				printf("单平台的第二步的起始数据  count %d %s close %f avg %f \n",nKnb, ss, kLineData[nPos].close, dAvg);
#endif // _DEBUG
				
				return true;
				
			}
			
		}
		else
		{
			if (nCount > nMin )
			{
				nPos = i+1;
				nKnb = nCount;
				printf("count %d\n", nKnb);
				return true;
			}
			else
			{
				nCount = 1;
				continue;
			}
		}
		
	}
	LOG("平台第一步筛选失败 Pos[%d]", nPos);
	return false;
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
	int nEnd = (nPos + 8) > kLineData.size() - 1 ? kLineData.size()-1 : nPos + 8;
	int nJEnd = (nPos + 12) > kLineData.size() - 1 ? kLineData.size() - 1 : nPos + 12;
	for (int i = nPos; i <= nEnd; ++i)
	{
		for (int j = i + 1; j <= nJEnd; ++j)
		{
			if (kLineData[j].close < kLineData[i].high)
			{

				if (++nCount == 4)
				{
#ifdef _DEBUG
					char ss[32];
					stamp_to_standard(kLineData[j].time, ss);
					printf("step2 time %s close %f high %f\n", ss, kLineData[j].close, kLineData[i].high);
#endif // _DEBUG
					nPos = j - 4;

					stamp_to_standard(kLineData[nPos].time, ss);
					LOG("平台第二步筛选成功 date[%s]", ss);
					return true;
				}
			}
			else
			{
				nCount = 0;
				break;
			}
		}
	
	}
	char ss[32];
	stamp_to_standard(kLineData[nPos].time, ss);
	LOG("平台第二步筛选失败 Pos[%s]", ss);
	return false;
}

bool CCaculateAlg::single_plat_step_third(const std::vector<tagKline>& kLineData, int& nPos)
{
	int nEnd = kLineData.size() - 2;
#ifdef _DEBUG
	printf("pos %d\n",nPos+5);
#endif // _DEBUG

	for (int i = nPos + 5; i <= nEnd; ++i)
	{
#ifdef _DEBUG
		char ss[32];
		stamp_to_standard(kLineData[i].time, ss);
		printf(" 3 time %s close %f high %f\n",ss, kLineData[i].close, kLineData[nPos].high);
#endif // DEBUG

		if (kLineData[i].close < kLineData[nPos].high)
		{
			continue;
		}
		else
		{
			return false;
		}
	}
	char ss[32];
	stamp_to_standard(kLineData[nPos].time, ss);
	LOG("单平台第三步筛选成功 date[%s]", ss);
	return true;
}



bool CCaculateAlg::is_fairing(const std::vector<tagKline>& kLineData, int& nPos, bool isFiring)
{
//起爆选项为true时返回判断，否则默认成功
	int size = kLineData.size();
	bool firRes = true;
	if (isFiring)
	{
		firRes = kLineData[size - 1].close > kLineData[nPos].high ? true : false;
		
	}
	if (firRes)
	{
		char ss[32];
		stamp_to_standard(kLineData[size-1].time, ss);
		LOG("起爆成功 date %s close %f high %f ", ss, kLineData[size - 1].close, kLineData[nPos].high);
	}
	else
	{
		LOG("起爆失败");
	}
	return firRes;
	
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

		bRet = single_plat_step_third(vecKline, nPos);
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

bool CCaculateAlg::multi_step_third(const std::vector<tagKline>& kLineData, int& nPos)
{
	/*
	若符合CLOSE(N+i)>HIGH(N)
	记录：M=i （此步骤定位了M数据）
	*/
	int nEnd = kLineData.size() - 2;
	for (int i = nPos + 5; i <= nEnd; ++i)
	{

		if (kLineData[i].close > kLineData[nPos].high)
		{
			nPos = i;
			char ss[32];
			stamp_to_standard(kLineData[nPos].time, ss);
			LOG("多平台第三步筛选成功 date[%s]", ss);
			return true;
		}
		else
		{
			continue;
		}
	}
	char ss[32];
	stamp_to_standard(kLineData[nPos].time, ss);
	LOG("多平台第三步筛选失败 date[%s]", ss);
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
		for (int j = i + 1; j < nEnd; ++j)
		{
			if (kLineData[j].close < kLineData[i].high)
			{
				if (++nCount == 4)
				{
					nPos = j - 4;

					char ss[32];
					stamp_to_standard(kLineData[i].time, ss);
					LOG("多平台第四步筛选成功 date[%s]", ss);
					#ifdef _DEBUG
					printf("多平台第四步筛选成功 date[%s]\n", ss);
					#endif
					return true;
				}
			}
			else
			{
				nCount = 0;
				break;
			}
		}
		
	}

	char ss[32];
	stamp_to_standard(kLineData[nPos].time, ss);
	LOG("多平台第四步筛选失败 date[%s]", ss);
	return false;
}

bool CCaculateAlg::multi_step_fifth(const std::vector<tagKline>& kLineData, int& nPos)
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
			char ss[32];
			stamp_to_standard(kLineData[nPos].time, ss);
			LOG("多平台第五步筛选失败 date[%s]", ss);

#ifdef _DEBUG
			printf("多平台第五步筛选失败 date[%s]", ss);
#endif // _DEBUG

			return false;
		}
	}

	char ss[32];
	stamp_to_standard(kLineData[nPos].time, ss);
	LOG("多平台第五步筛选成功 date[%s]", ss);

#ifdef _DEBUG
	printf("多平台第五步筛选成功 date[%s]", ss);
#endif // _DEBUG

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
		if (vecKline.size()  < unsigned(avgFac) )
		{
			//均线参数不足，满足大于等于nMin根K线，并且不超过nMax根k线收盘价>均线值（参数）
			LOG("K线数量 %d < 均线参数 %d", vecKline.size(), avgFac);
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
		//只有N+4 失败
		if (nPos + 4 == vecKline.size())
		{
			continue;
		}
		
		bRet = multi_step_third(vecKline, nPos);
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
	}
	return false;
}


