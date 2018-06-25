// stock.cpp ��
//
#include "stdafx.h"
#include "stock2.h"


extern void  stamp_to_standard(time_t stampTime, char* s, char* format = NULL );


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

	tagResult = itvDataBegin[0];
	if (bLow)
	{
		//��ȡ������̼�
		end = itvDataBegin.size() < end ? itvDataBegin.size() : end;
		for (int i = start + 1; i < end; ++i )
		{
			
			if (tagResult.close > itvDataBegin[i].close)
			{
				tagResult = itvDataBegin[i];
				nPos = i;
			}
			else if (tagResult.close == itvDataBegin[i].close)
			{
				tagResult = tagResult.low > itvDataBegin[i].close ? itvDataBegin[i] : tagResult;
				nPos = tagResult.low > itvDataBegin[i].close? i:nPos;
			}
		}
		

	}
	else
	{
		//��ȡ������̼�  ������̼���ͬ����߼۵͵�Ϊ������̼� 
		for (int i = start + 1; i < end; ++i)
		{
			if (tagResult.close < itvDataBegin[i].close)
			{
				tagResult = itvDataBegin[i];
				nPos = i;
			}
			else if (tagResult.close == itvDataBegin[i].close)
			{
				tagResult = tagResult.high < itvDataBegin[i].high ? itvDataBegin[i] : tagResult;
				nPos = tagResult.high > itvDataBegin[i].high ? i : nPos;
			}
		}
	}
	

	return true;
}




//���� ��ͼ�
bool CFilter2Alg::GetLow( std::vector<tagKline>::const_iterator itvDataBegin,
	 std::vector<tagKline>::const_iterator itvDataEnd, tagKline& tResult)
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





bool CFilter2Alg::filterStepA1(const std::vector<tagKline>& vecKline, tagKline& tLowA, int& nAPos, 
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
	bRet = GetLowOrHighClose(vecKline, 0, vecKline.size(), tAPos, nAPos, true);
	if (!bRet || nAPos == vecKline.size() -1 || nAPos == 0 )
	{
		m_pLog->logRecord("����ɸѡA1Ϊ��β\n");
		return false;
	}

	stamp_to_standard(tAPos.time, buf);
	m_pLog->logRecord("����ɸѡA1����������ݣ�%s open %f high %f low %f close %f \n", buf, tAPos.open, tAPos.high, tAPos.low, tAPos.close);


	//����A��������ǰ���̼����B1����
	tagKline tB1Pos;
	bRet = GetLowOrHighClose(vecKline, 0, nAPos - 1, tB1Pos,nB1Pos, false);
	if (!bRet)
	{
		m_pLog->logRecord("����ɸѡδ�ҵ�������̼�B1\n");
		return false;
	}
	stamp_to_standard(tB1Pos.time, buf);
	m_pLog->logRecord("����ɸѡB1����������ݣ�%s open %f high %f low %f close %f \n", buf, tB1Pos.open, tB1Pos.high, tB1Pos.low, tB1Pos.close);

	

	//����A�������ں����̼����B2����
	tagKline tB2Pos;
	bRet = GetLowOrHighClose(vecKline, nAPos + 1, vecKline.size(), tB2Pos, nB2Pos, false);
	if (!bRet)
	{
		m_pLog->logRecord("����ɸѡδ�ҵ�������̼�B2\n");
		return false;
	}
	stamp_to_standard(tB2Pos.time, buf);
	m_pLog->logRecord("����ɸѡB2����������ݣ�%s open %f high %f low %f close %f \n", buf, tB2Pos.open, tB2Pos.high, tB2Pos.low, tB2Pos.close);
	return true;
}


