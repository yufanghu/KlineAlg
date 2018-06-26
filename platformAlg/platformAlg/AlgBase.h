#pragma once
#ifndef ALG_BASE
#define	ALG_BASE
#endif // !ALG_BASE

class CAlgBase
{
public:
	CAlgBase();
	virtual ~CAlgBase();
	virtual bool createNewAlg( ) = 0;
};

class CAlg1
{
public:
	CAlg1();
	~CAlg1();
	virtual bool createNewAlg();
};

