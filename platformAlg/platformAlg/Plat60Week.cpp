#include "Plat60Week.h"
#include "Global.h"
#include <string>
#include <cmath>
using namespace std;


int minVal = 0.00001;

CPlat60Week::CPlat60Week(const std::vector<tagKline>& inStockKlines, std::vector<tagQuota> & quota)
	:m_inStockKlines(inStockKlines),
	m_quota(quota)
{


}

CPlat60Week::~CPlat60Week(void)
{
}



bool CPlat60Week::createNewAlg()
{
	CLog log;

	//sprintf_s(buf, sizeof(buf), "%lld_%d_%s", time(NULL), ++temp, tempString[m_id - 1]);
	log.Init("PlatForm4_Log");

	SetLogObj(&log);
	loopEntrance();
	log.Flush();
	return true;
}

bool CPlat60Week::get_avg()
{
	//计算60周均线
	int nCount = 60;

	int i;
	if (nCount <= 0)
	{
		//不合法数据
		m_pLog->logRecord("计算均线参数不足,根数[%d]\n",nCount);
		return false;
	}
	
	for (int k = 59; k < m_inStockKlines.size(); k++)
	{
		double dSum = 0.0;
		for (i = 0; i != 60; ++i)
		{
			dSum += m_inStockKlines[k - i].close;
		}
		if (i == 60)
		{
			string sTime;
			stamp_to_standard(m_inStockKlines[k].time, sTime);
			double avg1 = dSum / nCount ;
			avg1 = avg1 + 0.0051*(int(avg1/fabs(avg1)));
			double res = (int)(avg1 * 100) / 100.0;
			m_mapAvg[m_inStockKlines[k].time] = res;
			//m_pLog->logRecord("均线值%I64d  %s,%f %d\n", m_inStockKlines[k].time,sTime.c_str(), dSum / nCount,m_inStockKlines.size());
		}
	}



	return true;
}

