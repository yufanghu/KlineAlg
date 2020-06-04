#include "Plat60WeekMulti.h"
#include "Global.h"





CPlat60WeekMulti::CPlat60WeekMulti(const std::map<tagStockCodeInfo, std::vector<tagKline>>& inMap, std::map<tagStockCodeInfo, tagOutput> & outMap)
	:m_outMap(outMap),
	m_inMap(inMap)
{
	minVal = 0.000001;
}

CPlat60WeekMulti::~CPlat60WeekMulti()
{
}

bool CPlat60WeekMulti::createNewAlg()
{
	CLog log;
	log.Init("PlatForm4multi_Log");
	SetLogObj(&log);
	loopEntrance();
	log.Flush();
	return true;

}

bool CPlat60WeekMulti::get_avg()
{
	//����60�ܾ���
	int nCount = 60;

	int i;
	if (m_inStockKlines.size() <= 0)
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
			string sTime;
			stamp_to_standard(m_inStockKlines[k].time, sTime);
			double avg1 = dSum / nCount;
			avg1 = avg1 + 0.0051*(int(avg1 / fabs(avg1)));
			double res = (int)(avg1 * 100) / 100.0;
			m_mapAvg[m_inStockKlines[k].time] = res;
			//m_pLog->logRecord("����ֵ%I64d  %s,%f %d\n", m_inStockKlines[k].time,sTime.c_str(), dSum / nCount,m_inStockKlines.size());
		}
	}
	return true;
}

bool CPlat60WeekMulti::loopStepA(int start, int& end)
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
	if (m_inStockKlines[i].close  < avg)
	{
		return false;
	}

	double dHigh = m_inStockKlines[i].high;
	m_H1 = dHigh;
	//���ڵ���60�ܾ���
	/*string sTime;
	stamp_to_standard(m_inStockKlines[i].time, sTime);
	m_pLog->logRecord("stepA ���㿪ʼ����[ʱ��=%I64d][%s][close=%f][avg=%f]\n", m_inStockKlines[i].time,
	sTime.c_str(), m_inStockKlines[i].close, avg);*/
	//����CA
	m_CA = m_inStockKlines[i].close;
	m_CATime = m_inStockKlines[i].time;
	if (i + 4 >= m_inStockKlines.size())
	{
		/*sTime.clear();
		stamp_to_standard(m_inStockKlines[i].time, sTime);
		m_pLog->logRecord("stepA ��������[%I64d][�������=%I64d][%s]\n", m_inStockKlines[i].time, m_inStockKlines.back().time, sTime.c_str());*/
		end = m_inStockKlines.size();
		return false;
	}

	int j = 1;
	for (; j <= 4; ++j)
	{
		double & avg = m_mapAvg[m_inStockKlines[i + j].time];
		if (m_inStockKlines[i + j].close < avg && fabs(avg - m_inStockKlines[i + j].close) > minVal)
		{
			/*sTime.clear();
			stamp_to_standard(m_inStockKlines[i + j].time, sTime);
			m_pLog->logRecord("stepA ���̼۸�С�ھ���[����=%I64d][%s][close=%f][avg=%f]\n", m_inStockKlines[i + j].time, sTime.c_str(), m_inStockKlines[i + j].close,
			avg);*/
			end = i + j;
			return false;
		}
		if (m_inStockKlines[i + j].close > dHigh && fabs(dHigh - m_inStockKlines[i + j].close) > minVal)
		{

			/*string sTime1;
			stamp_to_standard(m_inStockKlines[i + j].time, sTime1);
			string sTime2;
			stamp_to_standard(m_inStockKlines[end].time, sTime2);
			m_pLog->logRecord("stepA ����������1 �������ܲ����ڸߵ�[�ߵ�����%I64d][%s][�ߵ㣺%lf][��ǰ����=%I64d][%s][���̼ۣ�%lf]\n",
			m_inStockKlines[i + j].time, sTime1.c_str(), dHigh, m_inStockKlines[end].time, sTime2.c_str(), m_inStockKlines[i + j].close);*/
			end = i;
			return false;
		}
		//Ѱ��������̼۸�
		if (m_CA < m_inStockKlines[i + j].close && fabs(m_inStockKlines[i + j].close - m_CA) > minVal)
		{
			m_CA = m_inStockKlines[i + j].close;
			m_CATime = m_inStockKlines[i + j].time;
		}
	}
	//Ѱ��ͻ��h2  ����h2λ��
	int l = 0;
	for (l = i + 4; l < m_inStockKlines.size(); ++l)
	{
		if (m_inStockKlines[l].close > dHigh && fabs(dHigh - m_inStockKlines[l].close) > minVal)
		{
			//h2ͻ��
			//����h2
			m_H2 = m_inStockKlines[l].close;
			//m_pLog->logRecord("stepA �ɹ� CA[%I64d][%f]��h1[%I64d][%f],h2[%I64d][%f]\n", m_CATime, m_CA, m_inStockKlines[i].time, dHigh, m_inStockKlines[k].time, m_H2);
			end = l;
			return true;
		}
		double & avg = m_mapAvg[m_inStockKlines[l].time];
		if (m_inStockKlines[l].close < avg && fabs(avg - m_inStockKlines[l].close) > minVal)
		{
			/*sTime.clear();
			stamp_to_standard(m_inStockKlines[l].time, sTime);
			m_pLog->logRecord("stepA ���̼۸�С�ھ���11[����=%I64d][%s][close=%f][avg=%f]\n", m_inStockKlines[l].time, sTime.c_str(), m_inStockKlines[l].close,
			avg);*/
			end = l;
			return false;
		}
		//Ѱ��������̼۸�
		if (m_CA < m_inStockKlines[l].close && fabs(m_inStockKlines[l].close - m_CA) > minVal)
		{
			m_CA = m_inStockKlines[l].close;
			m_CATime = m_inStockKlines[l].time;
		}
	}

	end = m_inStockKlines.size();
	return false;


}

