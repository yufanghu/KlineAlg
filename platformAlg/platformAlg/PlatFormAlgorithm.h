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
		eFitler2First3,    //�ĵ�ɸѡ2 ����ɸѡ
	};



	struct TA2
	{
		short iMinka;  //��СKA
		short iMaxka;  //���KA
		short iMinkb;  //��СKB
		short iMaxkb;  //���KB

		TA2(){
			iMinka = 6;
			iMaxka = 9;
			iMinkb = 6;
			iMaxkb = 8;
		}
	};

	//һ��ɸѡ
	struct TFirstFilter
	{
		TA2   tLineNum;
		short  sA3Switch;       //A3�������   0 -> off  1 -> AA   2 -> AB
		bool  bA4Switch;        // A4���迪�� 
		bool  bA5Switch;        // A5���迪��
		short sCallbackRange;   //˫�׵�������  ������ʽ��1-99  
		bool  bA6Switch;        // A6���迪��	
		short sRbcoe;           //��k�߱���  ������ʽ��1-99
		TFirstFilter(){
			sA3Switch = 0;
			bA4Switch = false;
			bA5Switch = false;
			bA6Switch = false;
			sCallbackRange = 1;
			sRbcoe = 1;
		}
	};

	//����ɸѡ
	struct TSecondFilter
	{
		TA2   tLineNum;
		bool   bA5Switch;
		short sCallbackRange;//˫�׵�������  ������ʽ��1-99
	};

	//����ɸѡ
	struct TThirdFilter
	{
		TA2   tLineNum;
		bool   bA5Switch;
		short sCallbackRange;//˫�׵�������  ������ʽ��1-99
	};



	//��������Ϣ ����չ
	struct tagOutput {
		//std::vector<tagKline> klineVect;
		//������Ϣ
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



//ɾ�����𿪹أ�����һ��ɸѡ���͵���һ���ӿڣ����ڶ���ɸѡ���͵��ڶ����ӿڣ���������ɸѡ���͵��������ӿ�
////��һ��ɸѡ�ӿ�
//bool DoAlgorithm(std::map<tagStockCodeInfo, std::vector<tagKline>> &inMap,
//	std::map<tagStockCodeInfo, tagOutput> & output,
//	TFirstFilter&);  //
//
////�ڶ���ɸѡ�ӿ�
//bool DoAlgorithm(std::map<tagStockCodeInfo, std::vector<tagKline>> &inMap,
//	std::map<tagStockCodeInfo, tagOutput> & output,
//	TSecondFilter&);
//
////������ɸѡ�ӿ�
//bool DoAlgorithm(std::map<tagStockCodeInfo, std::vector<tagKline>> &inMap,
//	std::map<tagStockCodeInfo, tagOutput> & output,
//	TThirdFilter&);


template<typename T> 
bool bool DoAlgorithm(std::map<tagStockCodeInfo, std::vector<tagKline>> &inMap,
	std::map<tagStockCodeInfo, tagOutput> & output,
	T&, int );

#endif


