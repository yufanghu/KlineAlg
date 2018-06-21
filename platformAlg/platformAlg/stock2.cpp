// stock.cpp ��
//
#include "stdafx.h"
#include "stock2.h"


using namespace std;
using namespace stock_second;

//������־
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
	return a.dColsePrice < b.dColsePrice;  //����ȡ�Ⱥ� ����
}

//���� ������̼�
DataT GetLowClose(std::vector<DataT>::iterator itvDataBegin, 
	               std::vector<DataT>::iterator itvDataEnd, bool bLow)
{
	DataT tAPos;
	//todo tApos ��ӹ��캯����������ʼ��Ϊ0

	if (itvDataBegin == itvDataEnd)
	{
		cout << "GetLowClose start == end " <<endl;
		tAPos.uDate = -1;
		return tAPos;
	}

	std::vector<DataT> vData(itvDataBegin, itvDataEnd);

	//1. ���� ��ͼ� ����������Ȼ��ȡ��һ��Ԫ��
	//todo: ����۸���ͬ��A�Ͱ�����ͼ����� B1�� B2 ��߼�����
	sort(vData.begin(), vData.end(), CmpLowClosePri);

	if (bLow)
	{
		tAPos = vData[0];
        //��͵���ͼ�
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
        //��ߵ���߼�
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

//���� ��ͼ�
DataT GetLow(std::vector<DataT>::iterator itvDataBegin,
              std::vector<DataT>::iterator itvDataEnd)
{
    DataT tAPos;
    //todo tApos ��ӹ��캯����������ʼ��Ϊ0

    if (itvDataBegin == itvDataEnd)
    {
        cout << "GetLow start == end " << endl;
		tAPos.uDate = -1;
        return tAPos;
    }

    std::vector<DataT> vData(itvDataBegin, itvDataEnd);

    //1. ���� ��ͼ� ����������Ȼ��ȡ��һ��Ԫ��
    //todo: ����۸���ͬ��A�Ͱ�����ͼ����� B1�� B2 ��߼�����
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


//��ȡ���߸���
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

	//ѭ����ӡ����Ԫ��
	std::map<tBaseInfo, std::vector<DataT>>::iterator itMap = inMap.begin();
	while (itMap != inMap.end())
	{
		//��ӡkey
		logstream2 << itMap->first.sSetcode;
		logstream2 << itMap->first.sTockcode;

		std::vector<DataT>&vStockData = itMap->second;
		std::vector<DataT>::iterator itVet = vStockData.begin();
		while (itVet != vStockData.end())
		{
			logstream2 << "���ڣ������ߣ��ͣ��� " <<itVet->uDate << " | "<< itVet->dOpenPrice 
				<< " | " << itVet->dHighPrice << " | " << itVet->dLowPrice
				<< " | "<< itVet->dColsePrice;

			itVet++;
			logstream2 <<endl;
		}
		logstream2 << "-------" <<endl;
		itMap++;
	}
}


//��ӡ��ʾɸѡ�����Ĺ�Ʊ
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


//һ��ɸѡ
std::map<tBaseInfo, tOutData> CStock::DoAlgorithm(std::map<tBaseInfo, std::vector<DataT>> &inMap,
	                                              TFirstFilter& tFirFilter)
{
	std::map<tBaseInfo, tOutData>  outMap;

	//������־
	logstream2.open("./stock.log", ofstream::app);
	logforpara2(inMap);

	logstream2 << "һ��ɸѡ����" << endl;
	logstream2 << "���ka | ��Сka | ���kb | ��Сkb�� " << tFirFilter.tLineNum.iMaxka <<" "<< tFirFilter.tLineNum.iMinka 
		       <<" " <<tFirFilter.tLineNum.iMaxkb << " " << tFirFilter.tLineNum.iMinkb << endl;
	logstream2 <<"����3|4|5|6 | �������� | ���߱���" <<tFirFilter.sA3Switch <<" "<< tFirFilter.bA4Switch <<" "<< tFirFilter.bA5Switch 
		       << " " << tFirFilter.bA6Switch << " " << tFirFilter.sCallbackRange<< " " << tFirFilter.sRbcoe << endl;

	//ѭ������ÿֻ��Ʊ
	std::map<tBaseInfo, std::vector<DataT>>::iterator itMap = inMap.begin();
	while (itMap != inMap.end())
	{
		logstream2 << "�г��� " <<itMap->first.sSetcode << " ���� " << itMap->first.sTockcode << endl;
		std::vector<DataT>& vStockData = itMap->second;

		//todo: k��С�ڵ���2��������
		if (vStockData.size() <= 2)
		{
			logstream2 << "k��С��2 ";
			cout<< "k��С��2 " <<endl;
			itMap++;
			continue;
		}
		//todo: A B1 B2 û��ֵ���ͽ�����һֻ��Ʊ����

		//ɸѡ��һ�׶�
		//1.ɸѡ1  ���̼���ͼ�
		DataT tAPos = GetLowClose(vStockData.begin(), vStockData.end(), true);
		if (tAPos.uDate == -1)
		{
			logstream2 << " ��ǰ����û��A " << endl;
			itMap++;
			continue;
		}
		logstream2 <<"A �� "<< tAPos.dColsePrice <<"  " <<tAPos.dLowPrice<< " |  ";
		
		//��ȡA �������
		std::vector<DataT>::iterator ptrmAPos = find(vStockData.begin(), vStockData.end(), tAPos);
		if (ptrmAPos == vStockData.end())
		{
			logstream2 <<"��A ��"<<endl;
			cout <<"not find Apos"<<endl;
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
		logstream2 <<"B1 �� "<< tB1Pos.dColsePrice<<"  " <<tB1Pos.dHighPrice<<" | ";

		DataT tB2Pos = GetLowClose(ptrmAPos + 1, vStockData.end(), false);
		if (tB2Pos.uDate == -1)
		{
			logstream2 <<" ��ǰ����û��B2 " <<endl;
			itMap++;
			continue;
		}
		logstream2 <<"B2 �� "<< tB2Pos.dColsePrice<<" | " <<tB2Pos.dHighPrice<<" | ";

		//ɸѡ2 
		//KB ����B
		std::vector<DataT>::iterator ptrmB2Pos = find(vStockData.begin(), vStockData.end(), tB2Pos);
		int iDis = distance(ptrmB2Pos, vStockData.end());

		logstream2 <<"KB �� "<< iDis <<" | ";
		if (iDis < tFirFilter.tLineNum.iMinka  || tFirFilter.tLineNum.iMaxka < iDis)
		{
			cout <<"����������ϵ��KB��Χ"<<endl;
			itMap++;
			continue;
		}

		//KA ����A(�Ƿ����B2 ������)
		iDis = distance(ptrmAPos, ptrmB2Pos) + 1;
		logstream2 <<"KA �� "<< iDis <<" | ";
		if (iDis < tFirFilter.tLineNum.iMinkb || tFirFilter.tLineNum.iMaxkb < iDis)
		{
			cout <<"����������ϵ��KA��Χ"<<endl;
	        itMap++;
			continue;
		}

		//ɸѡ3   0 ��ʾ�����ͣ��ж���AA,AB,AAP �� 1, 2, 3  
		int iDoubleType = 0; 
		if (tFirFilter.sA3Switch == 1 || tFirFilter.sA3Switch == 2)
		{
			if (tB2Pos.dColsePrice >= tB1Pos.dColsePrice)
			{
				iDoubleType = 1;
				logstream2 <<" AA�� | ";
			}
			else if (tB2Pos.dColsePrice < tB1Pos.dColsePrice)
			{
				iDoubleType = 2;
				logstream2 <<" AB�� | ";
			}

			if (tFirFilter.sA3Switch != iDoubleType)
			{
				//��ǰ�������ͺ��û�Ҫѡ��ģ���һ�£�������������
			    logstream2<<"��ǰ���ͣ�"<< iDoubleType << " �Ϳ������Ͳ�ͬ"<<endl;
				itMap++;
				continue;
			}
		}

		//ɸѡ4 : ������AA�ͣ���ֻ��AA�͵�����  
		if (tFirFilter.bA4Switch && iDoubleType == 1)
		{
			//ɸѡ��B2���ݺ�������̼۸�L2���ݣ�������B2���ݣ�
			DataT tL2Pos = GetLowClose((ptrmB2Pos+1), vStockData.end(), true);

			//todo: û��L2
			if (-1 == tL2Pos.uDate)
			{
				logstream2 <<" û��L2 "<<endl;
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

		//ɸѡ5
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
				logstream2 <<"B2����ĩβ����";
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

		//ɸѡ6
		if (tFirFilter.bA6Switch)
		{
			//ͳ��A���ݣ����������ݣ�������ĩ�ڣ���K����Ŀ
			int iSunlineNum    = 0;
			int iDisFromAToEnd = 0;
			float rb           = 0.0;

			iSunlineNum    = GetSunLineNum(ptrmAPos, vStockData.end());
			iDisFromAToEnd = distance(ptrmAPos, vStockData.end());
			rb             = (float)iSunlineNum / iDisFromAToEnd;
			logstream2<< " ������: " << iSunlineNum << " | " << "A���ݵ�����ĩβK�߸���" << iDisFromAToEnd <<" ";
			//todo: ɸѡ���� ���ڵ�������
			if (rb < static_cast<double>(tFirFilter.sRbcoe/100))
			{
				logstream2 <<" rb < ����  rb "<< rb <<" | " <<endl;
				itMap++;
				continue;
			}
		}

		//����һ��ɸѡ���
		tOutData tEmpty;
		outMap.insert(make_pair(itMap->first, tEmpty));

		itMap++;
	}

	//��ӡѡ�еĹ�Ʊ
	PrintHitStock(outMap, "һ��ɸѡѡ�й�Ʊ�б�");

	logstream2.close();
	return outMap;
}


//����ɸѡ
std::map<tBaseInfo, tOutData> CStock::DoAlgorithm(std::map<tBaseInfo, std::vector<DATAPP>> &inMap, 
	                                              TSecondFilter& tSedFilter)
{
	std::map<tBaseInfo, tOutData>  outMap;

	//������־
	logstream2.open("./stock.log", ofstream::app);
	logforpara2(inMap);

	logstream2 << "����ɸѡ����" << endl;
	logstream2 << "���ka | ��Сka | ���kb | ��Сkb�� "<< tSedFilter.tLineNum.iMaxka <<" "<< tSedFilter.tLineNum.iMinka
		       << " " << tSedFilter.tLineNum.iMaxkb << " " << tSedFilter.tLineNum.iMinkb << endl;
	logstream2 <<"����5�͵������ȣ�" <<tSedFilter.bA5Switch << " " << tSedFilter.sCallbackRange << endl;

	//ѭ������ÿֻ��Ʊ
	std::map<tBaseInfo, std::vector<DataT>>::iterator itMap = inMap.begin();
	while (itMap != inMap.end())
	{
		logstream2 << "�г��� " <<itMap->first.sSetcode << " ���� " << itMap->first.sTockcode << endl;
		std::vector<DataT>& vStockData = itMap->second;

		//todo: k��С�ڵ���2��������
		if (vStockData.size() <= 2)
		{
			logstream2 << "k��С��2 ";
			cout<< "k��С��2 " <<endl;
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
		logstream2 <<"A �� "<< tAPos.dColsePrice <<"  " <<tAPos.dLowPrice<< " |  ";
		std::vector<DataT>::iterator ptrmAPos = find(vStockData.begin(), vStockData.end(), tAPos);
		if (ptrmAPos == vStockData.end())
		{
			logstream2 <<"��A ��"<<endl;
			cout <<"not find Apos"<<endl;
			itMap++;
			continue;
		}

		//todo:B1��B2 ������A
		DataT tB1Pos = GetLowClose(vStockData.begin(), ptrmAPos, false);
		if (tB1Pos.uDate == -1)
		{
			logstream2 <<" ��ǰ����û��B1 " <<endl;
			itMap++;
			continue;
		}
		logstream2 <<"B1 �� "<< tB1Pos.dColsePrice<<"  " <<tB1Pos.dHighPrice<<" | ";



		DataT tB2Pos = GetLowClose(ptrmAPos + 1, vStockData.end(), false);
		if (tB2Pos.uDate == -1)
		{
			logstream2 <<" ��ǰ����û��B2 " <<endl;
			itMap++;
			continue;
		}
		logstream2 <<"B2 �� "<< tB2Pos.dColsePrice<<" | " <<tB2Pos.dHighPrice<<" | ";

		//ɸѡ2 
		//KB ����B
		std::vector<DataT>::iterator ptrmB2Pos = find(vStockData.begin(), vStockData.end(), tB2Pos);
		int iDis = distance(ptrmB2Pos, vStockData.end());

		logstream2 <<"KB �� "<< iDis <<" | ";
		if (iDis < tSedFilter.tLineNum.iMinka  || tSedFilter.tLineNum.iMaxka < iDis)
		{
			cout <<"����������ϵ��KB��Χ"<<endl;
			itMap++;
			continue;
		}

		//KA ����A
		iDis = distance(ptrmAPos, ptrmB2Pos) + 1;
		logstream2 <<"KA �� "<< iDis <<" | ";
		if (iDis < tSedFilter.tLineNum.iMinkb || tSedFilter.tLineNum.iMaxkb < iDis)
		{
			cout <<"����������ϵ��KA��Χ"<<endl;
			itMap++;
			continue;
		}

		//ɸѡ5
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

		//���ض���ɸѡ���
		tOutData tEmpty;
		outMap.insert(make_pair(itMap->first, tEmpty));

		itMap++;
	}

	PrintHitStock(outMap, "����ɸѡѡ�й�Ʊ�б�");

	logstream2.close();
	return outMap;
}

//����ɸѡ
std::map<tBaseInfo, tOutData> CStock::DoAlgorithm(std::map<tBaseInfo,std::vector<DATAPP>> &inMap, 
	                                      TThirdFilter& tThdFilter)
{
	std::map<tBaseInfo, tOutData>  outMap;

	//������־
	logstream2.open("./stock.log", ofstream::app);
	logforpara2(inMap);
	logstream2 << "����ɸѡ����" << endl;
	logstream2 <<"���ka | ��Сka | ���kb | ��Сkb�� "<< tThdFilter.tLineNum.iMaxka <<" " << tThdFilter.tLineNum.iMinka << "  "
		       << tThdFilter.tLineNum.iMaxkb <<" " << tThdFilter.tLineNum.iMinkb << endl;
	logstream2 << "����5 | ��������: " <<tThdFilter.bA5Switch << " "<< static_cast<double>(tThdFilter.sCallbackRange/100) << endl;

	//ѭ������ÿֻ��Ʊ
	std::map<tBaseInfo, std::vector<DataT>>::iterator itMap = inMap.begin();
	while (itMap != inMap.end())
	{
		logstream2 << "�г��� " <<itMap->first.sSetcode << " ���� " << itMap->first.sTockcode << endl;
		std::vector<DataT>& vStockData = itMap->second;

		//todo: k��С�ڵ���2��������
		if (vStockData.size() <= 2)
		{
			logstream2 << "k��С��2 ";
			cout<< "k��С��2 " <<endl;
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
		logstream2 <<"A �� "<< tAPos.dColsePrice <<"  " <<tAPos.dLowPrice<< " |  ";
		std::vector<DataT>::iterator ptrmAPos = find(vStockData.begin(), vStockData.end(), tAPos);
		if (ptrmAPos == vStockData.end())
		{
			logstream2 <<"��A ��"<<endl;
			cout <<"not find Apos"<<endl;
			itMap++;
			continue;
		}

		//todo:B1��B2 ������A
		DataT tB1Pos = GetLowClose(vStockData.begin(), ptrmAPos, false);
		if (tB1Pos.uDate == -1)
		{
			logstream2 <<" ��ǰ����û��B2 " <<endl;
			itMap++;
			continue;
		}
		logstream2 <<"B1 �� "<< tB1Pos.dColsePrice<<"  " <<tB1Pos.dHighPrice<<" | ";


		DataT tB2Pos = GetLowClose(ptrmAPos + 1, vStockData.end(), false);
		if (tB2Pos.uDate == -1)
		{
			logstream2 <<" ��ǰ����û��B2 " <<endl;
			itMap++;
			continue;
		}
		logstream2 <<"B2 �� "<< tB2Pos.dColsePrice<<" | " <<tB2Pos.dHighPrice<<" | ";

		//ɸѡ2 
		//KB ����B
		std::vector<DataT>::iterator ptrmB2Pos = find(vStockData.begin(), vStockData.end(), tB2Pos);
		int iDis = distance(ptrmB2Pos, vStockData.end());

		logstream2 <<"KB �� "<< iDis <<" | ";
		if (iDis < tThdFilter.tLineNum.iMinka  || tThdFilter.tLineNum.iMaxka < iDis)
		{
			cout <<"����������ϵ��KB��Χ"<<endl;
			itMap++;
			continue;
		}

		//KA ����A
		iDis = distance(ptrmAPos, ptrmB2Pos) + 1;
		logstream2 <<"KA �� "<< iDis <<" | ";
		if (iDis < tThdFilter.tLineNum.iMinkb || tThdFilter.tLineNum.iMaxkb < iDis)
		{
			cout <<"����������ϵ��KA��Χ"<<endl;
			itMap++;
			continue;
		}

		//ɸѡ5
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

		//��������ɸѡ���
		tOutData tEmpty;
		outMap.insert(make_pair(itMap->first, tEmpty));

		itMap++;
	}

	PrintHitStock(outMap, "����ɸѡѡ�й�Ʊ�б�");

	logstream2.close();
	return outMap;
}