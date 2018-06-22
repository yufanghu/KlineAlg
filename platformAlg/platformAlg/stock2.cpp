// stock.cpp 。
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
	return a.dColsePrice < b.dColsePrice;  //不能取等号 ？？
}

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


	tagResult.close = itvDataBegin.begin()->close;
	if (bLow)
	{
		//获取最低收盘价
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
		//获取最高收盘价  如果收盘价相同则最高价低的为最高收盘价 
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

//返回 最低价
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

	//k线小于等于2，不处理
	if (vecKline.size() <= 2)
	{
		m_pLog->logRecord("k线小于2\n");
		return false;
	}
	tagKline tAPos;
	bool bRet = true;
	char buf[32];
	bool bRet = GetLowOrHighClose(vecKline, 0,vecKline.size(), tAPos,nAPos, true);
	if (!bRet || nAPos == vecKline.size() -1 )
	{
		m_pLog->logRecord("进阶筛选A1找不到最低数据\n");
		bRet = false;
	}

	stamp_to_standard(tAPos.time, buf);
	m_pLog->logRecord("进阶筛选A1收盘最低数据：%s open %f high %f low %f close %f \n", buf, tAPos.open, tAPos.high, tAPos.low, tAPos.close);


	//搜索A数据日期前收盘价最高B1数据
	tagKline tB1Pos;
	bRet = GetLowOrHighClose(vecKline, 0, nAPos - 1, tB1Pos,nB1Pos, false);
	if (!bRet)
	{
		m_pLog->logRecord("进阶筛选未找到最高收盘价B1\n");
		bRet = false;
	}
	stamp_to_standard(tB1Pos.time, buf);
	m_pLog->logRecord("进阶筛选B1收盘最高数据：%s open %f high %f low %f close %f \n", buf, tB1Pos.open, tB1Pos.high, tB1Pos.low, tB1Pos.close);

	

	//搜索A数据日期后收盘价最高B2数据
	tagKline tB2Pos;
	bRet = GetLowOrHighClose(vecKline, nAPos + 1, vecKline.size(), tB2Pos, nB2Pos, false);
	if (!bRet)
	{
		m_pLog->logRecord("进阶筛选未找到最高收盘价B2\n");
		bRet = false;
	}
	stamp_to_standard(tB2Pos.time, buf);
	m_pLog->logRecord("进阶筛选B2收盘最高数据：%s open %f high %f low %f close %f \n", buf, tB2Pos.open, tB2Pos.high, tB2Pos.low, tB2Pos.close);
	return bRet;
}


