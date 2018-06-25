// stock.cpp 。
//
#include "stdafx.h"
#include "stock2.h"


extern void  stamp_to_standard(time_t stampTime, char* s, char* format = NULL );


//计算最高或者最低收盘价
bool CFilter2Alg::GetLowOrHighClose(const std::vector<tagKline>& itvDataBegin, int start, int end,
	tagKline& tagResult, int& nPos,bool bLow /* = false */)
{
	// 如果价格相同，A就按照最低价排序， B1， B2 最高价排序
	if (itvDataBegin.size() < 2)
	{
		m_pLog->logRecord("计算收盘价数据不足\n");
		return false;
	}

	tagResult = itvDataBegin[0];
	if (bLow)
	{
		//获取最低收盘价
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
		//获取最高收盘价  如果收盘价相同则最高价低的为最高收盘价 
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




//返回 最低价
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

	//k线小于等于2，不处理
	if (vecKline.size() <= 2)
	{
		m_pLog->logRecord("k线小于2\n");
		return false;
	}
	tagKline tAPos;
	bool bRet = true;
	char buf[32];
	bRet = GetLowOrHighClose(vecKline, 0, vecKline.size(), tAPos, nAPos, true);
	if (!bRet || nAPos == vecKline.size() -1 || nAPos == 0 )
	{
		m_pLog->logRecord("进阶筛选A1为首尾\n");
		return false;
	}

	stamp_to_standard(tAPos.time, buf);
	m_pLog->logRecord("进阶筛选A1收盘最低数据：%s open %f high %f low %f close %f \n", buf, tAPos.open, tAPos.high, tAPos.low, tAPos.close);


	//搜索A数据日期前收盘价最高B1数据
	tagKline tB1Pos;
	bRet = GetLowOrHighClose(vecKline, 0, nAPos - 1, tB1Pos,nB1Pos, false);
	if (!bRet)
	{
		m_pLog->logRecord("进阶筛选未找到最高收盘价B1\n");
		return false;
	}
	stamp_to_standard(tB1Pos.time, buf);
	m_pLog->logRecord("进阶筛选B1收盘最高数据：%s open %f high %f low %f close %f \n", buf, tB1Pos.open, tB1Pos.high, tB1Pos.low, tB1Pos.close);

	

	//搜索A数据日期后收盘价最高B2数据
	tagKline tB2Pos;
	bRet = GetLowOrHighClose(vecKline, nAPos + 1, vecKline.size(), tB2Pos, nB2Pos, false);
	if (!bRet)
	{
		m_pLog->logRecord("进阶筛选未找到最高收盘价B2\n");
		return false;
	}
	stamp_to_standard(tB2Pos.time, buf);
	m_pLog->logRecord("进阶筛选B2收盘最高数据：%s open %f high %f low %f close %f \n", buf, tB2Pos.open, tB2Pos.high, tB2Pos.low, tB2Pos.close);
	return true;
}


/*
周期系数KA 默认为6-9
周期系数KB 默认为6-8

条件A、筛选数据A（包含该数据）到数据B2，K线根数符合周期系数KA(6-9默认）范围。
条件B、筛选出数据B2（包含该数据）到数据末期，k线根数符合周期系数KB(6-8默认）范围内
*/
bool CFilter2Alg::filterStepA2(const std::vector<tagKline>& vecKline, TA2 period, int nAPos, int nB2Pos)
{


	//条件A、筛选数据A（包含该数据）到数据B2，K线根数符合周期系数KA(6-9默认）范围。
	int nAPeriod = nB2Pos - nAPos + 1;
	if ( nAPeriod < period.iMinka || nAPeriod > period.iMaxka)
	{
		return false;
	}
	//条件B、筛选出数据B2（包含该数据）到数据末期，k线根数符合周期系数KB(6-8默认）范围内
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
	进阶筛选（A3）(备注：用户选择项，默认开关OF，用于双底类型筛选)
	默认开关=of 跳过此项
	B2收盘价>=b1收盘价，记录为AA型双底
	B2收盘价<b1 收盘价， 记录为AB型双底。
	进阶筛选满足输入双底条件的股票
	*/
	//筛选3   0 表示无类型，判断是AA,AB,AAP 型 1, 2, 3  
	int iDoubleType = 0;
	if (tFirFilter.sA3Switch == eAA || tFirFilter.sA3Switch == eAB )
	{
		if (vecKline[nB2Pos].close >= vecKline[nB1Pos].close)
		{
			iDoubleType = eAA;
			if( iDoubleType != tFirFilter.sA3Switch)
			{
				m_pLog->logRecord("当前类型:[%d] 和开关类型不同 [%d]\n", iDoubleType, tFirFilter.sA3Switch);
				return false;
			}
			m_pLog->logRecord("筛选3 AA型\n");
			return true;
		}
		else if (vecKline[nB2Pos].close < vecKline[nB1Pos].close)
		{
			iDoubleType = eAB;
			if( iDoubleType != tFirFilter.sA3Switch)
			{
				m_pLog->logRecord("当前类型:[%d] 和开关类型不同 [%d]\n", iDoubleType, tFirFilter.sA3Switch);
				return false;
			}
			m_pLog->logRecord("筛选3 AB型\n");
			return true;
		}

	}
	
	return true;
}

bool CFilter2Alg::filterStepA4(const std::vector<tagKline>& vecKline, TFilter& tFirFilter, int nB1Pos, int nB2Pos, int & nL2Pos)
{
	/*
	进阶筛选（A4）（备注，用户选择项，默认开关0F,用于双平台选择）
	a、筛选过程（3）选择AA型双底
	b、筛选出B2数据后最低收盘价格L2数据（不包括B2数据）

	筛选满足L2收盘价价>B1收盘价条件的股票，记录为AAP型。
	*/
	//筛选4 : 假如是AA型，就只用AA型的数据  
	
	//筛选出B2数据后最低收盘价格L2数据（不包括B2数据）
	std::vector<tagKline> vecKlineTmp;
	vecKlineTmp.assign(vecKline.begin()+nB2Pos, vecKline.end());
	tagKline tL2Pos;
	bool bRet = GetLowOrHighClose(vecKlineTmp, nB2Pos, vecKline.size(), tL2Pos, nL2Pos, true);
	if (!bRet)
	{
		m_pLog->logRecord("筛选4 没有L2\n");
		return false;
	}

	if (tL2Pos.close > vecKline[nB1Pos].close)
	{
		tFirFilter.sA3Switch = eAAP; //AAP
	}
	else
	{
		m_pLog->logRecord("筛选A4 不满足L2[%f]>B1[%f]\n", tL2Pos.close, vecKline[nB1Pos].close);
		return false;
	}

	return true;
}

bool CFilter2Alg::filterStepA5(const std::vector<tagKline>& vecKline, TFilter& tFirFilter, int nAPos, int nB2Pos)
{
	//筛选5

	double h1 = vecKline[nB2Pos].high - vecKline[nAPos].low;
	m_pLog->logRecord("筛选5 幅度h1:[%f]\n", h1);

	tagKline tEndData = vecKline[0];

	bool bRet = GetLow(vecKline.begin()+nB2Pos, vecKline.end(), tEndData);
	if (!bRet)
	{
		m_pLog->logRecord("筛选A5失败,未找到最低价\n");
		return false;
	}

	double h2 = vecKline[nB2Pos].high - tEndData.low;
	m_pLog->logRecord("筛选5 幅度h2:[%f]\n", h2);

	if ((h1 == 0))
	{
		m_pLog->logRecord("筛选A5失败,h1 is 0\n");
		return false;
	}
	double ht = h2 / h1;
	if (ht > static_cast<double>(tFirFilter.sCallbackRange / 100))
	{
		m_pLog->logRecord("筛选A5失败,ht[%f]不满足条件\n", ht);
		return false;
	}

	return true;


}

bool CFilter2Alg::filterStepA6(const std::vector<tagKline>& vecKline, TFilter& tFirFilter, int nAPos)
{

	//筛选6

	//统计A数据（包括该数据）到数据末期，阳K线数目
	int iSunlineNum = 0;
	int iDisFromAToEnd = 0;
	float rb = 0.00;

	iSunlineNum = GetSunLineNum(vecKline.begin()+nAPos, vecKline.size() - nAPos );
	
	if (iSunlineNum == 0)
	{
		m_pLog->logRecord("筛选6未找到阳线\n");
		return false;
	}
	iDisFromAToEnd = vecKline.size()- nAPos ;
	if (iDisFromAToEnd <= 0)
	{
		m_pLog->logRecord("筛选6A数据到数据末尾K线根数不足\n");
		return false;
	}
	m_pLog->logRecord("阳线数:[%d] A数据到数据末尾K线根数[%d]\n ", iSunlineNum, iDisFromAToEnd);
	
	rb = (float)iSunlineNum / iDisFromAToEnd;
	

	//todo: 筛选条件 大于等于命中
	if (rb < static_cast<double>(tFirFilter.sRbcoe / 100))
	{
		m_pLog->logRecord("rb[%f] < 输入  rb\n", rb);
		return false;
	}
	
	return true;
}

//获取阳线个数
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
	//周期系数KA 默认为6 - 9
	//周期系数KB 默认为6-8
	tPer.iMaxka = tPer.iMaxka == 0 ? PERIODKAMAX : tPer.iMaxka;
	tPer.iMinka = tPer.iMinka == 0 ? PERIODKAMIN : tPer.iMinka;
	tPer.iMaxkb = tPer.iMaxkb == 0 ? PERIODKBMAX : tPer.iMaxkb;
	tPer.iMinkb = tPer.iMinkb == 0 ? PERIODKBMIN : tPer.iMinkb;

	m_pLog->logRecord("一级筛选\n");
	m_pLog->logRecord("周期系数ka [%d]-[%d]| 周期系数kb [%d]-[%d] | 双底调整幅度[%d]\n", tPer.iMaxka, tPer.iMinka,
		tPer.iMaxkb, tPer.iMinkb,tFirFilter.sCallbackRange);

	std::map<tagStockCodeInfo, std::vector<tagKline>>::const_iterator iter;
	bool bRet = true;
	for (iter = inMap.begin(); iter != inMap.end(); ++iter)
	{
		
		m_pLog->logRecord("市场：[%d] 代码：[%s]\n", iter->first.market, iter->first.stockcode.c_str());
		tagKline tLowA, tHighB1, tHighB2;
		int nAPos = 0, nB1Pos = 0, nB2Pos = 0, nL2Pos = 0;  //A最低点位置
		const std::vector<tagKline>  & vecKline = iter->second;
		bRet = filterStepA1(vecKline, tLowA,nAPos, tHighB1, nB1Pos, tHighB2 ,nB2Pos);
		if (!bRet)
		{
			m_pLog->logRecord("进阶A1失败\n");
			continue;
		}
		
	
		bRet = filterStepA2(vecKline, tPer, nAPos, nB2Pos);
		if (!bRet)
		{
			m_pLog->logRecord("进阶A2失败\n");
			continue;
		}

		
		if (tFirFilter.sA3Switch == eOff)
		{
			m_pLog->logRecord("A3开关未开\n");
		}
		else
		{
			bRet = filterStepA3(vecKline, tFirFilter, nB2Pos, nB1Pos);
			if (!bRet)
			{
				m_pLog->logRecord("进阶A3失败\n");
				continue;
			}
		}
	
		if (tFirFilter.bA4Switch == false)
		{
			m_pLog->logRecord("A4开关未开\n");
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
			m_pLog->logRecord("A5开关未开\n");
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
			m_pLog->logRecord("A6开关未开\n");
		}
		else
		{
			bRet = filterStepA6(vecKline, tFirFilter, nAPos);
			if (!bRet)
			{
				continue;
			}
		}
		
		//返回一级筛选结果
		tagOutput tOneItem;
		output[iter->first] = tOneItem;
	}
	return true;

}
	

