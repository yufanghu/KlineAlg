// stock.cpp ��
//
#include "stdafx.h"
#include "stock2.h"

using namespace std;


/*bool stock_second::operator<(const tBaseInfo& a1, const tBaseInfo& a2)
{  
	if (a1.sSetcode >= a2.sSetcode)
		return false;  
	return true;  
}*/ 



extern void  stamp_to_standard(time_t stampTime, char* s, char* format = NULL );

bool CmpLowClosePri(const DataT& a, const DataT& b)
{
	return a.dColsePrice < b.dColsePrice;  //����ȡ�Ⱥ� ����
}

//������߻���������̼�
bool CFilter2Alg::GetLowOrHighClose(const std::vector<tagKline>& itvDataBegin, int start, int end,
	tagKline& tagResult, int& nPos,bool bLow /* = false */)
{
	// ����۸���ͬ��A�Ͱ�����ͼ����� B1�� B2 ��߼�����
	if (itvDataBegin.size() < 2)
	{
		m_pLog->logRecord("�������̼����ݲ���\n");
		return false;
	}


	tagResult.close = itvDataBegin.begin()->close;
	if (bLow)
	{
		//��ȡ������̼�
		end = itvDataBegin.size() < end ? itvDataBegin.size() : end;
		for (int i = start; i < end; ++i )
		{
			
			if (tagResult.close > itvDataBegin[i].close)
			{
				tagResult = itvDataBegin[i];
				nPos = i;
			}
			else if (tagResult.close == itvDataBegin[i].close)
			{
				tagResult = tagResult.low > itvDataBegin[i].close ? itvDataBegin[i] : tagResult;
			}
		}
		

	}
	else
	{
		//��ȡ������̼�  ������̼���ͬ����߼۵͵�Ϊ������̼� 
		for (int i = start; i < end; ++i)
		{
			if (tagResult.close < itvDataBegin[i].close)
			{
				tagResult = itvDataBegin[i];
			}
			else if (tagResult.close == itvDataBegin[i].close)
			{
				tagResult = tagResult.high > itvDataBegin[i].high ? itvDataBegin[i] : tagResult;
			}
		}
	}
	

	return true;
}


bool CmpLowPri(const DataT& a, const DataT& b)
{
    return a.dLowPrice < b.dLowPrice;  //
}

//���� ��ͼ�
bool CFilter2Alg::GetLow( std::vector<tagKline>::const_iterator itvDataBegin,
	 std::vector<tagKline>::const_iterator itvDataEnd, tagKline tResult)
{
    
    if (itvDataBegin == itvDataEnd)
    {
		m_pLog->logRecord("GetLow start == end\n");
        return false;
    }

	tResult = *itvDataBegin;
	for (auto it = itvDataBegin + 1; it != itvDataEnd; ++it)
	{
		if (tResult.low > it->low)
		{
			tResult = *it;
		}
		
	}

    return true;
}





bool CFilter2Alg::filterStepA1(const std::vector<tagKline>& vecKline, tagKline& tLowA, int& nApos, 
		tagKline& tHighB1,int& nB1Pos,
		tagKline& tHighB2, int& nB2Pos)
{

	//k��С�ڵ���2��������
	if (vecKline.size() <= 2)
	{
		m_pLog->logRecord("k��С��2\n");
		return false;
	}
	tagKline tAPos;
	bool bRet = true;
	char buf[32];
	bool bRet = GetLowOrHighClose(vecKline, 0,vecKline.size(), tAPos,nAPos, true);
	if (!bRet || nAPos == vecKline.size() -1 )
	{
		m_pLog->logRecord("����ɸѡA1�Ҳ����������\n");
		bRet = false;
	}

	stamp_to_standard(tAPos.time, buf);
	m_pLog->logRecord("����ɸѡA1����������ݣ�%s open %f high %f low %f close %f \n", buf, tAPos.open, tAPos.high, tAPos.low, tAPos.close);


	//����A��������ǰ���̼����B1����
	tagKline tB1Pos;
	bRet = GetLowOrHighClose(vecKline, 0, nAPos - 1, tB1Pos,nB1Pos, false);
	if (!bRet)
	{
		m_pLog->logRecord("����ɸѡδ�ҵ�������̼�B1\n");
		bRet = false;
	}
	stamp_to_standard(tB1Pos.time, buf);
	m_pLog->logRecord("����ɸѡB1����������ݣ�%s open %f high %f low %f close %f \n", buf, tB1Pos.open, tB1Pos.high, tB1Pos.low, tB1Pos.close);

	

	//����A�������ں����̼����B2����
	tagKline tB2Pos;
	bRet = GetLowOrHighClose(vecKline, nAPos + 1, vecKline.size(), tB2Pos, nB2Pos, false);
	if (!bRet)
	{
		m_pLog->logRecord("����ɸѡδ�ҵ�������̼�B2\n");
		bRet = false;
	}
	stamp_to_standard(tB2Pos.time, buf);
	m_pLog->logRecord("����ɸѡB2����������ݣ�%s open %f high %f low %f close %f \n", buf, tB2Pos.open, tB2Pos.high, tB2Pos.low, tB2Pos.close);
	return bRet;
}


