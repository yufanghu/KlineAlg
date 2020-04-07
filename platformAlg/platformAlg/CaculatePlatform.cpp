#include "StdAfx.h"
#include "CaculatePlatform.h"
#include "Log.h"
#include <time.h>  
#include <string.h>  
#include "StockAlgorithm.h"
#include "Global.h"
typedef struct times
{
	int Year;
	int Mon;
	int Day;
	int Hour;
	int Min;
	int Second;
}Times;



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
	if (nStart + 1 - nCount  < 0 || nCount <= 0)
	{
		//不合法数据
		m_pLog->logRecord("计算均线参数不足\n");
		return false;
	}
	for (i = 0; i != nCount; ++i)
	{
		dSum += kLineData[nStart-i].close;
	}
	if (i == nCount)
	{
		dAvg = dSum / nCount;
		//LOG("均线值%d", dAvg);
		return true;
	}

	return false;

}

bool CCaculateAlg::single_double_step_one(const std::vector<tagKline>& kLineData, short avgFac,
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

		if (!bRet)
		{
			return false;
		}

		if (kLineData[i].close > dAvg)
		{
			if (nCount <= nMax)
			{
				std::string buf;
				if(!stamp_to_standard(kLineData[i].time, buf))
				{
					m_pLog->logRecord("k线时间戳无效 pos[%d]-time[%I64d]\n", i, kLineData[i].time);
					return false;
				}
				m_pLog->logRecord("step 1[%d] %s close %f avg %f \n", nCount, buf.c_str(), kLineData[i].close, dAvg);
				nCount++;
			}
			else
			{
				m_pLog->logRecord("step 1筛选失败 超过[%d]根（第%d根）满足条件\n", nMax, nMax+1);
				return false;
			}

		}
		else
		{
			if (nCount >= nMin)
			{
				nPos = i + 1;
				nKnb = nCount;
				m_pLog->logRecord("step 1筛选成功 knb[%d]\n", nKnb - 1);
				return true;
			}
			else if (i == kLineData.size() - 1)
			{
				m_pLog->logRecord("step 1筛选失败 第一根数据不满足\n");
				return false;
			}
			else
			{
				nCount = 1;
				m_pLog->logRecord("step 1筛选失败 小于最小根数（%d）\n",nMin);
				return false;
			}
		}

	}
	m_pLog->logRecord("平台第一步筛选失败 Pos[%d]\n", nPos);
	return false;
}