//获取阳线个数
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


	m_pLog->logRecord("一级筛选\n");
	m_pLog->logRecord("周期系数ka [%d]-[%d]| 周期系数kb [%d]-[%d]\n", tFirFilter.tLineNum.iMaxka, tFirFilter.tLineNum.iMinka,
		tFirFilter.tLineNum.iMaxkb, tFirFilter.tLineNum.iMinkb);
	m_pLog->logRecord("开关3 [%s] |4 [%]|5 [%s]| 6 [%s] | 调整幅度[%d] | 阳线比例[%d]\n", tFirFilter.sA3Switch ? "on" : "off",
		tFirFilter.bA4Switch ? "on" : "off", tFirFilter.bA5Switch ? "on" : "off", tFirFilter.bA6Switch ? "on" : "off", tFirFilter.sCallbackRange,
		tFirFilter.sRbcoe);

	std::map<tagStockCodeInfo, std::vector<tagKline>>::const_iterator iter;
	bool bRet = true;
	for (iter = inMap.begin(); iter != inMap.end(); ++iter)
	{
		

		tagKline tLowA, tHighB1, tHighB2;
		int nAPos = 0, nB1Pos = 0, nB2Pos = 0;  //A最低点位置
		const std::vector<tagKline>  & vecKline = iter->second;
		bRet = filterStepA1(vecKline, tLowA,nAPos, tHighB1, nB1Pos, tHighB2 ,nB2Pos);
		if (!bRet)
		{
			m_pLog->clearLog("进阶A1失败\n");
			continue;
		}
		
		//条件B、筛选出数据B2（包含该数据）到数据末期，k线根数符合周期系数KB(6-8默认）范围内
		std::vector<tagKline>::const_iterator ptrmB2Pos = find(vecKline.begin(), vecKline.end(), tB2Pos);
		int iDis = distance(ptrmAPos,ptrmB2Pos);  
		m_pLog->logRecord("进阶筛选A-B2根数[%d]\n",iDis);

		if (iDis < tFirFilter.tLineNum.iMinka || tFirFilter.tLineNum.iMaxka < iDis)
		{
			m_pLog->logRecord("进阶筛选A-B2根数不满足 min[%d]<[%d]<max[%d]\n",
				tFirFilter.tLineNum.iMinka,iDis, tFirFilter.tLineNum.iMaxka);
			continue;
		}

		//KA 包括A
		iDis = distance(ptrmB2Pos, vecKline.end()) + 1;
		m_pLog->logRecord("进阶筛选B2-end根数[%d]\n", iDis);
		if (iDis < tFirFilter.tLineNum.iMinkb || tFirFilter.tLineNum.iMaxkb < iDis)
		{
			m_pLog->logRecord("进阶筛选B2-end根数不满足 min[%d]<[%d]<max[%d]\n", 
				tFirFilter.tLineNum.iMinkb, iDis, tFirFilter.tLineNum.iMaxkb);
			continue;
		}

		//筛选3   0 表示无类型，判断是AA,AB,AAP 型 1, 2, 3  
		int iDoubleType = 0;
		if (tFirFilter.sA3Switch == 1 || tFirFilter.sA3Switch == 2)
		{
			if (tB2Pos.close >= tB1Pos.close)
			{
				iDoubleType = 1;
				m_pLog->logRecord("筛选3 AA型\n");
			}
			else if (tB2Pos.close < tB1Pos.close)
			{
				iDoubleType = 2;
				m_pLog->logRecord("筛选3 AB型\n");
			}

			if (tFirFilter.sA3Switch != iDoubleType)
			{
				//当前数据类型和用户要选择的，不一致，抛弃这组数据
				m_pLog->logRecord("当前类型:[%d] 和开关类型不同 [%d]\n", iDoubleType, tFirFilter.sA3Switch);
				continue;
			}
		}

		//筛选4 : 假如是AA型，就只用AA型的数据  
		if (tFirFilter.bA4Switch && iDoubleType == 1)
		{
			//筛选出B2数据后最低收盘价格L2数据（不包括B2数据）
			vecKlineTmp.assign(ptrmB2Pos + 1, vecKline.end());
			tagKline tL2Pos;
			bRet = GetLowClose(vecKlineTmp, vecKlineTmp.size(), tL2Pos, true);

			//todo: 没有L2
			if (!bRet)
			{
				m_pLog->logRecord("筛选4 没有L2\n");
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

		//筛选5
		if (tFirFilter.bA5Switch)
		{
			double h1 = tB2Pos.high - tAPos.low;
			m_pLog->logRecord("筛选5 幅度h1:[%f]\n", h1);

			tagKline tEndData;
			if (ptrmB2Pos + 1 != vecKline.end())
			{
				bRet =  GetLow(ptrmB2Pos, vecKline.end(), tEndData);
			}
			else
			{
				m_pLog->logRecord("B2就是末尾数据\n");
				continue;
			}

			double h2 = tB2Pos.high - tEndData.low;
			m_pLog->logRecord("筛选5 幅度h2:[%f]\n", h2);
			
			if ((h1 == 0))
			{
				m_pLog->logRecord("h1 is 0\n");
				continue;
			}
			double ht = h2 / h1;
			if (ht > static_cast<double>(tFirFilter.sCallbackRange / 100))
			{
				m_pLog->logRecord("ht[%f]不满足条件\n", ht);
				continue;
			}
		}

		//筛选6
		if (tFirFilter.bA6Switch)
		{
			//统计A数据（包括该数据）到数据末期，阳K线数目
			int iSunlineNum = 0;
			int iDisFromAToEnd = 0;
			float rb = 0.0;

			iSunlineNum = GetSunLineNum(ptrmAPos, vecKline.end());
			iDisFromAToEnd = distance(ptrmAPos, vecKline.end());
			if (iDisFromAToEnd == 0)
			{
				m_pLog->logRecord("筛选6错误\n");
				continue;
			}
			rb = (float)iSunlineNum / iDisFromAToEnd;
			m_pLog->logRecord("阳线数:[%d] A数据到数据末尾K线根数[%d]\n ", iSunlineNum, iDisFromAToEnd);
			
			//todo: 筛选条件 大于等于命中
			if (rb < static_cast<double>(tFirFilter.sRbcoe / 100))
			{
				m_pLog->logRecord("rb[%f] < 输入  rb\n",rb);
				continue;
			}
		}

		//返回一级筛选结果
		output[iter->first] = tagOutput();
	}
	return true;

}
	



bool CFilter2Alg::filter2Level2(std::map<tagStockCodeInfo, std::vector<tagKline>> &inMap, TSecondFilter& tSedFilter)
{
	std::map<tBaseInfo, tOutData>  outMap;

	//启动日志
	logstream2.open("./stock.log", ofstream::app);
	logforpara2(inMap);

	logstream2 << "二级筛选开关" << endl;
	logstream2 << "最大ka | 最小ka | 最大kb | 最小kb： " << tSedFilter.tLineNum.iMaxka << " " << tSedFilter.tLineNum.iMinka
		<< " " << tSedFilter.tLineNum.iMaxkb << " " << tSedFilter.tLineNum.iMinkb << endl;
	logstream2 << "开关5和调整幅度：" << tSedFilter.bA5Switch << " " << tSedFilter.sCallbackRange << endl;

	//循环处理每只股票
	std::map<tBaseInfo, std::vector<DataT>>::iterator itMap = inMap.begin();
	while (itMap != inMap.end())
	{
		logstream2 << "市场： " << itMap->first.sSetcode << " 代码 " << itMap->first.sTockcode << endl;
		std::vector<DataT>& vStockData = itMap->second;

		//todo: k线小于等于2，不处理
		if (vStockData.size() <= 2)
		{
			logstream2 << "k线小于2 ";
			cout << "k线小于2 " << endl;
			itMap++;
			continue;
		}
		//todo: A B1 B2 没有值，就进行下一只股票处理

		//1.筛选1  收盘价最低价
		DataT tAPos = GetLowClose(vStockData.begin(), vStockData.end(), true);
		if (tAPos.uDate == -1)
		{
			logstream2 << " 当前数据没有A " << endl;
			itMap++;
			continue;
		}
		logstream2 << "A 点 " << tAPos.dColsePrice << "  " << tAPos.dLowPrice << " |  ";
		std::vector<DataT>::iterator ptrmAPos = find(vStockData.begin(), vStockData.end(), tAPos);
		if (ptrmAPos == vStockData.end())
		{
			logstream2 << "无A 点" << endl;
			cout << "not find Apos" << endl;
			itMap++;
			continue;
		}

		//todo:B1，B2 不包含A
		DataT tB1Pos = GetLowClose(vStockData.begin(), ptrmAPos, false);
		if (tB1Pos.uDate == -1)
		{
			logstream2 << " 当前数据没有B1 " << endl;
			itMap++;
			continue;
		}
		logstream2 << "B1 点 " << tB1Pos.dColsePrice << "  " << tB1Pos.dHighPrice << " | ";



		DataT tB2Pos = GetLowClose(ptrmAPos + 1, vStockData.end(), false);
		if (tB2Pos.uDate == -1)
		{
			logstream2 << " 当前数据没有B2 " << endl;
			itMap++;
			continue;
		}
		logstream2 << "B2 点 " << tB2Pos.dColsePrice << " | " << tB2Pos.dHighPrice << " | ";

		//筛选2 
		//KB 包括B
		std::vector<DataT>::iterator ptrmB2Pos = find(vStockData.begin(), vStockData.end(), tB2Pos);
		int iDis = distance(ptrmB2Pos, vStockData.end());

		logstream2 << "KB 点 " << iDis << " | ";
		if (iDis < tSedFilter.tLineNum.iMinka || tSedFilter.tLineNum.iMaxka < iDis)
		{
			cout << "不符合周期系数KB范围" << endl;
			itMap++;
			continue;
		}

		//KA 包括A
		iDis = distance(ptrmAPos, ptrmB2Pos) + 1;
		logstream2 << "KA 点 " << iDis << " | ";
		if (iDis < tSedFilter.tLineNum.iMinkb || tSedFilter.tLineNum.iMaxkb < iDis)
		{
			cout << "不符合周期系数KA范围" << endl;
			itMap++;
			continue;
		}

		//筛选5
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

		//返回二级筛选结果
		tOutData tEmpty;
		outMap.insert(make_pair(itMap->first, tEmpty));

		itMap++;
	}

	PrintHitStock(outMap, "二级筛选选中股票列表");

	logstream2.close();
	return outMap;
}

bool CFilter2Alg::filter2Level3(std::map<tagStockCodeInfo, std::vector<tagKline>> &inMap, TThirdFilter& tThdFilter)
{
	std::map<tBaseInfo, tOutData>  outMap;

	//启动日志
	logstream2.open("./stock.log", ofstream::app);
	logforpara2(inMap);
	logstream2 << "三级筛选开关" << endl;
	logstream2 << "最大ka | 最小ka | 最大kb | 最小kb： " << tThdFilter.tLineNum.iMaxka << " " << tThdFilter.tLineNum.iMinka << "  "
		<< tThdFilter.tLineNum.iMaxkb << " " << tThdFilter.tLineNum.iMinkb << endl;
	logstream2 << "开关5 | 调整幅度: " << tThdFilter.bA5Switch << " " << static_cast<double>(tThdFilter.sCallbackRange / 100) << endl;

	//循环处理每只股票
	std::map<tBaseInfo, std::vector<DataT>>::iterator itMap = inMap.begin();
	while (itMap != inMap.end())
	{
		logstream2 << "市场： " << itMap->first.sSetcode << " 代码 " << itMap->first.sTockcode << endl;
		std::vector<DataT>& vStockData = itMap->second;

		//todo: k线小于等于2，不处理
		if (vStockData.size() <= 2)
		{
			logstream2 << "k线小于2 ";
			cout << "k线小于2 " << endl;
			itMap++;
			continue;
		}
		//todo: A B1 B2 没有值，就进行下一只股票处理

		//1.筛选1  收盘价最低价
		DataT tAPos = GetLowClose(vStockData.begin(), vStockData.end(), true);
		if (tAPos.uDate == -1)
		{
			logstream2 << " 当前数据没有A " << endl;
			itMap++;
			continue;
		}
		logstream2 << "A 点 " << tAPos.dColsePrice << "  " << tAPos.dLowPrice << " |  ";
		std::vector<DataT>::iterator ptrmAPos = find(vStockData.begin(), vStockData.end(), tAPos);
		if (ptrmAPos == vStockData.end())
		{
			logstream2 << "无A 点" << endl;
			cout << "not find Apos" << endl;
			itMap++;
			continue;
		}

		//todo:B1，B2 不包含A
		DataT tB1Pos = GetLowClose(vStockData.begin(), ptrmAPos, false);
		if (tB1Pos.uDate == -1)
		{
			logstream2 << " 当前数据没有B2 " << endl;
			itMap++;
			continue;
		}
		logstream2 << "B1 点 " << tB1Pos.dColsePrice << "  " << tB1Pos.dHighPrice << " | ";


		DataT tB2Pos = GetLowClose(ptrmAPos + 1, vStockData.end(), false);
		if (tB2Pos.uDate == -1)
		{
			logstream2 << " 当前数据没有B2 " << endl;
			itMap++;
			continue;
		}
		logstream2 << "B2 点 " << tB2Pos.dColsePrice << " | " << tB2Pos.dHighPrice << " | ";

		//筛选2 
		//KB 包括B
		std::vector<DataT>::iterator ptrmB2Pos = find(vStockData.begin(), vStockData.end(), tB2Pos);
		int iDis = distance(ptrmB2Pos, vStockData.end());

		logstream2 << "KB 点 " << iDis << " | ";
		if (iDis < tThdFilter.tLineNum.iMinka || tThdFilter.tLineNum.iMaxka < iDis)
		{
			cout << "不符合周期系数KB范围" << endl;
			itMap++;
			continue;
		}

		//KA 包括A
		iDis = distance(ptrmAPos, ptrmB2Pos) + 1;
		logstream2 << "KA 点 " << iDis << " | ";
		if (iDis < tThdFilter.tLineNum.iMinkb || tThdFilter.tLineNum.iMaxkb < iDis)
		{
			cout << "不符合周期系数KA范围" << endl;
			itMap++;
			continue;
		}

		//筛选5
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

		//返回三级筛选结果
		tOutData tEmpty;
		outMap.insert(make_pair(itMap->first, tEmpty));

		itMap++;
	}

	PrintHitStock(outMap, "三级筛选选中股票列表");

	logstream2.close();
	return outMap;

}

