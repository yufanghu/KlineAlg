#ifndef _GLOBAL__H
#define _GLOBAL__H

#include <time.h>
#include <string.h>
#include <string>

#define  DEADLINE_DATE "20181231"

static bool stamp_to_standard(time_t stampTime, std::string& str, char* format = NULL)
{

	struct tm tm_;

	int ret = localtime_s(&tm_, &stampTime);
	if(ret != 0)
	{
		return false;
	}
	char s[128] = {0};
	if (format == NULL)
	{
		strftime(s, sizeof(s), "%Y-%m-%d", &tm_);
	}
	else
	{
		strftime(s, sizeof(s), format, &tm_);
	}
	str = s;
	return true;
}


static bool CheckLicense(__int64 time){
	return true;
	std::string str;
	if(stamp_to_standard(time, str, "%Y%m%d")){
		if (strcmp(str.c_str(), DEADLINE_DATE) >= 0)
		{
			return false;
		}
	}
	else
		return false;
	
	return true;
}



#endif