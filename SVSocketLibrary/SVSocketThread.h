//******************************************************************************
//* COPYRIGHT (c) 2011 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVSocketThread
//* .File Name       : $Workfile:   SVSocketThread.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   01 Dec 2014 13:47:40  $
//******************************************************************************

#pragma once

#pragma region Includes
//Moved to precompiled header: #include <comdef.h>
//Moved to precompiled header: #include <string>
#include "SVSystemLibrary/SVThreadManager.h"
#pragma endregion Includes

/*
ThreadFunc must have the following prototype:
     void ThreadFunc();
*/

namespace SvSol
{
	template <typename ThreadFunc>
	class SVSocketThread
	{
	private:
	HANDLE m_hStopEvent;
		HANDLE m_hThread;
		unsigned long m_ulThreadID;
		_bstr_t m_tag;
		ThreadFunc m_threadFunc;

		static DWORD WINAPI ThreadProc( LPVOID lpParam );

	public:
		SVSocketThread();
		~SVSocketThread();

		HRESULT Create(const ThreadFunc& threadFunc, const _bstr_t& p_rTag);
		void Destroy();

		HRESULT Restart();

		int GetPriority() const;
		void SetPriority(int priority);

		bool IsActive() const;
		HANDLE GetThreadHandle() const;
	};
} //namespace SvSol

#include "SVSocketThread.inl"