bool CPlat60WeekMulti::loopStepB(int start, int& end)
{
	/*
	��ʼ������ѭ��ɸѡA�ɹ���������H2���ݣ�ͻ��K�ߣ����ɸѡ��
	�˳�������1. ���̼۵���60����  ��  2. HB1 / H1 > 1.8
	*/

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
	//
	int l = 0;
	for (l = end; l < m_inStockKlines.size(); ++l)
	{
		if (m_inStockKlines[l].close > m_HB1 && fabs(m_inStockKlines[l].close - m_HB1) > minVal)
		{
			/*string sTime;
			stamp_to_standard(m_inStockKlines[l].time, sTime);
			m_pLog->logRecord("stepB ͻ�Ƹߵ�[%I64d][%s]\n", m_inStockKlines[l].time, sTime.c_str());*/
			break;
		}
		double & avg = m_mapAvg[m_inStockKlines[l].time];
		if (m_inStockKlines[l].close  < avg && fabs(avg - m_inStockKlines[l].close) > minVal)
		{
			/*string sTime;
			stamp_to_standard(m_inStockKlines[l].time, sTime);
			m_pLog->logRecord("stepB ���̼۸�С�ھ���[����=%I64d][%s][close=%f][avg=%f]\n", m_inStockKlines[l].time, sTime.c_str(), m_inStockKlines[l].close,
			avg);*/
			end = l;
			return false;
		}

	}



	int mx = (l >= m_inStockKlines.size() - 1 ? m_inStockKlines.size() - 1 : l);
	int i = (end - 4 >= 0) ? end - 4 : 0;
	for (; i <= mx; ++i)
	{
		if (m_inStockKlines[i].close < m_CA && fabs(m_inStockKlines[l].close - m_CA) > minVal)
		{
			/*string sTime2;
			stamp_to_standard(m_inStockKlines[i].time, sTime2);
			m_pLog->logRecord("stepB ����׶�2 С��CA[%f][%I64d][%s][%f]\n", m_CA, m_inStockKlines[i].time, sTime2.c_str(), m_inStockKlines[i].close);*/
			break;
		}
	}

	if (i <= mx)
	{

		if (mx < m_inStockKlines.size())
		{
			/*string sTime2;
			stamp_to_standard(m_inStockKlines[i].time, sTime2);
			m_pLog->logRecord("stepB ��ʼ [%I64d][%s][%f]\n", m_inStockKlines[i].time, sTime2.c_str(), m_inStockKlines[i].close);*/
			return loopStepB(mx, end);
		}

	}
	else
	{
		return loopStepBThree(mx, end);


	}

	return false;

}