/*
����ϵ��KA Ĭ��Ϊ6-9
����ϵ��KB Ĭ��Ϊ6-8

����A��ɸѡ����A�����������ݣ�������B2��K�߸�����������ϵ��KA(6-9Ĭ�ϣ���Χ��
����B��ɸѡ������B2�����������ݣ�������ĩ�ڣ�k�߸�����������ϵ��KB(6-8Ĭ�ϣ���Χ��
*/
bool CFilter2Alg::filterStepA2(const std::vector<tagKline>& vecKline, TA2 period, int nAPos, int nB2Pos)
{


	//����A��ɸѡ����A�����������ݣ�������B2��K�߸�����������ϵ��KA(6-9Ĭ�ϣ���Χ��
	int nAPeriod = nB2Pos - nAPos + 1;
	if ( nAPeriod < period.iMinka || nAPeriod > period.iMaxka)
	{
		return false;
	}
	//����B��ɸѡ������B2�����������ݣ�������ĩ�ڣ�k�߸�����������ϵ��KB(6-8Ĭ�ϣ���Χ��
	int nBPeriod = vecKline.size() - nB2Pos + 1;
	if (nBPeriod < period.iMinkb || nBPeriod > period.iMaxkb)
	{
		return false;
	}
	
	return true;
}

bool CFilter2Alg::filterStepA3(const std::vector<tagKline>& vecKline, TFilter& tFirFilter, int nB2Pos, int nB1Pos)
{
	/*
	����ɸѡ��A3��(��ע���û�ѡ���Ĭ�Ͽ���OF������˫������ɸѡ)
	Ĭ�Ͽ���=of ��������
	B2���̼�>=b1���̼ۣ���¼ΪAA��˫��
	B2���̼�<b1 ���̼ۣ� ��¼ΪAB��˫�ס�
	����ɸѡ��������˫�������Ĺ�Ʊ
	*/
	//ɸѡ3   0 ��ʾ�����ͣ��ж���AA,AB,AAP �� 1, 2, 3  
	int iDoubleType = 0;
	if (tFirFilter.sA3Switch == eAA || tFirFilter.sA3Switch == eAB )
	{
		if (vecKline[nB2Pos].close >= vecKline[nB1Pos].close)
		{
			iDoubleType = eAA;
			if( iDoubleType != tFirFilter.sA3Switch)
			{
				m_pLog->logRecord("��ǰ����:[%d] �Ϳ������Ͳ�ͬ [%d]\n", iDoubleType, tFirFilter.sA3Switch);
				return false;
			}
			m_pLog->logRecord("ɸѡ3 AA��\n");
			return true;
		}
		else if (vecKline[nB2Pos].close < vecKline[nB1Pos].close)
		{
			iDoubleType = eAB;
			if( iDoubleType != tFirFilter.sA3Switch)
			{
				m_pLog->logRecord("��ǰ����:[%d] �Ϳ������Ͳ�ͬ [%d]\n", iDoubleType, tFirFilter.sA3Switch);
				return false;
			}
			m_pLog->logRecord("ɸѡ3 AB��\n");
			return true;
		}

	}
	
	return true;
}

bool CFilter2Alg::filterStepA4(const std::vector<tagKline>& vecKline, TFilter& tFirFilter, int nB1Pos, int nB2Pos, int & nL2Pos)
{
	/*
	����ɸѡ��A4������ע���û�ѡ���Ĭ�Ͽ���0F,����˫ƽ̨ѡ��
	a��ɸѡ���̣�3��ѡ��AA��˫��
	b��ɸѡ��B2���ݺ�������̼۸�L2���ݣ�������B2���ݣ�

	ɸѡ����L2���̼ۼ�>B1���̼������Ĺ�Ʊ����¼ΪAAP�͡�
	*/
	//ɸѡ4 : ������AA�ͣ���ֻ��AA�͵�����  
	
	//ɸѡ��B2���ݺ�������̼۸�L2���ݣ�������B2���ݣ�
	std::vector<tagKline> vecKlineTmp;
	vecKlineTmp.assign(vecKline.begin()+nB2Pos, vecKline.end());
	tagKline tL2Pos;
	bool bRet = GetLowOrHighClose(vecKlineTmp, nB2Pos, vecKline.size(), tL2Pos, nL2Pos, true);
	if (!bRet)
	{
		m_pLog->logRecord("ɸѡ4 û��L2\n");
		return false;
	}

	if (tL2Pos.close > vecKline[nB1Pos].close)
	{
		tFirFilter.sA3Switch = eAAP; //AAP
	}
	else
	{
		m_pLog->logRecord("ɸѡA4 ������L2[%f]>B1[%f]\n", tL2Pos.close, vecKline[nB1Pos].close);
		return false;
	}

	return true;
}

