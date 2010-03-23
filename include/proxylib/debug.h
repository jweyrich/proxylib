#pragma once

#include <cstdio>

#define DEBUG_0 0
#define DEBUG_1 1
#define _M_DEBUG_ENABLED(flag) \
	(DEBUG_ ## flag != 0)

#ifdef DEBUG
#define _M_DTRACE \
	printf("%s %s:%d %s called\n", "TRACE", __FILE__, __LINE__, __func__) //__PRETTY_FUNCTION__
#define _M_DLOG(format, args...) \
	printf("%s:%d " format "\n", __FILE__, __LINE__, ##args)
#define _M_DDEBUG(format, args...) \
	printf("%s %s:%d " format "\n", "DEBUG", __FILE__, __LINE__, ##args)
#define _M_DWARN(format, args...) \
	printf("%s %s:%d " format "\n", "WARN", __FILE__, __LINE__, ##args)
#define _M_DERROR(format, args...) \
	printf("%s %s:%d " format "\n", "ERROR", __FILE__, __LINE__, ##args)
#define _M_DFATAL(format, args...) \
	printf("%s %s:%d " format "\n", "FATAL", __FILE__, __LINE__, ##args)

#define _M_DPRINTF(flag, format, args...) \
	do { \
		if (_M_DEBUG_ENABLED(flag)) { \
			printf(format, ##args); \
		} \
	} while (0)

#define _M_DPRINTF_INDENT(flag, indent, format, args...) \
	do { \
		if (_M_DEBUG_ENABLED(flag)) { \
			printf("%*s" format, indent, " ", ##args); \
		} \
	} while (0)

#else // ifdef DEBUG
#define _M_DTRACE
#define _M_DLOG(format, args...)
#define _M_DDEBUG(format, args...)
#define _M_DWARN(format, args...)
#define _M_DERROR(format, args...)
#define _M_DFATAL(format, args...)
#define _M_DPRINTF(format, args...)
#define _M_DPRINTF_INDENT(format, args...)
#endif // ifdef DEBUG

//
// Use these
//
#define DEBUG_ENABLED	_M_DEBUG_ENABLED
#define DTRACE			_M_DTRACE
#define DLOG			_M_DLOG
#define DDEBUG			_M_DDEBUG
#define DWARN			_M_DWARN
#define DERROR			_M_DERROR
#define DFATAL			_M_DFATAL
#define DPRINTF			_M_DPRINTF
#define DPRINTF2		_M_DPRINTF_INDENT
