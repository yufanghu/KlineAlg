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

#define LOG(cFormat, ...) \
    CLog::logRecord(/*__FUNCTION__, __LINE__, __FILE__,  */cFormat, __VA_ARGS__);
#define LOG_DATA(cFormat, ...) \
    CLog::dataRecord(/*__FUNCTION__, __LINE__, __FILE__,  */cFormat, __VA_ARGS__);
#define LOG_FLUSH() \
	CLog::Flush()
#define LOG_INIT() \
	CLog::Init()

extern char* stamp_to_standard_ex_log(time_t stampTime, char* s);

class CLog{
public:
	CLog();
	~CLog();
	void Init();
	void Flush();
	void logRecord(/*string strFunc,int iLine, string strMod, */char* cFormat,...);
	void dataRecord(/*string strFunc,int iLine, string strMod, */char* cFormat,...);
private:
	std::string m_logBuffer ;
	std::string m_dataBuffer ;
};

#endif