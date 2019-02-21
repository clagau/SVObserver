// TRC_UT.cpp : Definiert den Einstiegspunkt für die Konsolenanwendung.
//

#include "stdafx.h"
#include "TriggerRecordControllerUnitTest.h"
#include "LogClass.h"

#include "TrcTester.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// Das einzige Anwendungsobjekt
CWinApp theApp;
LogClass g_logClass;

int main()
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
			CString tmpString;

			SvTrc::createTriggerRecordControllerInstance(SvTrc::TRC_DataType::Local);
			//if (g_logClass.Open(_T("log.txt"), false))
			if (g_logClass.Open())
			{
				MIL_ID appId = MappAlloc(M_DEFAULT, M_NULL);
				if (M_NULL != appId)
				{
					{
						const TrcTesterConfiguration config(g_logClass);
						TrcTester trc_ut(config, g_logClass);
						tmpString.Format(_T("Testing TRC (%d unittests, %ld iterations per step)"), config.getNumberOfUnittests(), config.getNoOfRepetitionsPerStep());
						g_logClass.LogText0(tmpString, LogLevel::Information_Level1);
						bool all_ok = true;
						for (int i = 0; i < config.getNumberOfUnittests(); i++)
						{
							bool ok = trc_ut.fullTest();
							tmpString.Format(_T("%2d of %d tests complete: %sok."), i + 1, config.getNumberOfUnittests(), ok ? L"" : L"NOT ");
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
