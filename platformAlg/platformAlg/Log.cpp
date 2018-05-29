#include "StdAfx.h"
#include "log.h"


CLog::CLog()
{}

CLog::~CLog()
{
}

void CLog::Init(){
	m_logBuffer = "";
	m_dataBuffer = "";
	system("mkdir Platform_Log");
}
extern void stamp_to_standard(time_t stampTime, char* s, char* format = NULL);

void  CLog::Flush(){
{
	static int j = 0;
	char buf[20] = { 0 };
	stamp_to_standard(time(NULL), buf, "%Y_%m_%d_%H_%M_%S");

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

void  CLog::logRecord(/*string strFunc,int iLine, string strMod, */char* cFormat, ...)
{
	char  acBuffer[1024];
	va_list args;
	va_start(args, cFormat);
	vsprintf_s(acBuffer, cFormat, args);

	string strBuffer = acBuffer;

	m_logBuffer += strBuffer;

	va_end(args);
}

void  CLog::dataRecord(/*string strFunc,int iLine, string strMod, */char* cFormat,...)
{
	char acBuffer[1024];

	va_list args;
	va_start(args, cFormat);
	vsprintf_s(acBuffer, cFormat, args);

	string strBuffer = acBuffer;
	m_dataBuffer += strBuffer;

	va_end(args);
}

void CLog::clearLog()
{
	m_dataBuffer = "";
	m_logBuffer = "";
}
