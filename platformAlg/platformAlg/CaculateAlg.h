#ifndef _CACULATE_ALG_H
#define _CACULATE_ALG_H
/*************************************
��������	:2018-05-05
����		: K��ƽ̨�㷨ʵ����
**************************************/
#include "PlatFormAlgorithm.h"

#define  DEADLINE_DATE "20181231"

class CCaculateAlg 
{
public:
	CCaculateAlg();
	~CCaculateAlg();
	CLog m_cLog;

	//��ƽ̨���
	bool single_plat(const std::map<tagStockCodeInfo, std::vector<tagKline>> & input,
		std::map<tagStockCodeInfo, tagOutput> & output, short avgFac, bool bFiring = false);

	//˫ƽ̨���
	bool double_plat(const std::map<tagStockCodeInfo, std::vector<tagKline>> & input,
		std::map<tagStockCodeInfo, tagOutput> & output, short avgFac, bool bFiring = false);

	void SetLogObj(CLog * pLog){m_pLog = pLog;}
private:

	/**
	*  ��������: ƽ̨ɸѡ��һ��������˫ƽ̨���ã�
	*  @param nMax	 �������   ��������
	*  @param nMin �������     ��С������
	*/
	bool single_double_step_one(const std::vector<tagKline>& kLineData, short avgFac, int& nPos, int& nKnb,
					int nMax, int nMin);
	/**
	*  ��������: ƽ̨ɸѡ�ڶ���������˫ƽ̨���ã�
	*/
	bool single_double_step_two(const std::vector<tagKline>& kLineData,  int& nPos);

	/**
	*  ��������: ��ƽ̨������
	*/
	bool single_plat_step_third(const std::vector<tagKline>& kLineData, int& nPos);

	/**
	*  ��������: ˫ƽ̨�����������Ĳ������岽
	*/
	bool double_step_third(const std::vector<tagKline>& kLineData, int& nPos);
	bool double_step_fourth(const std::vector<tagKline>& kLineData, int& nPos);
	bool double_step_fifth(const std::vector<tagKline>& kLineData, const int& nPos);

	/**
	*  ��������: �ж��Ƿ���
	*/
	bool is_fairing(const std::vector<tagKline>& kLineData, int& nPos, bool isFiring);
	
	/**
	*  ��������: ������ߵĲ���
	*/
	bool get_avg(const std::vector<tagKline>& kLineData, int nStart,
		int nCount, double& dAvg);

	void PrintData(const std::vector<tagKline>& kLineData);
	void PrintHead(const tagStockCodeInfo & codeInfo, int nPos = -1);

	CLog * m_pLog;
};

#endif

