
// TriggerRecordControllerTest.h: Hauptheaderdatei f�r die PROJECT_NAME-Anwendung
//

#pragma once

#ifndef __AFXWIN_H__
#error "'stdafx.h' vor dieser Datei f�r PCH einschlie�en"
#endif

namespace SvTrcT
{
	// CTriggerRecordControllerTestApp:
	// Siehe TriggerRecordControllerTest.cpp f�r die Implementierung dieser Klasse
	//

	class CTriggerRecordControllerTestApp : public CWinApp
	{
	public:
		CTriggerRecordControllerTestApp();

		// �berschreibungen
	public:
		virtual BOOL InitInstance();

		// Implementierung

		DECLARE_MESSAGE_MAP()

	private:
	};

	extern CTriggerRecordControllerTestApp theApp;
} // namespace SvTrcT
