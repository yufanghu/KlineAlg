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

using namespace std;

#define LOG(cFormat, ...) \
    CLog::logRecord(/*__FUNCTION__, __LINE__, __FILE__,  */cFormat, __VA_ARGS__);
#define LOG_DATA(cFormat, ...) \
    CLog::dataRecord(/*__FUNCTION__, __LINE__, __FILE__,  */cFormat, __VA_ARGS__);
#define LOG_FLUSH() \
	CLog::Flush()

class CLog{

public:
	static void Init(){
		
	}

	static void Flush(){
		{
			ofstream ofLogFile;
			ofLogFile.open("log.txt", ios::out | ios::app);
			if (!ofLogFile)
			{
			return;
			}
			ofLogFile.write(m_logBuffer.c_str(), m_logBuffer.length());
			ofLogFile.close();
		}
		{
			ofstream ofLogFile;
			ofLogFile.open("log_data.txt", ios::out | ios::app);
			if (!ofLogFile)
			{
			return;
			}
			ofLogFile.write(m_dataBuffer.c_str(), m_dataBuffer.length());
			ofLogFile.close();
		}
		
	}

	static void logRecord(/*string strFunc,int iLine, string strMod, */char* cFormat,...)
	{
		string strLog;
		strLog.clear();
		char acBuffer1[1024], acBuffer2[1024];
		//sprintf_s(acBuffer1, 254, "[%s] [%d] [%s]  ", strFunc.data(), iLine, strMod.data());
		va_list args;
		va_start(args, cFormat);
		vsprintf_s(acBuffer2, cFormat, args);

		//printf(acBuffer1);
		printf(acBuffer2);

		string strBuffer1, strBuffer2;
		//strBuffer1 = acBuffer1;
		strBuffer2 = acBuffer2;

		m_logBuffer += strBuffer2;

		va_end(args);
	}

	static void dataRecord(/*string strFunc,int iLine, string strMod, */char* cFormat,...)
	{
		string strLog;
		strLog.clear();
		char acBuffer1[1024], acBuffer2[1024];
		//sprintf_s(acBuffer1, 254, "[%s] [%d] [%s]  ", strFunc.data(), iLine, strMod.data());
		va_list args;
		va_start(args, cFormat);
		vsprintf_s(acBuffer2, cFormat, args);

		//printf(acBuffer1);
		printf(acBuffer2);

		string strBuffer1, strBuffer2;
		//strBuffer1 = acBuffer1;
		strBuffer2 = acBuffer2;

		m_dataBuffer += strBuffer2;

		va_end(args);
	}
private:
	static std::string m_logBuffer ;
	static std::string m_dataBuffer ;
};



#endif