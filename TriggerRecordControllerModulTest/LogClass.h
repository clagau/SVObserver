//*****************************************************************************
/// \copyright COPYRIGHT (c) 2019 by Seidenader Maschinenbau GmbH
/// \file LogClass.h
/// All Rights Reserved
//*****************************************************************************
/// Class to log result of the tests.
/// This class will be added also to TriggerRecordControllerReaderModuleTest-project.
//******************************************************************************

#pragma once
namespace SvStl
{
class MessageContainer;
}

// ***** Logging Level Defines ******
enum class LogLevel
{
	Always = 0,
	Error = 1,
	Information_Level1,
	Information_Level2,
	Information_Level3,
	Status
};

// ***** Type ***********
enum class LogType
{
	PASS = 0,
	FAIL, // 1
	WARN, // 2
	BLANK, // 3
	ABORT, // 4
};

class LogClass  
{
public:
	// Functions
	LogClass();
	virtual ~LogClass();

	// Log Open and Close
	bool Open(CString strFileName, bool bAppend);
	bool Open();
	void Close();
	void Flush();
	bool IsOpen();

	// Log functions.
	bool Log(LPCTSTR strEntry, const LogLevel logLevel, const LogType logType, int lineNumber, LPCTSTR strTestName = _T(""));
	bool LogText(LPCTSTR strEntry, const LogLevel logLevel, const LogType logType);
	bool logException(LPCTSTR strEntry, const SvStl::MessageContainer& rExp, int lineNumber, LPCTSTR strTestName = _T(""), const LogLevel logLevel = LogLevel::Error, const LogType logType = LogType::FAIL);
	bool LogText0(LPCTSTR strEntry, const LogLevel logLevel=LogLevel::Always);
	void Remark(LPCTSTR strEntry);

	// Batch count functions.
	void PrintSummary();
	void CountResults(const LogType logType, unsigned long lCount=1);

	void ResetCounts();

	long GetFailCount();	// Gets Fail count for the current script.
	long GetPassCount();	// Gets Pass count for the current script.
	long GetWarnCount();	// Gets Warn count for the current script.
	long GetAbortCount();	// Gets Abort count for the current script.

	void SetLogLevel(const LogLevel logLevel);
	LogLevel GetLogLevel( );

private:
	// Variables
	unsigned int m_uiFail;
	unsigned long m_uiPass;
	unsigned int m_uiWarn;
	unsigned int m_uiAbort;
private:
	LogLevel m_LogLevel;
	FILE *m_File;
	std::mutex m_logMutex;

	CString BuildLogString(const LogLevel logLevel, const LogType logType);
};
