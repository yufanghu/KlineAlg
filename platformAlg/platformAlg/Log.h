#ifndef _CMY_LOG_H
#define _CMY_LOG_H

/*************************************
创建日期	:2018-05-05
描述		: 日志实现文件
**************************************/

#include <string>
#include <stdio.h>
#include <fstream>
#include <stdarg.h>
#include <time.h>

using namespace std;

#define FILTERDOC1  _T("alg1")
#define FILTERDOC2  _T("alg2")
#define FILTERDOC3  _T("alg3")

enum e_doc_num
{
	e_doc_1,
	e_doc_2,
	e_doc_3
};

class CLog{
public:
	CLog();
	~CLog();
	void Init();
	void Flush(int nNum);
	void logRecord(/*string strFunc,int iLine, string strMod, */char* cFormat,...);
	void dataRecord(/*string strFunc,int iLine, string strMod, */char* cFormat,...);
	void clearLog();
private:
	std::string m_logBuffer ;
	std::string m_dataBuffer ;
};

#endif