bool CFilter2Alg::filterStepA5(const std::vector<tagKline>& vecKline, TFilter& tFirFilter, int nAPos, int nB2Pos)
{
	//ɸѡ5

	double h1 = vecKline[nB2Pos].high - vecKline[nAPos].low;
	m_pLog->logRecord("ɸѡ5 ����h1:[%f]\n", h1);

	tagKline tEndData = vecKline[0];

	bool bRet = GetLow(vecKline.begin()+nB2Pos, vecKline.end(), tEndData);
	if (!bRet)
	{
		m_pLog->logRecord("ɸѡA5ʧ��,δ�ҵ���ͼ�\n");
		return false;
	}

	double h2 = vecKline[nB2Pos].high - tEndData.low;
	m_pLog->logRecord("ɸѡ5 ����h2:[%f]\n", h2);

	if ((h1 == 0))
	{
		m_pLog->logRecord("ɸѡA5ʧ��,h1 is 0\n");
		return false;
	}
	double ht = h2 / h1;
	if (ht > static_cast<double>(tFirFilter.sCallbackRange / 100))
	{
		m_pLog->logRecord("ɸѡA5ʧ��,ht[%f]����������\n", ht);
		return false;
	}

	return true;


}

bool CFilter2Alg::filterStepA6(const std::vector<tagKline>& vecKline, TFilter& tFirFilter, int nAPos)
{

	//ɸѡ6

	//ͳ��A���ݣ����������ݣ�������ĩ�ڣ���K����Ŀ
	int iSunlineNum = 0;
	int iDisFromAToEnd = 0;
	float rb = 0.00;

	iSunlineNum = GetSunLineNum(vecKline.begin()+nAPos, vecKline.size() - nAPos );
	
	if (iSunlineNum == 0)
	{
		m_pLog->logRecord("ɸѡ6δ�ҵ�����\n");
		return false;
	}
	iDisFromAToEnd = vecKline.size()- nAPos ;
	if (iDisFromAToEnd <= 0)
	{
		m_pLog->logRecord("ɸѡ6A���ݵ�����ĩβK�߸�������\n");
		return false;
	}
	m_pLog->logRecord("������:[%d] A���ݵ�����ĩβK�߸���[%d]\n ", iSunlineNum, iDisFromAToEnd);
	
	rb = (float)iSunlineNum / iDisFromAToEnd;
	

	//todo: ɸѡ���� ���ڵ�������
	if (rb < static_cast<double>(tFirFilter.sRbcoe / 100))
	{
		m_pLog->logRecord("rb[%f] < ����  rb\n", rb);
		return false;
	}
	
	return true;
}

//��ȡ���߸���
int CFilter2Alg::GetSunLineNum(std::vector<tagKline>::const_iterator itvDataBegin,
	 int end)
{
	int iNum = 0;
	std::vector<tagKline>::const_iterator itBegin = itvDataBegin;
	for (auto iter = itvDataBegin; iter != itvDataBegin + end; ++iter)
	{
		if (iter->close >= iter->open )
		{
			++iNum;
		}
	}
	
	return iNum;
}



CFilter2Alg::CFilter2Alg()
{

}

CFilter2Alg::~CFilter2Alg()
{

}

