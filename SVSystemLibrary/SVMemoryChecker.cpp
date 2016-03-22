//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVMemoryChecker
//* .File Name       : $Workfile:   SVMemoryChecker.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   02 Oct 2013 10:12:42  $
//******************************************************************************

#include "stdafx.h"
//Moved to precompiled header: #include <Psapi.h>
#include "SVMemoryChecker.h"

size_t SVMemoryChecker::GetFreeProcessMemorySize()
{
	size_t l_Size = 0;

	// Determine the page size on this system.
	SYSTEM_INFO l_SystemInfo;

	GetSystemInfo( &l_SystemInfo );

	/*
	size_t l_MaxMemSize = static_cast< BYTE* >( l_SystemInfo.lpMaximumApplicationAddress ) + l_SystemInfo.dwPageSize - static_cast< BYTE* >( l_SystemInfo.lpMinimumApplicationAddress );

	PROCESS_MEMORY_COUNTERS_EX l_ProcessMemoryInfo;
	l_ProcessMemoryInfo.cb = sizeof( l_ProcessMemoryInfo );
	::GetProcessMemoryInfo( ::GetCurrentProcess(), reinterpret_cast< PROCESS_MEMORY_COUNTERS* >( &l_ProcessMemoryInfo ), l_ProcessMemoryInfo.cb );

	l_Size = l_MaxMemSize;

	l_Size -= l_ProcessMemoryInfo.WorkingSetSize;
	l_Size -= l_ProcessMemoryInfo.QuotaPagedPoolUsage;
	l_Size -= l_ProcessMemoryInfo.QuotaNonPagedPoolUsage;

	l_Size /= 1024;
	l_Size /= 1024;
	*/

	MEMORY_BASIC_INFORMATION l_Memory;

	DWORD l_Status = sizeof( MEMORY_BASIC_INFORMATION );
	BYTE* l_Address = static_cast< BYTE* >( l_SystemInfo.lpMinimumApplicationAddress );
	BYTE* l_EndAddress = static_cast< BYTE* >( l_SystemInfo.lpMaximumApplicationAddress );
	size_t l_FreeByteCount = 0;
	
	while( 0 < l_Status && l_Address <= l_EndAddress )
	{
		// Clear the results structure.
		memset( &l_Memory, 0, sizeof( MEMORY_BASIC_INFORMATION ) );

		l_Status = static_cast<DWORD>(VirtualQuery( l_Address, &l_Memory, sizeof( MEMORY_BASIC_INFORMATION ) ));

		if( 0 < l_Status && ( l_Memory.State & MEM_FREE ) == MEM_FREE )
		{
			l_FreeByteCount += l_Memory.RegionSize;
		}

		l_Address += l_Memory.RegionSize;
	}

	l_Size = l_FreeByteCount / ( 1024 * 1024 );

	return l_Size;
}

SVMemoryChecker::~SVMemoryChecker()
{
}

SVMemoryChecker::SVMemoryChecker()
{
}