bool CPlat60WeekMulti::loopStepBOne(int start, int& end)
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
		/*string sTime2;
		stamp_to_standard(m_inStockKlines[i].time, sTime2);
		m_pLog->logRecord("stepB1 ��һ�׶����̼۸�С�ھ���[%I64d][%s][%f]\n", m_inStockKlines[i].time, sTime2.c_str(), avg);*/
		return false;
	}

	m_HB1 = m_inStockKlines[i].high;
	m_posHB1 = i;
	m_stepBhb1 = m_inStockKlines[i].time;
	if (i + 4 >= m_inStockKlines.size())
	{
		/*string sTime2;
		stamp_to_standard(m_inStockKlines[i].time, sTime2);
		string sTime1;
		stamp_to_standard(m_inStockKlines.back().time, sTime1);
		m_pLog->logRecord("stepB1 ��������[%I64d][%s][�������=%I64d][%s]\n", m_inStockKlines[i].time, sTime2.c_str(), m_inStockKlines.back().time, sTime1.c_str());*/
		end = m_inStockKlines.size();
		return false;
	}
	string sTime2;
	stamp_to_standard(m_inStockKlines[i].time, sTime2);
	string sTime1;
	stamp_to_standard(m_inStockKlines[end].time, sTime1);
	if (m_H1 > 0.0f && m_HB1 / m_H1 > 1.8)
	{
		end = i;
		/*stamp_to_standard(m_inStockKlines[end].time, sTime1);
		m_pLog->logRecord("stepB ������׶�1����2������1.8 [�ߵ�����%I64d][%s][��ǰ����=%I64d][%s][rate=%f]\n",
		m_inStockKlines[i].time, sTime2.c_str(), m_inStockKlines[end].time, sTime1.c_str(), m_HB1 / m_H1);*/

		//����1.8֮��Ҫ����Ѱ�Ҿ�����λ��
		int m = end;
		for (; m < m_inStockKlines.size(); ++m)
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
		double & avg = m_mapAvg[m_inStockKlines[i + j].time];
		if (m_inStockKlines[i + j].close  < avg && fabs(avg - m_inStockKlines[i + j].close) > minVal)
		{
			/*string sTime2;
			stamp_to_standard(m_inStockKlines[i + j].time, sTime2);
			m_pLog->logRecord("stepB1 ����С�ھ���[%I64d][%s][close=%f][avg=%f][ca=%f]\n", m_inStockKlines[i + j].time, sTime2.c_str(),
			m_inStockKlines[i + j].close, avg, m_CA);*/
			end = i + j;
			return false;
		}

		if (m_inStockKlines[i + j].close > m_HB1 && fabs(m_HB1 - m_inStockKlines[i + j].close) > minVal)
		{
			//���̼۸�ߣ��ӵ�ǰ��ʼ����Ѱ��B
			end = i + j;
			/*string sTime2;
			stamp_to_standard(m_inStockKlines[i].time, sTime2);
			string sTime1;
			stamp_to_standard(m_inStockKlines[end].time, sTime1);
			m_pLog->logRecord("stepB ������׶�1:�������ܲ����ڸߵ� [�ߵ�����%I64d][%s][��ǰ����=%I64d][%s]\n",
			m_inStockKlines[i].time, sTime2.c_str(), m_inStockKlines[end].time, sTime1.c_str());*/
			start = i + 1;

			return loopStepBOne(start, end);
		}
	}



	end = i + 5;

	return true;

}