bool CFilter2Alg::filter2Level1(const  std::map<tagStockCodeInfo, std::vector<tagKline>> &inMap,
	std::map<tagStockCodeInfo, tagOutput> & output, TFilter& tFirFilter)
{


	TA2 tPer = tFirFilter.tLineNum;
	//����ϵ��KA Ĭ��Ϊ6 - 9
	//����ϵ��KB Ĭ��Ϊ6-8
	tPer.iMaxka = tPer.iMaxka == 0 ? PERIODKAMAX : tPer.iMaxka;
	tPer.iMinka = tPer.iMinka == 0 ? PERIODKAMIN : tPer.iMinka;
	tPer.iMaxkb = tPer.iMaxkb == 0 ? PERIODKBMAX : tPer.iMaxkb;
	tPer.iMinkb = tPer.iMinkb == 0 ? PERIODKBMIN : tPer.iMinkb;

	m_pLog->logRecord("һ��ɸѡ\n");
	m_pLog->logRecord("����ϵ��ka [%d]-[%d]| ����ϵ��kb [%d]-[%d] | ˫�׵�������[%d]\n", tPer.iMaxka, tPer.iMinka,
		tPer.iMaxkb, tPer.iMinkb,tFirFilter.sCallbackRange);

	std::map<tagStockCodeInfo, std::vector<tagKline>>::const_iterator iter;
	bool bRet = true;
	for (iter = inMap.begin(); iter != inMap.end(); ++iter)
	{
		
		m_pLog->logRecord("�г���[%d] ���룺[%s]\n", iter->first.market, iter->first.stockcode.c_str());
		tagKline tLowA, tHighB1, tHighB2;
		int nAPos = 0, nB1Pos = 0, nB2Pos = 0, nL2Pos = 0;  //A��͵�λ��
		const std::vector<tagKline>  & vecKline = iter->second;
		bRet = filterStepA1(vecKline, tLowA,nAPos, tHighB1, nB1Pos, tHighB2 ,nB2Pos);
		if (!bRet)
		{
			m_pLog->logRecord("����A1ʧ��\n");
			continue;
		}
		
	
		bRet = filterStepA2(vecKline, tPer, nAPos, nB2Pos);
		if (!bRet)
		{
			m_pLog->logRecord("����A2ʧ��\n");
			continue;
		}

		
		if (tFirFilter.sA3Switch == eOff)
		{
			m_pLog->logRecord("A3����δ��\n");
		}
		else
		{
			bRet = filterStepA3(vecKline, tFirFilter, nB2Pos, nB1Pos);
			if (!bRet)
			{
				m_pLog->logRecord("����A3ʧ��\n");
				continue;
			}
		}
	
		if (tFirFilter.bA4Switch == false)
		{
			m_pLog->logRecord("A4����δ��\n");
		}
		else
		{
			bRet = filterStepA4(vecKline, tFirFilter, nB1Pos, nB2Pos, nL2Pos);
			if (!bRet)
			{
				continue;
			}

		}
		

		if (tFirFilter.bA5Switch == false)
		{
			m_pLog->logRecord("A5����δ��\n");
		}
		else
		{
			bRet = filterStepA5(vecKline, tFirFilter, nAPos, nB2Pos);
			if (!bRet)
			{
				continue;
			}
		}
		
		
		if (tFirFilter.bA6Switch == false)
		{
			m_pLog->logRecord("A6����δ��\n");
		}
		else
		{
			bRet = filterStepA6(vecKline, tFirFilter, nAPos);
			if (!bRet)
			{
				continue;
			}
		}
		
		//����һ��ɸѡ���
		tagOutput tOneItem;
		output[iter->first] = tOneItem;
	}
	return true;

}
	

bool CFilter2Alg::filter2Level2(const std::map<tagStockCodeInfo, std::vector<tagKline>> &inMap,
	std::map<tagStockCodeInfo, tagOutput> & output, TFilter& tSedFilter)
{


	m_pLog->logRecord("����ɸѡ\n");
	TA2 tPer = tSedFilter.tLineNum;
	//����ϵ��KA Ĭ��Ϊ6 - 9
	//����ϵ��KB Ĭ��Ϊ6-8
	tPer.iMaxka = tPer.iMaxka == 0 ? PERIODKAMAXB1 : tPer.iMaxka;
	tPer.iMinka = tPer.iMinka == 0 ? PERIODKAMINB1 : tPer.iMinka;
	tPer.iMaxkb = tPer.iMaxkb == 0 ? PERIODKBMAXB1 : tPer.iMaxkb;
	tPer.iMinkb = tPer.iMinkb == 0 ? PERIODKBMINB1 : tPer.iMinkb;
	m_pLog->logRecord("���ka[%d] | ��Сka[%d] | ���kb[%d] | ��Сkb[%d]�� \n", tPer.iMaxka, tPer.iMinka, tPer.iMaxkb, tPer.iMinkb); 


	//ѭ������ÿֻ��Ʊ
	std::map<tagStockCodeInfo, std::vector<tagKline>>::const_iterator iter;
	bool bRet = true;
	for (iter = inMap.begin(); iter != inMap.end(); ++iter)
	{
		m_pLog->logRecord("�г���[%d] ���룺[%s]\n", iter->first.market, iter->first.stockcode);
		tagKline tLowA, tHighB1, tHighB2;
		int nAPos = 0, nB1Pos = 0, nB2Pos = 0,  nL2Pos = 0;  //A��͵�λ��
		const std::vector<tagKline>  & vecKline = iter->second;
		bRet = filterStepA1(vecKline, tLowA, nAPos, tHighB1, nB1Pos, tHighB2, nB2Pos);
		if (!bRet)
		{
			m_pLog->logRecord("�������A1ʧ��\n");
			continue;
		}

		bRet = filterStepA2(vecKline, tPer, nAPos, nB2Pos);
		if (!bRet)
		{
			m_pLog->logRecord("�������A2ʧ��\n");
			continue;
		}
		if (tSedFilter.bA5Switch == false)
		{
			m_pLog->logRecord("A5����δ��\n");
		}
		else
		{
			bRet = filterStepA5(vecKline, tSedFilter, nAPos, nB2Pos);
			if (!bRet)
			{
				continue;
			}
		}
		output[iter->first] = tagOutput();

	}

	return true;
}


