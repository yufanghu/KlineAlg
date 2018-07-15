// stock.cpp 。
//
#include "stdafx.h"
#include "CaculateStock2.h"
#include "StockAlgorithm.h"
#include "Global.h"
#include "Log.h"

//计算最高或者最低收盘价
bool CCaculateSotck2::GetLowOrHighClose(const std::vector<tagKline>& itvDataBegin, int start, int end,
	tagKline& tagResult, int& nPos,bool bLow /* = false */)
{
	// 如果价格相同，A就按照最低价排序， B1， B2 最高价排序
	if (itvDataBegin.size() < 2)
	{
		m_pLog->logRecord("计算收盘价数据不足\n");
		return false;
	}

	tagResult = itvDataBegin[start];
	nPos = start;
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
bool CCaculateSotck2::GetLow(std::vector<tagKline>::const_iterator itvDataBegin,
	 std::vector<tagKline>::const_iterator itvDataEnd, tagKline& tResult)
{
    
    if (itvDataBegin == itvDataEnd)
    {
		m_pLog->logRecord("GetLow start == end\n");
        return false;
    }


	tResult = *(itvDataBegin);
	auto temp = itvDataBegin;
	auto it = itvDataBegin;
	for ( ; it != itvDataEnd; ++it)
	{

		if (tResult.low > it->low)
		{
			tResult = *it;
			temp = it;
		}
		
	}
	//if (temp == itvDataEnd -1 )
	//{
	//	m_pLog->logRecord("B2就是末尾数据\n");
	//	return false;
	//}

    return true;
}


 void CCaculateSotck2::PrintInputParam(Alg2Filter * filter, int type)
 {
	auto func = [](bool bRet){
		return bRet ? "打开" : "关闭" ;
	};
		
	const static char* temp[] = {
		"OFF",
		"AA",
		"AB"
	};

	char buf[1024] = {0};
	if(type == 0){
		TFirstFilter * pFilter = dynamic_cast<TFirstFilter*>(filter);
		if(pFilter){
				sprintf_s(buf, sizeof(buf), "一级筛选参数：KA[%d~%d],KB[%d~%d],A3双底类型[%s],A4步骤[%s],A5步骤[%s],A6步骤[%s],双底调整幅度[%d],阳K比例[%d]\n", 
					pFilter->tLineNum.iMinka, pFilter->tLineNum.iMaxka, pFilter->tLineNum.iMinkb, pFilter->tLineNum.iMaxkb, 
				temp[pFilter->sA3Switch], 
				func(pFilter->bA4Switch),func(pFilter->bA5Switch),func(pFilter->bA6Switch),
				pFilter->sCallbackRange,pFilter->sRbcoe);
		}
	}
	else if(type == 1){
		TSecondFilter * pFilter = dynamic_cast<TSecondFilter*>(filter);
		if(pFilter){
				sprintf_s(buf, sizeof(buf), "二级筛选参数：KA[%d~%d],KB[%d~%d],A5步骤[%s],双底调整幅度[%d]\n",
					pFilter->tLineNum.iMinka, pFilter->tLineNum.iMaxka, pFilter->tLineNum.iMinkb, pFilter->tLineNum.iMaxkb, 
				func(pFilter->bA5Switch), pFilter->sCallbackRange);
		}
	}
	else if(type == 2){
		TThirdFilter * pFilter = dynamic_cast<TThirdFilter*>(filter);
		if(pFilter){
				sprintf_s(buf, sizeof(buf), "三级筛选参数：KA[%d~%d],KB[%d~%d],A5步骤[%s],双底调整幅度[%d]\n", 
					pFilter->tLineNum.iMinka, pFilter->tLineNum.iMaxka, pFilter->tLineNum.iMinkb, pFilter->tLineNum.iMaxkb, 
				func(pFilter->bA5Switch), pFilter->sCallbackRange);
		}
	}
	m_pLog->logRecord(buf);
}


void CCaculateSotck2::PrintData(const std::vector<tagKline>& kLineData)
{
	auto it = kLineData.begin();
	for (; it != kLineData.end(); ++it) {
		m_pLog->dataRecord("%lld,%f,%f,%f,%f,\n", it->time, it->open, it->high, it->low, it->close);
	}
	//printf("读取%d行数据,开 高 低 收\n\n", kLineData.size());
}


void CCaculateSotck2::PrintHead(const tagStockCodeInfo & codeInfo, int nPos)
{
	m_pLog->dataRecord("%d-%s\n", codeInfo.market, codeInfo.stockcode.c_str());
	m_pLog->logRecord("第%d支股票[%d-%s]\n", nPos, codeInfo.market, codeInfo.stockcode.c_str());
}

bool CCaculateSotck2::filterStepA1(const std::vector<tagKline>& vecKline, tagKline& tLowA, int& nAPos,
		tagKline& tHighB1,int& nB1Pos,
		tagKline& tHighB2, int& nB2Pos)
{

	//k线小于等于2，不处理
	if (vecKline.size() <= 2)
	{
		m_pLog->logRecord("筛选1 k线小于2\n");
		return false;
	}
	tagKline tAPos;
	bool bRet = true;
	char buf[32];
	bRet = GetLowOrHighClose(vecKline, 0, vecKline.size(), tAPos, nAPos, true);
	if (nAPos == 0)
	{
		m_pLog->logRecord("筛选1 A为第一根K线,没有B1，失败\n");
		return false;
	}
	else if (nAPos == vecKline.size() - 1)
	{
		m_pLog->logRecord("筛选1 A为最后一根K线，没有B2，失败\n");
		return false;
	}
	
	if (!bRet)
	{
		m_pLog->logRecord("筛选1 未找到A\n");
		return false;
	}


	stamp_to_standard(tAPos.time, buf);
	m_pLog->logRecord("筛选1 A收盘最低数据：pos:%d %s open %f high %f low %f close %f \n",nAPos, buf, tAPos.open, tAPos.high, tAPos.low, tAPos.close);


	//搜索A数据日期前收盘价最高B1数据
	tagKline tB1Pos;
	bRet = GetLowOrHighClose(vecKline, 0, nAPos - 1, tB1Pos,nB1Pos, false);
	if (!bRet)
	{
		m_pLog->logRecord("筛选1 筛选未找到最高收盘价B1\n");
		return false;
	}
	stamp_to_standard(tB1Pos.time, buf);
	m_pLog->logRecord("筛选1 B1收盘最高数据：pos:%d %s open %f high %f low %f close %f \n",nB1Pos, buf, tB1Pos.open, tB1Pos.high, tB1Pos.low, tB1Pos.close);

	

	//搜索A数据日期后收盘价最高B2数据
	tagKline tB2Pos;
	bRet = GetLowOrHighClose(vecKline, nAPos + 1, vecKline.size(), tB2Pos, nB2Pos, false);
	if (!bRet)
	{
		m_pLog->logRecord("筛选1 筛选未找到最高收盘价B2\n");
		return false;
	}
	stamp_to_standard(tB2Pos.time, buf);
	m_pLog->logRecord("筛选1 B2收盘最高数据：pos:%d %s open %f high %f low %f close %f \n", nB2Pos,buf, tB2Pos.open, tB2Pos.high, tB2Pos.low, tB2Pos.close);
	m_pLog->logRecord("筛选1 成功");
	return true;
}


/*
周期系数KA 默认为6-9
周期系数KB 默认为6-8

条件A、筛选数据A（包含该数据）到数据B2，K线根数符合周期系数KA(6-9默认）范围。
条件B、筛选出数据B2（包含该数据）到数据末期，k线根数符合周期系数KB(6-8默认）范围内
*/
bool CCaculateSotck2::filterStepA2(const std::vector<tagKline>& vecKline, TA2 period, int nAPos, int nB2Pos)
{


	//条件A、筛选数据A（包含该数据）到数据B2，K线根数符合周期系数KA(6-9默认）范围。
	int nAPeriod = nB2Pos - nAPos + 1;
	if ( nAPeriod < period.iMinka || nAPeriod > period.iMaxka)
	{
		m_pLog->logRecord("筛选2 A-B2 K线数[%d] 不在范围[%d]-[%d]\n", nAPeriod, period.iMinka, period.iMaxka);
		return false;
	}
	m_pLog->logRecord("筛选2 A-B2 K线数[%d] 满足[%d]-[%d]\n", nAPeriod, period.iMinka, period.iMaxka);
	//条件B、筛选出数据B2（包含该数据）到数据末期，k线根数符合周期系数KB(6-8默认）范围内
	int nBPeriod = vecKline.size() - nB2Pos + 1;
	if (nBPeriod < period.iMinkb || nBPeriod > period.iMaxkb)
	{
		m_pLog->logRecord("筛选2 末尾-B2 K线数[%d] 不在范围[%d]-[%d]\n", nBPeriod, period.iMinkb, period.iMaxkb);
		return false;
	}
	m_pLog->logRecord("筛选2 末尾-B2 K线数[%d] 满足[%d]-[%d]\n", nBPeriod, period.iMinkb, period.iMaxkb);
	m_pLog->logRecord("筛选2 成功\n");
	return true;
}

bool CCaculateSotck2::filterStepA3(const std::vector<tagKline>& vecKline, TFirstFilter& tFirFilter, int nB2Pos, int nB1Pos)
{
	/*
	筛选（A3）(备注：用户选择项，默认开关OF，用于双底类型筛选)
	默认开关=of 跳过此项
	B2收盘价>=b1收盘价，记录为AA型双底
	B2收盘价<b1 收盘价， 记录为AB型双底。
	筛选满足输入双底条件的股票
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
				m_pLog->logRecord("筛选3 当前类型:[%d] 和开关类型不同 [%d]\n", iDoubleType, tFirFilter.sA3Switch);
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
				const static std::string temp[4] = {
					"关闭",
					"AA",
					"AB",
					"AAP"
				};
				
				m_pLog->logRecord("筛选3 当前类型:[%s] 和开关类型不同 [%s]\n", temp[iDoubleType].c_str(), temp[tFirFilter.sA3Switch].c_str());
				return false;
			}
			m_pLog->logRecord("筛选3 AB型\n");
			return true;
		}

	}
	
	return true;
}

bool CCaculateSotck2::filterStepA4(const std::vector<tagKline>& vecKline, TFirstFilter& tFirFilter, int nB1Pos, int nB2Pos, int & nL2Pos)
{
	/*
	筛选（A4）（备注，用户选择项，默认开关0F,用于双平台选择）
	a、筛选过程（3）选择AA型双底
	b、筛选出B2数据后最低收盘价格L2数据（不包括B2数据）

	筛选满足L2收盘价价>B1收盘价条件的股票，记录为AAP型。
	*/
	//筛选4 : 假如是AA型，就只用AA型的数据  
	
	//筛选出B2数据后最低收盘价格L2数据（不包括B2数据）
	if (tFirFilter.sA3Switch != eAA)
	{
		m_pLog->logRecord("筛选4 不是AA型\n");
		return true;
	}
	tagKline tL2Pos;
	bool bRet = GetLowOrHighClose(vecKline, nB2Pos, vecKline.size(), tL2Pos, nL2Pos, true);
	if ( nL2Pos == nB2Pos)
	{
		m_pLog->logRecord("筛选4 失败 L2为B2\n");
		return false;
	}
	if (!bRet)
	{
		m_pLog->logRecord("筛选4 没有L2\n");
		return false;
	}

	if (tL2Pos.close > vecKline[nB1Pos].close)
	{
		//tFirFilter.sA3Switch = eAAP; //AAP
		m_pLog->logRecord("筛选4 AAP型\n");
	}
	else
	{
		m_pLog->logRecord("筛选4 不满足L2[%f]>B1[%f]\n", tL2Pos.close, vecKline[nB1Pos].close);
		return false;
	}


	m_pLog->logRecord("筛选4 最低收盘价格L2 open:%f  high:%f  low:%f clos:%f\n",tL2Pos.open, tL2Pos.high,tL2Pos.low,tL2Pos.close);
	return true;
}

bool CCaculateSotck2::filterStepA5(const std::vector<tagKline>& vecKline, TFirstFilter& tFirFilter, int nAPos, int nB2Pos)
{
	//筛选5

	double h1 = vecKline[nB2Pos].high - vecKline[nAPos].low;
	m_pLog->logRecord("筛选5 幅度h1:[%f]\n", h1);

	tagKline tEndData = vecKline[0];

	bool bRet = GetLow(vecKline.begin()+nB2Pos + 1, vecKline.end(), tEndData);
	if (!bRet)
	{
		m_pLog->logRecord("筛选5 失败,未找到最低价\n");
		return false;
	}

	double h2 = vecKline[nB2Pos].high - tEndData.low;
	m_pLog->logRecord("筛选5 幅度h2:[%f]\n", h2);

	if ((h1 == 0))
	{
		m_pLog->logRecord("筛选5 失败,h1 is 0\n");
		return false;

	}
	double ht = h2 / h1;
	if (ht > (float)tFirFilter.sCallbackRange / 100)
	{
		m_pLog->logRecord("筛选5 失败,ht[%f]不满足 > sCallBackRange [%f]\n", ht, (float)tFirFilter.sCallbackRange / 100);
		return false;
	}


	m_pLog->logRecord("筛选5 成功 满足ht[%f] <=callbackRange[%f]\n", ht, (float)tFirFilter.sCallbackRange / 100);
	return true;


}

bool CCaculateSotck2::filterStepA6(const std::vector<tagKline>& vecKline, TFirstFilter& tFirFilter, int nAPos)
{

	//筛选6

	//统计A数据（包括该数据）到数据末期，阳K线数目
	int iSunlineNum = 0;
	int iDisFromAToEnd = 0;
	float rb = 0.00;

	iSunlineNum = GetSunLineNum(vecKline.begin()+nAPos, vecKline.size() - nAPos );
	
	if (iSunlineNum == 0)
	{
		m_pLog->logRecord("筛选6 未找到阳线\n");
		return false;
	}
	iDisFromAToEnd = vecKline.size()- nAPos ;
	if (iDisFromAToEnd <= 0)
	{
		m_pLog->logRecord("筛选6 A数据到数据末尾K线根数不足\n");
		return false;
	}
	m_pLog->logRecord("筛选6 阳线数:[%d] A数据到数据末尾K线根数[%d]\n", iSunlineNum, iDisFromAToEnd);
	
	rb = (float)iSunlineNum / iDisFromAToEnd;
	

	//todo: 筛选条件 大于等于命中
	if (rb < (float)tFirFilter.sRbcoe / 100)
	{
		m_pLog->logRecord("筛选6 失败： rb[%f] < 输入  rb\n", rb);
		return false;
	}
	
	m_pLog->logRecord("筛选6 成功 rb:[%f] > sRbcore[%f]\n", rb, (float)tFirFilter.sRbcoe / 100);
	return true;
}

//获取阳线个数
int CCaculateSotck2::GetSunLineNum(std::vector<tagKline>::const_iterator itvDataBegin,
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



CCaculateSotck2::CCaculateSotck2()
{

}

CCaculateSotck2::~CCaculateSotck2()
{

}


bool CCaculateSotck2::filter2Level1(const  std::map<tagStockCodeInfo, std::vector<tagKline>> &inMap,
	std::map<tagStockCodeInfo, tagOutput> & output, TFirstFilter& tFirFilter)
{
	TA2 tPer = tFirFilter.tLineNum;
	//周期系数KA 默认为6 - 9
	//周期系数KB 默认为6-8
	tPer.iMaxka = tPer.iMaxka == 0 ? PERIODKAMAX : tPer.iMaxka;
	tPer.iMinka = tPer.iMinka == 0 ? PERIODKAMIN : tPer.iMinka;
	tPer.iMaxkb = tPer.iMaxkb == 0 ? PERIODKBMAX : tPer.iMaxkb;
	tPer.iMinkb = tPer.iMinkb == 0 ? PERIODKBMIN : tPer.iMinkb;

	PrintInputParam(&tFirFilter, 0);

	std::map<tagStockCodeInfo, std::vector<tagKline>>::const_iterator iter;
	bool bRet = true;
	int nIndex = 0;
	for (iter = inMap.begin(); iter != inMap.end(); ++iter)
	{
		PrintHead(iter->first, ++nIndex);
		PrintData(iter->second);

		if (iter->second.size() > 0){
			tagKline temp = iter->second.at(iter->second.size() - 1);
			if (!CheckLicense(temp.time)){
				m_pLog->clearLog();
				m_pLog->logRecord("程序试用期已经结束，程序退出\n");
				return true;
			}
		}

		//m_pLog->logRecord("\n");
		//m_pLog->logRecord("市场：[%d] 代码：[%s]\n", iter->first.market, iter->first.stockcode.c_str());
		tagKline tLowA, tHighB1, tHighB2;
		int nAPos = 0, nB1Pos = 0, nB2Pos = 0, nL2Pos = 0;  //A最低点位置
		const std::vector<tagKline>  & vecKline = iter->second;
		bRet = filterStepA1(vecKline, tLowA,nAPos, tHighB1, nB1Pos, tHighB2 ,nB2Pos);
		if (!bRet)
		{
			m_pLog->logRecord("筛选第1步失败\n");
			continue;
		}
		
	
		bRet = filterStepA2(vecKline, tPer, nAPos, nB2Pos);
		if (!bRet)
		{
			m_pLog->logRecord("筛选第2步失败\n");
			continue;
		}

		
		if (tFirFilter.sA3Switch == eOff)
		{
			m_pLog->logRecord("第3步开关未开\n");
		}
		else
		{
			bRet = filterStepA3(vecKline, tFirFilter, nB2Pos, nB1Pos);
			if (!bRet)
			{
				m_pLog->logRecord("筛选第3步失败\n");
				continue;
			}
		}
	
		if (tFirFilter.bA4Switch == false)
		{
			m_pLog->logRecord("第4步开关未开\n");
		}
		else
		{
			bRet = filterStepA4(vecKline, tFirFilter, nB1Pos, nB2Pos, nL2Pos);
			if (!bRet)
			{
				m_pLog->logRecord("筛选第4步失败\n");
				continue;
			}

		}
		

		if (tFirFilter.bA5Switch == false)
		{
			m_pLog->logRecord("第5步开关未开\n");
		}
		else
		{
			bRet = filterStepA5(vecKline, tFirFilter, nAPos, nB2Pos);
			if (!bRet)
			{
				m_pLog->logRecord("筛选第5步失败\n");
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
				m_pLog->logRecord("筛选第6步失败\n");
				continue;
			}
		}
		
		//返回一级筛选结果
		output[iter->first] = tagOutput();
	}
	return true;

}
	

bool CCaculateSotck2::filter2Level2(const std::map<tagStockCodeInfo, std::vector<tagKline>> &inMap,
	std::map<tagStockCodeInfo, tagOutput> & output, TSecondFilter& tSedFilter)
{
	PrintInputParam(&tSedFilter, 1);
	TA2 tPer = tSedFilter.tLineNum;
	//周期系数KA 默认为6 - 9
	//周期系数KB 默认为6-8
	tPer.iMaxka = tPer.iMaxka == 0 ? PERIODKAMAXB1 : tPer.iMaxka;
	tPer.iMinka = tPer.iMinka == 0 ? PERIODKAMINB1 : tPer.iMinka;
	tPer.iMaxkb = tPer.iMaxkb == 0 ? PERIODKBMAXB1 : tPer.iMaxkb;
	tPer.iMinkb = tPer.iMinkb == 0 ? PERIODKBMINB1 : tPer.iMinkb;

	//循环处理每只股票
	std::map<tagStockCodeInfo, std::vector<tagKline>>::const_iterator iter;
	bool bRet = true;
	int nIndex = 0;
	for (iter = inMap.begin(); iter != inMap.end(); ++iter)
	{
		PrintHead(iter->first, ++nIndex);
		PrintData(iter->second);

		if (iter->second.size() > 0){
			tagKline temp = iter->second.at(iter->second.size() - 1);
			if (!CheckLicense(temp.time)){
				m_pLog->clearLog();
				m_pLog->logRecord("程序试用期已经结束，程序退出\n");
				return true;
			}
		}

		/*m_pLog->logRecord("\n");
		m_pLog->logRecord("市场：[%d] 代码：[%s]\n", iter->first.market, iter->first.stockcode.c_str());*/
		tagKline tLowA, tHighB1, tHighB2;
		int nAPos = 0, nB1Pos = 0, nB2Pos = 0,  nL2Pos = 0;  //A最低点位置
		const std::vector<tagKline>  & vecKline = iter->second;
		bRet = filterStepA1(vecKline, tLowA, nAPos, tHighB1, nB1Pos, tHighB2, nB2Pos);
		if (!bRet)
		{
			m_pLog->logRecord("二层A1失败\n");
			continue;
		}

		bRet = filterStepA2(vecKline, tPer, nAPos, nB2Pos);
		if (!bRet)
		{
			m_pLog->logRecord("二层A2失败\n");
			continue;
		}
		if (tSedFilter.bA5Switch == false)
		{
			m_pLog->logRecord("A5开关未开\n");
		}
		else
		{
			TFirstFilter tFilter;
			tFilter.tLineNum = tSedFilter.tLineNum;
			tFilter.bA5Switch = tSedFilter.bA5Switch;
			tFilter.sCallbackRange = tSedFilter.sCallbackRange;
			bRet = filterStepA5(vecKline, tFilter, nAPos, nB2Pos);
			if (!bRet)
			{
				continue;
			}
		}
		output[iter->first] = tagOutput();

	}

	return true;
}


bool CCaculateSotck2::filter2Level3(const std::map<tagStockCodeInfo, std::vector<tagKline>> &inMap,
	std::map<tagStockCodeInfo, tagOutput> & output, TThirdFilter& tThdFilter)
{
	PrintInputParam(&tThdFilter, 2);
	TA2 tPer = tThdFilter.tLineNum;
	//周期系数KA 默认为6 - 9
	//周期系数KB 默认为6-8
	tPer.iMaxka = tPer.iMaxka == 0 ? PERIODKAMAXB1 : tPer.iMaxka;
	tPer.iMinka = tPer.iMinka == 0 ? PERIODKAMINB1 : tPer.iMinka;
	tPer.iMaxkb = tPer.iMaxkb == 0 ? PERIODKBMAXB1 : tPer.iMaxkb;
	tPer.iMinkb = tPer.iMinkb == 0 ? PERIODKBMINB1 : tPer.iMinkb;

	//循环处理每只股票
	std::map<tagStockCodeInfo, std::vector<tagKline>>::const_iterator iter;
	bool bRet = true;
	int nIndex = 0;
	for (iter = inMap.begin(); iter != inMap.end(); ++iter)
	{
		PrintHead(iter->first, ++nIndex);
		PrintData(iter->second);

		if (iter->second.size() > 0){
			tagKline temp = iter->second.at(iter->second.size() - 1);
			if (!CheckLicense(temp.time)){
				m_pLog->clearLog();
				m_pLog->logRecord("程序试用期已经结束，程序退出\n");
				return true;
			}
		}


		//m_pLog->logRecord("\n");
		//m_pLog->logRecord("市场：[%d] 代码：[%s]\n", iter->first.market, iter->first.stockcode.c_str());
		tagKline tLowA, tHighB1, tHighB2;
		int nAPos = 0, nB1Pos = 0, nB2Pos = 0, nL2Pos = 0;  //A最低点位置
		const std::vector<tagKline>  & vecKline = iter->second;
		bRet = filterStepA1(vecKline, tLowA, nAPos, tHighB1, nB1Pos, tHighB2, nB2Pos);
		if (!bRet)
		{
			m_pLog->logRecord("二层A1失败\n");
			continue;
		}

		bRet = filterStepA2(vecKline, tPer, nAPos, nB2Pos);
		if (!bRet)
		{
			m_pLog->logRecord("二层A2失败\n");
			continue;
		}
		if (tThdFilter.bA5Switch == false)
		{
			m_pLog->logRecord("A5开关未开\n");
		}
		else
		{
			TFirstFilter tFilter;
			tFilter.tLineNum = tThdFilter.tLineNum;
			tFilter.bA5Switch = tThdFilter.bA5Switch;
			tFilter.sCallbackRange = tThdFilter.sCallbackRange;
			bRet = filterStepA5(vecKline, tFilter, nAPos, nB2Pos);
			if (!bRet)
			{
				continue;
			}
		}
		output[iter->first] = tagOutput();

	}

	return true;
}