bool CPlat60Week::loopStepA(int start, int& end)
{
	/*
	开始条件：收盘价首次大于或等于60周均线数值为一个循环起始点，该数据定义为数据A。
	从数据A开始向后筛选（包括数据A）：
	必要条件1：某一个K线高点后，至少连续4周收盘价小于或等于该高点，该高点定义为H1。
	必要条件2：从数据A开始，要满足所有数据收盘价高于或等于60周均线值，否则退出筛选循环A。
	必要条件3：要满足上述两个条件，并且后续收盘价突破（大于）高点H1，突破K线定义为H2.
	*/

	int lastPos = -1;
	int i = start;
	end = i;
	double & avg = m_mapAvg[m_inStockKlines[i].time];
	if (m_inStockKlines[i].close  < avg && fabs(avg - m_inStockKlines[i].close) > minVal)
	{
		return false;
	}

	double dHigh = m_inStockKlines[i].high;
	m_H1 = dHigh;
	//大于等于60周均线
	string sTime;
	stamp_to_standard(m_inStockKlines[i].time, sTime);
	m_pLog->logRecord("stepA 满足开始条件[时间=%I64d][%s][close=%f][avg=%f]\n", m_inStockKlines[i].time, 
		sTime.c_str(), m_inStockKlines[i].close, avg);
	//定义CA
	m_CA = m_inStockKlines[i].close;
	m_CATime = m_inStockKlines[i].time;
	if (i + 4 >= m_inStockKlines.size())
	{
		sTime.clear();
		stamp_to_standard(m_inStockKlines[i].time, sTime);
		m_pLog->logRecord("stepA 不足四周[%I64d][最大日期=%I64d][%s]\n", m_inStockKlines[i].time, m_inStockKlines.back().time,sTime.c_str());
		end = m_inStockKlines.size();
		return false;
	}

	int j = 1;
	for (; j <= 4; ++j)
	{
		double & avg = m_mapAvg[m_inStockKlines[i+j].time];
		if (m_inStockKlines[i + j].close < avg && fabs(avg - m_inStockKlines[i + j].close) > minVal)
		{
			sTime.clear();
			stamp_to_standard(m_inStockKlines[i+j].time, sTime);
			m_pLog->logRecord("stepA 收盘价格小于均价[日期=%I64d][%s][close=%f][avg=%f]\n", m_inStockKlines[i + j].time, sTime.c_str(), m_inStockKlines[i + j].close,
				avg);
			end = i + j;
			return false;
		}
		if (m_inStockKlines[i + j].close > dHigh && fabs(dHigh - m_inStockKlines[i+j].close) > minVal)
		{
			
			string sTime1;
			stamp_to_standard(m_inStockKlines[i+j].time, sTime1);
			string sTime2;
			stamp_to_standard(m_inStockKlines[end].time, sTime2);
			m_pLog->logRecord("stepA 不满足条件1 连续四周不大于高点[高点日期%I64d][%s][高点：%lf][当前日期=%I64d][%s][收盘价：%lf]\n", 
				m_inStockKlines[i+j].time, sTime1.c_str(), dHigh, m_inStockKlines[end].time, sTime2.c_str(), m_inStockKlines[i + j].close);
			end = i;
			return false;
		}
		//寻找最高收盘价格
		if (m_CA < m_inStockKlines[i + j].close && fabs(m_inStockKlines[i + j].close - m_CA) > minVal)
		{
			m_CA = m_inStockKlines[i + j].close;
			m_CATime = m_inStockKlines[i + j].time;
		}
	}
	//寻找突破h2  返回h2位置
	int l = 0;
	for (l = i + 4; l < m_inStockKlines.size(); ++l)
	{
		if (m_inStockKlines[l].close > dHigh && fabs(dHigh - m_inStockKlines[l].close) > minVal)
		{
			//h2突破
			//定义h2
			m_H2 = m_inStockKlines[l].close;
			//m_pLog->logRecord("stepA 成功 CA[%I64d][%f]，h1[%I64d][%f],h2[%I64d][%f]\n", m_CATime, m_CA, m_inStockKlines[i].time, dHigh, m_inStockKlines[k].time, m_H2);
			end = l;
			return true;
		}
		double & avg = m_mapAvg[m_inStockKlines[l].time];
		if (m_inStockKlines[l].close < avg && fabs(avg - m_inStockKlines[l].close) > minVal)
		{
			sTime.clear();
			stamp_to_standard(m_inStockKlines[l].time, sTime);
			m_pLog->logRecord("stepA 收盘价格小于均价11[日期=%I64d][%s][close=%f][avg=%f]\n", m_inStockKlines[l].time, sTime.c_str(), m_inStockKlines[l].close,
				avg);
			end = l;
			return false;
		}
		//寻找最高收盘价格
		if (m_CA < m_inStockKlines[l].close && fabs(m_inStockKlines[l].close - m_CA) > minVal)
		{
			m_CA = m_inStockKlines[l].close;
			m_CATime = m_inStockKlines[l].time;
		}
	}

	end = m_inStockKlines.size();
	return false;


}

int CPlat60Week::getPlatGrade(const int start, const int end, alg::EPlatLevel& level)
{
	int grade = end - start + 1;
	m_pLog->logRecord("平台根数[%d]\n", grade);
	if (grade >= 6 && grade <= 8)
	{
		level = EPlatLevel::eLevelOne;
		return 1;
	}
	else if (grade >= 9 && grade <= 16)
	{
		level = EPlatLevel::eLevelTwo;
		return 2;
	}
	else if (grade >= 17)
	{
		level = EPlatLevel::eLevelThree;
		return 3;
	}
	else
	{

		return -1;
	}

}

