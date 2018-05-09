#pragma once
#include <string>
#include <stdio.h>
#include <fstream>
#include <stdarg.h>

using namespace std;

#define LOG(cFormat, ...) \
    logRecord(__FUNCTION__, __LINE__, __FILE__,  cFormat, __VA_ARGS__);

//print log module
static void logRecord(string strFunc,int iLine, string strMod, char* cFormat,...)
{
	string strLog;
	strLog.clear();
	char acBuffer1[1024], acBuffer2[1024];
	sprintf_s(acBuffer1, 254, "[%s] [%d] [%s]  ", strFunc.data(), iLine, strMod.data());
	va_list args;
	va_start(args, cFormat);
	vsprintf_s(acBuffer2, cFormat, args);

	ofstream ofLogFile;
	ofLogFile.open("log.txt", ios::out | ios::app);
	if (!ofLogFile)
	{
		return;
	}

	string strBuffer1, strBuffer2;
	strBuffer1 = acBuffer1;
	strBuffer2 = acBuffer2;

	ofLogFile.write(strBuffer1.data(), strBuffer1.size());
	ofLogFile.write(strBuffer2.data(), strBuffer2.size());
	ofLogFile.put('\n');
	ofLogFile.close();

	va_end(args);
}