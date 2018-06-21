// stock.cpp 。
//
#include "stdafx.h"
#include "stock2.h"


using namespace std;
using namespace stock_second;

//创建日志
ofstream logstream2;

/*bool stock_second::operator<(const tBaseInfo& a1, const tBaseInfo& a2)
{  
	if (a1.sSetcode >= a2.sSetcode)
		return false;  
	return true;  
}*/ 


bool stock_second::operator<(const tBaseInfo& a1, const tBaseInfo& a2)
{
	if (a1.sSetcode < a2.sSetcode)
		return true;
	else if (a1.sSetcode > a2.sSetcode)
		return false;
	else
	{
		if (a1.sTockcode.compare(a2.sTockcode) > 0)
			return false;
		else if (a1.sTockcode.compare(a2.sTockcode) < 0)
			return true;
		else
			return false;
	}
};


bool CmpLowClosePri(const DataT& a, const DataT& b)
{
	return a.dColsePrice < b.dColsePrice;  //不能取等号 ？？
}

//返回 最低收盘价
DataT GetLowClose(std::vector<DataT>::iterator itvDataBegin, 
	               std::vector<DataT>::iterator itvDataEnd, bool bLow)
{
	DataT tAPos;
	//todo tApos 添加构造函数，让它初始化为0

	if (itvDataBegin == itvDataEnd)
	{
		cout << "GetLowClose start == end " <<endl;
		tAPos.uDate = -1;
		return tAPos;
	}

	std::vector<DataT> vData(itvDataBegin, itvDataEnd);

	//1. 按照 最低价 做升序排序，然后取第一个元素
	//todo: 如果价格相同，A就按照最低价排序， B1， B2 最高价排序
	sort(vData.begin(), vData.end(), CmpLowClosePri);

	if (bLow)
	{
		tAPos = vData[0];
        //最低的最低价
		DataT tLowPri;
        std::vector<DataT>::iterator itTmp = vData.begin();
        tLowPri = *itTmp++;
		while (itTmp != vData.end())
        {
            if (tLowPri.dColsePrice - itTmp->dColsePrice)
            {
                break;
            }
			else if (tLowPri.dLowPrice > itTmp->dLowPrice)
			{
				tLowPri = *itTmp;
			}
			itTmp++;
        }

		tAPos = tLowPri;
	}
	else 
	{
		std::vector<DataT>::iterator itend = (vData.end() -1);
		tAPos = *itend;
        //最高的最高价
		DataT tHighPri;
		std::vector<DataT>::iterator itTmp = itend;
		tHighPri = *(itTmp);
		while (itTmp != vData.begin())
		{
			itTmp--;
			if (tHighPri.dColsePrice != itTmp->dColsePrice)
			{
				break;
			}
			else if (tHighPri.dHighPrice < itTmp->dHighPrice)
			{
				tHighPri = *itTmp;
			}
		}

		tAPos = tHighPri;
	}
	

	return tAPos;
}


bool CmpLowPri(const DataT& a, const DataT& b)
{
    return a.dLowPrice < b.dLowPrice;  //
}

//返回 最低价
DataT GetLow(std::vector<DataT>::iterator itvDataBegin,
              std::vector<DataT>::iterator itvDataEnd)
{
    DataT tAPos;
    //todo tApos 添加构造函数，让它初始化为0

    if (itvDataBegin == itvDataEnd)
    {
        cout << "GetLow start == end " << endl;
		tAPos.uDate = -1;
        return tAPos;
    }

    std::vector<DataT> vData(itvDataBegin, itvDataEnd);

    //1. 按照 最低价 做升序排序，然后取第一个元素
    //todo: 如果价格相同，A就按照最低价排序， B1， B2 最高价排序
    sort(vData.begin(), vData.end(), CmpLowPri);

	tAPos = vData[0];

    return tAPos;
}


/*bool DATAPP::operator ==(const uint64_t uDes)
{
	return this->uDate == uDes;
}

typedef pair<uint64_t, double> PAIR; 
struct CmpByValue {    
	bool operator()(const PAIR& lhs, const PAIR& rhs) {    
		return lhs.second > rhs.second;    
	}    
};*/


//获取阳线个数
int GetSunLineNum(std::vector<DataT>::iterator itvDataBegin, 
	               std::vector<DataT>::iterator itvDataEnd)
{
	int iNum = 0;


	std::vector<DataT>::iterator itBegin = itvDataBegin;
	std::vector<DataT>::iterator itEnd   = itvDataEnd;
	while (itBegin != itEnd)
	{
	    (itBegin->dColsePrice >= itBegin->dOpenPrice) ? iNum++:iNum;
		itBegin++;
	}

	return iNum;
}

