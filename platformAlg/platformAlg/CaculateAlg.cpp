#include "StdAfx.h"
#include "CaculateAlg.h"
#include "Log.h"
#include <time.h>  
#include <string.h>  


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
//�������ֵ
	double dSum = 0.0;
	int i;
	if (nStart - nCount < 0 || nCount <= 0)
	{
		//���Ϸ�����
		LOG("������߲�������");
		return false;
	}
	for (i = 0; i != nCount; ++i)
	{
		dSum += kLineData[nStart-i].close;
	}
	if (i == nCount)
	{
		dAvg = dSum / nCount;
		//LOG("����ֵ%d", dAvg);
		return true;
	}

	return false;

}

bool CCaculateAlg::single_double_step_one(const std::vector<tagKline>& kLineData, short avgFac,
	int& nPos,int& nKnb,int nMax, int nMin)
{
	/*
	������ĩβ��ǰɸѡ
	������������ڵ���5��K�ߣ����Ҳ�����15��k�����̼� > ����ֵ��������
	��¼����K����Ŀ��KNB, �ӵ�1�����̼۸��ھ������𣬱���ɸѡ���Ϊ21����
	�����ϸù�Ʊ����ɸѡ���������ؿ�ֵ
	*/
	int nCount = 1;
	nPos = kLineData.size();
	for (int i = kLineData.size() - 1; i >= 0; --i)
	{
		double dAvg;
		bool bRet = get_avg(kLineData, i, avgFac, dAvg);

		if (!bRet && nCount < nMin)
		{
			LOG("ƽ̨��һ��ɸѡʧ��,���߲�������");
			return false;
		}

		if ( kLineData[i].close > dAvg )
		{
			if (nCount <= nMax)
			{	
				char buf[32];
				stamp_to_standard(kLineData[i].time, buf);
				LOG("step 1[%d] %s close %f avg %f \n",nCount, buf, kLineData[i].close, dAvg);
				nCount++;
			}
			else
			{
				nPos = i + 1;
				nKnb = nCount - 1 ;

				char buf[32];
				stamp_to_standard(kLineData[nPos].time, buf);
				//LOG("step 1ɸѡ�ɹ� knb[%d] %s close %f avg %f \n", nKnb, buf, kLineData[nPos].close, dAvg);	
				LOG("step 1ɸѡ�ɹ� knb[%d]\n", nKnb);
				return true;
			}
			
		}
		else
		{
			if (nCount >= nMin )
			{
				nPos = i+1;
				nKnb = nCount;
				LOG("step 1ɸѡ�ɹ� knb[%d]\n\n", nKnb-1);
				return true;
			}
			else
			{
				nCount = 1;
				continue;
			}
		}
		
	}
	LOG("ƽ̨��һ��ɸѡʧ�� Pos[%d]\n", nPos);
	return false;
}

bool CCaculateAlg::single_double_step_two(const std::vector<tagKline>& kLineData, int& nPos)
{
	/*
	FOR(N=1;N<=8;N++)
	�������N+1��.N+4 �������̼۶�����N������߼ۡ�
	����N<=8 ����¼Nֵ�����˲��趨λ��N��������K�ߣ�
	��ô�ù��̽����������3������ɸѡ��
	���򣺸ù�Ʊ����ɸѡ���������ؿ�ֵ
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
					char ss[32];
					nPos = j - 4;
					stamp_to_standard(kLineData[nPos].time, ss);
					LOG("step2ɸѡ�ɹ� date[%s] close %f high %f\n\n", ss, kLineData[nPos].close, kLineData[nPos].high);
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
	LOG("ƽ̨�ڶ���ɸѡʧ�� Pos[%s]\n\n", ss);

	return false;
}

bool CCaculateAlg::single_plat_step_third(const std::vector<tagKline>& kLineData, int& nPos)
{
	int nEnd = kLineData.size() - 2;

	for (int i = nPos + 5; i <= nEnd; ++i)
	{
		char ss[32];
		stamp_to_standard(kLineData[i].time, ss);
		
		if (kLineData[i].close < kLineData[nPos].high)
		{
			LOG("step3 %s close %f С�� high %f\n", ss, kLineData[i].close, kLineData[nPos].high);
			continue;
		}
		else
		{
			LOG("step3 %s close %f ��С�� high %f\n�� ������ʧ��", ss, kLineData[i].close, kLineData[nPos].high);
			return false;
		}
	}
	char ss[32];
	stamp_to_standard(kLineData[nPos].time, ss);
	LOG("��ƽ̨������ɸѡ�ɹ�[%s]\n\n", ss);
	return true;
}

bool CCaculateAlg::is_fairing(const std::vector<tagKline>& kLineData, int& nPos, bool isFiring)
{
//��ѡ��Ϊtrueʱ�����жϣ�����Ĭ�ϳɹ�
	int size = kLineData.size();
	bool firRes = true;
	if (isFiring)
	{
		firRes = kLineData[size - 1].close > kLineData[nPos].high ? true : false;
		if (firRes)
		{
			char ss[32];
			stamp_to_standard(kLineData[size - 1].time, ss);
			LOG("�𱬳ɹ� %s close %f high %f\n\n", ss, kLineData[size - 1].close, kLineData[nPos].high);
			return true;
		}
		else
		{
			LOG("��ʧ��\n");
			return false;
		}
	}
	else
	{
		LOG("�𱬹رգ�ɸѡ�ɹ�\n\n")
		return true;
	}

}

bool CCaculateAlg::double_step_third(const std::vector<tagKline>& kLineData, int& nPos)
{
	/*
	������CLOSE(N+i)>HIGH(N)
	��¼��M=i ���˲��趨λ��M���ݣ�
	*/
	int nEnd = kLineData.size() - 2;
	for (int i = nPos + 5; i <= nEnd; ++i)
	{

		if (kLineData[i].close > kLineData[nPos].high)
		{
			nPos = i;
			char ss[32];
			stamp_to_standard(kLineData[nPos].time, ss);
			LOG("��ƽ̨������ɸѡ�ɹ� [%s]\n\n", ss);
			return true;
		}
		else
		{
			continue;
		}
	}

	char ss[32];
	stamp_to_standard(kLineData[nPos].time, ss);
	LOG("��ƽ̨������ɸѡʧ�� [%s]\n\n", ss);
	return false;
}

