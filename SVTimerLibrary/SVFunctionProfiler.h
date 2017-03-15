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

#pragma once

#pragma region Includes
//Moved to precompiled header: #include <iostream>
//Moved to precompiled header: #include <map>
#include "SVClock.h"
#include "SVUtilityLibrary/SVString.h"
#pragma endregion Includes

/* // this class is meant to be used like this:
void MyClass::MyFunction()
{
	static SVFunctionProfiler profile("MyClass::MyFunction");
	SVFunctionProfilerLocal profiler(profile);
	// do stuff
} */

class SVFunctionProfiler
{
public:
	SVFunctionProfiler(LPCTSTR Name);
	~SVFunctionProfiler();
public:
	void Begin();
	void End();

private:
	__int64 m_iCount;
	SVClock::SVTimeStamp m_TotalTime;
	SVClock::SVTimeStamp m_BeginTime;
	SVString m_Name;

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


inline SVFunctionProfiler::SVFunctionProfiler(LPCTSTR Name) :
  m_Name( Name )
, m_iCount( 0 )
, m_TotalTime( 0.0 )
{
}

inline SVFunctionProfiler::~SVFunctionProfiler()
{
	__int64 iTotalTime = static_cast< __int64 >( m_TotalTime );
	FILE* fh = fopen("c:\\temp\\profile.txt","a");
	if( fh )
	{
		fprintf(fh,_T("PROFILER: %s count=%I64u, total time=%I64u ms\n"), m_Name.c_str(), m_iCount, iTotalTime);
		fclose( fh );
	}
	
#if defined (TRACE_THEM_ALL) || defined (TRACE_PROFILER)
	TRACE(_T("PROFILER: %s count=%I64u, total time=%I64u ms\n"), m_Name.c_str(), m_iCount, iTotalTime);
#endif
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
	SVSizeProfiler(LPCTSTR Name);
	~SVSizeProfiler();
	void Add(int iSize);
private:
	std::map<int,int> m_mapSizeCounter;
	SVString m_Name;
};

inline SVSizeProfiler::SVSizeProfiler(LPCTSTR Name) :
m_Name( Name )
{
}

inline SVSizeProfiler::~SVSizeProfiler()
{
	FILE* fh = fopen("c:\\temp\\profile_count.txt","a");
	if( fh )
	{
		std::map<int,int>::iterator iter;
		for (iter = m_mapSizeCounter.begin(); iter != m_mapSizeCounter.end(); ++iter)
		{
#if defined (TRACE_THEM_ALL) || defined (TRACE_PROFILER)
			TRACE(_T("COUNT_PROFILER: %s size %d count=%d\n"), m_Name.c_str(), iter->first, iter->second);
#endif
			fprintf(fh,_T("COUNT_PROFILER: %s size %d count=%d\n"), m_Name.c_str(), iter->first, iter->second);
		}
		fclose( fh );
	}
	
}

inline void SVSizeProfiler::Add(int iSize)
{
	m_mapSizeCounter[iSize]++;
}
