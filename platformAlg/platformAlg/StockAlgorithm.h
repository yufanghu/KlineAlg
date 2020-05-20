#ifndef _PLATFORM_SELECT_H
#define _PLATFORM_SELECT_H
/*************************************
����		:Jarvis
��������	:2018-04-29
�޸�����	:2018-05-01
����		: K��ɸѡ�㷨�ӿ��ļ�
**************************************/

#include <vector>
#include <map>
#include <string>

namespace alg{

#pragma region ��������
	//k�߽ṹ��
	struct tagKline {
		__int64 time;	//ʱ���
		double open;	//���̼�
		double high;	//��߼�
		double low;		//��ͼ�
		double close;	//���̼�
		bool operator == (const tagKline uDes){ return this->time == uDes.time; }
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

	//��������Ϣ ����չ
	struct tagOutput {
		//std::vector<tagKline> klineVect;
		//������Ϣ
	};



#pragma endregion ��������

#pragma region �㷨һ

	enum ESign
	{
		Normal = 1, NoPos, Abnormal
	};

	struct klinePoint
	{
		__int64 uDate;   //
		double   dPrice;  //
		ESign    eRet;    // ��ʾ�Ƿ���������ֵ   1 �������㷨���д˵����Nֵ�ļ��㣻ǰ�˻��˵㣬��������Nֵ��  3 �쳣 �㷨�����д˵����Nֵ�ļ��㣬N��ֵΪ0��ǰ�˲����˵� ��N��ֵ0
	};

	enum eTendKind { rise = 1, down, fault }; // //���Ʒ��ࣺrise--��������  down--�½�����(����Ľṹ����������Ӧ�½����Ƹ����㼰��ֵ)  fault--�����ƣ����е��N����ֵ

	//��ifRise=riseʱ������Ľṹ����������Ӧ�������Ƹ����㼰��ֵ
	struct outRiseKline
	{
		eTendKind tendency;
		klinePoint A;        //ifRise=rise����K�߽����ʾ�õ㣬���ڸõ��Ա�ע����A��
		klinePoint B;        //ifRise=rise����K�߽����ʾ�õ㣬���ڸõ��Ա�ע����B��
		klinePoint C;        //ifRise=rise����K�߽����ʾ�õ㣬���ڸõ��Ա�ע����C��
		klinePoint B1;       //ifRise=rise����K�߽����ʾ�õ㣬���ڸõ��Ա�ע����B1��
		klinePoint V1;       //ifRise=rise����K�߽����ʾ�õ㣬���ڸõ��Ա�ע����V1��
		klinePoint V2;       //ifRise=rise����K�߽����ʾ�õ㣬���ڸõ��Ա�ע����V2��
		klinePoint V3;       //ifRise=rise����K�߽����ʾ�õ㣬���ڸõ��Ա�ע����V3��
		klinePoint M1;      // ifRise=rise����K�߽����ʾ�õ㣬���ڸõ��Ա�ע����M1��

		int nV1;            //ifRise=rise����K�߽��浯����ʾ������   nV1=  nV2=   nV3=   nM1=  nN0=
		int nV2;
		int nV3;
		int nM1;
		int nN0;
	};

	//��ifRise=downʱ������Ľṹ����������Ӧ�½����Ƹ����㼰��ֵ
	struct outDownKline
	{
		eTendKind tendency;
		klinePoint A;        //ifRise=down����K�߽����ʾ�õ㣬���ڸõ��Ա�ע����A��
		klinePoint B;        //ifRise=down����K�߽����ʾ�õ㣬���ڸõ��Ա�ע����B��
		klinePoint C;        //ifRise=down����K�߽����ʾ�õ㣬���ڸõ��Ա�ע����C��
		klinePoint B1;       //ifRise=down����K�߽����ʾ�õ㣬���ڸõ��Ա�ע����B1��
		klinePoint M1;       //ifRise=down����K�߽����ʾ�õ㣬���ڸõ��Ա�ע����M1��
		klinePoint M2;       //ifRise=down����K�߽����ʾ�õ㣬���ڸõ��Ա�ע����M2��
		klinePoint M3;       //ifRise=down����K�߽����ʾ�õ㣬���ڸõ��Ա�ע����M3��
		klinePoint V1;      // ifRise=down����K�߽����ʾ�õ㣬���ڸõ��Ա�ע����V1��

