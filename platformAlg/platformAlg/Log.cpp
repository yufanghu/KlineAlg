#include "StdAfx.h"
#include "log.h"

using namespace std;

bool CLog::m_bEnableLog = true;
#define CHECK_LOG_ENABLE if(!m_bEnableLog) return;
void stamp_to_standard(time_t stampTime, char* s, char* format = NULL)
{
	time_t tick = (time_t)stampTime;
	struct tm tm;

	tm = *localtime(&tick);
	int size = strlen(s) > 32 ? strlen(s) : 32;
	if (format == NULL)
	{
		strftime(s, size, "%Y-%m-%d", &tm);
	}
	else
	{
		strftime(s, size, format, &tm);
	}
}
CLog::CLog()
{}

CLog::~CLog()
{
}

void CLog::Init(const std::string & path){
	CHECK_LOG_ENABLE
	m_logBuffer = "";
	m_dataBuffer = "";
	m_path = path;
	
	system(("mkdir " + m_path).c_str());
}


void  CLog::Flush(){
	{
		CHECK_LOG_ENABLE
		static int j = 0;
		char buf[20] = { 0 };
		stamp_to_standard(time(NULL), buf, "%Y_%m_%d_%H_%M_%S");

		char name[256] = { 0 };

		sprintf(name, "%s/%s_%d_log.txt", m_path.c_str(), buf, j++);

		FILE * pFile = fopen(name, "a");
		if (pFile){

			fwrite(m_logBuffer.c_str(), m_logBuffer.length(), 1, pFile);
			fclose(pFile);
		}
	}
	{
	static int i = 0;
	char buf[256] = { 0 };
	sprintf(buf, "%s/%lld_%d_log_data.txt", m_path.c_str(), time(NULL), i++);
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
