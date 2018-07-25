// TRC_UT.cpp : Definiert den Einstiegspunkt für die Konsolenanwendung.
//

#include "stdafx.h"
#include "TriggerRecordControllerUnitTest.h"

#include "TrcTester.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// Das einzige Anwendungsobjekt
CWinApp theApp;

bool unitTest(const struct TrcTesterConfiguration &rConfig);

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
			const struct TrcTesterConfiguration config;
			wprintf(L"Testing TRC (%d unittests, %ld iterations per step):\n", config.m_NumberOfUnittests, config.m_NoOfRepetitionsPerStep);
			bool all_ok = true;
			for (int i = 0; i < config.m_NumberOfUnittests; i++)
			{
				bool ok = unitTest(config);
				wprintf(L"%2d of %d tests complete: %sok.\n", i + 1 , config.m_NumberOfUnittests, ok ? L"" : L"NOT ");
				all_ok &= ok;
			}

			wprintf(L"%d tests complete: %s\n", config.m_NumberOfUnittests, all_ok ? L"ok." : L"PROBLEMS occurred.");
		}
    }
    else
    {
		nRetCode = 1;
    }

    return nRetCode;
}


bool unitTest(const struct TrcTesterConfiguration &config)
{
	TrcTester trc_ut(config);

	if (!trc_ut.hasAppId())
	{
		AfxMessageBox(_T("MIL System could not be allocated. Aborting."));
		return false;
	}
	

	for (int i = 0; i < config.m_NoOfRepetitionsPerStep; i++)
	{
		bool CreateInspectionsOk = trc_ut.createInspections();

		if (!CreateInspectionsOk)
		{
			wprintf(L"CreateInspections(%d, %d) ERROR on %d. iteration!\n", config.m_NumberOfImagesPerInspection, config.m_NumberOfInspections,i);
			return false;
		}
	}
	bool ok = true;

	for (int i = 0; i < config.m_NoOfRepetitionsPerStep; i++)
	{
		bool inspectionBuffersOk = trc_ut.setInspectionBuffers();
		if (!inspectionBuffersOk)
		{
			wprintf(L"setInspectionBuffers(%d, %d) ERROR on %d. iteration!\n", config.m_NumberOfImagesPerInspection, config.m_NumberOfInspections, i);
			return false;
		}

		bool independentBuffersOk = trc_ut.setIndependentBuffers();
		if (!independentBuffersOk)
		{
			wprintf(L"setIndependentBuffers(%d, %d) ERROR on %d. iteration!\n", config.m_NumberOfImagesPerInspection, config.m_NumberOfInspections, i);
			return false;
		}

	}

	return true;
}

