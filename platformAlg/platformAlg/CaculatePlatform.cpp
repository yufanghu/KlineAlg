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
//�������ֵ
	double dSum = 0.0;
	int i;
	if (nStart + 1 - nCount  < 0 || nCount <= 0)
	{
		//���Ϸ�����
		m_pLog->logRecord("������߲�������\n");
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
					m_pLog->logRecord("k��ʱ�����Ч pos[%d]-time[%I64d]\n", i, kLineData[i].time);
					return false;
				}
				m_pLog->logRecord("step 1[%d] %s close %f avg %f \n", nCount, buf.c_str(), kLineData[i].close, dAvg);
				nCount++;
			}
			else
			{
				m_pLog->logRecord("step 1ɸѡʧ�� ����[%d]������%d������������\n", nMax, nMax+1);
				return false;
			}

		}
		else
		{
			if (nCount >= nMin)
			{
				nPos = i + 1;
				nKnb = nCount;
				m_pLog->logRecord("step 1ɸѡ�ɹ� knb[%d]\n", nKnb - 1);
				return true;
			}
			else if (i == kLineData.size() - 1)
			{
				m_pLog->logRecord("step 1ɸѡʧ�� ��һ�����ݲ�����\n");
				return false;
			}
			else
			{
				nCount = 1;
				m_pLog->logRecord("step 1ɸѡʧ�� С����С������%d��\n",nMin);
				return false;
			}
		}

	}
	m_pLog->logRecord("ƽ̨��һ��ɸѡʧ�� Pos[%d]\n", nPos);
	return false;
}