bool CPlat60Week::loopStepB(int start, int& end)
{
	/*
	开始条件：循环筛选A成功后续，从H2数据（突破K线）向后筛选。
	退出条件：1. 收盘价低于60均线  或  2. HB1 / H1 > 1.8
	*/

	bool bStepOne = loopStepBOne(start, end);
	if (!bStepOne)
	{
		return false;
	}

	/*
	选择条件：
	1、所有收盘价都高于CA数据（前一平台最高收盘价），进入第三阶段。
	2、有数据收盘价低于CA数据，进入第二阶段筛选。
	*/
	//
	int l = 0;
	for (l = end ; l < m_inStockKlines.size(); ++l)
	{
		if (m_inStockKlines[l].close > m_HB1 && fabs(m_inStockKlines[l].close - m_HB1) > minVal)
		{
			string sTime;
			stamp_to_standard(m_inStockKlines[l].time, sTime);
			m_pLog->logRecord("stepB 突破高点[%I64d][%s]\n", m_inStockKlines[l].time,sTime.c_str());
			break;
		}
		double & avg = m_mapAvg[m_inStockKlines[l].time];
		if (m_inStockKlines[l].close  < avg && fabs(avg - m_inStockKlines[l].close) > minVal)
		{
			string sTime;
			stamp_to_standard(m_inStockKlines[l].time, sTime);
			m_pLog->logRecord("stepB 收盘价格小于均价[日期=%I64d][%s][close=%f][avg=%f]\n", m_inStockKlines[l].time, sTime.c_str(), m_inStockKlines[l].close,
				avg);
			end = l;
			return false;
		}

	}

	

	int mx = (l >= m_inStockKlines.size() -1 ? m_inStockKlines.size() - 1 : l);
	int i = (end - 4 >= 0 )? end - 4 : 0;
	for (; i <= mx; ++i)
	{
		if (m_inStockKlines[i].close < m_CA && fabs(m_inStockKlines[l].close - m_CA) > minVal)
		{
			string sTime2;
			stamp_to_standard(m_inStockKlines[i].time, sTime2);
			m_pLog->logRecord("stepB 进入阶段2 小于CA[%f][%I64d][%s][%f]\n", m_CA, m_inStockKlines[i].time, sTime2.c_str(), m_inStockKlines[i].close);
			break;
		}
	}
	
	if (i <= mx)
	{
		
		if (mx < m_inStockKlines.size())
		{
			string sTime2;
			stamp_to_standard(m_inStockKlines[i].time, sTime2);
			m_pLog->logRecord("stepB 开始 [%I64d][%s][%f]\n",  m_inStockKlines[i].time, sTime2.c_str(), m_inStockKlines[i].close);
			return loopStepB(mx, end);
		}
		
	}
	else
	{
		return loopStepBThree(mx,end);
		

	}
	
	return false;
}

bool CPlat60Week::loopStepBOne(int start, int& end)
{

	
	end = start;
	if (start >= m_inStockKlines.size())
	{
		return false;
	}
	int i = start;
	double & avg = m_mapAvg[m_inStockKlines[i].time];
	if (m_inStockKlines[i].close  < avg  && fabs(m_inStockKlines[i].close - avg) > minVal)
	{
		string sTime2;
		stamp_to_standard(m_inStockKlines[i].time, sTime2);
		m_pLog->logRecord("stepB1 第一阶段收盘价格小于均价[%I64d][%s][%f]\n", m_inStockKlines[i].time, sTime2.c_str(), avg);
		return false;
	}

	m_HB1 = m_inStockKlines[i].high;
	m_posHB1 = i;
	m_stepBhb1 = m_inStockKlines[i].time;
	if (i + 4 >= m_inStockKlines.size())
	{
		string sTime2;
		stamp_to_standard(m_inStockKlines[i].time, sTime2);
		string sTime1;
		stamp_to_standard(m_inStockKlines.back().time, sTime1);
		m_pLog->logRecord("stepB1 不足四周[%I64d][%s][最大日期=%I64d][%s]\n", m_inStockKlines[i].time, sTime2.c_str(), m_inStockKlines.back().time, sTime1.c_str());
		end = m_inStockKlines.size();
		return false;
	}
	string sTime2;
	stamp_to_standard(m_inStockKlines[i].time, sTime2);
	string sTime1;
	stamp_to_standard(m_inStockKlines[end].time, sTime1);
	if (m_H1 > 0.0f && m_HB1 / m_H1 > 1.8)
	{
		end = i ;
		stamp_to_standard(m_inStockKlines[end].time, sTime1);
		m_pLog->logRecord("stepB 不满足阶段1条件2：大于1.8 [高点日期%I64d][%s][当前日期=%I64d][%s][rate=%f]\n",
			m_inStockKlines[i].time, sTime2.c_str(), m_inStockKlines[end].time, sTime1.c_str(), m_HB1 / m_H1);

		//大于1.8之后要重新寻找均线下位置
		int m = end;
		for (; m < m_inStockKlines.size();++m)
		{
			double & avg = m_mapAvg[m_inStockKlines[m].time];
			if (m_inStockKlines[m].close < avg  && fabs(avg - m_inStockKlines[m].close) > minVal)
			{
				break;
			}
		}
		end = m;
		return false;
	}
	int j = 1;
	for (; j <= 4; ++j)
	{
		double & avg = m_mapAvg[m_inStockKlines[i+j].time];
		if (m_inStockKlines[i + j].close  < avg && fabs(avg - m_inStockKlines[i + j].close) > minVal)
		{
			string sTime2;
			stamp_to_standard(m_inStockKlines[i+j].time, sTime2);
			m_pLog->logRecord("stepB1 收盘小于均价[%I64d][%s][close=%f][avg=%f][ca=%f]\n", m_inStockKlines[i + j].time, sTime2.c_str(),
				m_inStockKlines[i + j].close, avg,m_CA);
			end = i + j;
			return false;
		}
		
		if (m_inStockKlines[i + j].close > m_HB1 && fabs(m_HB1 - m_inStockKlines[i + j].close) > minVal)
		{
			//收盘价格高，从当前开始继续寻找B
			end = i + j;
			string sTime2;
			stamp_to_standard(m_inStockKlines[i].time, sTime2);
			string sTime1;
			stamp_to_standard(m_inStockKlines[end].time, sTime1);
			m_pLog->logRecord("stepB 不满足阶段1:连续四周不高于高点 [高点日期%I64d][%s][当前日期=%I64d][%s]\n",
				m_inStockKlines[i].time,sTime2.c_str(), m_inStockKlines[end].time, sTime1.c_str());
			start = i + 1;
			
			return loopStepBOne(start, end);
		}
	}

	

	end = i + 5;
	
	return true;
}

