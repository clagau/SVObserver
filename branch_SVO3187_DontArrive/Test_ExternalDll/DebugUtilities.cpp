#include "StdAfx.h"

#ifdef _DEBUG

#include <stdio.h>
#include <stdarg.h>
#include "defines.h"
#include "DebugUtilities.h"

int start_fdb(int mode)
{
	debugmode=mode;

	if (debugmode==0) return 0;

	_unlink( DEBUGFILE );
	FILE* mydbhFile = NULL;
	fopen_s( &mydbhFile, DEBUGFILE ,"w+t");
	fprintf(mydbhFile,"Test %d\n",mode);
	fclose(mydbhFile);

	return 0;
}

int cdecl fdb(const char *msg,...)
{
	if (debugmode==0) return 0;

	va_list marker;

	FILE* mydbhFile = NULL;
	fopen_s( &mydbhFile, DEBUGFILE ,"a+t");
	if (mydbhFile == NULL)
		return 0;
	va_start(marker,msg);
	vfprintf(mydbhFile,msg,marker);
	va_end(marker);
	fflush(mydbhFile);
	fclose(mydbhFile);

	return 0;
}

#endif