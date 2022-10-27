// TRC_UT.cpp : Definiert den Einstiegspunkt für die Konsolenanwendung.
//

#include "stdafx.h"
#include "LogClass.h"

#include "TrcTester.h"
#include "TrcTesterConfiguration.h"
#include "SVLibrary\SVOINIClass.h"
#include "ObjectInterfaces/ITriggerRecordControllerR.h"


LogClass g_logClass;

int main(int argc, const char* argv[])
{
	int nRetCode = 0;

	_set_error_mode(_OUT_TO_MSGBOX);

	std::string iniFileName;
	if (1 < argc)
	{
		iniFileName = argv[1];
	}

	SvLib::SVOINIClass testIni(iniFileName.c_str());

	bool isLocal = (testIni.GetValueInt(_T("General"), _T("IsLocal"), 1) != 0);

	SvOi::createTriggerRecordControllerInstance(isLocal ? SvOi::TRC_DataType::Local : SvOi::TRC_DataType::Writer);
	//if (g_logClass.Open(_T("log.txt"), false))
	if (g_logClass.Open())
	{
		MIL_ID appId = MappAlloc(M_DEFAULT, M_NULL);
		if (M_NULL != appId)
		{
			// Disable MIL error message to be displayed by MIL
			MappControl(M_ERROR, M_PRINT_DISABLE);

			{
				LogLevel logLevel = static_cast<LogLevel>(testIni.GetValueInt(_T("General"), _T("LogLevel"), 3));
				g_logClass.SetLogLevel(logLevel);

				TrcTesterConfiguration config(g_logClass, testIni, isLocal);
				int maxSpecfiyBufferFactor = config.getMaxSpecifyBufferFactor();
				TrcTester trc_ut(config, g_logClass);
				std::string tmpString = std::format(_T("Testing TRC (Type:{}, Runs:{}, {} iterations per step, {} number of add one records, {} number of keep free records, maxSpecfiyBufferFactor = {})"), isLocal ? _T("Local") : _T("Writer"), config.getNumberOfRuns(), config.getNoOfRepetitionsPerStep(), config.getNumberOfRecordsAddOne(), config.getNumberOfKeepFreeRecords(), maxSpecfiyBufferFactor);
				g_logClass.LogText0(tmpString.c_str(), LogLevel::Information_Level1);
				for (int i = 0; i < config.getNumberOfRuns(); i++)
				{
					bool ok = trc_ut.fullTest();
					tmpString = std::format(_T("{:2d} of {} tests complete: {}OK."), i + 1, config.getNumberOfRuns(), ok ? _T("") : _T("NOT "));
					g_logClass.LogText(tmpString.c_str(), LogLevel::Information_Level1, ok ? LogType::PASS : LogType::FAIL);
					g_logClass.LogText("--------------------------------------\n", LogLevel::Information_Level1, LogType::BLANK_RET);
					g_logClass.Flush();
				}
			}
			MappFree(appId);
		}
		else
		{
			g_logClass.LogText(_T("MIL System could not be allocated. Aborting."), LogLevel::Error, LogType::ABORT);
		}

		g_logClass.PrintSummary();
		g_logClass.Close();
	}
	else
	{
		wprintf(L"Error opening log-file\n");
	}

    return nRetCode;
}
