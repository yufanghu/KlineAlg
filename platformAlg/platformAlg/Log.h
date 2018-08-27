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
namespace AlgorithmLog{
	const std::string LOG_FOLDER = "Algorithm_Log\\";
}
class CLog{
public:
	CLog();
	~CLog();
	void Init(const std::string & path);
	void Flush();
	void logRecord(/*string strFunc,int iLine, string strMod, */char* cFormat, ...);
	void dataRecord(/*string strFunc,int iLine, string strMod, */char* cFormat, ...);
	void clearLog();
	int CreatDir(const char * dirPath);
private:
	std::string m_logBuffer;
	std::string m_dataBuffer;
	std::string m_path;
};

#endif