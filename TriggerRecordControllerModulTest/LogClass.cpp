/// \copyright COPYRIGHT (c) 2019 by Seidenader Maschinenbau GmbH
/// \file LogClass.cpp
/// All Rights Reserved
//*****************************************************************************
/// Class to log result of the tests.
/// This class will be added also to TriggerRecordControllerReaderModuleTest-project.
//******************************************************************************

#include "stdafx.h"
#include "SVStatusLibrary\MessageContainer.h"
#include "LogClass.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

LogClass::LogClass()
{
	m_File = nullptr;
	m_LogLevel = LogLevel::Debug; // Default Log everything.
	ResetCounts();
}

LogClass::~LogClass()
{
}



bool LogClass::Open(CString strFileName, bool bAppend)
{
	Close();
	// Open default file.
	if (bAppend)
	{
#ifdef _UNICODE
		 _wfopen_s(&m_File, strFileName, _T("a, ccs=UNICODE"));
	}
	else
	{
		_wfopen_s(&m_File, strFileName, _T("w, ccs=UNICODE"));
#else
		fopen_s(&m_File, strFileName, _T("a"));
	}
	else
	{
		fopen_s(&m_File, strFileName, _T("w"));
#endif
	}
	if(nullptr == m_File)
	{
		AfxMessageBox(_T("Error Opening File"));
		return false;
	}
	else
	{
		return true;
	}
}

bool LogClass::Open()
{
	Close();
	m_File = stdout;
	return nullptr != m_File;
}

bool LogClass::IsOpen()
{
	return m_File == nullptr ? false : true;
}

void LogClass::Close()
{
	if(m_File)
	{
		_ftprintf(m_File,_T("\n"));
		fclose(m_File);
		m_File = nullptr;
	}
}

bool LogClass::Log(LPCTSTR strEntry, const LogLevel logLevel, const LogType logType, int lineNumber, LPCTSTR strTestName)
{
	if (logLevel <= m_LogLevel)
	{
		CountResults(logType);
		CString strTmp;
		strTmp = BuildLogString(logLevel, logType);
		strTmp.Format(_T("%s[%s][%d] "), strTmp, strTestName, lineNumber);
		std::lock_guard<std::mutex> guard(m_logMutex);
		if (m_File)
		{
			_ftprintf(m_File, _T("%s\n"), (strTmp + strEntry).GetString());
		}
		else
		{
			AfxMessageBox(strEntry);
			return false;
		}
	}
	return true;
}

bool LogClass::LogText(LPCTSTR strEntry, const LogLevel logLevel, const LogType logType)
{
	if(logLevel <= m_LogLevel)
	{
		CountResults(logType);
		CString strTmp;
		strTmp = BuildLogString(logLevel, logType);
		std::lock_guard<std::mutex> guard(m_logMutex);
		if(m_File)
		{
			_ftprintf(m_File, _T("%s\n"), (strTmp + strEntry).GetString());
		}
		else
		{
			AfxMessageBox(strEntry);
			return false;
		}
	}
	return true;
}

bool LogClass::logException(LPCTSTR strEntry, const SvStl::MessageContainer& rExp, int lineNumber, LPCTSTR strTestName, const LogLevel logLevel, const LogType logType)
{
	CString strTmp;
	strTmp.Format("%s\n\tFollowing Exception was caught: %s\n\tFileinfo: %s (%d)", strEntry, rExp.what(), rExp.getMessage().m_SourceFile.m_FileName.c_str(), rExp.getMessage().m_SourceFile.m_Line);
	return Log(strTmp, logLevel, logType, lineNumber, strTestName);
}

bool LogClass::LogText0(LPCTSTR strEntry, const LogLevel logLevel)
{
	if(logLevel <= m_LogLevel)
	{
		std::lock_guard<std::mutex> guard(m_logMutex);
		if(m_File)
		{
			_ftprintf(m_File, _T("%s\n"), strEntry);
		}
		else
		{
			AfxMessageBox(strEntry);
			return false;
		}
	}
	return true;
}


void LogClass::Flush()
{
	std::lock_guard<std::mutex> guard(m_logMutex);
	fflush(m_File);
}

