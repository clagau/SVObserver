
// TriggerRecordControllerTestDlg.h: Headerdatei
//

#pragma once
#include "resource.h"
#include "ToolObject.h"
#include "ObjectInterfaces/ITriggerRecordControllerRW.h"
#include "SVMFCControls/ObjectsListBox.h"
#include "SVOGuiUtility/PictureDisplay.h"

namespace SvTrcT
{
	typedef std::shared_ptr< ToolObject > ToolObjectPtr;

	// CTriggerRecordControllerTestDlg-Dialogfeld
	class CTriggerRecordControllerTestDlg : public CDialogEx
	{
		// Konstruktion
	public:
		explicit CTriggerRecordControllerTestDlg(bool isReader, CWnd* pParent = NULL);	// Standardkonstruktor
		~CTriggerRecordControllerTestDlg();

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
		void OnResetUpdate();
		void OnReadyUpdate();
		void OnNewTrId(SvOi::TrEventData data);
		LRESULT OnGetCallback(WPARAM wParam, LPARAM lParam);
		void OnSelchangeToolSetList();
		void OnCheckHidden();

		SvOi::ITRCImagePtr LoadMainImage(const CString& Path);

		void updateControls(bool shouldUpdateToolList = true);

		void updateButton();

		void resetController();
		void updateToolList();
		void updateImageCombo();
		void updateValueCombo();
		void OnResetTRCData();

	private:
		SvOi::ITriggerRecordControllerR* m_pTRC = SvOi::getTriggerRecordControllerRInstance();
		bool m_bResetStart = false;
		bool m_isEdit = false;
		int m_trNumbers = 10;
		int m_mainWidth = 640;
		int m_mainHeigth = 480;
		int m_trIDToDisplay = -1;
		BOOL m_isCopyTR = true;
		BOOL m_isHidden = false;
		CString m_lastID = _T("-1");
		CString m_valueText;
		std::vector<ToolObjectPtr> m_toolList;
		CString m_toolDescription = _T("<No description>");
		uint32_t m_mainId = SvDef::InvalidObjectId;
		SvMc::ObjectsListBox m_toolListBox;
		CComboBox	m_ToolSelectCombo;
		CComboBox	m_ImageCombo;
		CComboBox	m_ValueCombo;
		SvOgu::PictureDisplay m_dialogImage;
		const int m_inspectionPos = 0;
		bool m_isReader = false;
		SvOi::RAIIPtr m_resetCallbackRAII;
		SvOi::RAIIPtr m_readyCallbackRAII;
		SvOi::RAIIPtr m_newTrIdCallbackRAII;
		std::atomic_bool m_isTRCValid = true;
	};

	static uint32_t getNextObjectId()
	{
		static uint32_t nextId = 1;
		return nextId++;
	}
} // namespace SvTrcT