//��ȡ���߸���
int CFilter2Alg::GetSunLineNum(std::vector<tagKline>::const_iterator itvDataBegin,
	std::vector<tagKline>::const_iterator itvDataEnd)
{
	int iNum = 0;


	std::vector<tagKline>::const_iterator itBegin = itvDataBegin;
	std::vector<tagKline>::const_iterator itEnd = itvDataEnd;
	while (itBegin != itEnd)
	{
	    (itBegin->close >= itBegin->open) ? iNum++:iNum;
		itBegin++;
	}

	return iNum;
}



bool CFilter2Alg::filter2Level1(const  std::map<tagStockCodeInfo, std::vector<tagKline>> &inMap,
	std::map<tagStockCodeInfo, tagOutput> & output, TFirstFilter& tFirFilter)
{


	m_pLog->logRecord("һ��ɸѡ\n");
	m_pLog->logRecord("����ϵ��ka [%d]-[%d]| ����ϵ��kb [%d]-[%d]\n", tFirFilter.tLineNum.iMaxka, tFirFilter.tLineNum.iMinka,
		tFirFilter.tLineNum.iMaxkb, tFirFilter.tLineNum.iMinkb);
	m_pLog->logRecord("����3 [%s] |4 [%]|5 [%s]| 6 [%s] | ��������[%d] | ���߱���[%d]\n", tFirFilter.sA3Switch ? "on" : "off",
		tFirFilter.bA4Switch ? "on" : "off", tFirFilter.bA5Switch ? "on" : "off", tFirFilter.bA6Switch ? "on" : "off", tFirFilter.sCallbackRange,
		tFirFilter.sRbcoe);

	std::map<tagStockCodeInfo, std::vector<tagKline>>::const_iterator iter;
	bool bRet = true;
	for (iter = inMap.begin(); iter != inMap.end(); ++iter)
	{
		

		tagKline tLowA, tHighB1, tHighB2;
		int nAPos = 0, nB1Pos = 0, nB2Pos = 0;  //A��͵�λ��
		const std::vector<tagKline>  & vecKline = iter->second;
		bRet = filterStepA1(vecKline, tLowA,nAPos, tHighB1, nB1Pos, tHighB2 ,nB2Pos);
		if (!bRet)
		{
			m_pLog->clearLog("����A1ʧ��\n");
			continue;
		}
		
		//����B��ɸѡ������B2�����������ݣ�������ĩ�ڣ�k�߸�����������ϵ��KB(6-8Ĭ�ϣ���Χ��
		std::vector<tagKline>::const_iterator ptrmB2Pos = find(vecKline.begin(), vecKline.end(), tB2Pos);
		int iDis = distance(ptrmAPos,ptrmB2Pos);  
		m_pLog->logRecord("����ɸѡA-B2����[%d]\n",iDis);

		if (iDis < tFirFilter.tLineNum.iMinka || tFirFilter.tLineNum.iMaxka < iDis)
		{
			m_pLog->logRecord("����ɸѡA-B2���������� min[%d]<[%d]<max[%d]\n",
				tFirFilter.tLineNum.iMinka,iDis, tFirFilter.tLineNum.iMaxka);
			continue;
		}

		//KA ����A
		iDis = distance(ptrmB2Pos, vecKline.end()) + 1;
		m_pLog->logRecord("����ɸѡB2-end����[%d]\n", iDis);
		if (iDis < tFirFilter.tLineNum.iMinkb || tFirFilter.tLineNum.iMaxkb < iDis)
		{
			m_pLog->logRecord("����ɸѡB2-end���������� min[%d]<[%d]<max[%d]\n", 
				tFirFilter.tLineNum.iMinkb, iDis, tFirFilter.tLineNum.iMaxkb);
			continue;
		}

		//ɸѡ3   0 ��ʾ�����ͣ��ж���AA,AB,AAP �� 1, 2, 3  
		int iDoubleType = 0;
		if (tFirFilter.sA3Switch == 1 || tFirFilter.sA3Switch == 2)
		{
			if (tB2Pos.close >= tB1Pos.close)
			{
				iDoubleType = 1;
				m_pLog->logRecord("ɸѡ3 AA��\n");
			}
			else if (tB2Pos.close < tB1Pos.close)
			{
				iDoubleType = 2;
				m_pLog->logRecord("ɸѡ3 AB��\n");
			}

			if (tFirFilter.sA3Switch != iDoubleType)
			{
				//��ǰ�������ͺ��û�Ҫѡ��ģ���һ�£�������������
				m_pLog->logRecord("��ǰ����:[%d] �Ϳ������Ͳ�ͬ [%d]\n", iDoubleType, tFirFilter.sA3Switch);
				continue;
			}
		}

		//ɸѡ4 : ������AA�ͣ���ֻ��AA�͵�����  
		if (tFirFilter.bA4Switch && iDoubleType == 1)
		{
			//ɸѡ��B2���ݺ�������̼۸�L2���ݣ�������B2���ݣ�
			vecKlineTmp.assign(ptrmB2Pos + 1, vecKline.end());
			tagKline tL2Pos;
			bRet = GetLowClose(vecKlineTmp, vecKlineTmp.size(), tL2Pos, true);

			//todo: û��L2
			if (!bRet)
			{
				m_pLog->logRecord("ɸѡ4 û��L2\n");
				continue;
			}

			if (tL2Pos.close > tB1Pos.close)
			{
				iDoubleType = 3; //AAP
			}
			else
			{
				continue;
			}
		}

		//ɸѡ5
		if (tFirFilter.bA5Switch)
		{
			double h1 = tB2Pos.high - tAPos.low;
			m_pLog->logRecord("ɸѡ5 ����h1:[%f]\n", h1);

			tagKline tEndData;
			if (ptrmB2Pos + 1 != vecKline.end())
			{
				bRet =  GetLow(ptrmB2Pos, vecKline.end(), tEndData);
			}
			else
			{
				m_pLog->logRecord("B2����ĩβ����\n");
				continue;
			}

			double h2 = tB2Pos.high - tEndData.low;
			m_pLog->logRecord("ɸѡ5 ����h2:[%f]\n", h2);
			
			if ((h1 == 0))
			{
				m_pLog->logRecord("h1 is 0\n");
				continue;
			}
			double ht = h2 / h1;
			if (ht > static_cast<double>(tFirFilter.sCallbackRange / 100))
			{
				m_pLog->logRecord("ht[%f]����������\n", ht);
				continue;
			}
		}

		//ɸѡ6
		if (tFirFilter.bA6Switch)
		{
			//ͳ��A���ݣ����������ݣ�������ĩ�ڣ���K����Ŀ
			int iSunlineNum = 0;
			int iDisFromAToEnd = 0;
			float rb = 0.0;

			iSunlineNum = GetSunLineNum(ptrmAPos, vecKline.end());
			iDisFromAToEnd = distance(ptrmAPos, vecKline.end());
			if (iDisFromAToEnd == 0)
			{
				m_pLog->logRecord("ɸѡ6����\n");
				continue;
			}
			rb = (float)iSunlineNum / iDisFromAToEnd;
			m_pLog->logRecord("������:[%d] A���ݵ�����ĩβK�߸���[%d]\n ", iSunlineNum, iDisFromAToEnd);
			
			//todo: ɸѡ���� ���ڵ�������
			if (rb < static_cast<double>(tFirFilter.sRbcoe / 100))
			{
				m_pLog->logRecord("rb[%f] < ����  rb\n",rb);
				continue;
			}
		}

		//����һ��ɸѡ���
		output[iter->first] = tagOutput();
	}
	return true;

}
	



