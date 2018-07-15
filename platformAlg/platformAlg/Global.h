#ifndef _GLOBAL__H
#define _GLOBAL__H

#include <time.h>
#include <string.h>

#define  DEADLINE_DATE "20181231"

static void stamp_to_standard(time_t stampTime, char* s, char* format = NULL)
{
	time_t tick = (time_t)stampTime;
	struct tm tm_;

	localtime_s(&tm_, &tick);
	int size = strlen(s) > 32 ? strlen(s) : 32;
	if (format == NULL)
	{
		strftime(s, size, "%Y-%m-%d", &tm_);
	}
	else
	{
		strftime(s, size, format, &tm_);
	}
}


static bool CheckLicense(__int64 time){
	char buf[256] = { 0 };
	memset(buf, 0, sizeof(buf));
	stamp_to_standard(time, buf, "%Y%m%d");
	if (strcmp(buf, DEADLINE_DATE) >= 0)
	{
		return false;
	}
	return true;
}



#endif