		int nM1;            //ifRise=down����K�߽��浯����ʾ������   nM1=  nM2=   nM3=   nV1=  nN0=
		int nM2;
		int nM3;
		int nV1;
		int nN0;
	};

	//��ifRise=fault�������8�����ö��ֵΪ3�������5��Nֵ��Ϊ0

	struct outKline
	{
		eTendKind tendency;
		union
		{
			outRiseKline riseKline;
			outDownKline downKline;
			outDownKline noKline;
		};
	};

#pragma endregion �㷨һ

#pragma region �㷨��

	//ɸѡ��������
	enum EFilterType{
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

		TA2()
			:iMaxkb(0),
			iMaxka(0),
			iMinkb(0),
			iMinka(0)
		{

		};
	};

	struct Alg2Filter{
		Alg2Filter(){}
		virtual ~Alg2Filter(){}
	};
	//ɸѡ����
	struct TFirstFilter : public Alg2Filter
	{
		TA2		tLineNum;
		short	sA3Switch;        //A3�������   0 -> off  1 -> AA   2 -> AB
		bool	bA4Switch;        //A4���迪�� 
		bool	bA5Switch;        //A5���迪��
		bool	bA6Switch;        //A6���迪��	
		short	sCallbackRange;   //˫�׵�������  ������ʽ��1-99  
		short	sRbcoe;           //��k�߱���  ������ʽ��1-99
		short   sUpLimit;             //���Ƿ��ȣ�Ĭ��20���� 1-200
		short   sDownLimit;           //�µ����ȣ�Ĭ��200����1-200
		TFirstFilter()
			:sA3Switch(0),
			bA4Switch(false),
			bA5Switch(false),
			sCallbackRange(1),
			bA6Switch(false),
			sRbcoe(1),
			sUpLimit(20),
			sDownLimit(200)
		{
		};
	};

	//����ɸѡ
	struct TSecondFilter : public Alg2Filter
	{
		TA2    tLineNum;
		bool   bA5Switch;
		short  sCallbackRange;//˫�׵�������  ������ʽ��1-99
		TSecondFilter()
			:bA5Switch(false),
			sCallbackRange(1)
		{
		};
	};


	//����ɸѡ
	struct TThirdFilter : public Alg2Filter
	{
		TA2   tLineNum;
		bool   bA5Switch;
		short sCallbackRange;//˫�׵�������  ������ʽ��1-99
		TThirdFilter()
			:bA5Switch(false),
			sCallbackRange(1)
		{
		};
	};


#pragma endregion �㷨��

#pragma region �㷨��
	//ƽ̨ö��
	enum EPlatFormType{
		eSinglePlatForm,    //��ƽ̨
		eDoublePlatForm,    //˫ƽ̨
	};

#pragma endregion �㷨��


#pragma endregion �㷨��
	enum EPlatLevel{
		eLevelOne,   //һ��ƽ̨
		eLevelTwo,   //����ƽ̨
		eLevelThree  //����ƽ̨
	};

	enum EStep{
		eStepA,
		eStepAB,
	};


	enum EQuotaInfoType{
		eType1, //ͻ�ƺ����̼�һֱ��60����֮��
		eType2  //����60����
	};

	enum EQuotaPeriodType{
		ePeriodA,       //����ֵA
		ePeriodB		//����ֵB
	};


	struct tagQuotaA{
		__int64 h1Time;		//h1ʱ���
		__int64 h2Time;		//h2ʱ���
		__int64 caTime;		//caʱ���
		EPlatLevel level;	//ƽ̨����
	};

