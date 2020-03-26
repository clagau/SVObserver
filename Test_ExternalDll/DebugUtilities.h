#ifndef DEBUG_UTILITIES_H_
#define DEBUG_UTILITIES_H_

static	int	debugmode=0;

int start_fdb(int mode);

#if _DEBUG
#      define FDB(str, ...) fdb(str, __VA_ARGS__)
       int cdecl fdb(char *msg, ...);
#else
#  define FDB(str, ...)
#endif


#endif //DEBUG_UTILITIES_H_