bool CFilter2Alg::filter2Level2(std::map<tagStockCodeInfo, std::vector<tagKline>> &inMap, TSecondFilter& tSedFilter)
{
	std::map<tBaseInfo, tOutData>  outMap;

	//������־
	logstream2.open("./stock.log", ofstream::app);
	logforpara2(inMap);

	logstream2 << "����ɸѡ����" << endl;
	logstream2 << "���ka | ��Сka | ���kb | ��Сkb�� " << tSedFilter.tLineNum.iMaxka << " " << tSedFilter.tLineNum.iMinka
		<< " " << tSedFilter.tLineNum.iMaxkb << " " << tSedFilter.tLineNum.iMinkb << endl;
	logstream2 << "����5�͵������ȣ�" << tSedFilter.bA5Switch << " " << tSedFilter.sCallbackRange << endl;

	//ѭ������ÿֻ��Ʊ
	std::map<tBaseInfo, std::vector<DataT>>::iterator itMap = inMap.begin();
	while (itMap != inMap.end())
	{
		logstream2 << "�г��� " << itMap->first.sSetcode << " ���� " << itMap->first.sTockcode << endl;
		std::vector<DataT>& vStockData = itMap->second;

		//todo: k��С�ڵ���2��������
		if (vStockData.size() <= 2)
		{
			logstream2 << "k��С��2 ";
			cout << "k��С��2 " << endl;
			itMap++;
			continue;
		}
		//todo: A B1 B2 û��ֵ���ͽ�����һֻ��Ʊ����

		//1.ɸѡ1  ���̼���ͼ�
		DataT tAPos = GetLowClose(vStockData.begin(), vStockData.end(), true);
		if (tAPos.uDate == -1)
		{
			logstream2 << " ��ǰ����û��A " << endl;
			itMap++;
			continue;
		}
		logstream2 << "A �� " << tAPos.dColsePrice << "  " << tAPos.dLowPrice << " |  ";
		std::vector<DataT>::iterator ptrmAPos = find(vStockData.begin(), vStockData.end(), tAPos);
		if (ptrmAPos == vStockData.end())
		{
			logstream2 << "��A ��" << endl;
			cout << "not find Apos" << endl;
			itMap++;
			continue;
		}

		//todo:B1��B2 ������A
		DataT tB1Pos = GetLowClose(vStockData.begin(), ptrmAPos, false);
		if (tB1Pos.uDate == -1)
		{
			logstream2 << " ��ǰ����û��B1 " << endl;
			itMap++;
			continue;
		}
		logstream2 << "B1 �� " << tB1Pos.dColsePrice << "  " << tB1Pos.dHighPrice << " | ";



		DataT tB2Pos = GetLowClose(ptrmAPos + 1, vStockData.end(), false);
		if (tB2Pos.uDate == -1)
		{
			logstream2 << " ��ǰ����û��B2 " << endl;
			itMap++;
			continue;
		}
		logstream2 << "B2 �� " << tB2Pos.dColsePrice << " | " << tB2Pos.dHighPrice << " | ";

		//ɸѡ2 
		//KB ����B
		std::vector<DataT>::iterator ptrmB2Pos = find(vStockData.begin(), vStockData.end(), tB2Pos);
		int iDis = distance(ptrmB2Pos, vStockData.end());

		logstream2 << "KB �� " << iDis << " | ";
		if (iDis < tSedFilter.tLineNum.iMinka || tSedFilter.tLineNum.iMaxka < iDis)
		{
			cout << "����������ϵ��KB��Χ" << endl;
			itMap++;
			continue;
		}

		//KA ����A
		iDis = distance(ptrmAPos, ptrmB2Pos) + 1;
		logstream2 << "KA �� " << iDis << " | ";
		if (iDis < tSedFilter.tLineNum.iMinkb || tSedFilter.tLineNum.iMaxkb < iDis)
		{
			cout << "����������ϵ��KA��Χ" << endl;
			itMap++;
			continue;
		}

		//ɸѡ5
		if (tSedFilter.bA5Switch)
		{
			double h1 = tB2Pos.dHighPrice - tAPos.dLowPrice;
			logstream2 << "h1: " << h1 << " | ";
			DataT tEndData;
			if (ptrmB2Pos + 1 != vStockData.end())
			{
				tEndData = GetLow(ptrmB2Pos, vStockData.end());
			}

			double h2 = tB2Pos.dHighPrice - tEndData.dLowPrice;
			logstream2 << " h2 " << h2 << " ";
			double ht = h2 / h1;
			if ((h1 == 0) || ht > static_cast<double>(tSedFilter.sCallbackRange / 100))
			{
				logstream2 << " ht:" << ht << " | ";
				itMap++;
				continue;
			}
		}

		//���ض���ɸѡ���
		tOutData tEmpty;
		outMap.insert(make_pair(itMap->first, tEmpty));

		itMap++;
	}

	PrintHitStock(outMap, "����ɸѡѡ�й�Ʊ�б�");

	logstream2.close();
	return outMap;
}

