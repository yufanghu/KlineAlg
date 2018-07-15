#include "StdAfx.h"
#include "log.h"
#include "Global.h"

using namespace std;

bool CLog::m_bEnableLog = true;
#define CHECK_LOG_ENABLE if(!m_bEnableLog) return;

CLog::CLog()
{}

CLog::~CLog()
{
}

void CLog::Init(const std::string & path){
	CHECK_LOG_ENABLE
	m_logBuffer = "";
	m_dataBuffer = "";
	m_path = AlgorithmLog::LOG_FOLDER + path;
	
	system(("mkdir " + m_path).c_str());
}


void  CLog::Flush(){

	char buf_time[20] = { 0 };
	static int j = 0;
	stamp_to_standard(time(NULL), buf_time, "%Y_%m_%d_%H_%M_%S");
	{
		CHECK_LOG_ENABLE
		char name[256] = { 0 };

		sprintf_s(name, "%s/%s_%d_log.txt", m_path.c_str(), buf_time, j++);

		FILE * pFile = fopen(name, "a");
		if (pFile){

			fwrite(m_logBuffer.c_str(), m_logBuffer.length(), 1, pFile);
			fclose(pFile);
		}
	}
	{
	char buf[256] = { 0 };
	sprintf(buf, "%s/%s_%d_data.txt", m_path.c_str(), buf_time, j-1);
	FILE * pFile = fopen(buf, "a");
	if (pFile){
		fwrite(m_dataBuffer.c_str(), m_dataBuffer.length(), 1, pFile);
		fclose(pFile);
	}
}

}

void  CLog::logRecord(/*string strFunc,int iLine, string strMod, */char* cFormat, ...)
{
	CHECK_LOG_ENABLE
	char  acBuffer[1024];
	va_list args;
	va_start(args, cFormat);
	vsprintf_s(acBuffer, cFormat, args);

	string strBuffer = acBuffer;

	m_logBuffer += strBuffer;

	va_end(args);
}

void  CLog::dataRecord(/*string strFunc,int iLine, string strMod, */char* cFormat, ...)
{
	CHECK_LOG_ENABLE
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
	CHECK_LOG_ENABLE
	m_dataBuffer = "";
	m_logBuffer = "";
}
