#pragma once
#ifndef ALG_BASE
#define	ALG_BASE
#endif // !ALG_BASE

class CAlgBase
{
public:
	CAlgBase();
	virtual ~CAlgBase();
	virtual bool createNewAlg() = 0;
};

class CAlg3 : public CAlgBase
{
public:
	CAlg3(std::map<tagStockCodeInfo, std::vector<tagKline>> &inMap,
		std::map<tagStockCodeInfo, tagOutput> & output, TFirstFilter& tFirFilter);
	CAlg3(std::map<tagStockCodeInfo, std::vector<tagKline>> &inMap,
		std::map<tagStockCodeInfo, tagOutput> & output, TSecondFilter& tFirFilter);
	CAlg3(std::map<tagStockCodeInfo, std::vector<tagKline>> &inMap,
		std::map<tagStockCodeInfo, tagOutput> & output, TThirdFilter& tFirFilter);
	~CAlg3();

	virtual bool createNewAlg() override;

private:

	enum EAlgStep{
		eStep1,
		eStep2,
		eStep3
	};

	std::map<tagStockCodeInfo, std::vector<tagKline>>* m_inMap;
	std::map<tagStockCodeInfo, tagOutput> * m_output;

	TFirstFilter* m_tFirFilter;

	TSecondFilter* m_tSecFilter;

	TThirdFilter* m_tTirFilter;

	EAlgStep m_step;
	
};

