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

namespace alg{

	//k�߽ṹ��
	struct tagKline {
		__int64 time;	//ʱ���
		double open;	//���̼�
		double high;	//��߼�
		double low;		//��ͼ�
		double close;	//���̼�
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
		eDoublePlatForm		//˫ƽ̨
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
class CPlatFormAlgorithm
{
public:
	CPlatFormAlgorithm();
	virtual ~CPlatFormAlgorithm();

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

};

#endif