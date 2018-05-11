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
//�������ֵ
	double dSum = 0.0;
	int i;
	if (nStart - nCount < 0 || nCount <= 0)
	{
		//���Ϸ�����
		LOG("���߲�������");
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
	������ĩβ��ǰɸѡ
	������������ڵ���nMin��K�ߣ����Ҳ�����nMax��k�����̼� > ����ֵ��������
	��¼����K����Ŀ��KNB, �ӵ�1�����̼۸��ھ�������
	�����ϸù�Ʊ����ɸѡ���������ؿ�ֵ
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
				printf("ƽ̨�ĵڶ�������ʼ����  count %d date %s close %f \n",nKnb, ss, kLineData[nPos].close);
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
#ifdef _DEBUG
				char ss[32];
				stamp_to_standard(kLineData[nPos].time, ss);
				printf("ƽ̨�ĵڶ�������ʼ����  count %d %s close %f \n", nKnb, ss, kLineData[nPos].close);
#endif // _DEBUG
				return true;
			}
			else
			{
				nCount = 1;
				continue;
			}
		}
		
	}
	LOG("ƽ̨��һ��ɸѡʧ�� Pos[%d]", nPos);
	return false;
}



bool CCaculateAlg::single_multi_step_two(const std::vector<tagKline>& kLineData, int& nPos)
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
#ifdef _DEBUG
					char ss[32];
					stamp_to_standard(kLineData[j].time, ss);
					printf("ƽ̨�ڶ���ɸѡ��� date %s close %f high %f\n", ss, kLineData[j].close, kLineData[i].high);
#endif // _DEBUG
					nPos = j - 4;

					stamp_to_standard(kLineData[nPos].time, ss);
					LOG("ƽ̨�ڶ���ɸѡ�ɹ� date[%s]", ss);
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
	LOG("ƽ̨�ڶ���ɸѡʧ�� Pos[%s]", ss);
	return false;
}

bool CCaculateAlg::single_plat_step_third(const std::vector<tagKline>& kLineData, int& nPos)
{
	int nEnd = kLineData.size() - 2;

	for (int i = nPos + 5; i <= nEnd; ++i)
	{

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
	LOG("��ƽ̨������ɸѡ�ɹ� date[%s]", ss);

#ifdef _DEBUG
	printf(" ��ƽ̨������ɸѡ�ɹ�\n");
#endif // DEBUG
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
		
	}
	if (firRes)
	{
		char ss[32];
		stamp_to_standard(kLineData[size-1].time, ss);
		LOG("�𱬳ɹ� date %s close %f high %f ", ss, kLineData[size - 1].close, kLineData[nPos].high);
#ifdef _DEBUG
		printf("�𱬳ɹ� date %s close %f high %f\n ", ss, kLineData[size - 1].close, kLineData[nPos].high);
#endif
	}
	else
	{
		LOG("��ʧ��");
#ifdef _DEBUG
		printf("��ʧ��\n");
#endif
	}
	return firRes;
	
}

bool CCaculateAlg::single_plat(const std::map<tagStockCodeInfo, std::vector<tagKline>> & input, std::map<tagStockCodeInfo,
						tagOutput> & output, EPlatFormType platformType, short avgFac, bool bFiring /*= false*/)
{
	std::map<tagStockCodeInfo, std::vector<tagKline>>::iterator iter ;
	std::map<tagStockCodeInfo, std::vector<tagKline>> mapInput = input;
	int nPos = 0;  //����������K��λ��
	bool bRet = false;
	int nKnb = 0;
	for (iter = mapInput.begin(); iter != mapInput.end(); ++iter)
	{
		tagStockCodeInfo tagOne = iter->first;
		//K������
		std::vector<tagKline> vecKline = iter->second;
		if (vecKline.size() - 5 < avgFac)
		{
			//���߲������㣬������ڵ���5��K�ߣ����Ҳ�����15��k�����̼�>����ֵ��������
			
			return false;
		}
		

		bRet = single_multi_step_one(vecKline, avgFac, nPos, nKnb,15, 5);
		if (!bRet)
		{
			//��һ��ʧ��
			continue;
		}
		bRet = single_multi_step_two(vecKline, nPos);
		if (!bRet)
		{
			//�ڶ���ʧ��
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
			//ɸѡ�ɹ�
			output[tagOne] = {};
		}
	}

	return false;
}

