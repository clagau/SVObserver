//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVMemoryChecker
//* .File Name       : $Workfile:   SVMemoryChecker.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   25 Apr 2013 17:52:16  $
//******************************************************************************

#include "stdafx.h"
#include <Psapi.h>
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

		l_Status = VirtualQuery( l_Address, &l_Memory, sizeof( MEMORY_BASIC_INFORMATION ) );

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

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVSystemLibrary\SVMemoryChecker.cpp_v  $
 * 
 *    Rev 1.0   25 Apr 2013 17:52:16   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   13 Jul 2010 12:11:40   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  697
 * SCR Title:  Add Free Memory Information to Status Bar
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added new memory checker class to library to caculate free memory.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/