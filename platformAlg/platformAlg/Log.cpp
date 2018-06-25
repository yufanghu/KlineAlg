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
	m_logBuffer.shrink_to_fit();
	m_dataBuffer.shrink_to_fit();
	system("mkdir Platform_Log");
}
extern void stamp_to_standard(time_t stampTime, char* s, char* format = NULL);

void CLog::Flush(int nNum)
{
	{
		static int doc1 = 0;  //算法1
		static int doc2 = 0;  //算法2
		static int doc3 = 0;  //算法3
		char buf[20] = { 0 };
		stamp_to_standard(time(NULL), buf, "%Y_%m_%d_%H_%M_%S");

		char name[256] = { 0 };

		switch (nNum)
		{
		case e_doc_1:
			sprintf(name, "Platform_Log/%s_%s_%d_log.txt", buf, FILTERDOC1, doc1++);
			break;
		case e_doc_2:
			sprintf(name, "Platform_Log/%s_%s_%d_log.txt", buf, FILTERDOC2, doc2++);
			break;
		case e_doc_3:
			sprintf(name, "Platform_Log/%s_%s_%d_log.txt", buf, FILTERDOC3, doc3++);
			break;
		default:
			break;
		}
	

		FILE * pFile = fopen(name, "a");
		if (pFile){

			fwrite(m_logBuffer.c_str(), m_logBuffer.length(), 1, pFile);
			fclose(pFile);
		}
	}
	{
	//保存调用数据
		static int doc1 = 0;  //算法1
		static int doc2 = 0;  //算法2
		static int doc3 = 0; //算法3
		char buf[256] = { 0 };
		switch (nNum)
		{
		case e_doc_1:
			sprintf(buf, "Platform_Log/%lld_%s_%d_log_data.txt", time(NULL), FILTERDOC1, doc1++);
			break;
		case e_doc_2:
			sprintf(buf, "Platform_Log/%lld_%s_%d_log_data.txt", time(NULL), FILTERDOC1, doc1++);
			break;
		case e_doc_3:
			sprintf(buf, "Platform_Log/%lld_%s_%d_log_data.txt", time(NULL), FILTERDOC1, doc1++);
			break;
		default:
			break;
		}
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
	m_dataBuffer.shrink_to_fit();
	m_logBuffer = "";
	m_logBuffer.shrink_to_fit();
}