/*
void stock_second::logforpara2(std::map<tBaseInfo, std::vector<DataT>>& inMap)
{}
*/


void stock_second::logforpara2(std::map<tBaseInfo, std::vector<DataT>>& inMap)
{
	time_t currtime = time(NULL);  
	tm* p = localtime(&currtime);  
	char timestring[100] = {0};  

	sprintf(timestring,"=>%d-%02d-%02d %02d:%02d:%02d",p->tm_year+1900,p->tm_mon+1,p->tm_mday,p->tm_hour,p->tm_min,p->tm_sec);  

	logstream2 << timestring <<endl;

	//循环打印所有元素
	std::map<tBaseInfo, std::vector<DataT>>::iterator itMap = inMap.begin();
	while (itMap != inMap.end())
	{
		//打印key
		logstream2 << itMap->first.sSetcode;
		logstream2 << itMap->first.sTockcode;

		std::vector<DataT>&vStockData = itMap->second;
		std::vector<DataT>::iterator itVet = vStockData.begin();
		while (itVet != vStockData.end())
		{
			logstream2 << "日期，开，高，低，收 " <<itVet->uDate << " | "<< itVet->dOpenPrice 
				<< " | " << itVet->dHighPrice << " | " << itVet->dLowPrice
				<< " | "<< itVet->dColsePrice;

			itVet++;
			logstream2 <<endl;
		}
		logstream2 << "-------" <<endl;
		itMap++;
	}
}


//打印显示筛选出来的股票
inline void PrintHitStock(const std::map<tBaseInfo, tOutData>& outMap, 
	                      const std::string& strLogInfo)
{
	logstream2 << strLogInfo.c_str() << endl;
	std::map<tBaseInfo, tOutData>::const_iterator citMap = outMap.begin();
	while (citMap != outMap.end())
	{
		logstream2 << citMap->first.sSetcode << "  " << citMap->first.sTockcode << endl;
		citMap++;
	}
}