bool CCaculateAlg::double_step_fourth(const std::vector<tagKline>& kLineData, int& nPos)
{
	int nEnd = kLineData.size();
	//KNB - M <4 ����ɸѡ
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
					LOG("��ƽ̨���Ĳ�ɸѡ�ɹ� [%s]\n\n", ss);
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
	LOG("��ƽ̨���Ĳ�ɸѡʧ�� [%s]\n\n", ss);
	return false;
}

bool CCaculateAlg::double_step_fifth(const std::vector<tagKline>& kLineData, int& nPos)
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
			LOG("��ƽ̨���岽ɸѡʧ�� [%s]\n\n", ss);
			return false;
		}
	}

	char ss[32];
	stamp_to_standard(kLineData[nPos].time, ss);
	LOG("��ƽ̨���岽ɸѡ�ɹ� [%s]", ss);
	return true;
}

bool CCaculateAlg::double_plat(const std::pair<tagStockCodeInfo, std::vector<tagKline>> & input,
	std::map<tagStockCodeInfo, tagOutput> & output, short avgFac, bool bFiring /*= false*/)
{
	LOG("˫ƽ̨-���߲���[%d]\n");
	
	int nPos = 0;  //����������K��λ��
	bool bRet = false;
	int nKnb = 0;

	PrintData(input.second);
	std::vector<tagKline> vecKline = input.second;
	if (vecKline.size() < unsigned(avgFac))
	{
		//���߲������㣬������ڵ���nMin��K�ߣ����Ҳ�����nMax��k�����̼�>����ֵ��������
		LOG("����[%d]��������, ֻ��[%d]��k��\n", avgFac, vecKline.size());
		return false;
	}

	bRet = single_double_step_one(vecKline, avgFac, nPos, nKnb, 25, 10);
	if (!bRet)
	{
		//��һ��ʧ��
		return false;
	}
	bRet = single_double_step_two(vecKline, nPos);

	if (!bRet)
	{
		//�ڶ���ʧ��
		return false;
	}
	//ֻ��N+4 ʧ��
	if (nPos + 4 == vecKline.size())
	{
		return false;
	}

	bRet = double_step_third(vecKline, nPos);
	if (!bRet)
	{
		//������ʧ��
		return false;
	}
	bRet = double_step_fourth(vecKline, nPos);
	if (!bRet)
	{
		//���Ĳ�ʧ��
		return false;
	}
	bRet = double_step_fifth(vecKline, nPos);
	if (!bRet)
	{
		//���岽ʧ��
		return false;
	}

	bRet = is_fairing(vecKline, nPos, bFiring);
	if (bRet)
	{
		//ɸѡ�ɹ�
		output.insert(make_pair(input.first, tagOutput()));
	}

	return false;
}

bool CCaculateAlg::single_plat(const std::pair<tagStockCodeInfo, std::vector<tagKline>> & input, std::map<tagStockCodeInfo,
	tagOutput> & output, short avgFac, bool bFiring /*= false*/)
{
	LOG("��ʼ=========================\n��ƽ̨-���߲���[%d]\n", avgFac);

	int nPos = 0;  //����������K��λ��
	bool bRet = false;
	int nKnb = 0;


	PrintData(input.second);
	std::vector<tagKline> vecKline = input.second;
	if (vecKline.size() < unsigned(avgFac))
	{
		//���߲������㣬������ڵ���nMin��K�ߣ����Ҳ�����nMax��k�����̼�>����ֵ��������
		LOG("����[%d]��������, ֻ��[%d]��k��\n", avgFac, vecKline.size());
		return false;
	}

	bRet = single_double_step_one(vecKline, avgFac, nPos, nKnb, 25, 10);
	if (!bRet)
	{
		//��һ��ʧ��
		return false;
	}
	bRet = single_double_step_two(vecKline, nPos);

	if (!bRet)
	{
		//�ڶ���ʧ��
		return false;
	}
	//ֻ��N+4 ʧ��
	if (nPos + 4 == vecKline.size())
	{
		return false;
	}

	bRet = double_step_third(vecKline, nPos);
	if (!bRet)
	{
		//������ʧ��
		return false;
	}
	bRet = double_step_fourth(vecKline, nPos);
	if (!bRet)
	{
		//���Ĳ�ʧ��
		return false;
	}
	bRet = double_step_fifth(vecKline, nPos);
	if (!bRet)
	{
		//���岽ʧ��
		return false;
	}

	bRet = is_fairing(vecKline, nPos, bFiring);
	if (bRet)
	{
		//ɸѡ�ɹ�
		output.insert(make_pair(input.first, tagOutput()));
	}

	return false;
}

void CCaculateAlg::PrintData(const std::vector<tagKline>& kLineData)
{
	auto it = kLineData.begin();
	for (; it != kLineData.end(); ++it) {
		LOG_DATA("%lld,%f,%f,%f,%f\n", it->time, it->open, it->high, it->low, it->close);
	}

	printf("��ȡ%d������,�� �� �� ��\n\n", kLineData.size());
}