bool CCaculateAlg::single_double_step_two(const std::vector<tagKline>& kLineData, int& nPos)
{
	/*
	FOR(N=1;N<=8;N++)
	�������N+1��.N+4 �������̼۶�����N������߼ۡ�
	����N<=8 ����¼Nֵ�����˲��趨λ��N��������K�ߣ�
	��ô�ù��� ����¼Nֵ�����˲��趨λ��N��������K�ߣ�
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
						m_pLog->logRecord("k��ʱ�����Ч pos[%d]-time[%I64d]\n", nPos, kLineData[nPos].time);
						return false;
					}

					m_pLog->logRecord("step2ɸѡ�ɹ� date[%s] close %f high %f\n", ss.c_str(), kLineData[nPos].close, kLineData[nPos].high);
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
		m_pLog->logRecord("k��ʱ�����Ч pos[%d]-time[%I64d]\n", nPos, kLineData[nPos].time);
		return false;
	}
	m_pLog->logRecord("ƽ̨�ڶ���ɸѡʧ�� N[%s]\n", ss.c_str());

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
		m_pLog->logRecord("k��ʱ�����Ч pos[%d]-time[%I64d]\n", i, kLineData[i].time);
		return false;
	}

		//stamp_to_standard(kLineData[i].time, ss);
		
		if (kLineData[i].close < kLineData[nPos].high)
		{
			m_pLog->logRecord("step3 %s close %f С�� high %f\n", ss.c_str(), kLineData[i].close, kLineData[nPos].high);
			continue;
		}
		else
		{
			m_pLog->logRecord("step3 %s close %f ��С�� high %f\n�� ������ʧ��", ss.c_str(), kLineData[i].close, kLineData[nPos].high);
			return false;
		}
	}
	
	std::string ss;
	if(!stamp_to_standard(kLineData[nPos].time, ss))
	{
		m_pLog->logRecord("k��ʱ�����Ч pos[%d]-time[%I64d]\n",nPos, kLineData[nPos].time);
		return false;
	}


	m_pLog->logRecord("��ƽ̨������ɸѡ�ɹ�[%s]\n", ss.c_str());
	return true;
}

bool CCaculateAlg::is_fairing(const std::vector<tagKline>& kLineData, int& nPos, bool isFiring)
{
//��ѡ��Ϊtrueʱ�����жϣ�����Ĭ�ϳɹ�
	int size = kLineData.size();
	bool firRes = true;
	if (isFiring)
	{
		if((nPos+4) == size -1){
			m_pLog->logRecord("��ʧ��\n");
			return false;
		}

		firRes = kLineData[size - 1].close > kLineData[nPos].high ? true : false;
		if (firRes)
		{
			
			std::string ss;
			if(!stamp_to_standard(kLineData[size - 1].time, ss))
			{
				m_pLog->logRecord("k��ʱ�����Ч pos[%d]-time[%I64d]\n",size - 1, kLineData[size - 1].time);
				return false;
			}

			m_pLog->logRecord("�𱬳ɹ� %s close %f high %f\n", ss, kLineData[size - 1].close, kLineData[nPos].high);
			return true;
		}
		else
		{
			m_pLog->logRecord("��ʧ��\n");
			return false;
		}
	}
	else
	{
		m_pLog->logRecord("�𱬹رգ�ɸѡ�ɹ�\n");
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
			std::string ss;
			if(!stamp_to_standard(kLineData[nPos].time, ss))
			{
				m_pLog->logRecord("k��ʱ�����Ч pos[%d]-time[%I64d]\n",nPos, kLineData[nPos].time);
				return false;
			}

			//char ss[32];
			//stamp_to_standard(kLineData[nPos].time, ss);
			m_pLog->logRecord("˫ƽ̨������ɸѡ�ɹ� [%s]\n", ss.c_str());
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
		m_pLog->logRecord("k��ʱ�����Ч pos[%d]-time[%I64d]\n",nPos, kLineData[nPos].time);
		return false;
	}


	m_pLog->logRecord("˫ƽ̨������ɸѡʧ�� [%s]\n", ss.c_str());
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
						m_pLog->logRecord("k��ʱ�����Ч pos[%d]-time[%I64d]\n",nPos, kLineData[i].time);
						return false;
					}

					m_pLog->logRecord("˫ƽ̨���Ĳ�ɸѡ�ɹ� [%s]\n", ss.c_str());
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
		m_pLog->logRecord("k��ʱ�����Ч pos[%d]-time[%I64d]\n",nPos, kLineData[nPos].time);
		return false;
	}

	//char ss[32];
	//stamp_to_standard(kLineData[nPos].time, ss);
	m_pLog->logRecord("˫ƽ̨���Ĳ�ɸѡʧ�� [%s]\n", ss.c_str());
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
				m_pLog->logRecord("k��ʱ�����Ч pos[%d]-time[%I64d]\n",nPos, kLineData[nPos].time);
				return false;
			}

			m_pLog->logRecord("˫ƽ̨���岽ɸѡʧ�� [%s]\n", ss.c_str());
			return false;
		}
	}


	std::string ss;
	if(!stamp_to_standard(kLineData[nPos].time, ss))
	{
		m_pLog->logRecord("k��ʱ�����Ч pos[%d]-time[%I64d]\n",nPos, kLineData[nPos].time);
		return false;
	}

	m_pLog->logRecord("˫ƽ̨���岽ɸѡ�ɹ� [%s]", ss.c_str());
	return true;
}

bool CCaculateAlg::double_plat(const std::map<tagStockCodeInfo, std::vector<tagKline>> & input,
	std::map<tagStockCodeInfo, tagOutput> & output, short avgFac, bool bFiring /*= false*/)
{
	std::string buf;
	stamp_to_standard(time(NULL), buf,"%Y-%m-%d %H:%M:%S");

	if (bFiring)
		m_pLog->logRecord("��ʼ[%s]\n=========================\n˫ƽ̨-���߲���[%d]��[����]\n", buf.c_str(), avgFac);
	else
		m_pLog->logRecord("��ʼ[%s]\n=========================\n˫ƽ̨-���߲���[%d]��[�ر�]\n", buf.c_str(), avgFac);

	std::map<tagStockCodeInfo, std::vector<tagKline>>::const_iterator iter;
	//std::map<tagStockCodeInfo, std::vector<tagKline>> mapInput = input;


	int nPos = 0;  //����������K��λ��
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
				m_pLog->logRecord("�����������Ѿ������������˳�");
				return true;
			}
		}

		PrintHead(iter->first, ++nIndex);
		PrintData(iter->second);
		tagStockCodeInfo tagOne = iter->first;
		//K������
		const std::vector<tagKline>  & vecKline = iter->second;
		if (vecKline.size()  < unsigned(avgFac) )
		{
			//���߲������㣬������ڵ���nMin��K�ߣ����Ҳ�����nMax��k�����̼�>����ֵ��������
			m_pLog->logRecord("����[%d]��������, ֻ��[%d]��k��\n", avgFac, vecKline.size());
			continue;
		}

		bRet = single_double_step_one(vecKline, avgFac, nPos, nKnb, 25, 10);
		if (!bRet)
		{
			//��һ��ʧ��
			continue;
		}
		bRet = single_double_step_two(vecKline, nPos);
		
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
		
		bRet = double_step_third(vecKline, nPos);
		if (!bRet)
		{
			//������ʧ��
			continue;
		}
		bRet = double_step_fourth(vecKline, nPos);
		if (!bRet)
		{
			//���Ĳ�ʧ��
			continue;
		}

		if ( nPos + 4 == vecKline.size() - 1)
		{
			if(bFiring){
				m_pLog->logRecord("��ʧ��,�����ݲ���\n");
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
			//���岽ʧ��
			continue;
		}

		bRet = is_fairing(vecKline, nPos, bFiring);
		if (bRet)
		{
			//ɸѡ�ɹ�
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
		m_pLog->logRecord("��ʼ[%s]\n=========================\n��ƽ̨-���߲���[%d]��[����]\n", buf.c_str(), avgFac);
	else
		m_pLog->logRecord("��ʼ[%s]\n=========================\n��ƽ̨-���߲���[%d]��[�ر�]\n", buf.c_str(), avgFac);

	std::map<tagStockCodeInfo, std::vector<tagKline>>::const_iterator iter;
	//std::map<tagStockCodeInfo, std::vector<tagKline>> mapInput = input;

	int nPos = 0;  //����������K��λ��
	bool bRet = false;
	int nKnb = 0;
	int nIndex = 0;
	for (iter = input.begin(), nIndex = 0; iter != input.end(); ++iter)
	{
		if(iter->second.size() > 0){
			tagKline temp = iter->second.at(iter->second.size()-1);
			if (!CheckLicense(temp.time)){
				m_pLog->clearLog();
				m_pLog->logRecord("�����������Ѿ������������˳�\n");
				return true;
			}				
		}
		PrintHead(iter->first, ++nIndex);
		PrintData(iter->second);

		tagStockCodeInfo tagOne = iter->first;
		//K������
		const std::vector<tagKline> vecKline = iter->second;
		if (vecKline.size() - 5 < avgFac)
		{
			//���߲������㣬������ڵ���5��K�ߣ����Ҳ�����15��k�����̼�>����ֵ��������
			m_pLog->logRecord("����[%d]��������, ֻ��[%d]��k��\n", avgFac, vecKline.size());
			continue;
		}

		bRet = single_double_step_one(vecKline, avgFac, nPos, nKnb, 15, 5);
		if (!bRet)
		{
			//��һ��ʧ��
			continue;
		}
		bRet = single_double_step_two(vecKline, nPos);
		if (!bRet)
		{
			//�ڶ���ʧ��
			continue;
		}
		if ( nPos + 4 == vecKline.size() - 1)
		{
			if(bFiring){
				m_pLog->logRecord("��ʧ��,�����ݲ���\n");
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
			//ɸѡ�ɹ�
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
	//printf("��ȡ%d������,�� �� �� ��\n\n", kLineData.size());
}


void CCaculateAlg::PrintHead(const tagStockCodeInfo & codeInfo, int nPos)
{
	m_pLog->dataRecord("%d-%s\n", codeInfo.market, codeInfo.stockcode.c_str());
	m_pLog->logRecord("��%d֧��Ʊ[%d-%s]\n", nPos, codeInfo.market, codeInfo.stockcode.c_str());
}