bool CPlat60WeekMulti::loopStepBTwo(int start, int&end)
{
	while (start  < m_inStockKlines.size())
	{
		double & avg = m_mapAvg[m_inStockKlines[start].time];
		if (m_inStockKlines[start].close < avg && fabs(avg - m_inStockKlines[start].close) > minVal)
		{
			/*string sTime1;
			stamp_to_standard(m_inStockKlines[start].time, sTime1);
			m_pLog->logRecord("stepB2 ����С�ھ���[%I64d][%s][close=%f][avg=%f]", m_inStockKlines[start].time, sTime1.c_str(),
			m_inStockKlines[start].close, avg);*/
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
		//�������һ����ѭ������
		end = m_inStockKlines.size() - 1;
		return false;
	}

	//��ͻ�ƿ�ʼ����Ѱ��B
	bool bRet = loopStepB(start, end);


	return bRet;

}

bool CPlat60WeekMulti::loopStepBThree(int start, int&end)
{
	while (start < m_inStockKlines.size())
	{
		double & avg = m_mapAvg[m_inStockKlines[start].time];
		if (m_inStockKlines[start].close < avg && fabs(avg - m_inStockKlines[start].close) > minVal)
		{
			/*string sTime1;
			stamp_to_standard(m_inStockKlines[start].time, sTime1);
			m_pLog->logRecord("stepB3 ����С�ھ���[%I64d][%s][close=%f][avg=%f]", m_inStockKlines[start].time, sTime1.c_str(),
			m_inStockKlines[start].close, avg);*/
			end = start;
			return false;
		}
		end = start;
		if (m_inStockKlines[start].close > m_HB1 && fabs(m_HB1 - m_inStockKlines[start].close) > minVal)
		{
			//�������̼�ͻ�ƣ����ڣ�HB1��߼�
			m_type = true;
			/*m_pLog->logRecord("step3 ͻ��\n");*/
			return true;
		}
		++start;

	}
	m_type = false;
	return true;

}

bool CPlat60WeekMulti::loopEntrance()
{
	int nIndex = 0;
	std::map<tagStockCodeInfo, std::vector<tagKline>>::iterator iter = m_inMap.begin();
	for (; iter != m_inMap.end(); ++iter)
	{
		m_inStockKlines = iter->second;
		PrintHead(iter->first, ++nIndex);
		PrintData(iter->second);
		for (int i = 0; i < m_inStockKlines.size(); ++i)
		{
			double h = m_inStockKlines[i].high;
			double res = h + 0.0051*(int(h / fabs(h)));
			m_inStockKlines[i].high = (int)(res * 100) / 100.0;

			//���̼�
			h = m_inStockKlines[i].close;
			res = h + 0.0051*(int(h / fabs(h)));
			m_inStockKlines[i].close = (int)(res * 100) / 100.0;
		}
		//��ʼѭ��֮ǰ��Ĭ��ֵ
		m_H2 = m_HB1 = m_CA = 0.0f;
		m_stepBhb1 = 0;
		bool bAvg = get_avg();
		if (!bAvg)
		{
			continue;
		}
		//����60���߲ſ��Խ���ѭ��
		int end = 60;
		int i = 59;
		bool bInfo1 = false;
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

			string sTime2;
			stamp_to_standard(tagOne.quotaA.h1Time, sTime2);
			string sTime1;
			stamp_to_standard(tagOne.quotaA.h2Time, sTime1);
			string sTiem3;
			stamp_to_standard(tagOne.quotaA.caTime, sTiem3);
			/*m_pLog->logRecord("stepA ɸѡ�ɹ�h1:[%I64d][%s] h2:[%I64d][%s] CA:[%I64d][%s][%lf] ƽ̨����:[%d]\n\n", tagOne.quotaA.h1Time, sTime2.c_str(),
			tagOne.quotaA.h2Time, sTime1.c_str(), tagOne.quotaA.caTime, sTiem3.c_str(), m_CA, grade);*/

			//����h1
			m_H1 = m_inStockKlines[i].high;
			i = end;
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
				tagOne.quotaB.level = lev;
				tagOne.quotaB.h1Time = m_stepBhb1;
				tagOne.quotaB.h2Time = m_inStockKlines[end].time;
				tagOne.quotaB.bIsBreak = m_type;

				/*string sTime2;
				stamp_to_standard(tagOne.quotaB.h1Time, sTime2);
				string sTime1;
				stamp_to_standard(tagOne.quotaB.h2Time, sTime1);
				m_pLog->logRecord("stepB ɸѡ�ɹ�h1:[%I64d][%s][%f] h2:[%I64d][%s][%f] ƽ̨����:[%d] �Ƿ�ͻ��:[%d]\n\n", tagOne.quotaB.h1Time, sTime2.c_str(), m_HB1,
				tagOne.quotaB.h2Time, sTime1.c_str(), m_inStockKlines[end].close, grade, tagOne.quotaB.bIsBreak);*/
			}
			m_quota.push_back(tagOne);
			if (bRet && tagOne.quotaB.bIsBreak)
			{
				bInfo1 = true;
			}
			else
			{
				bInfo1 = false;
			}
			i = end + 1;

			//Bɸѡ�ɹ�֮��Ҫ����Ѱ��С��60���ߵ�λ��
			while (bRet && i < m_inStockKlines.size())
			{
				double & avg = m_mapAvg[m_inStockKlines[i].time];
				if (m_inStockKlines[i].close < avg&& fabs(avg - m_inStockKlines[i].close) > minVal)
				{
					if (m_quota.back().quotaB.bIsBreak)
					{
						//string sTime2;
						//m_quota.back().quotaB.info.type = EQuotaInfoType::eType2; //����60����
						//m_quota.back().quotaB.info.timeInfo = m_inStockKlines[i].time;
						//����60��
						bInfo1 = false;
					}
					break;
				}
				++i;
			}
		}

		if (!m_quota.empty() && bInfo1 && m_quota.back().quotaB.bIsBreak)
		{
			//m_quota.back().quotaB.info.type = EQuotaInfoType::eType1;  //һֱ��60��������
			//m_quota.back().quotaB.info.timeInfo = m_inStockKlines.back().time;
			/*string sTime2;
			stamp_to_standard(m_quota.back().quotaB.info.timeInfo, sTime2);*/
			//m_pLog->logRecord("����ɸѡ����:����[%s]\n", iter->first.stockcode.c_str());
			tagStockCodeInfo retTag(iter->first.market, iter->first.stockcode);

			tagOutput& oneTag = m_outMap[retTag];
		}



	}



	std::map<tagStockCodeInfo, tagOutput>::iterator it;
	m_pLog->logRecord("-------ɸѡ�����[%d]֧��������--------\n", m_outMap.size());
	int ii = 1;
	for (it = m_outMap.begin(); it != m_outMap.end(); ++it)
	{
		m_pLog->logRecord("[%d-%s],", it->first.market, it->first.stockcode.c_str());
		if (ii % 5 == 0)
		{
			m_pLog->logRecord("\n");
		}
		++ii;
	}

	return true;

}

void CPlat60WeekMulti::PrintData(const std::vector<tagKline>& kLineData)
{
	auto it = kLineData.begin();
	for (; it != kLineData.end(); ++it) {
		m_pLog->dataRecord("%lld,%f,%f,%f,%f,\n", it->time, it->open, it->high, it->low, it->close);
	}
}


void CPlat60WeekMulti::PrintHead(const tagStockCodeInfo & codeInfo, int nPos)
{
	m_pLog->dataRecord("%d-%s\n", codeInfo.market, codeInfo.stockcode.c_str());
	m_pLog->logRecord("��%d֧��Ʊ[%d-%s]\n", nPos, codeInfo.market, codeInfo.stockcode.c_str());
}

int CPlat60WeekMulti::getPlatGrade(const int start, const int end, alg::EPlatLevel& level)
{
	int grade = end - start + 1;
	/*m_pLog->logRecord("ƽ̨����[%d]\n", grade);*/
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
