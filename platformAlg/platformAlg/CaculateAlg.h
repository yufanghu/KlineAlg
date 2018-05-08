#pragma once
#include "PlatFormAlgorithm.h"
class CCaculateAlg 
{
public:
	CCaculateAlg();
	~CCaculateAlg();

	bool single_plat(const std::map<tagStockCodeInfo, std::vector<tagKline>> & input,
		std::map<tagStockCodeInfo, tagOutput> & output, EPlatFormType platformType, short avgFac, bool bFiring = false);

	bool multi_plat(const std::map<tagStockCodeInfo, std::vector<tagKline>> & input,
		std::map<tagStockCodeInfo, tagOutput> & output, EPlatFormType platformType, short avgFac, bool bFiring = false);

private:
	/*
	��ƽ̨�µ�һ��ɸѡ
	*/
	bool single_multi_step_one(const std::vector<tagKline>& kLineData, short avgFac,int& nPos, int& nKnb,
					int nMax, int nMin);
	bool single_multi_step_two(const std::vector<tagKline>& kLineData,  int& nPos);
	//isMulti�Ƿ�Ϊ˫ƽ̨��Ĭ��Ϊ��ƽ̨
	bool single_plat_step_third(const std::vector<tagKline>& kLineData, int& nPos);
	bool multi_step_third(const std::vector<tagKline>& kLineData, int& nPos);
	bool is_fairing(const std::vector<tagKline>& kLineData, int& nPos, bool isFiring);
	bool multi_step_fourth(const std::vector<tagKline>& kLineData, int& nPos);
	bool multi_step_fifth(const std::vector<tagKline>& kLineData, int& nPos);
	bool get_avg(const std::vector<tagKline>& kLineData, int nStart,
		int nCount, double& dAvg);
};