bool CCaculateAlg::single_double_step_two(const std::vector<tagKline>& kLineData, int& nPos)
{
	/*
	FOR(N=1;N<=8;N++)
	如果连续N+1….N+4 数据收盘价都低于N数据最高价。
	并且N<=8 ，记录N值。（此步骤定位了N数据所在K线）
	那么该过程 ，记录N值。（此步骤定位了N数据所在K线）
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
		nCount = 0;
		for (int j = i + 1; j <= nJEnd; ++j)
		{
			nPos = i;
			if (kLineData[j].close < kLineData[i].high)
			{

				if (++nCount == 4)
				{
					std::string ss;

					nPos = j - 4;
					if(!stamp_to_standard(kLineData[nPos].time, ss))
					{
						m_pLog->logRecord("k线时间戳无效 pos[%d]-time[%I64d]\n", nPos, kLineData[nPos].time);
						return false;
					}

					m_pLog->logRecord("step2筛选成功 date[%s] close %f high %f\n", ss.c_str(), kLineData[nPos].close, kLineData[nPos].high);
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


	std::string ss;
	if(!stamp_to_standard(kLineData[nPos].time, ss))
	{
		m_pLog->logRecord("k线时间戳无效 pos[%d]-time[%I64d]\n", nPos, kLineData[nPos].time);
		return false;
	}
	m_pLog->logRecord("平台第二步筛选失败 N[%s]\n", ss.c_str());

	return false;
}

bool CCaculateAlg::single_plat_step_third(const std::vector<tagKline>& kLineData, int& nPos)
{
	int nEnd = kLineData.size() - 2;

	for (int i = nPos + 5; i <= nEnd; ++i)
	{
		std::string ss;
	if(!stamp_to_standard(kLineData[i].time, ss))
	{
		m_pLog->logRecord("k线时间戳无效 pos[%d]-time[%I64d]\n", i, kLineData[i].time);
		return false;
	}

		//stamp_to_standard(kLineData[i].time, ss);
		
		if (kLineData[i].close < kLineData[nPos].high)
		{
			m_pLog->logRecord("step3 %s close %f 小于 high %f\n", ss.c_str(), kLineData[i].close, kLineData[nPos].high);
			continue;
		}
		else
		{
			m_pLog->logRecord("step3 %s close %f 不小于 high %f\n， 第三步失败", ss.c_str(), kLineData[i].close, kLineData[nPos].high);
			return false;
		}
	}
	
	std::string ss;
	if(!stamp_to_standard(kLineData[nPos].time, ss))
	{
		m_pLog->logRecord("k线时间戳无效 pos[%d]-time[%I64d]\n",nPos, kLineData[nPos].time);
		return false;
	}


	m_pLog->logRecord("单平台第三步筛选成功[%s]\n", ss.c_str());
	return true;
}

bool CCaculateAlg::is_fairing(const std::vector<tagKline>& kLineData, int& nPos, bool isFiring)
{
//起爆选项为true时返回判断，否则默认成功
	int size = kLineData.size();
	bool firRes = true;
	if (isFiring)
	{
		if((nPos+4) == size -1){
			m_pLog->logRecord("起爆失败\n");
			return false;
		}

		firRes = kLineData[size - 1].close > kLineData[nPos].high ? true : false;
		if (firRes)
		{
			
			std::string ss;
			if(!stamp_to_standard(kLineData[size - 1].time, ss))
			{
				m_pLog->logRecord("k线时间戳无效 pos[%d]-time[%I64d]\n",size - 1, kLineData[size - 1].time);
				return false;
			}

			m_pLog->logRecord("起爆成功 %s close %f high %f\n", ss, kLineData[size - 1].close, kLineData[nPos].high);
			return true;
		}
		else
		{
			m_pLog->logRecord("起爆失败\n");
			return false;
		}
	}
	else
	{
		m_pLog->logRecord("起爆关闭，筛选成功\n");
		return true;
	}

}

bool CCaculateAlg::double_step_third(const std::vector<tagKline>& kLineData, int& nPos)
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
			std::string ss;
			if(!stamp_to_standard(kLineData[nPos].time, ss))
			{
				m_pLog->logRecord("k线时间戳无效 pos[%d]-time[%I64d]\n",nPos, kLineData[nPos].time);
				return false;
			}

			//char ss[32];
			//stamp_to_standard(kLineData[nPos].time, ss);
			m_pLog->logRecord("双平台第三步筛选成功 [%s]\n", ss.c_str());
			return true;
		}
		else
		{
			continue;
		}
	}

	std::string ss;
	if(!stamp_to_standard(kLineData[nPos].time, ss))
	{
		m_pLog->logRecord("k线时间戳无效 pos[%d]-time[%I64d]\n",nPos, kLineData[nPos].time);
		return false;
	}


	m_pLog->logRecord("双平台第三步筛选失败 [%s]\n", ss.c_str());
	return false;
}

bool CCaculateAlg::double_step_fourth(const std::vector<tagKline>& kLineData, int& nPos)
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
		nCount = 0;
		for (int j = i + 1; j < nEnd; ++j)
		{
			if (kLineData[j].close < kLineData[i].high)
			{
				if (++nCount == 4)
				{
					nPos = j - 4;

					//char ss[32];
					//stamp_to_standard(kLineData[i].time, ss);

					std::string ss;
					if(!stamp_to_standard(kLineData[i].time, ss))
					{
						m_pLog->logRecord("k线时间戳无效 pos[%d]-time[%I64d]\n",nPos, kLineData[i].time);
						return false;
					}

					m_pLog->logRecord("双平台第四步筛选成功 [%s]\n", ss.c_str());
					return true;
				}
			}
			else
			{
				
				break;
			}
		}
		
	}

	std::string ss;
	if(!stamp_to_standard(kLineData[nPos].time, ss))
	{
		m_pLog->logRecord("k线时间戳无效 pos[%d]-time[%I64d]\n",nPos, kLineData[nPos].time);
		return false;
	}

	//char ss[32];
	//stamp_to_standard(kLineData[nPos].time, ss);
	m_pLog->logRecord("双平台第四步筛选失败 [%s]\n", ss.c_str());
	return false;
}

bool CCaculateAlg::double_step_fifth(const std::vector<tagKline>& kLineData, const int& nPos)
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
		/*	char ss[32];
			stamp_to_standard(kLineData[nPos].time, ss);*/
			std::string ss;
			if(!stamp_to_standard(kLineData[nPos].time, ss))
			{
				m_pLog->logRecord("k线时间戳无效 pos[%d]-time[%I64d]\n",nPos, kLineData[nPos].time);
				return false;
			}

			m_pLog->logRecord("双平台第五步筛选失败 [%s]\n", ss.c_str());
			return false;
		}
	}


	std::string ss;
	if(!stamp_to_standard(kLineData[nPos].time, ss))
	{
		m_pLog->logRecord("k线时间戳无效 pos[%d]-time[%I64d]\n",nPos, kLineData[nPos].time);
		return false;
	}

	m_pLog->logRecord("双平台第五步筛选成功 [%s]", ss.c_str());
	return true;
}

