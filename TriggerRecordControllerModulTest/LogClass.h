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
	Debug
};

// ***** Type ***********
enum class LogType
{
	PASS = 0,
	FAIL, // 1
	WARN, // 2
	BLANK, // 3
	BLANK_RET, //4
	ABORT, // 5
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

	// Batch count functions.
	void PrintSummary();
	void CountResults(const LogType logType, unsigned long lCount=1);

	void ResetCounts();

	unsigned long GetFailCount() const;	// Gets Fail count for the current script.
	unsigned long GetPassCount() const;	// Gets Pass count for the current script.
	unsigned long GetWarnCount() const;	// Gets Warn count for the current script.
	unsigned long GetAbortCount() const;	// Gets Abort count for the current script.

	void SetLogLevel(const LogLevel logLevel);
	LogLevel GetLogLevel( ) const;

	/// Convert the string, check state and log the string.
	/// \param line [in] log line
	/// \returns bool false, if state fail or abort.
	bool convertAndLogString(std::string line);

private:
	// Variables
	unsigned long m_uiFail;
	unsigned long m_uiPass;
	unsigned long m_uiWarn;
	unsigned long m_uiAbort;
private:
	LogLevel m_LogLevel;
	FILE *m_File;
	std::mutex m_logMutex;
	const std::map<LogType, std::string> c_typeMap = {{LogType::PASS, "PASS"}, {LogType::FAIL, "FAIL"}, {LogType::WARN, "WARN"}, {LogType::ABORT, "ABORT"}, {LogType::BLANK, "    "}};
	const std::map<LogLevel, std::string> c_levelMap = {{LogLevel::Always, "Always*"}, {LogLevel::Error, "Error**"}, {LogLevel::Information_Level1, "Info  1"}, {LogLevel::Information_Level2, "Info  2"}, {LogLevel::Information_Level3, "Info  3"}, {LogLevel::Debug, "Debug  "}};

	CString BuildLogString(const LogLevel logLevel, const LogType logType);
};