bool CPlat60Week::loopStepBTwo(int start, int&end)
{

	while (start  < m_inStockKlines.size())
	{
		double & avg = m_mapAvg[m_inStockKlines[start].time];
		if (m_inStockKlines[start].close < avg && fabs(avg - m_inStockKlines[start].close) > minVal)
		{
			string sTime1;
			stamp_to_standard(m_inStockKlines[start].time, sTime1);
			m_pLog->logRecord("stepB2 收盘小于均价[%I64d][%s][close=%f][avg=%f]", m_inStockKlines[start].time, sTime1.c_str(),
				m_inStockKlines[start].close, avg);
			end = start;
			return false;
		}
		if (m_inStockKlines[start].close > m_HB1 && fabs(m_HB1 - m_inStockKlines[start].close) > minVal)
		{
			end = start;
			break;
		}
		++start;
	}
	if (start == m_inStockKlines.size())
	{
		//到了最后一根，循环结束
		end = m_inStockKlines.size() - 1;
		return false;
	}

	//从突破开始重新寻找B
	bool bRet = loopStepB(start, end);


	return bRet;
}

bool CPlat60Week::loopStepBThree(int start, int &end)
{

	while (start < m_inStockKlines.size())
	{
		double & avg = m_mapAvg[m_inStockKlines[start].time];
		if (m_inStockKlines[start].close < avg && fabs(avg - m_inStockKlines[start].close) > minVal)
		{
			string sTime1;
			stamp_to_standard(m_inStockKlines[start].time, sTime1);
			m_pLog->logRecord("stepB3 收盘小于均价[%I64d][%s][close=%f][avg=%f]", m_inStockKlines[start].time, sTime1.c_str(),
				m_inStockKlines[start].close, avg);
			end = start;
			return false;
		}
		end = start;
		if (m_inStockKlines[start].close > m_HB1 && fabs(m_HB1 - m_inStockKlines[start].close) > minVal)
		{
			//后续收盘价突破（大于）HB1最高价
			m_type = true;
			m_pLog->logRecord("step3 突破\n");
			return true;
		}
		++start;
		
	}
	m_type = false;
	return true;
}

