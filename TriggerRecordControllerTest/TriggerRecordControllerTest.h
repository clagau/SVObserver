
// TriggerRecordControllerTest.h: Hauptheaderdatei für die PROJECT_NAME-Anwendung
//

#pragma once

#ifndef __AFXWIN_H__
#error "'stdafx.h' vor dieser Datei für PCH einschließen"
#endif

namespace SvTrcT
{
	// CTriggerRecordControllerTestApp:
	// Siehe TriggerRecordControllerTest.cpp für die Implementierung dieser Klasse
	//

	class CTriggerRecordControllerTestApp : public CWinApp
	{
	public:
		CTriggerRecordControllerTestApp();

		// Überschreibungen
	public:
		virtual BOOL InitInstance();

		// Implementierung

		DECLARE_MESSAGE_MAP()

	private:
	};

	extern CTriggerRecordControllerTestApp theApp;
} // namespace SvTrcT