bool CFilter2Alg::filter2Level3(std::map<tagStockCodeInfo, std::vector<tagKline>> &inMap, TThirdFilter& tThdFilter)
{
	std::map<tBaseInfo, tOutData>  outMap;

	//������־
	logstream2.open("./stock.log", ofstream::app);
	logforpara2(inMap);
	logstream2 << "����ɸѡ����" << endl;
	logstream2 << "���ka | ��Сka | ���kb | ��Сkb�� " << tThdFilter.tLineNum.iMaxka << " " << tThdFilter.tLineNum.iMinka << "  "
		<< tThdFilter.tLineNum.iMaxkb << " " << tThdFilter.tLineNum.iMinkb << endl;
	logstream2 << "����5 | ��������: " << tThdFilter.bA5Switch << " " << static_cast<double>(tThdFilter.sCallbackRange / 100) << endl;

	//ѭ������ÿֻ��Ʊ
	std::map<tBaseInfo, std::vector<DataT>>::iterator itMap = inMap.begin();
	while (itMap != inMap.end())
	{
		logstream2 << "�г��� " << itMap->first.sSetcode << " ���� " << itMap->first.sTockcode << endl;
		std::vector<DataT>& vStockData = itMap->second;

		//todo: k��С�ڵ���2��������
		if (vStockData.size() <= 2)
		{
			logstream2 << "k��С��2 ";
			cout << "k��С��2 " << endl;
			itMap++;
			continue;
		}
		//todo: A B1 B2 û��ֵ���ͽ�����һֻ��Ʊ����

		//1.ɸѡ1  ���̼���ͼ�
		DataT tAPos = GetLowClose(vStockData.begin(), vStockData.end(), true);
		if (tAPos.uDate == -1)
		{
			logstream2 << " ��ǰ����û��A " << endl;
			itMap++;
			continue;
		}
		logstream2 << "A �� " << tAPos.dColsePrice << "  " << tAPos.dLowPrice << " |  ";
		std::vector<DataT>::iterator ptrmAPos = find(vStockData.begin(), vStockData.end(), tAPos);
		if (ptrmAPos == vStockData.end())
		{
			logstream2 << "��A ��" << endl;
			cout << "not find Apos" << endl;
			itMap++;
			continue;
		}

		//todo:B1��B2 ������A
		DataT tB1Pos = GetLowClose(vStockData.begin(), ptrmAPos, false);
		if (tB1Pos.uDate == -1)
		{
			logstream2 << " ��ǰ����û��B2 " << endl;
			itMap++;
			continue;
		}
		logstream2 << "B1 �� " << tB1Pos.dColsePrice << "  " << tB1Pos.dHighPrice << " | ";


		DataT tB2Pos = GetLowClose(ptrmAPos + 1, vStockData.end(), false);
		if (tB2Pos.uDate == -1)
		{
			logstream2 << " ��ǰ����û��B2 " << endl;
			itMap++;
			continue;
		}
		logstream2 << "B2 �� " << tB2Pos.dColsePrice << " | " << tB2Pos.dHighPrice << " | ";

		//ɸѡ2 
		//KB ����B
		std::vector<DataT>::iterator ptrmB2Pos = find(vStockData.begin(), vStockData.end(), tB2Pos);
		int iDis = distance(ptrmB2Pos, vStockData.end());

		logstream2 << "KB �� " << iDis << " | ";
		if (iDis < tThdFilter.tLineNum.iMinka || tThdFilter.tLineNum.iMaxka < iDis)
		{
			cout << "����������ϵ��KB��Χ" << endl;
			itMap++;
			continue;
		}

		//KA ����A
		iDis = distance(ptrmAPos, ptrmB2Pos) + 1;
		logstream2 << "KA �� " << iDis << " | ";
		if (iDis < tThdFilter.tLineNum.iMinkb || tThdFilter.tLineNum.iMaxkb < iDis)
		{
			cout << "����������ϵ��KA��Χ" << endl;
			itMap++;
			continue;
		}

		//ɸѡ5
		if (tThdFilter.bA5Switch)
		{
			double h1 = tB2Pos.dHighPrice - tAPos.dLowPrice;
			logstream2 << "h1: " << h1 << " | ";
			DataT tEndData;
			if (ptrmB2Pos + 1 != vStockData.end())
			{
				tEndData = GetLow(ptrmB2Pos, vStockData.end());
			}

			double h2 = tB2Pos.dHighPrice - tEndData.dLowPrice;
			logstream2 << " h2 " << h2 << " ";
			double ht = h2 / h1;
			if ((h1 == 0) || (ht > static_cast<double>(tThdFilter.sCallbackRange / 100)))
			{
				logstream2 << " ht:" << ht << " | ";
				itMap++;
				continue;
			}
		}

		//��������ɸѡ���
		tOutData tEmpty;
		outMap.insert(make_pair(itMap->first, tEmpty));

		itMap++;
	}

	PrintHitStock(outMap, "����ɸѡѡ�й�Ʊ�б�");

	logstream2.close();
	return outMap;

}

