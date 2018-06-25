#ifndef _PLATFORM_SELECT_H
#define _PLATFORM_SELECT_H
/*************************************
����		:Jarvis
��������	:2018-04-29
�޸�����	:2018-05-01
����		: K�ߡ���/˫��ƽ̨�㷨
**************************************/

#include <vector>
#include <map>
#include <string>
#include "Log.h"


#define PERIODKAMIN     6
#define PERIODKAMINB1   2
#define PERIODKAMINC2   2
#define PERIODKAMAX     9
#define PERIODKAMAXB1   8
#define PERIODKAMAXC1   8
#define PERIODKBMIN     6
#define PERIODKBMINB1   5
#define PERIODKBMINC1   5
#define PERIODKBMAX     8
#define PERIODKBMAXB1   8
#define PERIODKBMAXC1   8


namespace alg{

	//k�߽ṹ��
	struct tagKline {
		__int64 time;	//ʱ���
		double open;	//���̼�
		double high;	//��߼�
		double low;		//��ͼ�
		double close;	//���̼�
		bool operator == (const tagKline uDes){ return this->time == uDes.time; }
	};


	//k������  --��ʱû���õ�
	enum EKlinePeriod {
		eInvalidPeriod = -1, //��Ч���� ռλ
		ePeriod5Min = 1,
		ePeriod15Min = 2,
		ePeriod30Min = 3,
		ePeriod60Min = 4,
		ePeriodDay = 5,
		ePeriodWeek = 6,
		ePeriodMouth = 7,
		ePeriodYear = 8,
		ePeriod1Min = 35   // 1������
	};

	//ƽ̨ö��
	enum EPlatFormType{
		eSinglePlatForm,   //��ƽ̨
		eDoublePlatForm,	//˫ƽ̨
		eFitler2First1,		//�ĵ�ɸѡ2 һ��ɸѡ
		eFitler2First2,    //�ĵ�ɹѡ2 ����ɸѡ
		eFitler2First3    //�ĵ�ɸѡ2 ����ɸѡ
	};

	//ɸѡ��������
	enum eFilterType{
		eOff,  //�ر�
		eAA,
		eAB,
		eAAP
	};


	struct TA2
	{
		short iMinka;  //��СKA
		short iMaxka;  //���KA
		short iMinkb;  //��СKB
		short iMaxkb;  //���KB

		TA2(){
			iMinka = 0;
			iMaxka = 0;
			iMinkb = 0;
			iMaxkb = 0;
		};
	};

	//һ��ɸѡ
	struct TFilter
	{
		TA2   tLineNum;
		short  sA3Switch;       //A3�������   0 -> off  1 -> AA   2 -> AB
		bool  bA4Switch;        // A4���迪�� 
		bool  bA5Switch;        // A5���迪��
		bool  bA6Switch;        // A6���迪��	
		short sCallbackRange;   //˫�׵�������  ������ʽ��1-99  
		short sRbcoe;           //��k�߱���  ������ʽ��1-99
		TFilter(){
			bA4Switch = false;
			bA5Switch = false;
			bA6Switch = false;
			sCallbackRange = 1;
			sRbcoe = 1;
			sA3Switch = 0;
		};
		
	};





	//��������Ϣ ����չ
	struct tagOutput {
		//std::vector<tagKline> klineVect;
		//������Ϣ
		int eType;
		tagOutput(){
			eType = eOff;
		};
	};

	//��Ʊ������Ϣ
	struct tagStockCodeInfo {
		short market;		//������|�г�
		std::string stockcode;	//��Լ����

		tagStockCodeInfo(short  market, const std::string & code) :
			market(market), stockcode(code) {}

		bool operator<(const tagStockCodeInfo  & another)const {
			if (this->market == another.market) {
				return this->stockcode < another.stockcode;
			}
			else
				return this->market < another.market;
		}

		bool operator==(const tagStockCodeInfo  & another)const {
			if (this->market == another.market && this->stockcode == another.stockcode)
				return true;
			else
				return false;
		}

	};
}
using namespace alg;

/**
*  ��������: �㷨������ýӿ�
*  @param input	 �������  key-��Ʊ��Ϣ�� value-k�߼���
*  @param output �������  key-��Ʊ��Ϣ�� value-��ʱ������
*  @param EPlatFormType ƽ̨����
*  @param avgFac ���߲���
*  @param bFiring �Ƿ���
*  @���� true-�ɹ� false-ʧ��
*/
//ƽ̨ɸѡ�ӿ�
bool select_entrance(const std::map<tagStockCodeInfo, std::vector<tagKline>> & input,
	std::map<tagStockCodeInfo, tagOutput> & output, EPlatFormType platformType, short avgFac, bool bFiring =false);


/************************************************************************/
/* 
�㷨2
@param inMap  �������  key-��Ʊ��Ϣ�� value-k�߼���
@param output �������  key-��Ʊ��Ϣ�� value-�洢��Ʊ���� off   AA AB  AAP
@param filter �������  TFilter �û�ѡ��Ĭ�Ͼ��ر�
@param eType  �������	EPlatFormType ����ɸѡ����ƽ̨
*/
/************************************************************************/
bool  DoAlgorithm(std::map<tagStockCodeInfo, std::vector<tagKline>> &inMap,
	std::map<tagStockCodeInfo, tagOutput> & output,
	TFilter& filter, int eType = eFitler2First1);

#endif