bool CCaculateAlg::multi_step_third(const std::vector<tagKline>& kLineData, int& nPos)
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
			LOG("��ƽ̨������ɸѡ�ɹ� date[%s]", ss);
#ifdef _DEBUG
			printf("��ƽ̨������ɸѡ�ɹ� date[%s]", ss);
#endif
			return true;
		}
		else
		{
			continue;
		}
	}
	char ss[32];
	stamp_to_standard(kLineData[nPos].time, ss);
	LOG("��ƽ̨������ɸѡʧ�� date[%s]", ss);

#ifdef _DEBUG
	printf("��ƽ̨������ɸѡʧ�� date[%s]", ss);
#endif
	return false;
}

bool CCaculateAlg::multi_step_fourth(const std::vector<tagKline>& kLineData, int& nPos)
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
					LOG("��ƽ̨���Ĳ�ɸѡ�ɹ� date[%s]", ss);
					#ifdef _DEBUG
					printf("��ƽ̨���Ĳ�ɸѡ�ɹ� date[%s]\n", ss);
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
	LOG("��ƽ̨���Ĳ�ɸѡʧ�� date[%s]", ss);
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
			LOG("��ƽ̨���岽ɸѡʧ�� date[%s]", ss);

#ifdef _DEBUG
			printf("��ƽ̨���岽ɸѡʧ�� date[%s]", ss);
#endif // _DEBUG

			return false;
		}
	}

	char ss[32];
	stamp_to_standard(kLineData[nPos].time, ss);
	LOG("��ƽ̨���岽ɸѡ�ɹ� date[%s]", ss);

#ifdef _DEBUG
	printf("��ƽ̨���岽ɸѡ�ɹ� date[%s]", ss);
#endif // _DEBUG

	return true;
}
bool CCaculateAlg::multi_plat(const std::map<tagStockCodeInfo, std::vector<tagKline>> & input,
	std::map<tagStockCodeInfo, tagOutput> & output, EPlatFormType platformType,
	short avgFac, bool bFiring /*= false*/)
{
	std::map<tagStockCodeInfo, std::vector<tagKline>>::iterator iter;
	std::map<tagStockCodeInfo, std::vector<tagKline>> mapInput = input;
	int nPos = 0;  //����������K��λ��
	bool bRet = false;
	int nKnb = 0;
	for (iter = mapInput.begin(); iter != mapInput.end(); ++iter)
	{
		tagStockCodeInfo tagOne = iter->first;
		//K������
		std::vector<tagKline> vecKline = iter->second;
		if (vecKline.size()  < unsigned(avgFac) )
		{
			//���߲������㣬������ڵ���nMin��K�ߣ����Ҳ�����nMax��k�����̼�>����ֵ��������
			LOG("K������ %d < ���߲��� %d", vecKline.size(), avgFac);
			return false;
		}

		bRet = single_multi_step_one(vecKline, avgFac, nPos, nKnb, 25, 10);
		if (!bRet)
		{
			//��һ��ʧ��
			continue;
		}
		bRet = single_multi_step_two(vecKline, nPos);
		
		if (!bRet)
		{
			//�ڶ���ʧ��
			continue;
		}
		//ֻ��N+4 ʧ��
		if (nPos + 4 == vecKline.size())
		{
			continue;
		}
		
		bRet = multi_step_third(vecKline, nPos);
		if (!bRet)
		{
			//������ʧ��
			continue;
		}
		bRet = multi_step_fourth(vecKline, nPos);
		if (!bRet)
		{
			//���Ĳ�ʧ��
			continue;
		}
		bRet = multi_step_fifth(vecKline, nPos);
		if (!bRet)
		{
			//���岽ʧ��
			continue;
		}

		bRet = is_fairing(vecKline, nPos, bFiring);
		if (bRet)
		{
			//ɸѡ�ɹ�
			output[tagOne] = {};
		}
	}
	return false;
}


