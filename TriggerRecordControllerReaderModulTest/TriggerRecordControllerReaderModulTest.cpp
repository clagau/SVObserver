// TRC_UT.cpp : Definiert den Einstiegspunkt für die Konsolenanwendung.
//

#include "stdafx.h"
#include <filesystem>
#include <fstream>

#include "ObjectInterfaces/ITriggerRecordControllerR.h"
#include "TriggerRecordControllerModulTest\LogClass.h"
#include "TriggerRecordControllerModulTest\TesterFunction.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// Das einzige Anwendungsobjekt
LogClass g_logClass;

int main(int argc, char* argv[])
{
	int nRetCode = 0;

	_set_error_mode(_OUT_TO_MSGBOX);

	std::string testName;
	LogLevel logLevel = LogLevel::Always;
	if (1 < argc)
	{
		testName = argv[1];
	}
	if (2 < argc)
	{
		logLevel = LogLevel(atoi(argv[2]));
	}
	bool isLogFileOpen = false;
	if (!testName.empty())
	{
		std::string logFile{testName};
		logFile += _T("_log.txt");
		isLogFileOpen = g_logClass.Open(logFile,false);
	}
	else
	{
		isLogFileOpen = g_logClass.Open();
	}


	if (isLogFileOpen)
	{
		g_logClass.SetLogLevel(logLevel);
		MIL_ID appId = MappAlloc(M_DEFAULT, M_NULL);
		if (M_NULL != appId)
		{
			SvOi::createTriggerRecordControllerInstance(SvOi::TRC_DataType::Reader);
			// Disable MIL error message to be displayed by MIL
			MappControl(M_ERROR, M_PRINT_DISABLE);

			{
				std::string tmpString{_T("Testing TRC-Reader(")};
				tmpString += testName + ')';
				g_logClass.LogText0(tmpString.c_str(), LogLevel::Information_Level1);
				TrcTesterConfiguration config(g_logClass);
				bool retReaderTest = readerTest(testName.c_str(), {g_logClass, 100, config.getTestData(), true });
				g_logClass.Log("Finished readerTest", retReaderTest ? LogLevel::Information_Level1 : LogLevel::Error, retReaderTest ? LogType::PASS : LogType::FAIL, __LINE__, testName.c_str());
			}
			MappFree(appId);
		}
		else
		{
			g_logClass.LogText(_T("MIL System could not be allocated. Aborting."), LogLevel::Error, LogType::ABORT);
		}

		//g_logClass.PrintSummary();
		g_logClass.Close();
	}
	else
	{
		wprintf(L"Error opening log-file\n");
	}

    return nRetCode;
}