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

extern char* stamp_to_standard_ex_log(int stampTime, char* s);

class CLog{

public:
	CLog();
	~CLog();
	void Init(){
		m_logBuffer = "";
		m_dataBuffer = "";
		system("mkdir Platform_Log");
	}

	 void Flush(){
		{
			static int j = 0;
			char buf[20] = { 0 };
			stamp_to_standard_ex_log(time(NULL), buf);

			char name[256] = { 0 };

			sprintf(name, "Platform_Log/%s_%d_log.txt", buf, j++);

			FILE * pFile = fopen(name, "a");
			if (pFile){

				fwrite(m_logBuffer.c_str(), m_logBuffer.length(), 1, pFile);
				fclose(pFile);
			}
		}
		{
			static int i = 0;
			char buf[256] = { 0 };
			sprintf(buf, "Platform_Log/%lld_%d_log_data.txt", time(NULL), i++);
			FILE * pFile = fopen(buf, "a");
			if (pFile){
				fwrite(m_dataBuffer.c_str(), m_dataBuffer.length(), 1, pFile);
				fclose(pFile);
			}
		}
		
	}

	void logRecord(/*string strFunc,int iLine, string strMod, */char* cFormat,...)
	{
		string strLog;
		strLog.clear();
		char  acBuffer2[1024];
		//sprintf_s(acBuffer1, 254, "[%s] [%d] [%s]  ", strFunc.data(), iLine, strMod.data());
		va_list args;
		va_start(args, cFormat);
		vsprintf_s(acBuffer2, cFormat, args);

		//printf(acBuffer1);
		//printf(acBuffer2);

		string strBuffer1, strBuffer2;
		//strBuffer1 = acBuffer1;
		strBuffer2 = acBuffer2;

		m_logBuffer += strBuffer2;

		va_end(args);
	}

	void dataRecord(/*string strFunc,int iLine, string strMod, */char* cFormat,...)
	{
		string strLog;
		strLog.clear();
		char acBuffer1[1024], acBuffer2[1024];
		//sprintf_s(acBuffer1, 254, "[%s] [%d] [%s]  ", strFunc.data(), iLine, strMod.data());
		va_list args;
		va_start(args, cFormat);
		vsprintf_s(acBuffer2, cFormat, args);

		//printf(acBuffer1);
		//printf(acBuffer2);

		string strBuffer1, strBuffer2;
		//strBuffer1 = acBuffer1;
		strBuffer2 = acBuffer2;

		m_dataBuffer += strBuffer2;

		va_end(args);
	}
private:
	std::string m_logBuffer ;
	std::string m_dataBuffer ;
};



#endif