bool CPlat60Week::loopEntrance()
{
	for (int i = 0; i < m_inStockKlines.size(); ++i)
	{
		double h = m_inStockKlines[i].high;
		double res = h + 0.0051*(int(h/fabs(h)));
		m_inStockKlines[i].high = (int)(res * 100) / 100.0;

		//收盘价
		h = m_inStockKlines[i].close;
		res = h + 0.0051*(int(h/fabs(h)));
		m_inStockKlines[i].close = (int)(res * 100) / 100.0;
	}

	PrintData(m_inStockKlines);
	//开始循环之前赋默认值
	m_H2 = m_HB1 = m_CA = 0.0f;
	m_stepBhb1 = 0;
	bool bAvg = get_avg();
	if (!bAvg)
	{
		return false;
	}
	//存在60周线才可以进入循环
	int end = 60;
	int i = 59;
	bool bInfo1 = true;
	while (i < m_inStockKlines.size())
	{

		bool bLoopA = loopStepA(i, end);

		if (!bLoopA)
		{
			i = end + 1;
			continue;
		}
		alg::EPlatLevel lev;
		//获取平台级别
		int grade = getPlatGrade(i, end, lev);
		if (grade < 0)
		{
			i = end + 1;
			continue;
		}

		

		//赋值StepA
		tagQuota  tagOne;
		tagOne.quotaA.h1Time = m_inStockKlines[i].time;
		tagOne.quotaA.h2Time = m_inStockKlines[end].time;
		tagOne.quotaA.caTime = m_CATime;
		tagOne.quotaA.level = lev;
		tagOne.step = eStepA;

		string sTime2;
		stamp_to_standard(tagOne.quotaA.h1Time, sTime2);
		string sTime1;
		stamp_to_standard(tagOne.quotaA.h2Time,sTime1);
		string sTiem3;
		stamp_to_standard(tagOne.quotaA.caTime, sTiem3);
		m_pLog->logRecord("stepA 筛选成功h1:[%I64d][%s] h2:[%I64d][%s] CA:[%I64d][%s][%lf] 平台级别:[%d]\n\n", tagOne.quotaA.h1Time, sTime2.c_str(),
			tagOne.quotaA.h2Time, sTime1.c_str(),tagOne.quotaA.caTime, sTiem3.c_str(),m_CA, grade);

		//保存h1
		m_H1 = m_inStockKlines[i].high;
		i = end;
		//循环B
		end = i + 1;
		bool bRet = loopStepB(i, end);
		if (bRet)
		{
			//获取平台级别
			grade = getPlatGrade(m_posHB1, end, lev);
			if (grade < 0)
			{
				i = end + 1;
				m_quota.push_back(tagOne);
				continue;
			}
			tagOne.step = eStepAB;
			tagOne.quotaB.level = lev;
			tagOne.quotaB.h1Time = m_stepBhb1;
			tagOne.quotaB.h2Time = m_inStockKlines[end].time;
			tagOne.quotaB.bIsBreak = m_type;

			string sTime2;
			stamp_to_standard(tagOne.quotaB.h1Time, sTime2);
			string sTime1;
			stamp_to_standard(tagOne.quotaB.h2Time, sTime1);
			m_pLog->logRecord("stepB 筛选成功h1:[%I64d][%s][%f] h2:[%I64d][%s][%f] 平台级别:[%d] 是否突破:[%d]\n\n", tagOne.quotaB.h1Time, sTime2.c_str(), m_HB1,
				tagOne.quotaB.h2Time, sTime1.c_str(), m_inStockKlines[end].close, grade, tagOne.quotaB.bIsBreak);
		}
		m_quota.push_back(tagOne);
		if (tagOne.quotaB.bIsBreak)
		{
			bInfo1 = true;
		}
		else
		{
			bInfo1 = false;
		}
		i = end + 1;

		//B筛选成功之后要重新寻找小于60均线的位置
		while (bRet && i < m_inStockKlines.size())
		{
			double & avg = m_mapAvg[m_inStockKlines[i].time];
			if (m_inStockKlines[i].close < avg&& fabs(avg - m_inStockKlines[i].close) > minVal)
			{			
				if (m_quota.back().quotaB.bIsBreak)
				{
					string sTime2;
					m_quota.back().quotaB.info.type = EQuotaInfoType::eType2; //跌破60周线
					m_quota.back().quotaB.info.timeInfo = m_inStockKlines[i].time;
					stamp_to_standard(m_quota.back().quotaB.info.timeInfo, sTime2);
					m_pLog->logRecord("满足信息2:突破B平台跌破60周[%I64d][%s]\n", m_inStockKlines[i].time, sTime2.c_str());

					bInfo1 = false;
				}
				break;
			}
			++i;
		}
	}

	if (bInfo1 && m_quota.back().quotaB.bIsBreak)
	{
		m_quota.back().quotaB.info.type = EQuotaInfoType::eType1;  //一直在60周线以上
		m_quota.back().quotaB.info.timeInfo = m_inStockKlines.back().time;
		string sTime2;
		stamp_to_standard(m_quota.back().quotaB.info.timeInfo, sTime2);
		m_pLog->logRecord("满足信息1:突破B平台未跌破60周[%I64d][%s]\n", m_inStockKlines.back().time, sTime2.c_str());
	}

	//查找包容周期
	m_pLog->logRecord("\n开始查找包容周期数\n");
	int posA = 0; //记录A平台最低价位置
	int posB = 0; //记录B平台最低价位置
	double lowPriceA = 0.0f;
	double lowPriceB = 0.0f;
	for (int i = 0; i < m_quota.size(); ++i)
	{
		if (m_quota[i].step == EStep::eStepAB &&m_quota[i].quotaB.bIsBreak)
		{
		   //A平台
			int d = MaxPeriod(m_quota[i].quotaA.h1Time, m_quota[i].quotaA.h2Time,posA);
			int d2 = MaxPeriod(m_quota[i].quotaB.h1Time, m_quota[i].quotaB.h2Time,posB);
			if (d2 > d)
			{
				m_quota[i].quotaB.period.periodValue = d2;
				m_quota[i].quotaB.period.type = EQuotaPeriodType::ePeriodB;
				string sTime2;
				stamp_to_standard(m_inStockKlines[posB].time, sTime2);
				string sTime1;
				stamp_to_standard(m_inStockKlines[posA].time, sTime1);
				m_pLog->logRecord("最大包容周期结果：B平台[%s][%I64d]周期数:[%d] (A平台[%s][%I64d]周期数:[%d])\n", 
					sTime2.c_str(), m_inStockKlines[posB].time, m_quota[i].quotaB.period.periodValue,
					sTime1.c_str(), m_inStockKlines[posA].time,d);
			}
			else
			{
				m_quota[i].quotaB.period.periodValue = d;
				m_quota[i].quotaB.period.type = EQuotaPeriodType::ePeriodA;
				string sTime2;
				stamp_to_standard(m_inStockKlines[posA].time, sTime2);
				string sTime1;
				stamp_to_standard(m_inStockKlines[posA].time, sTime1);
				m_pLog->logRecord("最大包容周期结果：A平台[%s][%I64d]周期数:[%d] (B平台[%s][%I64d]周期数:[%d])\n", 
					sTime2.c_str(), m_inStockKlines[posA].time, m_quota[i].quotaB.period.periodValue,
					sTime1.c_str(), m_inStockKlines[posB].time, d2);
			}
		}
	}

	m_pLog->logRecord("筛选结束\n");
	return true;
}

