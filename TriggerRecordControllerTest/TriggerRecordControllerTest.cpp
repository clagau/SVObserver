
// TriggerRecordControllerTest.cpp: Definiert das Klassenverhalten f�r die Anwendung.
//

#include "stdafx.h"
#include "TriggerRecordControllerTest.h"
#include "TriggerRecordControllerTestDlg.h"
#include "TriggerRecordController\ITriggerRecordControllerRW.h"
#include "TriggerRecordController\ITriggerRecordRW.h"
#include <mil.h>
#include "SVMatroxLibrary\SVMatroxBufferCreateStruct.h"
#include "SVMatroxLibrary\SVMatroxResourceMonitor.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace SvTrcT
{
	// CTriggerRecordControllerTestApp

	BEGIN_MESSAGE_MAP(CTriggerRecordControllerTestApp, CWinApp)
		ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
	END_MESSAGE_MAP()


	// CTriggerRecordControllerTestApp-Erstellung

	CTriggerRecordControllerTestApp::CTriggerRecordControllerTestApp()
	{
		// Neustart-Manager unterst�tzen
		m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;

		// TODO: Hier Code zur Konstruktion einf�gen
		// Alle wichtigen Initialisierungen in InitInstance positionieren

		//The next call is only to avoid a crash at the end of the application. 
		//Reason of the crash was that SVMatroxResourceMonitor was destructed before the ImageBufferController, but this need it in its destructor.
		//If the singleton of SVMatroxResourceMonitor created before of the singleton of ImageBufferController the destruction it in the right order.
		//In Release-Mode this call do nothing.
		SVMatroxResourceMonitor::SVAutoLock autoLock;
		SVMatroxResourceMonitor::GetAutoLock(autoLock);
	}


	// Das einzige CTriggerRecordControllerTestApp-Objekt

	CTriggerRecordControllerTestApp theApp;


	// CTriggerRecordControllerTestApp-Initialisierung

	BOOL CTriggerRecordControllerTestApp::InitInstance()
	{
		// InitCommonControlsEx() ist f�r Windows XP erforderlich, wenn ein Anwendungsmanifest
		// die Verwendung von ComCtl32.dll Version 6 oder h�her zum Aktivieren
		// von visuellen Stilen angibt.  Ansonsten treten beim Erstellen von Fenstern Fehler auf.
		INITCOMMONCONTROLSEX InitCtrls;
		InitCtrls.dwSize = sizeof(InitCtrls);
		// Legen Sie dies fest, um alle allgemeinen Steuerelementklassen einzubeziehen,
		// die Sie in Ihrer Anwendung verwenden m�chten.
		InitCtrls.dwICC = ICC_WIN95_CLASSES;
		InitCommonControlsEx(&InitCtrls);

		CWinApp::InitInstance();

		MIL_ID AppId = MappAlloc(M_DEFAULT, M_NULL);
		if (AppId == M_NULL)
		{
			AfxMessageBox(_T("Mil System could not be allocated. RRSView will be closed"));
			return FALSE;
		}

		AfxEnableControlContainer();

		// Shell-Manager erstellen, falls das Dialogfeld
		// Shellbaumansicht- oder Shelllistenansicht-Steuerelemente enth�lt.
		CShellManager *pShellManager = new CShellManager;

		//Visuellen Manager "Windows Native" aktivieren, um Designs f�r MFC-Steuerelemente zu aktivieren
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));

		// Standardinitialisierung
		// Wenn Sie diese Funktionen nicht verwenden und die Gr��e
		// der ausf�hrbaren Datei verringern m�chten, entfernen Sie
		// die nicht erforderlichen Initialisierungsroutinen.
		// �ndern Sie den Registrierungsschl�ssel, unter dem Ihre Einstellungen gespeichert sind.
		// TODO: �ndern Sie diese Zeichenfolge entsprechend,
		// z.B. zum Namen Ihrer Firma oder Organisation.
		SetRegistryKey(_T("Vom lokalen Anwendungs-Assistenten generierte Anwendungen"));
		auto bRet =  MessageBox(nullptr, _T("Should test in LocalMode (Yes), WriterMode(No) or ReaderMode (Cancel)?"), _T("Start"), MB_YESNOCANCEL);
		bool readerMode = (IDCANCEL == bRet);
		SvTrc::createTriggerRecordControllerInstance(readerMode ? SvTrc::TRC_DataType::Reader : (IDYES == bRet ? SvTrc::TRC_DataType::Local: SvTrc::TRC_DataType::Writer));

		CTriggerRecordControllerTestDlg dlg(readerMode);
		m_pMainWnd = &dlg;
		INT_PTR nResponse = dlg.DoModal();
		if (nResponse == IDOK)
		{
			// TODO: F�gen Sie hier Code ein, um das Schlie�en des
			//  Dialogfelds �ber "OK" zu steuern
		}
		else if (nResponse == IDCANCEL)
		{
			// TODO: F�gen Sie hier Code ein, um das Schlie�en des
			//  Dialogfelds �ber "Abbrechen" zu steuern
		}
		else if (nResponse == -1)
		{
			TRACE(traceAppMsg, 0, "Warnung: Fehler bei der Dialogfelderstellung, unerwartetes Beenden der Anwendung.\n");
			TRACE(traceAppMsg, 0, "Warnung: Wenn Sie MFC-Steuerelemente im Dialogfeld verwenden, ist #define _AFX_NO_MFC_CONTROLS_IN_DIALOGS nicht m�glich.\n");
		}

		// Den oben erstellten Shell-Manager l�schen.
		if (pShellManager != NULL)
		{
			delete pShellManager;
		}

#ifndef _AFXDLL
		ControlBarCleanUp();
#endif

		MappFree(AppId);
		// Da das Dialogfeld geschlossen wurde, FALSE zur�ckliefern, sodass wir die
		//  Anwendung verlassen, anstatt das Nachrichtensystem der Anwendung zu starten.
		return FALSE;
	}
} // namespace SvTrcT
