// TRC_UT.cpp : Definiert den Einstiegspunkt für die Konsolenanwendung.
//

#include "stdafx.h"
#include <filesystem>
#include <fstream>

#include "SVLibrary\SVOINIClass.h"
#include "TriggerRecordController\ITriggerRecordControllerR.h"
#include "TriggerRecordControllerModulTest\LogClass.h"
#include "TriggerRecordControllerModulTest\TesterFunction.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// Das einzige Anwendungsobjekt
CWinApp theApp;
LogClass g_logClass;

int main(int argc, char* argv[])
{
	int nRetCode = 0;

	HMODULE hModule = ::GetModuleHandle(nullptr);

	if (hModule != nullptr)
	{
		// MFC initialisieren. Bei Fehlschlag Fehlermeldung aufrufen.
		if (!AfxWinInit(hModule, nullptr, ::GetCommandLine(), 0))
		{
			wprintf(L"Schwerwiegender Fehler bei der MFC-Initialisierung\n");
			nRetCode = 1;
		}
		else
		{
			_set_error_mode(_OUT_TO_MSGBOX);

			CString testName;
			CString logName;
			if (1 < argc)
			{
				testName = argv[1];
			}
			bool isLogFileOpen = false;
			if (!testName.IsEmpty())
			{
				CString logFile;
				logFile.Format("%s_log.txt", testName);
				isLogFileOpen = g_logClass.Open(logFile,false);
			}
			else
			{
				isLogFileOpen = g_logClass.Open();
			}


			if (isLogFileOpen)
			{
				MIL_ID appId = MappAlloc(M_DEFAULT, M_NULL);
				if (M_NULL != appId)
				{
					SvTrc::createTriggerRecordControllerInstance(SvTrc::TRC_DataType::Reader);
					// Disable MIL error message to be displayed by MIL
					MappControl(M_ERROR, M_PRINT_DISABLE);

					{
						CString tmpString;
						tmpString.Format(_T("Testing TRC-Reader(%s)"), testName);
						g_logClass.LogText0(tmpString, LogLevel::Information_Level1);
						TrcTesterConfiguration config(g_logClass);
						bool retReaderTest = readerTest(testName, g_logClass, 100, config.getTestData(), true);
						g_logClass.Log("Finished readerTest", retReaderTest ? LogLevel::Information_Level1 : LogLevel::Error, retReaderTest ? LogType::PASS : LogType::FAIL, __LINE__, testName);
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
		}
    }
    else
    {
		nRetCode = 1;
    }

    return nRetCode;
}