void LogClass::PrintSummary()
{
	CString strTmp;

	_ftprintf(m_File,_T("---------------------------------------------\n"));
	strTmp.Format(_T("[ Summary  ] Passed %lu Steps"), m_uiPass);
	LogText(strTmp, LogLevel::Always, LogType::BLANK_RET);
	strTmp.Format(_T("[ Summary  ] Failed %u Steps"), m_uiFail);
	LogText(strTmp, LogLevel::Always, LogType::BLANK_RET);
	strTmp.Format(_T("[ Summary  ] Warning %u Steps"), m_uiWarn);
	LogText(strTmp, LogLevel::Always, LogType::BLANK_RET);
	constexpr int cStrLength = 128;
	TCHAR szTime[cStrLength];
	TCHAR szDate[cStrLength];
#ifdef _UNICODE
	_wstrtime_s(szTime, cStrLength);
	_wstrdate_s(szDate, cStrLength);
#else
	_strtime_s(szTime, cStrLength);
	_strdate_s(szDate, cStrLength);
#endif

	if (m_uiAbort)
		strTmp.Format(_T("[ Summary  ] Script Aborted !!!"));
	else
		strTmp.Format(_T("[ Summary  ] Completed %s %s"), szTime, szDate);

	LogText(strTmp, LogLevel::Always, LogType::BLANK_RET);
	_ftprintf(m_File,_T("---------------------------------------------\n"));

}

void LogClass::CountResults(const LogType logType, unsigned long lCount)
{
	switch (logType)
	{
		case LogType::PASS:
			m_uiPass += lCount;
			break;
		case LogType::FAIL:
			m_uiFail += lCount;
			break;
		case LogType::WARN:
			m_uiWarn += lCount;
			break;
		case LogType::ABORT:
			m_uiAbort += lCount;
			break;
		default:
			break;
	}
}

void LogClass::ResetCounts()
{
	m_uiFail = 0; // Used in final print Out
	m_uiPass = 0; // .
	m_uiWarn = 0; // .
	m_uiAbort = 0; // .

}

long LogClass::GetFailCount()
{
	return m_uiFail;
}

long LogClass::GetWarnCount()
{
	return m_uiWarn;
}

long LogClass::GetPassCount()
{
	return m_uiPass;
}

long LogClass::GetAbortCount()
{
	return m_uiAbort;
}

void LogClass::SetLogLevel(const LogLevel logLevel)
{
	m_LogLevel = logLevel;
}

LogLevel LogClass::GetLogLevel( )
{
	return m_LogLevel;
}

bool LogClass::convertAndLogString(std::string line)
{
	static const std::regex regExCmd(R"(^\[([^\]]*)\]\[([^\]]*)\]*)");
	if (!line.empty())
	{
		std::smatch m;
		LogType type = LogType::BLANK;
		LogLevel level = LogLevel::Always;
		if (std::regex_search(line, m, regExCmd) && 3 <= m.size())
		{
			auto typeIter = find_if(c_typeMap.begin(), c_typeMap.end(), [m](auto pair)->bool { return pair.second == m[1]; });
			if (c_typeMap.end() != typeIter)
			{
				type = typeIter->first;
			}
			auto levelIter = find_if(c_levelMap.begin(), c_levelMap.end(), [m](auto pair) { return pair.second == m[2]; });
			if (c_levelMap.end() != levelIter)
			{
				level = levelIter->first;
			}
		}

		LogText0(line.c_str(), level);
		return type != LogType::FAIL && type != LogType::ABORT;
	}
	return true;
}

CString LogClass::BuildLogString(const LogLevel logLevel, const LogType logType)
{
	CString strTmp = _T("[????]");

	if (logType != LogType::BLANK_RET)
	{
		auto findIter = c_typeMap.find(logType);
		if (c_typeMap.end() == findIter)
		{
			return _T("[    ]");
		}
		strTmp = (_T("[") + findIter->second + _T("]")).c_str();
	}
	else
	{
		return _T("[    ]");
	}
	auto findIter = c_levelMap.find(logLevel);
	if (c_levelMap.end() != findIter)
	{
		strTmp += (_T("[") + findIter->second + _T("]")).c_str();
	}

	return strTmp;

}