void CPlat60Week::PrintData(const std::vector<tagKline>& kLineData)
{
	auto it = kLineData.begin();
	for (; it != kLineData.end(); ++it) {
		m_pLog->dataRecord("%lld,%f,%f,%f,%f,\n", it->time, it->open, it->high, it->low, it->close);
	}
	//printf("读取%d行数据,开 高 低 收\n\n", kLineData.size());
}

int  CPlat60Week::MaxPeriod(__int64 beginD, __int64 endD, int& pos)
{
	int tmp = 0;
	int maxDay = 0;
	pos = 0;
	int k = 0;
	for (int j = 0; j < m_inStockKlines.size(); ++j)
	{
		if (m_inStockKlines[j].time >= beginD && m_inStockKlines[j].time <= endD)
		{
			for ( k = 2; k <= 200; ++k)
			{
				int res = getMaxAvg(j, k);
				if (res == 1)
				{
					if (k > tmp)
					{
						tmp = k;
						pos = j;
					}
					break;
				}
				if (res == -1)
				{
					break;
				}
			}
			if (k < 200 && tmp > maxDay)
			{
				maxDay = tmp;
			}
		}

	}

	return maxDay;
}

int CPlat60Week::getMaxAvg(int pos, int weeks)
{
	if (pos + 1 < weeks || weeks <= 0)
	{
		string sTime2;
		stamp_to_standard(m_inStockKlines[pos].time, sTime2);
		m_pLog->logRecord("计算包容周期不足[%I64d][%s] 周期[%d]", m_inStockKlines[pos].time, sTime2.c_str(), weeks);
		return -1;
	}

	double dsum = 0.0f;
	for (int i = weeks -1; i >= 0; --i)
	{
		dsum += m_inStockKlines[pos - i].close;
	}

	double avg = dsum / weeks;
	if (avg <= m_inStockKlines[pos].low)
	{
		return 1;
	}

	return 0;
}