	//��������
	struct tagQuotaPeriod{
		EQuotaPeriodType type; //������������
		int periodValue;       //���ݾ�������ֵ
	};

	//ƽ̨B�Ƿ����60����
	struct tagQuotaInfo{
		EQuotaInfoType type;
		__int64 timeInfo;       //��type=eType1ʱΪ��Ч����<����60���ߵ�k��ʱ���>
	};

	struct tagQuotaB{
		__int64 h1Time;			//h1ʱ���  
		__int64 h2Time;			//h2ʱ���
		EPlatLevel level;		//ƽ̨����
		bool bIsBreak;			//���һ���Ƿ�ͻ�� true��ʾh2�Ѿ�ͻ�ƣ��ѳ���;false��ʾδͻ�ƣ��γ���
		tagQuotaInfo info;		//��bIsBreakΪtrueʱ Ϊ��Ч����
		tagQuotaPeriod period;	//��bIsBreakΪtrueʱ Ϊ��Ч����
	};

	struct tagQuota{
		EStep step;          //�������  eStepA: ֻ��quotaA����Ч���ݣ�ֻ��A����eStepB: quotaA��quotaB��Ϊ��Ч���ݣ�����A��B�� 
		tagQuotaA quotaA;
		tagQuotaB quotaB;
	};

#pragma region �㷨��
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
bool alg_platform(const std::map<tagStockCodeInfo, std::vector<tagKline>> & input,
	std::map<tagStockCodeInfo, tagOutput> & output, EPlatFormType platformType, short avgFac, bool bFiring = false);

/************************************************************************/
/*
�㷨2
@param inMap  �������  key-��Ʊ��Ϣ�� value-k�߼���
@param output �������  key-��Ʊ��Ϣ�� value-
@param filter �������  TFilter TFirstFilterһ��ɸѡ TSecondFilter����ɸѡ TThirdFilter����ɸѡ
*/
/************************************************************************/
bool  alg_stock2(const std::map<tagStockCodeInfo, std::vector<tagKline>> &inMap,
	std::map<tagStockCodeInfo, tagOutput> & output, TFirstFilter& tFirFilter);


bool  alg_stock2(const std::map<tagStockCodeInfo, std::vector<tagKline>> &inMap,
	std::map<tagStockCodeInfo, tagOutput> & output, TSecondFilter& filter);


bool  alg_stock2(const std::map<tagStockCodeInfo, std::vector<tagKline>> &inMap,
	std::map<tagStockCodeInfo, tagOutput> & output, TThirdFilter& filter);

/************************************************************************/
/*
�㷨1 ���
@param inMap  �������  key-��Ʊ��Ϣ�� value-k�߼���
@param output �������  key-��Ʊ��Ϣ�� value-
*/
/************************************************************************/
bool alg_stock1(const std::map<tagStockCodeInfo, std::vector<tagKline>>& inMap, std::map<tagStockCodeInfo, outKline> & outMap);


/************************************************************************/
/*
�㷨����� 20200401
@param inStockKlines  ������� ĳֻ��Ʊ��k�߼���
@param quota ������� �����ָ��������
*/
/************************************************************************/
void alg_stock4(const std::vector<tagKline>& inStockKlines, std::vector<tagQuota> & quota);


/************************************************************************/
/*
�㷨4��չ���   ����A��B����Bͻ�ƺ�һֱ��60�����ϵĹ�Ʊ
@param inMap  �������   key-��Ʊ��Ϣ�� value-k�߼���
@param outMap �������   key-��Ʊ��Ϣ�� value-���
*/
/************************************************************************/
void alg_stock4_multi(const std::map<tagStockCodeInfo, std::vector<tagKline>>& inMap, std::map<tagStockCodeInfo, tagOutput> & outMap);

/************************************************************************/
/*
�ر�/������־
@param bEnable  �������  true-������־�� false-�ر���־
*/
/************************************************************************/

void EnableAlgLog(bool bEnable = true);

#endif