bool CCaculateAlg::double_plat(const std::map<tagStockCodeInfo, std::vector<tagKline>> & input,
	std::map<tagStockCodeInfo, tagOutput> & output, short avgFac, bool bFiring /*= false*/)
{
	std::string buf;
	stamp_to_standard(time(NULL), buf,"%Y-%m-%d %H:%M:%S");

	if (bFiring)
		m_pLog->logRecord("开始[%s]\n=========================\n双平台-均线参数[%d]起爆[开启]\n", buf.c_str(), avgFac);
	else
		m_pLog->logRecord("开始[%s]\n=========================\n双平台-均线参数[%d]起爆[关闭]\n", buf.c_str(), avgFac);

	std::map<tagStockCodeInfo, std::vector<tagKline>>::const_iterator iter;
	//std::map<tagStockCodeInfo, std::vector<tagKline>> mapInput = input;


	int nPos = 0;  //满足条件的K线位置
	bool bRet = false;
	int nKnb = 0;
	int nIndex = 0;
	for (iter = input.begin(), nIndex = 0; iter != input.end(); ++iter)
	{
		if(iter->second.size() > 0){
			tagKline temp = iter->second.at(iter->second.size()-1);
			buf="";
			stamp_to_standard(temp.time, buf, "%Y%m%d");
			if (strcmp(buf.c_str(), DEADLINE_DATE) >= 0)
			{
				m_pLog->clearLog();
				m_pLog->logRecord("程序试用期已经结束，程序退出");
				return true;
			}
		}

		PrintHead(iter->first, ++nIndex);
		PrintData(iter->second);
		tagStockCodeInfo tagOne = iter->first;
		//K线数据
		const std::vector<tagKline>  & vecKline = iter->second;
		if (vecKline.size()  < unsigned(avgFac) )
		{
			//均线参数不足，满足大于等于nMin根K线，并且不超过nMax根k线收盘价>均线值（参数）
			m_pLog->logRecord("均线[%d]参数不足, 只有[%d]根k线\n", avgFac, vecKline.size());
			continue;
		}

		bRet = single_double_step_one(vecKline, avgFac, nPos, nKnb, 25, 10);
		if (!bRet)
		{
			//第一步失败
			continue;
		}
		bRet = single_double_step_two(vecKline, nPos);
		
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
		
		bRet = double_step_third(vecKline, nPos);
		if (!bRet)
		{
			//第三步失败
			continue;
		}
		bRet = double_step_fourth(vecKline, nPos);
		if (!bRet)
		{
			//第四步失败
			continue;
		}

		if ( nPos + 4 == vecKline.size() - 1)
		{
			if(bFiring){
				m_pLog->logRecord("起爆失败,起爆数据不足\n");
				continue;
			}
			else{
				output[tagOne] = tagOutput();
				continue;
			}
			
		}
		bRet = double_step_fifth(vecKline, nPos);
		if (!bRet)
		{
			//第五步失败
			continue;
		}

		bRet = is_fairing(vecKline, nPos, bFiring);
		if (bRet)
		{
			//筛选成功
			output[tagOne] = tagOutput();
		}
	}
	return false;
}


