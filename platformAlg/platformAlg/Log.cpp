#include "StdAfx.h"
#include "log.h"
#include "Global.h"
#include <windows.h>
#include <direct.h> //_mkdir函数的头文件
#include <io.h>     //_access函数的头文件

#define ACCESS _access 
#define MKDIR(a) _mkdir((a)) 

using namespace std;

bool CLog::m_bEnableLog = true;
#define CHECK_LOG_ENABLE if(!m_bEnableLog) return;

CLog::CLog()
{}

CLog::~CLog()
{
}


int CLog::CreatDir(const char * dirPath) 
{ 
   char* pszDir = new char[300];
   memset(pszDir, 0, 300);
   memcpy(pszDir, dirPath, strlen(dirPath));
  int i = 0; 
  int iRet; 
  int iLen = strlen(pszDir); 
  
  //在末尾加/ 
  if (pszDir[iLen - 1] != '\\' && pszDir[iLen - 1] != '/') 
  { 
    pszDir[iLen] = '/'; 
    pszDir[iLen + 1] = '\0'; 
  } 
  
  // 创建目录 
  for (i = 0;i <= iLen;i ++) 
  { 
    if (pszDir[i] == '\\' || pszDir[i] == '/') 
    {  
      pszDir[i] = '\0'; 
  
      //如果不存在,创建 
      iRet = ACCESS(pszDir,0); 
      if (iRet != 0) 
      { 
        iRet = MKDIR(pszDir); 
        if (iRet != 0) 
        { 
          return -1; 
        }  
      } 
      //支持linux,将所有\换成/ 
      pszDir[i] = '/'; 
    }  
  } 
  return 0; 
} 
void CLog::Init(const std::string & path){
	CHECK_LOG_ENABLE
	m_logBuffer = "";
	m_dataBuffer = "";
	m_path = AlgorithmLog::LOG_FOLDER + path;
	
	CreatDir((m_path).c_str() );
}


void  CLog::Flush(){
	static int j = 0;
	std::string buf_time;
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
