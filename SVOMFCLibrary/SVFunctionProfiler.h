// ******************************************************************************
// * COPYRIGHT (c) 2003 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVFunctionProfiler
// * .File Name       : $Workfile:   SVFunctionProfiler.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.1  $
// * .Check In Date   : $Date:   08 May 2013 15:33:38  $
// ******************************************************************************

#ifndef SVFUNCTIONPROFILER_H
#define SVFUNCTIONPROFILER_H
#pragma once

#include <iostream>
#include <map>
#include "SVTimerLibrary/SVClock.h"

#if 0
// this class is meant to be used like this:
void MyClass::MyFunction()
{
	static SVFunctionProfiler profile("MyClass::MyFunction");
	SVFunctionProfilerLocal profiler(profile);
	// do stuff
}
#endif

class SVFunctionProfiler
{
public:
	SVFunctionProfiler(const CString& sName);
	~SVFunctionProfiler();
public:
	void Begin();
	void End();

private:
	__int64 m_iCount;
	SVClock::SVTimeStamp m_TotalTime;
	SVClock::SVTimeStamp m_BeginTime;
	CString m_sName;

	friend class SVFunctionProfilerLocal;
};


class SVFunctionProfilerLocal
{
public:
	SVFunctionProfilerLocal(SVFunctionProfiler& rProfiler);
	~SVFunctionProfilerLocal();
private:
	SVFunctionProfiler& m_rProfiler;
};

inline SVFunctionProfilerLocal::SVFunctionProfilerLocal(SVFunctionProfiler& rProfiler) : m_rProfiler(rProfiler)
{
	m_rProfiler.Begin();
}

inline SVFunctionProfilerLocal::~SVFunctionProfilerLocal()
{
	m_rProfiler.End();
}


inline SVFunctionProfiler::SVFunctionProfiler(const CString& sName)
{
	m_sName = sName;
	m_iCount = 0;
	m_TotalTime = 0.0;
}

inline SVFunctionProfiler::~SVFunctionProfiler()
{
	//TRACE();
	__int64 iTotalTime = static_cast< __int64 >( m_TotalTime );
	FILE* fh = fopen("c:\\temp\\profile.txt","a");
	if( fh )
	{
		fprintf(fh,_T("PROFILER: %s count=%I64u, total time=%I64u ms\n"), m_sName, m_iCount, iTotalTime);
		fclose( fh );
	}
	
	TRACE(_T("PROFILER: %s count=%I64u, total time=%I64u ms\n"), m_sName, m_iCount, iTotalTime);
	/*
	std::cout << m_sName.GetBuffer(1);
	m_sName.ReleaseBuffer();
	std::cout << "count=";
	std::cout << (int)m_iCount;
	std::cout << "; total time = ";
	std::cout << (int)iTotalTime;
	std::cout << "ms";
	std::cout << std::endl;
	*/
}

inline void SVFunctionProfiler::Begin()
{
	++m_iCount;
	m_BeginTime = SVClock::GetTimeStamp();
}

inline void SVFunctionProfiler::End()
{
	SVClock::SVTimeStamp EndTime = SVClock::GetTimeStamp();
	EndTime -= m_BeginTime;
	m_TotalTime += EndTime;
}





class SVSizeProfiler
{
public:
	SVSizeProfiler(const CString& sName);
	~SVSizeProfiler();
	void Add(int iSize);
private:
	std::map<int,int> m_mapSizeCounter;
	CString m_sName;
};

inline SVSizeProfiler::SVSizeProfiler(const CString& sName)
{
	m_sName = sName;
}

inline SVSizeProfiler::~SVSizeProfiler()
{
	FILE* fh = fopen("c:\\temp\\profile_count.txt","a");
	if( fh )
	{
		std::map<int,int>::iterator iter;
		for (iter = m_mapSizeCounter.begin(); iter != m_mapSizeCounter.end(); ++iter)
		{
			TRACE(_T("COUNT_PROFILER: %s size %d count=%d\n"), m_sName, iter->first, iter->second);
			fprintf(fh,_T("COUNT_PROFILER: %s size %d count=%d\n"), m_sName, iter->first, iter->second);
		}
		fclose( fh );
	}
	
}

inline void SVSizeProfiler::Add(int iSize)
{
	m_mapSizeCounter[iSize]++;
}

#endif //__SVFUNCTIONPROFILER__

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVOMFCLibrary\SVFunctionProfiler.h_v  $
 * 
 *    Rev 1.1   08 May 2013 15:33:38   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 012.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   29 Apr 2013 13:19:00   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  830
 * SCR Title:  Consolidate all Time Stamp and Clock Functionality
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to use the new SVClock::SVTimeStamp type for all time stamp variables.  Update to use new SVClock::GetTimeStamp() function to get the time stamp value in milliseconds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   25 Apr 2013 13:05:58   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   04 Oct 2012 09:32:56   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Changed from post incrementor to pre incrementor.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   08 Jul 2004 11:50:20   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  406
 * SCR Title:  Implement External Tool
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   first iteration
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
