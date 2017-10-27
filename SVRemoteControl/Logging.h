//******************************************************************************
//* COPYRIGHT (c) 2014 by Seidenader
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : Logging
//* .File Name       : $Workfile:   Logging.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.5  $
//* .Check In Date   : $Date:   16 Jan 2015 10:07:40  $
//******************************************************************************

#pragma once

#pragma region Includes
#include <winnt.h>
#include <sstream>
#include "SVEventMsg/SVEventMsg.h"
#include "SVStringConversions.h"
#include "SVCriticalSection.h"
#include "SVAutoLockAndReleaseTemplate.h"
#include "SVStringConversions.h"
#pragma endregion Includes


class EventLog
{
private:
	EventLog();
	~EventLog();

public:
	static void Write(DWORD errorCode, const std::wstring& errorData, WORD severity = EVENTLOG_ERROR_TYPE, WORD category = 0)
	{
		static const std::wstring EventSource = L"SVRemoteControl";
		static SVCriticalSection l_CriticalSection;
		SVAutoLockAndReleaseTemplate< SVCriticalSection > l_AutoLock;

		if (l_AutoLock.Assign(&l_CriticalSection, 50))
		{
			HANDLE hEventLog = RegisterEventSourceW(nullptr, EventSource.c_str());
			if (hEventLog)
			{
				DWORD eventID = MSG_INFO_1;
				if (S_OK == errorCode)
				{
					eventID = MSG_SUCCESS_1;
				}
				else if (IS_ERROR(errorCode))
				{
					eventID = MSG_ERROR_1;
				}
				LPCWSTR pMsg = errorData.c_str();
				ReportEventW(hEventLog, severity, category, eventID, nullptr, 1, 0, &pMsg, nullptr);
				DeregisterEventSource(hEventLog);
			}
		}
	}
	static void WriteDebug(const std::wstring& errorData, WORD severity = EVENTLOG_INFORMATION_TYPE, WORD category = 0)
	{
		static const std::wstring EventSource = L"SVRemoteControl";
		static SVCriticalSection l_CriticalSection;
		SVAutoLockAndReleaseTemplate< SVCriticalSection > l_AutoLock;

		if (l_AutoLock.Assign(&l_CriticalSection, 50))
		{
			HANDLE hEventLog = RegisterEventSourceW(nullptr, EventSource.c_str());
			if (hEventLog)
			{
				LPCWSTR pMsg = errorData.c_str();
				ReportEventW(hEventLog, severity, category, MSG_INFO_1, nullptr, 1, 0, &pMsg, nullptr);
				DeregisterEventSource(hEventLog);
			}
		}
	}
};

inline void SVLog(HRESULT hr, const wchar_t * file, long line)
{
	if (!SUCCEEDED(hr))
	{
		std::wstringstream ws;
		wchar_t * msg = nullptr;
		const std::wstring separator = L" ";
		if (SUCCEEDED(FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
			nullptr,
			hr,
			0,
			reinterpret_cast<wchar_t *>(&msg),
			64,
			nullptr)) && msg != nullptr)
		{
			ws << msg << separator << file << separator << line;
			::LocalFree(msg);
		}
		else
		{
			std::wstringstream hr_ws;
			hr_ws << std::hex << hr;
			ws << hr_ws.str().c_str() << separator << file << separator << line;
		}
		EventLog::Write(hr, ws.str());

		ws << std::endl;
		::OutputDebugStringW(ws.str().c_str());
	}
}

inline void SVLog(const wchar_t * msg, const wchar_t * file, long line)
{
	std::wstringstream ws;
	const std::wstring separator = L" ";
	ws << msg << separator << file << separator << line;

	EventLog::Write(E_FAIL, ws.str());

	ws << std::endl;
	::OutputDebugStringW(ws.str().c_str());
}

inline void SVLogDebug(const wchar_t * msg)
{
	std::wstringstream ws;
	const std::wstring separator = L" ";
	ws << msg;

	EventLog::WriteDebug(ws.str());

	ws << std::endl;
	::OutputDebugStringW(ws.str().c_str());
}


#ifdef SVLOG
#undef SVLOG
#endif

#define SVLOG(x) SVLog(x, __FILEW__, __LINE__)