//一级筛选
std::map<tBaseInfo, tOutData> CStock::DoAlgorithm(std::map<tBaseInfo, std::vector<DataT>> &inMap,
	                                              TFirstFilter& tFirFilter)
{
	std::map<tBaseInfo, tOutData>  outMap;

	//启动日志
	logstream2.open("./stock.log", ofstream::app);
	logforpara2(inMap);

	logstream2 << "一级筛选开关" << endl;
	logstream2 << "最大ka | 最小ka | 最大kb | 最小kb： " << tFirFilter.tLineNum.iMaxka <<" "<< tFirFilter.tLineNum.iMinka 
		       <<" " <<tFirFilter.tLineNum.iMaxkb << " " << tFirFilter.tLineNum.iMinkb << endl;
	logstream2 <<"开关3|4|5|6 | 调整幅度 | 阳线比例" <<tFirFilter.sA3Switch <<" "<< tFirFilter.bA4Switch <<" "<< tFirFilter.bA5Switch 
		       << " " << tFirFilter.bA6Switch << " " << tFirFilter.sCallbackRange<< " " << tFirFilter.sRbcoe << endl;

	//循环处理每只股票
	std::map<tBaseInfo, std::vector<DataT>>::iterator itMap = inMap.begin();
	while (itMap != inMap.end())
	{
		logstream2 << "市场： " <<itMap->first.sSetcode << " 代码 " << itMap->first.sTockcode << endl;
		std::vector<DataT>& vStockData = itMap->second;

		//todo: k线小于等于2，不处理
		if (vStockData.size() <= 2)
		{
			logstream2 << "k线小于2 ";
			cout<< "k线小于2 " <<endl;
			itMap++;
			continue;
		}
		//todo: A B1 B2 没有值，就进行下一只股票处理

		//筛选第一阶段
		//1.筛选1  收盘价最低价
		DataT tAPos = GetLowClose(vStockData.begin(), vStockData.end(), true);
		if (tAPos.uDate == -1)
		{
			logstream2 << " 当前数据没有A " << endl;
			itMap++;
			continue;
		}
		logstream2 <<"A 点 "<< tAPos.dColsePrice <<"  " <<tAPos.dLowPrice<< " |  ";
		
		//获取A 点迭代器
		std::vector<DataT>::iterator ptrmAPos = find(vStockData.begin(), vStockData.end(), tAPos);
		if (ptrmAPos == vStockData.end())
		{
			logstream2 <<"无A 点"<<endl;
			cout <<"not find Apos"<<endl;
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
		logstream2 <<"B1 点 "<< tB1Pos.dColsePrice<<"  " <<tB1Pos.dHighPrice<<" | ";

		DataT tB2Pos = GetLowClose(ptrmAPos + 1, vStockData.end(), false);
		if (tB2Pos.uDate == -1)
		{
			logstream2 <<" 当前数据没有B2 " <<endl;
			itMap++;
			continue;
		}
		logstream2 <<"B2 点 "<< tB2Pos.dColsePrice<<" | " <<tB2Pos.dHighPrice<<" | ";

		//筛选2 
		//KB 包括B
		std::vector<DataT>::iterator ptrmB2Pos = find(vStockData.begin(), vStockData.end(), tB2Pos);
		int iDis = distance(ptrmB2Pos, vStockData.end());

		logstream2 <<"KB 点 "<< iDis <<" | ";
		if (iDis < tFirFilter.tLineNum.iMinka  || tFirFilter.tLineNum.iMaxka < iDis)
		{
			cout <<"不符合周期系数KB范围"<<endl;
			itMap++;
			continue;
		}

		//KA 包括A(是否包含B2 ？？？)
		iDis = distance(ptrmAPos, ptrmB2Pos) + 1;
		logstream2 <<"KA 点 "<< iDis <<" | ";
		if (iDis < tFirFilter.tLineNum.iMinkb || tFirFilter.tLineNum.iMaxkb < iDis)
		{
			cout <<"不符合周期系数KA范围"<<endl;
	        itMap++;
			continue;
		}

		//筛选3   0 表示无类型，判断是AA,AB,AAP 型 1, 2, 3  
		int iDoubleType = 0; 
		if (tFirFilter.sA3Switch == 1 || tFirFilter.sA3Switch == 2)
		{
			if (tB2Pos.dColsePrice >= tB1Pos.dColsePrice)
			{
				iDoubleType = 1;
				logstream2 <<" AA型 | ";
			}
			else if (tB2Pos.dColsePrice < tB1Pos.dColsePrice)
			{
				iDoubleType = 2;
				logstream2 <<" AB型 | ";
			}

			if (tFirFilter.sA3Switch != iDoubleType)
			{
				//当前数据类型和用户要选择的，不一致，抛弃这组数据
			    logstream2<<"当前类型："<< iDoubleType << " 和开关类型不同"<<endl;
				itMap++;
				continue;
			}
		}

		//筛选4 : 假如是AA型，就只用AA型的数据  
		if (tFirFilter.bA4Switch && iDoubleType == 1)
		{
			//筛选出B2数据后最低收盘价格L2数据（不包括B2数据）
			DataT tL2Pos = GetLowClose((ptrmB2Pos+1), vStockData.end(), true);

			//todo: 没有L2
			if (-1 == tL2Pos.uDate)
			{
				logstream2 <<" 没有L2 "<<endl;
				itMap++;
				continue;
			}

			if (tL2Pos.dColsePrice > tB1Pos.dColsePrice)
			{
				iDoubleType = 3; //AAP
			}
			else
		    {
				itMap++;
				continue;
			}
		}

		//筛选5
		if (tFirFilter.bA5Switch)
		{
			double h1 = tB2Pos.dHighPrice - tAPos.dLowPrice;
			logstream2 <<"h1: " << h1 <<" | ";
            DataT tEndData;
            if (ptrmB2Pos + 1 != vStockData.end())
            {
                tEndData = GetLow(ptrmB2Pos, vStockData.end());
            }
			else
			{
				logstream2 <<"B2就是末尾数据";
				itMap++;
				continue;
			}
            
			double h2 = tB2Pos.dHighPrice - tEndData.dLowPrice;
			logstream2 << " h2 "<< h2 <<" ";
			double ht = h2/h1;
			if ((h1 == 0) ||  ht > static_cast<double>(tFirFilter.sCallbackRange/100))
			{
				logstream2 << " ht:" << ht <<" | ";
				itMap++;
				continue;
			}
		}

		//筛选6
		if (tFirFilter.bA6Switch)
		{
			//统计A数据（包括该数据）到数据末期，阳K线数目
			int iSunlineNum    = 0;
			int iDisFromAToEnd = 0;
			float rb           = 0.0;

			iSunlineNum    = GetSunLineNum(ptrmAPos, vStockData.end());
			iDisFromAToEnd = distance(ptrmAPos, vStockData.end());
			rb             = (float)iSunlineNum / iDisFromAToEnd;
			logstream2<< " 阳线数: " << iSunlineNum << " | " << "A数据到数据末尾K线根数" << iDisFromAToEnd <<" ";
			//todo: 筛选条件 大于等于命中
			if (rb < static_cast<double>(tFirFilter.sRbcoe/100))
			{
				logstream2 <<" rb < 输入  rb "<< rb <<" | " <<endl;
				itMap++;
				continue;
			}
		}

		//返回一级筛选结果
		tOutData tEmpty;
		outMap.insert(make_pair(itMap->first, tEmpty));

		itMap++;
	}

	//打印选中的股票
	PrintHitStock(outMap, "一级筛选选中股票列表");

	logstream2.close();
	return outMap;
}


//二级筛选
std::map<tBaseInfo, tOutData> CStock::DoAlgorithm(std::map<tBaseInfo, std::vector<DATAPP>> &inMap, 
	                                              TSecondFilter& tSedFilter)
{
	std::map<tBaseInfo, tOutData>  outMap;

	//启动日志
	logstream2.open("./stock.log", ofstream::app);
	logforpara2(inMap);

	logstream2 << "二级筛选开关" << endl;
	logstream2 << "最大ka | 最小ka | 最大kb | 最小kb： "<< tSedFilter.tLineNum.iMaxka <<" "<< tSedFilter.tLineNum.iMinka
		       << " " << tSedFilter.tLineNum.iMaxkb << " " << tSedFilter.tLineNum.iMinkb << endl;
	logstream2 <<"开关5和调整幅度：" <<tSedFilter.bA5Switch << " " << tSedFilter.sCallbackRange << endl;

	//循环处理每只股票
	std::map<tBaseInfo, std::vector<DataT>>::iterator itMap = inMap.begin();
	while (itMap != inMap.end())
	{
		logstream2 << "市场： " <<itMap->first.sSetcode << " 代码 " << itMap->first.sTockcode << endl;
		std::vector<DataT>& vStockData = itMap->second;

		//todo: k线小于等于2，不处理
		if (vStockData.size() <= 2)
		{
			logstream2 << "k线小于2 ";
			cout<< "k线小于2 " <<endl;
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
		logstream2 <<"A 点 "<< tAPos.dColsePrice <<"  " <<tAPos.dLowPrice<< " |  ";
		std::vector<DataT>::iterator ptrmAPos = find(vStockData.begin(), vStockData.end(), tAPos);
		if (ptrmAPos == vStockData.end())
		{
			logstream2 <<"无A 点"<<endl;
			cout <<"not find Apos"<<endl;
			itMap++;
			continue;
		}

		//todo:B1，B2 不包含A
		DataT tB1Pos = GetLowClose(vStockData.begin(), ptrmAPos, false);
		if (tB1Pos.uDate == -1)
		{
			logstream2 <<" 当前数据没有B1 " <<endl;
			itMap++;
			continue;
		}
		logstream2 <<"B1 点 "<< tB1Pos.dColsePrice<<"  " <<tB1Pos.dHighPrice<<" | ";



		DataT tB2Pos = GetLowClose(ptrmAPos + 1, vStockData.end(), false);
		if (tB2Pos.uDate == -1)
		{
			logstream2 <<" 当前数据没有B2 " <<endl;
			itMap++;
			continue;
		}
		logstream2 <<"B2 点 "<< tB2Pos.dColsePrice<<" | " <<tB2Pos.dHighPrice<<" | ";

		//筛选2 
		//KB 包括B
		std::vector<DataT>::iterator ptrmB2Pos = find(vStockData.begin(), vStockData.end(), tB2Pos);
		int iDis = distance(ptrmB2Pos, vStockData.end());

		logstream2 <<"KB 点 "<< iDis <<" | ";
		if (iDis < tSedFilter.tLineNum.iMinka  || tSedFilter.tLineNum.iMaxka < iDis)
		{
			cout <<"不符合周期系数KB范围"<<endl;
			itMap++;
			continue;
		}

		//KA 包括A
		iDis = distance(ptrmAPos, ptrmB2Pos) + 1;
		logstream2 <<"KA 点 "<< iDis <<" | ";
		if (iDis < tSedFilter.tLineNum.iMinkb || tSedFilter.tLineNum.iMaxkb < iDis)
		{
			cout <<"不符合周期系数KA范围"<<endl;
			itMap++;
			continue;
		}

		//筛选5
		if (tSedFilter.bA5Switch)
		{
			double h1 = tB2Pos.dHighPrice - tAPos.dLowPrice;
			logstream2 <<"h1: " << h1 <<" | ";
			DataT tEndData;
			if (ptrmB2Pos + 1 != vStockData.end())
			{
				tEndData = GetLow(ptrmB2Pos, vStockData.end());
			}

			double h2 = tB2Pos.dHighPrice - tEndData.dLowPrice;
			logstream2 << " h2 "<< h2 <<" ";
			double ht = h2/h1;
			if ((h1 == 0) || ht > static_cast<double>(tSedFilter.sCallbackRange/100))
			{
				logstream2 << " ht:" << ht <<" | ";
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

//三级筛选
std::map<tBaseInfo, tOutData> CStock::DoAlgorithm(std::map<tBaseInfo,std::vector<DATAPP>> &inMap, 
	                                      TThirdFilter& tThdFilter)
{
	std::map<tBaseInfo, tOutData>  outMap;

	//启动日志
	logstream2.open("./stock.log", ofstream::app);
	logforpara2(inMap);
	logstream2 << "三级筛选开关" << endl;
	logstream2 <<"最大ka | 最小ka | 最大kb | 最小kb： "<< tThdFilter.tLineNum.iMaxka <<" " << tThdFilter.tLineNum.iMinka << "  "
		       << tThdFilter.tLineNum.iMaxkb <<" " << tThdFilter.tLineNum.iMinkb << endl;
	logstream2 << "开关5 | 调整幅度: " <<tThdFilter.bA5Switch << " "<< static_cast<double>(tThdFilter.sCallbackRange/100) << endl;

	//循环处理每只股票
	std::map<tBaseInfo, std::vector<DataT>>::iterator itMap = inMap.begin();
	while (itMap != inMap.end())
	{
		logstream2 << "市场： " <<itMap->first.sSetcode << " 代码 " << itMap->first.sTockcode << endl;
		std::vector<DataT>& vStockData = itMap->second;

		//todo: k线小于等于2，不处理
		if (vStockData.size() <= 2)
		{
			logstream2 << "k线小于2 ";
			cout<< "k线小于2 " <<endl;
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
		logstream2 <<"A 点 "<< tAPos.dColsePrice <<"  " <<tAPos.dLowPrice<< " |  ";
		std::vector<DataT>::iterator ptrmAPos = find(vStockData.begin(), vStockData.end(), tAPos);
		if (ptrmAPos == vStockData.end())
		{
			logstream2 <<"无A 点"<<endl;
			cout <<"not find Apos"<<endl;
			itMap++;
			continue;
		}

		//todo:B1，B2 不包含A
		DataT tB1Pos = GetLowClose(vStockData.begin(), ptrmAPos, false);
		if (tB1Pos.uDate == -1)
		{
			logstream2 <<" 当前数据没有B2 " <<endl;
			itMap++;
			continue;
		}
		logstream2 <<"B1 点 "<< tB1Pos.dColsePrice<<"  " <<tB1Pos.dHighPrice<<" | ";


		DataT tB2Pos = GetLowClose(ptrmAPos + 1, vStockData.end(), false);
		if (tB2Pos.uDate == -1)
		{
			logstream2 <<" 当前数据没有B2 " <<endl;
			itMap++;
			continue;
		}
		logstream2 <<"B2 点 "<< tB2Pos.dColsePrice<<" | " <<tB2Pos.dHighPrice<<" | ";

		//筛选2 
		//KB 包括B
		std::vector<DataT>::iterator ptrmB2Pos = find(vStockData.begin(), vStockData.end(), tB2Pos);
		int iDis = distance(ptrmB2Pos, vStockData.end());

		logstream2 <<"KB 点 "<< iDis <<" | ";
		if (iDis < tThdFilter.tLineNum.iMinka  || tThdFilter.tLineNum.iMaxka < iDis)
		{
			cout <<"不符合周期系数KB范围"<<endl;
			itMap++;
			continue;
		}

		//KA 包括A
		iDis = distance(ptrmAPos, ptrmB2Pos) + 1;
		logstream2 <<"KA 点 "<< iDis <<" | ";
		if (iDis < tThdFilter.tLineNum.iMinkb || tThdFilter.tLineNum.iMaxkb < iDis)
		{
			cout <<"不符合周期系数KA范围"<<endl;
			itMap++;
			continue;
		}

		//筛选5
		if (tThdFilter.bA5Switch)
		{
			double h1 = tB2Pos.dHighPrice - tAPos.dLowPrice;
			logstream2 <<"h1: " << h1 <<" | ";
			DataT tEndData;
			if (ptrmB2Pos + 1 != vStockData.end())
			{
				tEndData = GetLow(ptrmB2Pos, vStockData.end());
			}

			double h2 = tB2Pos.dHighPrice - tEndData.dLowPrice;
			logstream2 << " h2 "<< h2 <<" ";
			double ht = h2/h1;
			if ((h1 == 0) || (ht > static_cast<double>(tThdFilter.sCallbackRange/100)))
			{
				logstream2 << " ht:" << ht <<" | ";
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