bool CCaculateAlg::single_plat(const std::map<tagStockCodeInfo, std::vector<tagKline>> & input, std::map<tagStockCodeInfo,
	tagOutput> & output, short avgFac, bool bFiring /*= false*/)
{
	std::string buf;
	stamp_to_standard(time(NULL), buf, "%Y-%m-%d %H:%M:%S");
	if (bFiring)
		m_pLog->logRecord("开始[%s]\n=========================\n单平台-均线参数[%d]起爆[开启]\n", buf.c_str(), avgFac);
	else
		m_pLog->logRecord("开始[%s]\n=========================\n单平台-均线参数[%d]起爆[关闭]\n", buf.c_str(), avgFac);

	std::map<tagStockCodeInfo, std::vector<tagKline>>::const_iterator iter;
	//std::map<tagStockCodeInfo, std::vector<tagKline>> mapInput = input;

	int nPos = 0;  //满足条件的K线位置
	bool bRet = false;
	int nKnb = 0;
	int nIndex = 0;
	for (iter = input.begin(), nIndex = 0; iter != input.end(); ++iter)
	{
		if(iter->second.size() > 0){
			tagKline temp = iter->second.at(iter->second.size()-1);
			if (!CheckLicense(temp.time)){
				m_pLog->clearLog();
				m_pLog->logRecord("程序试用期已经结束，程序退出\n");
				return true;
			}				
		}
		PrintHead(iter->first, ++nIndex);
		PrintData(iter->second);

		tagStockCodeInfo tagOne = iter->first;
		//K线数据
		const std::vector<tagKline> vecKline = iter->second;
		if (vecKline.size() - 5 < avgFac)
		{
			//均线参数不足，满足大于等于5根K线，并且不超过15根k线收盘价>均线值（参数）
			m_pLog->logRecord("均线[%d]参数不足, 只有[%d]根k线\n", avgFac, vecKline.size());
			continue;
		}

		bRet = single_double_step_one(vecKline, avgFac, nPos, nKnb, 15, 5);
		if (!bRet)
		{
			//第一步失败
			continue;
		}
		bRet = single_double_step_two(vecKline, nPos);
		if (!bRet)
		{
			//第二步失败
			continue;
		}
		if ( nPos + 4 == vecKline.size() - 1)
		{
			if(bFiring){
				m_pLog->logRecord("起爆失败,起爆数据不足\n");
				continue;
			}
			else{
				output[tagOne] = tagOutput();
				continue;
			}
			
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
			output[tagOne] = tagOutput();
		}
	}

	return false;
}

void CCaculateAlg::PrintData(const std::vector<tagKline>& kLineData)
{
	auto it = kLineData.begin();
	for (; it != kLineData.end(); ++it) {
		m_pLog->dataRecord("%lld,%f,%f,%f,%f,\n", it->time, it->open, it->high, it->low, it->close);
	}
	//printf("读取%d行数据,开 高 低 收\n\n", kLineData.size());
}


void CCaculateAlg::PrintHead(const tagStockCodeInfo & codeInfo, int nPos)
{
	m_pLog->dataRecord("%d-%s\n", codeInfo.market, codeInfo.stockcode.c_str());
	m_pLog->logRecord("第%d支股票[%d-%s]\n", nPos, codeInfo.market, codeInfo.stockcode.c_str());
}