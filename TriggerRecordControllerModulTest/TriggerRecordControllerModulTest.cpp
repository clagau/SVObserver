// TRC_UT.cpp : Definiert den Einstiegspunkt für die Konsolenanwendung.
//

#include "stdafx.h"
#include "LogClass.h"

#include "TrcTester.h"
#include "SVLibrary\SVOINIClass.h"
#include "TriggerRecordController\ITriggerRecordControllerR.h"


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

			CString iniFileName;
			if (1 < argc)
			{
				iniFileName = argv[1];
			}

			SvLib::SVOINIClass testIni(iniFileName);

			bool isLocal = (testIni.GetValueInt(_T("General"), _T("IsLocal"), 1) != 0);

			SvTrc::createTriggerRecordControllerInstance(isLocal ? SvTrc::TRC_DataType::Local : SvTrc::TRC_DataType::Writer);
			//if (g_logClass.Open(_T("log.txt"), false))
			if (g_logClass.Open())
			{
				MIL_ID appId = MappAlloc(M_DEFAULT, M_NULL);
				if (M_NULL != appId)
				{
					// Disable MIL error message to be displayed by MIL
					MappControl(M_ERROR, M_PRINT_DISABLE);

					{
						const TrcTesterConfiguration config(g_logClass, testIni);
						TrcTester trc_ut(config, g_logClass);
						CString tmpString;
						tmpString.Format(_T("Testing TRC (Type:%s, Runs:%d, %d iterations per step)"), isLocal ? _T("Local") : _T("Writer"), config.getNumberOfRuns(), config.getNoOfRepetitionsPerStep());
						g_logClass.LogText0(tmpString, LogLevel::Information_Level1);
						bool all_ok = true;
						for (int i = 0; i < config.getNumberOfRuns(); i++)
						{
							bool ok = trc_ut.fullTest();
							tmpString.Format(_T("%2d of %d tests complete: %sok."), i + 1, config.getNumberOfRuns(), ok ? L"" : L"NOT ");
							g_logClass.LogText(tmpString, LogLevel::Information_Level1, ok ? LogType::PASS : LogType::FAIL);
							g_logClass.LogText("--------------------------------------\n", LogLevel::Information_Level1, LogType::BLANK);
							g_logClass.Flush();
							all_ok &= ok;
						}
					}
					MappFree(appId);
				}
				else
				{
					g_logClass.LogText(_T("MIL System could not be allocated. Aborting."), LogLevel::Always, LogType::ABORT);
				}

				g_logClass.PrintSummary();
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
