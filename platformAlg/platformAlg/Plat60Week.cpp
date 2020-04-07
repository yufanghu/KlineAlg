#include "Plat60Week.h"




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
		m_pLog->logRecord("计算均线参数不足,根数[%d]\n", nCount);
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
			m_mapAvg[m_inStockKlines[k].time] = dSum / nCount;
			//m_pLog->logRecord("均线值%I64d,%f\n", m_inStockKlines[k].time, dSum / nCount);
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
	if (m_inStockKlines[i].close < avg)
	{
		return false;
	}

	double dHigh = m_inStockKlines[i].high;
	//大于等于60周均线
	m_pLog->logRecord("stepA 满足开始条件[时间=%I64d][close=%f][avg=%f]\n", m_inStockKlines[i].time, m_inStockKlines[i].close, avg);
	//定义CA
	m_CA = m_inStockKlines[i].close;
	m_CATime = m_inStockKlines[i].time;
	if (i + 4 >= m_inStockKlines.size())
	{
		m_pLog->logRecord("stepA 不足四周[%I64d][最大日期=%I64d]\n", m_inStockKlines[i].time, m_inStockKlines.back().time);
		end = m_inStockKlines.size();
		return false;
	}

	int j = 1;
	for (; j <= 4; ++j)
	{

		if (m_inStockKlines[i + j].close > dHigh)
		{
			end = i;
			m_pLog->logRecord("stepA 不满足条件1 连续四周不大于高点[高点日期%I64d][高点：%f][当前日期=%I64d][收盘价：%f]\n",
				m_inStockKlines[i].time, dHigh, m_inStockKlines[end].time, m_inStockKlines[i + j].close);
			return false;
		}
		//寻找最高收盘价格
		if (m_CA < m_inStockKlines[i + j].close)
		{
			m_CA = m_inStockKlines[i + j].close;
			m_CATime = m_inStockKlines[i + j].time;
		}
	}
	//寻找突破h2  返回h2位置
	for (int k = i + j; k < m_inStockKlines.size(); ++k)
	{
		if (m_inStockKlines[k].close > dHigh)
		{
			//h2突破
			//定义h2
			m_H2 = m_inStockKlines[k].close;
			//m_pLog->logRecord("stepA 成功 CA[%I64d][%f]，h1[%I64d][%f],h2[%I64d][%f]\n", m_CATime, m_CA, m_inStockKlines[i].time, dHigh, m_inStockKlines[k].time, m_H2);
			end = k;
			return true;
		}
		//寻找最高收盘价格
		if (m_CA < m_inStockKlines[i + j].close)
		{
			m_CA = m_inStockKlines[i + j].close;
			m_CATime = m_inStockKlines[i + j].time;
		}
	}


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
	退出条件：1. 收盘价低于60均线  或  2. HB1 / H2 > 1.8
	*/
	for (int i = start; i < m_inStockKlines.size(); ++i)
	{
		double &avg = m_mapAvg[m_inStockKlines[i].time];
		if (m_inStockKlines[i].close < avg)
		{
			end = i;
			//小于60，从下一根开始重新A
			m_pLog->logRecord("stepB 收盘价低于60均线[%I64d][%f] 均价:[%f]\n", m_inStockKlines[i].time, m_inStockKlines[i].close, avg);
			return false;
		}
	}

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
	int i = end - 4;
	start = end + 1;
	while (i < m_inStockKlines.size())
	{
		if (m_inStockKlines[i].close < m_CA)
		{
			//有数据低于ca,从当前开始查找是否有数据突破hb1
			m_pLog->logRecord("stepB 进入阶段2 小于CA[%f]:[%I64d][%f]\n", m_CA, m_inStockKlines[i].time, m_inStockKlines[i].close);
			return loopStepBTwo(i, end);
		}
		++i;
	}
	if (i == m_inStockKlines.size())
	{
		m_pLog->logRecord("stepB 进入阶段3\n");
		return loopStepBThree(start, end);
	}

	return true;
}

bool CPlat60Week::loopStepBOne(int start, int& end)
{

	int i = start;

	double & avg = m_mapAvg[m_inStockKlines[i].time];
	if (m_inStockKlines[i].close < avg)
	{
		m_pLog->logRecord("stepB 第一阶段收盘价格小于均价[%I64d][%f]\n", m_inStockKlines[i].time, avg);
		return false;
	}

	m_HB1 = m_inStockKlines[i].high;
	m_posHB1 = i;
	m_stepBhb1 = m_inStockKlines[i].time;
	if (i + 4 >= m_inStockKlines.size())
	{
		m_pLog->logRecord("stepA 不足四周[%I64d][最大日期=%I64d]\n", m_inStockKlines[i].time, m_inStockKlines.back().time);
		end = m_inStockKlines.size();
		return false;
	}
	int j = 1;
	for (; j <= 4; ++j)
	{

		if (m_inStockKlines[i + j].close > m_HB1)
		{
			//收盘价格高，从当前开始继续寻找B
			end = i + j;
			m_pLog->logRecord("stepB 不满足阶段1:连续四周不高于高点 [高点日期%I64d][当前日期=%I64d]\n",
				m_inStockKlines[i].time, m_inStockKlines[end].time);
			start = end;
			return loopStepBOne(start, end);
		}
	}

	if (m_HB1 / m_H2 > 1.8)
	{
		end = i + j;
		m_pLog->logRecord("stepB 不满足阶段1条件2：大于1.8 [高点日期%I64d][当前日期=%I64d]\n",
			m_inStockKlines[i].time, m_inStockKlines[end].time);
		return false;
	}
	end = i + j;
	m_pLog->logRecord("stepB 阶段一满足[高点日期%I64d][当前日期%I64d][HB1=%f]\n",
		m_inStockKlines[i].time, m_inStockKlines[end].time, m_HB1);
	return true;
}

bool CPlat60Week::loopStepBTwo(int start, int&end)
{

	while (start  < m_inStockKlines.size())
	{

		if (m_inStockKlines[start].close > m_HB1)
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

bool CPlat60Week::loopStepBThree(int start, int&end)
{

	while (start < m_inStockKlines.size())
	{
		end = start;
		if (m_inStockKlines[start].close > m_HB1)
		{
			//后续收盘价突破（大于）HB1最高价
			m_type = true;
			return true;
		}
		++start;
	}
	m_type = false;
	return true;
}

bool CPlat60Week::loopEntrance()
{
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
		m_pLog->logRecord("stepA 筛选成功h1:[%I64d] h2:[%I64d] CA:[%I64d] 平台级别:[%d]\n", tagOne.quotaA.h1Time,
			tagOne.quotaA.h2Time, tagOne.quotaA.caTime, grade);

		i = end + 1;
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
			tagOne.quotaA.level = lev;
			tagOne.quotaB.h1Time = m_stepBhb1;
			tagOne.quotaB.h2Time = m_inStockKlines[end].time;
			tagOne.quotaB.bIsBreak = m_type;
			m_pLog->logRecord("stepB 筛选成功h1:[%I64d] h2:[%I64d] 平台级别:[%d] 是否突破:[%d]\n\n", tagOne.quotaB.h1Time,
				tagOne.quotaB.h2Time, grade, m_type);
		}
		m_quota.push_back(tagOne);
		i = end + 1;
	}
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