bool CFilter2Alg::filter2Level2(const std::map<tagStockCodeInfo, std::vector<tagKline>> &inMap,
	std::map<tagStockCodeInfo, tagOutput> & output, TFilter& tSedFilter)
{


	m_pLog->logRecord("二级筛选\n");
	TA2 tPer = tSedFilter.tLineNum;
	//周期系数KA 默认为6 - 9
	//周期系数KB 默认为6-8
	tPer.iMaxka = tPer.iMaxka == 0 ? PERIODKAMAXB1 : tPer.iMaxka;
	tPer.iMinka = tPer.iMinka == 0 ? PERIODKAMINB1 : tPer.iMinka;
	tPer.iMaxkb = tPer.iMaxkb == 0 ? PERIODKBMAXB1 : tPer.iMaxkb;
	tPer.iMinkb = tPer.iMinkb == 0 ? PERIODKBMINB1 : tPer.iMinkb;
	m_pLog->logRecord("最大ka[%d] | 最小ka[%d] | 最大kb[%d] | 最小kb[%d]： \n", tPer.iMaxka, tPer.iMinka, tPer.iMaxkb, tPer.iMinkb); 


	//循环处理每只股票
	std::map<tagStockCodeInfo, std::vector<tagKline>>::const_iterator iter;
	bool bRet = true;
	for (iter = inMap.begin(); iter != inMap.end(); ++iter)
	{
		m_pLog->logRecord("市场：[%d] 代码：[%s]\n", iter->first.market, iter->first.stockcode);
		tagKline tLowA, tHighB1, tHighB2;
		int nAPos = 0, nB1Pos = 0, nB2Pos = 0,  nL2Pos = 0;  //A最低点位置
		const std::vector<tagKline>  & vecKline = iter->second;
		bRet = filterStepA1(vecKline, tLowA, nAPos, tHighB1, nB1Pos, tHighB2, nB2Pos);
		if (!bRet)
		{
			m_pLog->logRecord("二层进阶A1失败\n");
			continue;
		}

		bRet = filterStepA2(vecKline, tPer, nAPos, nB2Pos);
		if (!bRet)
		{
			m_pLog->logRecord("二层进阶A2失败\n");
			continue;
		}
		if (tSedFilter.bA5Switch == false)
		{
			m_pLog->logRecord("A5开关未开\n");
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
	m_pLog->logRecord("三级筛选\n");
	TA2 tPer = tThdFilter.tLineNum;
	//周期系数KA 默认为6 - 9
	//周期系数KB 默认为6-8
	tPer.iMaxka = tPer.iMaxka == 0 ? PERIODKAMAXB1 : tPer.iMaxka;
	tPer.iMinka = tPer.iMinka == 0 ? PERIODKAMINB1 : tPer.iMinka;
	tPer.iMaxkb = tPer.iMaxkb == 0 ? PERIODKBMAXB1 : tPer.iMaxkb;
	tPer.iMinkb = tPer.iMinkb == 0 ? PERIODKBMINB1 : tPer.iMinkb;
	m_pLog->logRecord("最大ka[%d] | 最小ka[%d] | 最大kb[%d] | 最小kb[%d]： \n", tPer.iMaxka, tPer.iMinka, tPer.iMaxkb, tPer.iMinkb);
	/*logstream2 << "三级筛选开关" << endl;
	logstream2 << "最大ka | 最小ka | 最大kb | 最小kb： " << tThdFilter.tLineNum.iMaxka << " " << tThdFilter.tLineNum.iMinka << "  "
		<< tThdFilter.tLineNum.iMaxkb << " " << tThdFilter.tLineNum.iMinkb << endl;
	logstream2 << "开关5 | 调整幅度: " << tThdFilter.bA5Switch << " " << static_cast<double>(tThdFilter.sCallbackRange / 100) << endl;
*/
	//循环处理每只股票
	std::map<tagStockCodeInfo, std::vector<tagKline>>::const_iterator iter;
	bool bRet = true;
	for (iter = inMap.begin(); iter != inMap.end(); ++iter)
	{
		m_pLog->logRecord("市场：[%d] 代码：[%s]\n", iter->first.market, iter->first.stockcode);
		tagKline tLowA, tHighB1, tHighB2;
		int nAPos = 0, nB1Pos = 0, nB2Pos = 0, nL2Pos = 0;  //A最低点位置
		const std::vector<tagKline>  & vecKline = iter->second;
		bRet = filterStepA1(vecKline, tLowA, nAPos, tHighB1, nB1Pos, tHighB2, nB2Pos);
		if (!bRet)
		{
			m_pLog->logRecord("二层进阶A1失败\n");
			continue;
		}

		bRet = filterStepA2(vecKline, tPer, nAPos, nB2Pos);
		if (!bRet)
		{
			m_pLog->logRecord("二层进阶A2失败\n");
			continue;
		}
		if (tThdFilter.bA5Switch == false)
		{
			m_pLog->logRecord("A5开关未开\n");
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


