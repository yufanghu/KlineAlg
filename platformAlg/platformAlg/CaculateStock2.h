#ifndef FILTER2_H
#define FILTER2_H


#include "StockAlgorithm.h"
#include "Log.h"
class CCaculateSotck2
{
public:
	CCaculateSotck2();
	~CCaculateSotck2();

	bool filter2Level1(const std::map<tagStockCodeInfo, std::vector<tagKline>> &inMap,
		std::map<tagStockCodeInfo, tagOutput> & output,
		TFirstFilter& tFirFilter);

	bool filter2Level2(const std::map<tagStockCodeInfo, std::vector<tagKline>> &inMap,
		std::map<tagStockCodeInfo, tagOutput> & output,
		TSecondFilter& tSedFilter);

	bool filter2Level3(const std::map<tagStockCodeInfo, std::vector<tagKline>> &inMap,
		std::map<tagStockCodeInfo, tagOutput> & output,
		TThirdFilter& tThdFilter);

	void SetLogObj(CLog * pLog){ m_pLog = pLog; }

private:
	//��ȡ���߸���
	int GetSunLineNum(std::vector<tagKline>::const_iterator itvDataBegin,
		 int end);
	bool GetLowOrHighClose(const std::vector<tagKline>& itvDataBegin, int start, int end,
		tagKline& tagResult, int& nPos, bool bLow = false);
	bool GetLow(std::vector<tagKline>::const_iterator itvDataBegin,
		std::vector<tagKline>::const_iterator itvDataEnd, tagKline& tResult);

	/*
	ɸѡ��A1��
	a��������Χ�����̼��������A
	b������A��������ǰ���̼����B1����
	c.����A�������ں����̼����B2����
	*/
	bool filterStepA1(const std::vector<tagKline>& vecKline, tagKline& tLowA, int& nApos,
		tagKline& tHighB1, int& nB1Pos,
		tagKline& tHighB2, int& nB2Pos);

	bool filterStepA2(const std::vector<tagKline>& vecKline, TA2 period, int nAPos, int nB2Pos );
	bool filterStepA3(const std::vector<tagKline>& vecKline, TFirstFilter& tFirFilter, int nB2Pos, int nB1Pos);
	bool filterStepA4(const std::vector<tagKline>& vecKline, TFirstFilter& tFirFilter, int nB1Pos, int nB2Pos, int & nL2Pos);
	bool filterStepA5(const std::vector<tagKline>& vecKline, TFirstFilter& tFirFilter, int nAPos, int nB2Pos);
	bool filterStepA6(const std::vector<tagKline>& vecKline, TFirstFilter& tFirFilter, int nAPos);
	bool filterStepA6Advanced(const std::vector<tagKline>& vecKline, TFirstFilter& tFirFilter, int nAPos, int nB2Pos);

	void PrintData(const std::vector<tagKline>& kLineData);
	void PrintHead(const tagStockCodeInfo & codeInfo, int nPos);

	void PrintInputParam(Alg2Filter * filter, int type);



private:
	CLog * m_pLog;

};


#endif