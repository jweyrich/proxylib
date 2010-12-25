#pragma once

#include <cstdio>

#define LOG_0 0
#define LOG_1 1
#define LOG_ENABLED(flag) \
	(LOG_ ## flag != 0)

#ifdef DEBUG
#define LOG_TRACE \
	printf("%s %s:%d %s called\n", "TRACE", __FILE__, __LINE__, __func__) //__PRETTY_FUNCTION__
#define LOG(format, args...) \
	printf("%s:%d " format "\n", __FILE__, __LINE__, ##args)
#define LOG_DEBUG(format, args...) \
	printf("%s %s:%d " format "\n", "DEBUG", __FILE__, __LINE__, ##args)
#define LOG_INFO(format, args...) \
	printf("%s %s:%d " format "\n", "INFO", __FILE__, __LINE__, ##args)
#define LOG_WARN(format, args...) \
	printf("%s %s:%d " format "\n", "WARN", __FILE__, __LINE__, ##args)
#define LOG_ERROR(format, args...) \
	printf("%s %s:%d " format "\n", "ERROR", __FILE__, __LINE__, ##args)
#define LOG_FATAL(format, args...) \
	printf("%s %s:%d " format "\n", "FATAL", __FILE__, __LINE__, ##args)

#define LOG_PRINTF(flag, format, args...) \
	do { \
		if (LOG_ENABLED(flag)) { \
			printf(format, ##args); \
		} \
	} while (0)

#define LOG_PRINTF_INDENT(flag, indent, format, args...) \
	do { \
		if (LOG_ENABLED(flag)) { \
			printf("%*s" format, indent, " ", ##args); \
		} \
	} while (0)

#else // ifdef DEBUG
#define LOG_TRACE											(void)0
#define LOG(format, args...)								(void)0
#define LOG_DEBUG(format, args...)							(void)0
#define LOG_INFO(format, args...)							(void)0
#define LOG_WARN(format, args...)							(void)0
#define LOG_ERROR(format, args...)							(void)0
#define LOG_FATAL(format, args...)							(void)0
#define LOG_PRINTF(flag, format, args...)					(void)0
#define LOG_PRINTF_INDENT(flag, indent, format, args...)	(void)0
#endif // ifdef DEBUG

//
// Use these
//
//	LOG_ENABLED
//	LOG_TRACE
//	LOG
//	LOG_DEBUG
//	LOG_INFO
//	LOG_WARN
//	LOG_ERROR
//	LOG_FATAL
//	LOG_PRINTF
//	LOG_PRINTF_INDENT
