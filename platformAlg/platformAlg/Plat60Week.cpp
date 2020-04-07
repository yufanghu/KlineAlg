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
	//����60�ܾ���
	int nCount = 60;

	int i;
	if (nCount <= 0)
	{
		//���Ϸ�����
		m_pLog->logRecord("������߲�������,����[%d]\n", nCount);
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
			//m_pLog->logRecord("����ֵ%I64d,%f\n", m_inStockKlines[k].time, dSum / nCount);
		}
	}



	return true;
}

bool CPlat60Week::loopStepA(int start, int& end)
{
	/*
	��ʼ���������̼��״δ��ڻ����60�ܾ�����ֵΪһ��ѭ����ʼ�㣬�����ݶ���Ϊ����A��
	������A��ʼ���ɸѡ����������A����
	��Ҫ����1��ĳһ��K�߸ߵ����������4�����̼�С�ڻ���ڸøߵ㣬�øߵ㶨��ΪH1��
	��Ҫ����2��������A��ʼ��Ҫ���������������̼۸��ڻ����60�ܾ���ֵ�������˳�ɸѡѭ��A��
	��Ҫ����3��Ҫ���������������������Һ������̼�ͻ�ƣ����ڣ��ߵ�H1��ͻ��K�߶���ΪH2.
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
	//���ڵ���60�ܾ���
	m_pLog->logRecord("stepA ���㿪ʼ����[ʱ��=%I64d][close=%f][avg=%f]\n", m_inStockKlines[i].time, m_inStockKlines[i].close, avg);
	//����CA
	m_CA = m_inStockKlines[i].close;
	m_CATime = m_inStockKlines[i].time;
	if (i + 4 >= m_inStockKlines.size())
	{
		m_pLog->logRecord("stepA ��������[%I64d][�������=%I64d]\n", m_inStockKlines[i].time, m_inStockKlines.back().time);
		end = m_inStockKlines.size();
		return false;
	}

	int j = 1;
	for (; j <= 4; ++j)
	{

		if (m_inStockKlines[i + j].close > dHigh)
		{
			end = i;
			m_pLog->logRecord("stepA ����������1 �������ܲ����ڸߵ�[�ߵ�����%I64d][�ߵ㣺%f][��ǰ����=%I64d][���̼ۣ�%f]\n",
				m_inStockKlines[i].time, dHigh, m_inStockKlines[end].time, m_inStockKlines[i + j].close);
			return false;
		}
		//Ѱ��������̼۸�
		if (m_CA < m_inStockKlines[i + j].close)
		{
			m_CA = m_inStockKlines[i + j].close;
			m_CATime = m_inStockKlines[i + j].time;
		}
	}
	//Ѱ��ͻ��h2  ����h2λ��
	for (int k = i + j; k < m_inStockKlines.size(); ++k)
	{
		if (m_inStockKlines[k].close > dHigh)
		{
			//h2ͻ��
			//����h2
			m_H2 = m_inStockKlines[k].close;
			//m_pLog->logRecord("stepA �ɹ� CA[%I64d][%f]��h1[%I64d][%f],h2[%I64d][%f]\n", m_CATime, m_CA, m_inStockKlines[i].time, dHigh, m_inStockKlines[k].time, m_H2);
			end = k;
			return true;
		}
		//Ѱ��������̼۸�
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
	m_pLog->logRecord("ƽ̨����[%d]\n", grade);
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
	��ʼ������ѭ��ɸѡA�ɹ���������H2���ݣ�ͻ��K�ߣ����ɸѡ��
	�˳�������1. ���̼۵���60����  ��  2. HB1 / H2 > 1.8
	*/
	for (int i = start; i < m_inStockKlines.size(); ++i)
	{
		double &avg = m_mapAvg[m_inStockKlines[i].time];
		if (m_inStockKlines[i].close < avg)
		{
			end = i;
			//С��60������һ����ʼ����A
			m_pLog->logRecord("stepB ���̼۵���60����[%I64d][%f] ����:[%f]\n", m_inStockKlines[i].time, m_inStockKlines[i].close, avg);
			return false;
		}
	}

	bool bStepOne = loopStepBOne(start, end);
	if (!bStepOne)
	{
		return false;
	}

	/*
	ѡ��������
	1���������̼۶�����CA���ݣ�ǰһƽ̨������̼ۣ�����������׶Ρ�
	2�����������̼۵���CA���ݣ�����ڶ��׶�ɸѡ��
	*/
	int i = end - 4;
	start = end + 1;
	while (i < m_inStockKlines.size())
	{
		if (m_inStockKlines[i].close < m_CA)
		{
			//�����ݵ���ca,�ӵ�ǰ��ʼ�����Ƿ�������ͻ��hb1
			m_pLog->logRecord("stepB ����׶�2 С��CA[%f]:[%I64d][%f]\n", m_CA, m_inStockKlines[i].time, m_inStockKlines[i].close);
			return loopStepBTwo(i, end);
		}
		++i;
	}
	if (i == m_inStockKlines.size())
	{
		m_pLog->logRecord("stepB ����׶�3\n");
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
		m_pLog->logRecord("stepB ��һ�׶����̼۸�С�ھ���[%I64d][%f]\n", m_inStockKlines[i].time, avg);
		return false;
	}

	m_HB1 = m_inStockKlines[i].high;
	m_posHB1 = i;
	m_stepBhb1 = m_inStockKlines[i].time;
	if (i + 4 >= m_inStockKlines.size())
	{
		m_pLog->logRecord("stepA ��������[%I64d][�������=%I64d]\n", m_inStockKlines[i].time, m_inStockKlines.back().time);
		end = m_inStockKlines.size();
		return false;
	}
	int j = 1;
	for (; j <= 4; ++j)
	{

		if (m_inStockKlines[i + j].close > m_HB1)
		{
			//���̼۸�ߣ��ӵ�ǰ��ʼ����Ѱ��B
			end = i + j;
			m_pLog->logRecord("stepB ������׶�1:�������ܲ����ڸߵ� [�ߵ�����%I64d][��ǰ����=%I64d]\n",
				m_inStockKlines[i].time, m_inStockKlines[end].time);
			start = end;
			return loopStepBOne(start, end);
		}
	}

	if (m_HB1 / m_H2 > 1.8)
	{
		end = i + j;
		m_pLog->logRecord("stepB ������׶�1����2������1.8 [�ߵ�����%I64d][��ǰ����=%I64d]\n",
			m_inStockKlines[i].time, m_inStockKlines[end].time);
		return false;
	}
	end = i + j;
	m_pLog->logRecord("stepB �׶�һ����[�ߵ�����%I64d][��ǰ����%I64d][HB1=%f]\n",
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
		//�������һ����ѭ������
		end = m_inStockKlines.size() - 1;
		return false;
	}

	//��ͻ�ƿ�ʼ����Ѱ��B
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
			//�������̼�ͻ�ƣ����ڣ�HB1��߼�
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
	//��ʼѭ��֮ǰ��Ĭ��ֵ
	m_H2 = m_HB1 = m_CA = 0.0f;
	m_stepBhb1 = 0;
	bool bAvg = get_avg();
	if (!bAvg)
	{
		return false;
	}
	//����60���߲ſ��Խ���ѭ��
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
		//��ȡƽ̨����
		int grade = getPlatGrade(i, end, lev);
		if (grade < 0)
		{
			i = end + 1;
			continue;
		}


		//��ֵStepA
		tagQuota  tagOne;
		tagOne.quotaA.h1Time = m_inStockKlines[i].time;
		tagOne.quotaA.h2Time = m_inStockKlines[end].time;
		tagOne.quotaA.caTime = m_CATime;
		tagOne.quotaA.level = lev;
		tagOne.step = eStepA;
		m_pLog->logRecord("stepA ɸѡ�ɹ�h1:[%I64d] h2:[%I64d] CA:[%I64d] ƽ̨����:[%d]\n", tagOne.quotaA.h1Time,
			tagOne.quotaA.h2Time, tagOne.quotaA.caTime, grade);

		i = end + 1;
		//ѭ��B
		end = i + 1;
		bool bRet = loopStepB(i, end);
		if (bRet)
		{
			//��ȡƽ̨����
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
			m_pLog->logRecord("stepB ɸѡ�ɹ�h1:[%I64d] h2:[%I64d] ƽ̨����:[%d] �Ƿ�ͻ��:[%d]\n\n", tagOne.quotaB.h1Time,
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
	//printf("��ȡ%d������,�� �� �� ��\n\n", kLineData.size());
}