bool CFilter2Alg::filter2Level3(const std::map<tagStockCodeInfo, std::vector<tagKline>> &inMap, 
	std::map<tagStockCodeInfo, tagOutput> & output, TFilter& tThdFilter)
{
	m_pLog->logRecord("����ɸѡ\n");
	TA2 tPer = tThdFilter.tLineNum;
	//����ϵ��KA Ĭ��Ϊ6 - 9
	//����ϵ��KB Ĭ��Ϊ6-8
	tPer.iMaxka = tPer.iMaxka == 0 ? PERIODKAMAXB1 : tPer.iMaxka;
	tPer.iMinka = tPer.iMinka == 0 ? PERIODKAMINB1 : tPer.iMinka;
	tPer.iMaxkb = tPer.iMaxkb == 0 ? PERIODKBMAXB1 : tPer.iMaxkb;
	tPer.iMinkb = tPer.iMinkb == 0 ? PERIODKBMINB1 : tPer.iMinkb;
	m_pLog->logRecord("���ka[%d] | ��Сka[%d] | ���kb[%d] | ��Сkb[%d]�� \n", tPer.iMaxka, tPer.iMinka, tPer.iMaxkb, tPer.iMinkb);
	/*logstream2 << "����ɸѡ����" << endl;
	logstream2 << "���ka | ��Сka | ���kb | ��Сkb�� " << tThdFilter.tLineNum.iMaxka << " " << tThdFilter.tLineNum.iMinka << "  "
		<< tThdFilter.tLineNum.iMaxkb << " " << tThdFilter.tLineNum.iMinkb << endl;
	logstream2 << "����5 | ��������: " << tThdFilter.bA5Switch << " " << static_cast<double>(tThdFilter.sCallbackRange / 100) << endl;
*/
	//ѭ������ÿֻ��Ʊ
	std::map<tagStockCodeInfo, std::vector<tagKline>>::const_iterator iter;
	bool bRet = true;
	for (iter = inMap.begin(); iter != inMap.end(); ++iter)
	{
		m_pLog->logRecord("�г���[%d] ���룺[%s]\n", iter->first.market, iter->first.stockcode);
		tagKline tLowA, tHighB1, tHighB2;
		int nAPos = 0, nB1Pos = 0, nB2Pos = 0, nL2Pos = 0;  //A��͵�λ��
		const std::vector<tagKline>  & vecKline = iter->second;
		bRet = filterStepA1(vecKline, tLowA, nAPos, tHighB1, nB1Pos, tHighB2, nB2Pos);
		if (!bRet)
		{
			m_pLog->logRecord("�������A1ʧ��\n");
			continue;
		}

		bRet = filterStepA2(vecKline, tPer, nAPos, nB2Pos);
		if (!bRet)
		{
			m_pLog->logRecord("�������A2ʧ��\n");
			continue;
		}
		if (tThdFilter.bA5Switch == false)
		{
			m_pLog->logRecord("A5����δ��\n");
		}
		else
		{
			bRet = filterStepA5(vecKline, tThdFilter, nAPos, nB2Pos);
			if (!bRet)
			{
				continue;
			}
		}
		output[iter->first] = tagOutput();

	}

	return true;
}


