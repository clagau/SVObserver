
// TriggerRecordControllerTestDlg.h: Headerdatei
//

#pragma once
#include "TriggerRecordController\ITriggerRecordControllerRW.h"
#include "ToolObject.h"
#include "SVMFCControls\ObjectsListBox.h"
#include "SVOGui\PictureDisplay.h"

namespace SvTRCT
{
	typedef std::shared_ptr< ToolObject > ToolObjectPtr;

	// CTriggerRecordControllerTestDlg-Dialogfeld
	class CTriggerRecordControllerTestDlg : public CDialogEx
	{
		// Konstruktion
	public:
		CTriggerRecordControllerTestDlg(CWnd* pParent = NULL);	// Standardkonstruktor

	// Dialogfelddaten
#ifdef AFX_DESIGN_TIME
		enum { IDD = IDD_TRIGGERRECORDCONTROLLERTEST_DIALOG };
#endif

	protected:
		virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV-Unterstützung


	// Implementierung
	protected:
		HICON m_hIcon;

		// Generierte Funktionen für die Meldungstabellen
		virtual BOOL OnInitDialog();
		afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
		afx_msg void OnPaint();
		afx_msg HCURSOR OnQueryDragIcon();
		DECLARE_MESSAGE_MAP()

		void OnBnClickedButtonReset();
		void OnBnClickedButtonAdd();
		void OnBnClickedButtonRemove();
		void OnInputChanged();
		void OnTrigger();
		void OnSetImage();
		void OnToolTypeChanged();

		bool LoadMainImage(SvTRC::ITriggerRecordRWPtr triggerRecord, const CString& Path);

		void updateControls();

		void updateButton();

		void resetController();
		void updateToolList();
		void updateImageCombo();

	private:
		SvTRC::ITriggerRecordControllerRW& m_recordController = SvTRC::getTriggerRecordControllerRWInstance();
		bool m_bResetStart = false;
		bool m_isEdit = false;
		int m_trNumbers = 10;
		int m_mainWidth = 640;
		int m_mainHeigth = 480;
		int m_trIDToDisplay = -1;
		BOOL m_isCopyTR = true;
		CString m_lastID = _T("-1");
		std::vector<ToolObjectPtr> m_toolList;
		CString m_toolDescription = _T("<No description>");
		GUID m_mainGuid = GUID_NULL;
		SvMc::ObjectsListBox m_toolListBox;
		CComboBox	m_ToolSelectCombo;
		CComboBox	m_ImageCombo;
		SvOg::PictureDisplay m_dialogImage;
		const int m_inspectionPos = 0;
	};
} // namespace SvTRCT