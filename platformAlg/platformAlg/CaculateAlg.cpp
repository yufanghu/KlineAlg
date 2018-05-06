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
//�������ֵ
	double dSum = 0.0;
	int i;
	if (nStart - nCount < 0 || nCount <= 0)
	{
		//���Ϸ�����
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
	������������ڵ���5��K�ߣ����Ҳ�����15��k�����̼� > ����ֵ��������
	��¼����K����Ŀ��KNB, �ӵ�1�����̼۸��ھ������𣬱���ɸѡ���Ϊ21����
	�����ϸù�Ʊ����ɸѡ���������ؿ�ֵ
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
//��ѡ��Ϊtrueʱ�����жϣ�����Ĭ�ϳɹ�
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

		bRet = single_multi_step_three(vecKline, nPos);
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
	int nPos = 0;  //����������K��λ��
	bool bRet = false;
	int nKnb = 0;
	for (iter = mapInput.begin(); iter != mapInput.end(); ++iter)
	{
		tagStockCodeInfo tagOne = iter->first;
		//K������
		std::vector<tagKline> vecKline = iter->second;
		if (vecKline.size() - 10 < avgFac)
		{
			//���߲������㣬������ڵ���10��K�ߣ����Ҳ�����25��k�����̼�>����ֵ��������
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
		//��ȷ�� ��N+5Ϊ���һ���Ƿ���ɹ�
		//ֻ��N+4 ʧ��
		if (nPos + 4 == vecKline.size())
		{
			continue;
		}
		bRet = single_multi_step_three(vecKline, nPos);
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

	return false;
}


