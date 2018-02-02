
// TriggerRecordControllerTestDlg.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "TriggerRecordControllerTest.h"
#include "TriggerRecordControllerTestDlg.h"
#include "SVUtilityLibrary\NameGuidList.h"
#include "SVMatroxLibrary\SVMatroxBufferCreateStruct.h"
#include "CopyTool.h"
#include "WindowTool.h"
#include "TriggerRecordController\ITriggerRecordRW.h"
#include "SVMatroxLibrary\SVMatroxBufferInterface.h"
#include "RotationTool.h"
#include "DeactivedTool.h"
#pragma warning( push )
#pragma warning( disable : 4800 ) 
#include "SVProtoBuf\TriggerRecordController.pb.h"
#pragma warning( pop )

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace SvTRCT
{
	// CAboutDlg-Dialogfeld für Anwendungsbefehl "Info"

	class CAboutDlg : public CDialogEx
	{
	public:
		CAboutDlg();

		// Dialogfelddaten
#ifdef AFX_DESIGN_TIME
		enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
		virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung

	// Implementierung
	protected:
		DECLARE_MESSAGE_MAP()
	};

	CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
	{
	}

	void CAboutDlg::DoDataExchange(CDataExchange* pDX)
	{
		CDialogEx::DoDataExchange(pDX);
	}

	BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
	END_MESSAGE_MAP()


	// CTriggerRecordControllerTestDlg-Dialogfeld



	CTriggerRecordControllerTestDlg::CTriggerRecordControllerTestDlg(CWnd* pParent /*=NULL*/)
		: CDialogEx(IDD_TRIGGERRECORDCONTROLLERTEST_DIALOG, pParent)
	{
		m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
		UuidCreateSequential(&m_mainGuid);
	}

	void CTriggerRecordControllerTestDlg::DoDataExchange(CDataExchange* pDX)
	{
		CDialogEx::DoDataExchange(pDX);

		//{{AFX_DATA_MAP(CTriggerRecordControllerTestDlg)
		DDX_Text(pDX, IDC_EDIT_MAINIMAGE_WIDTH, m_mainWidth);
		DDV_MinMaxLong(pDX, m_mainWidth, 10, 2000);
		DDX_Text(pDX, IDC_EDIT_MAINIMAGE_HEIGHT, m_mainHeigth);
		DDV_MinMaxLong(pDX, m_mainHeigth, 10, 2000);
		DDX_Text(pDX, IDC_EDIT_TRNUMBERS, m_trNumbers);
		DDV_MinMaxLong(pDX, m_trNumbers, 1, 50);
		DDX_Text(pDX, IDC_EDIT_RECORD_ID, m_trIDToDisplay);
		DDV_MinMaxLong(pDX, m_trIDToDisplay, -1, 500000);
		DDX_Text(pDX, IDC_STATIC_LASTID, m_lastID);
		DDX_Text(pDX, IDC_TEXT_TOOL_DESCRIPTION, m_toolDescription);
		DDX_Control(pDX, IDC_TOOLSET_LIST, m_toolListBox);
		DDX_Control(pDX, IDC_COMBO_TOOLTYPE, m_ToolSelectCombo);
		DDX_Control(pDX, IDC_COMBO_IMAGE, m_ImageCombo);
		DDX_Control(pDX, IDC_DIALOGIMAGE, m_dialogImage);
		DDX_Check(pDX, IDC_CHECK_COPY_IMAGE, m_isCopyTR);
		//}}AFX_DATA_MAP
	}

	BEGIN_MESSAGE_MAP(CTriggerRecordControllerTestDlg, CDialogEx)
		ON_WM_SYSCOMMAND()
		ON_WM_PAINT()
		ON_WM_QUERYDRAGICON()
		ON_BN_CLICKED(IDC_BUTTON_RESET, OnBnClickedButtonReset)
		ON_BN_CLICKED(IDC_BUTTON_ADD, OnBnClickedButtonAdd)
		ON_BN_CLICKED(IDC_BUTTON_REMOVE, OnBnClickedButtonRemove)
		ON_BN_CLICKED(IDC_BUTTON_TRIGGER, OnTrigger)
		ON_BN_CLICKED(IDC_BUTTON_SETIMAGE, OnSetImage)
		ON_CBN_SELCHANGE(IDC_COMBO_TOOLTYPE, OnToolTypeChanged)
		ON_EN_CHANGE(IDC_EDIT_MAINIMAGE_HEIGHT, OnInputChanged)
		ON_EN_CHANGE(IDC_EDIT_MAINIMAGE_WIDTH, OnInputChanged)
		ON_EN_CHANGE(IDC_EDIT_TRNUMBERS, OnInputChanged)
	END_MESSAGE_MAP()


	// CTriggerRecordControllerTestDlg-Meldungshandler

	BOOL CTriggerRecordControllerTestDlg::OnInitDialog()
	{
		CDialogEx::OnInitDialog();

		// Hinzufügen des Menübefehls "Info..." zum Systemmenü.

		// IDM_ABOUTBOX muss sich im Bereich der Systembefehle befinden.
		ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
		ASSERT(IDM_ABOUTBOX < 0xF000);

		CMenu* pSysMenu = GetSystemMenu(FALSE);
		if (pSysMenu != NULL)
		{
			BOOL bNameValid;
			CString strAboutMenu;
			bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
			ASSERT(bNameValid);
			if (!strAboutMenu.IsEmpty())
			{
				pSysMenu->AppendMenu(MF_SEPARATOR);
				pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
			}
		}

		// Symbol für dieses Dialogfeld festlegen.  Wird automatisch erledigt
		//  wenn das Hauptfenster der Anwendung kein Dialogfeld ist
		SetIcon(m_hIcon, TRUE);			// Großes Symbol verwenden
		SetIcon(m_hIcon, FALSE);		// Kleines Symbol verwenden

		// TODO: Hier zusätzliche Initialisierung einfügen
		SvPB::InspectionList inspList;
		inspList.add_inspectionid();
		m_recordController.setInspections(inspList);
		resetController();
		updateControls();
		m_ToolSelectCombo.SetCurSel(0);
		m_ImageCombo.SetCurSel(0);
		OnToolTypeChanged();
		m_dialogImage.AddTab("Image");

		return TRUE;  // TRUE zurückgeben, wenn der Fokus nicht auf ein Steuerelement gesetzt wird
	}

	void CTriggerRecordControllerTestDlg::OnSysCommand(UINT nID, LPARAM lParam)
	{
		if ((nID & 0xFFF0) == IDM_ABOUTBOX)
		{
			CAboutDlg dlgAbout;
			dlgAbout.DoModal();
		}
		else
		{
			CDialogEx::OnSysCommand(nID, lParam);
		}
	}

	// Wenn Sie dem Dialogfeld eine Schaltfläche "Minimieren" hinzufügen, benötigen Sie
	//  den nachstehenden Code, um das Symbol zu zeichnen.  Für MFC-Anwendungen, die das 
	//  Dokument/Ansicht-Modell verwenden, wird dies automatisch ausgeführt.

	void CTriggerRecordControllerTestDlg::OnPaint()
	{
		if (IsIconic())
		{
			CPaintDC dc(this); // Gerätekontext zum Zeichnen

			SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

			// Symbol in Clientrechteck zentrieren
			int cxIcon = GetSystemMetrics(SM_CXICON);
			int cyIcon = GetSystemMetrics(SM_CYICON);
			CRect rect;
			GetClientRect(&rect);
			int x = (rect.Width() - cxIcon + 1) / 2;
			int y = (rect.Height() - cyIcon + 1) / 2;

			// Symbol zeichnen
			dc.DrawIcon(x, y, m_hIcon);
		}
		else
		{
			CDialogEx::OnPaint();
		}
	}

	// Die System ruft diese Funktion auf, um den Cursor abzufragen, der angezeigt wird, während der Benutzer
	//  das minimierte Fenster mit der Maus zieht.
	HCURSOR CTriggerRecordControllerTestDlg::OnQueryDragIcon()
	{
		return static_cast<HCURSOR>(m_hIcon);
	}

	void CTriggerRecordControllerTestDlg::OnBnClickedButtonReset()
	{
		BOOL updateState = UpdateData();

		if (updateState)
		{
			resetController();
			m_isEdit = false;
			updateButton();
		}
		UpdateData(false);
	}

	void CTriggerRecordControllerTestDlg::OnBnClickedButtonAdd()
	{
		GUID guid;
		UuidCreateSequential(&guid);
		ToolObjectPtr pTool = nullptr;

		switch (m_ToolSelectCombo.GetCurSel())
		{
		case 0:
			pTool = ToolObjectPtr(new CopyTool(guid));
			break;
		case 1:
			pTool = ToolObjectPtr(new WindowTool(guid));
			break;
		case 2:
			pTool = ToolObjectPtr(new RotationTool(guid));
			break;
		case 3:
			pTool = ToolObjectPtr(new DeactivedTool(guid));
			break;
		default:
			MessageBoxA("Invalid Tool-type selected");
			break;
		}
		
		if (nullptr != pTool)
		{
			int index = m_toolListBox.GetCurSel();
			if (0 <= index && m_toolList.size() >= index)
			{
				m_toolList.insert(m_toolList.begin() + index, pTool);
			}
			else
			{
				m_toolList.push_back(pTool);
			}
			m_isEdit = true;
		}

		updateControls();
	}

	void CTriggerRecordControllerTestDlg::OnBnClickedButtonRemove()
	{
		int index = m_toolListBox.GetCurSel();
		if (0 <= index && m_toolList.size() > index)
		{
			//delete m_toolList[index];
			m_toolList.erase(m_toolList.begin() + index);
			m_isEdit = true;
		}
		updateControls();
	}

	void CTriggerRecordControllerTestDlg::OnInputChanged()
	{
		m_isEdit = true;
		updateButton();
	}

	void CTriggerRecordControllerTestDlg::OnTrigger()
	{
		CFileDialog dlg(true, nullptr, nullptr, OFN_HIDEREADONLY | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR, "Image Files(*.bmp)|*.bmp||", this, 0, false);
		dlg.m_ofn.lpstrTitle = _T("Select Main Image");

		if (IDOK != dlg.DoModal())			// Start the FileDialog
		{									
			return;
		}

		int lastTRid = m_recordController.getLastTRId(m_inspectionPos);
		auto& lastTriggerRecord = m_recordController.CreateTriggerRecordObject(m_inspectionPos, lastTRid);
		auto& triggerRecord = m_recordController.CreateTriggerRecordObjectToWrite(m_inspectionPos);
		if (m_isCopyTR && nullptr != lastTriggerRecord)
		{
			triggerRecord->setImages(*(lastTriggerRecord.get()));
		}
		bool bOk = LoadMainImage(triggerRecord, dlg.GetPathName());
		if (bOk)
		{
			int pos = 0;
			for (const auto& tool : m_toolList)
			{
				bOk = tool->run(triggerRecord);
				if (!bOk)
				{
					CString text;
					text.Format("Trigger: Tool %s (%d) failed!", tool->getName(), pos);
					MessageBox(text);
					break;
				}
				pos++;
			}
		}
		else
		{
			MessageBox("Trigger: LoadMainImage failed!");
		}

		m_lastID.Format("%d", m_recordController.getLastTRId(m_inspectionPos));
		UpdateData(false);
	}

	void CTriggerRecordControllerTestDlg::OnSetImage()
	{
		UpdateData();
		int id = m_trIDToDisplay;
		if (-1 == id)
		{
			id = m_recordController.getLastTRId(m_inspectionPos);
		}
		auto triggerRecord = m_recordController.CreateTriggerRecordObject(m_inspectionPos, id);
		if (nullptr != triggerRecord && triggerRecord->getImage(m_ImageCombo.GetCurSel()))
		{
			m_dialogImage.setImage(triggerRecord->getImage(m_ImageCombo.GetCurSel())->getHandle());
			m_dialogImage.Refresh();
		}
		else
		{
			m_dialogImage.setImage((IPictureDisp*)nullptr);
			m_dialogImage.Refresh();
		}
	}

	void CTriggerRecordControllerTestDlg::OnToolTypeChanged()
	{
		switch (m_ToolSelectCombo.GetCurSel())
		{
		case 0:
			m_toolDescription = CopyTool::getDescription();
			break;
		case 1:
			m_toolDescription = WindowTool::getDescription();
			break;
		case 2:
			m_toolDescription = RotationTool::getDescription();
			break;
		case 3:
			m_toolDescription = DeactivedTool::getDescription();
			break;
		default:
			m_toolDescription = _T("<No description>");
			break;
		}
		UpdateData(false);
	}

	void CTriggerRecordControllerTestDlg::updateControls()
	{
		updateToolList();
		updateImageCombo();
		updateButton();
	}

	void CTriggerRecordControllerTestDlg::resetController()
	{
		SVMatroxBufferCreateStruct bufferStruct;
		bufferStruct.m_lSizeX = m_mainWidth;
		bufferStruct.m_lSizeY = m_mainHeigth;
		bufferStruct.m_eAttribute = SVBufAttImageProcDib;
		bufferStruct.m_eType = SV8BitUnsigned;
		m_recordController.StartResetTriggerRecordStructure(m_inspectionPos, m_trNumbers);
		m_recordController.AddImage2TriggerRecordStructure(m_mainGuid, bufferStruct);
		int pos = 0;
		for (const auto& tool : m_toolList)
		{
			tool->reset(pos++, bufferStruct);
			bufferStruct = tool->getBufferOut();
			m_recordController.AddImage2TriggerRecordStructure(tool->getGuid(), bufferStruct);
		}
		m_recordController.FinishResetTriggerRecordStructure();
		m_lastID.Format("%d", m_recordController.getLastTRId(m_inspectionPos));
	}

	void CTriggerRecordControllerTestDlg::updateToolList()
	{
		SvUl::NameGuidList availableList;
		for (const auto& tool : m_toolList)
		{
			availableList.push_back(SvUl::NameGuidPair(tool->getName(), tool->getGuid()));
		}
		m_toolListBox.init(availableList, "<No Tool>");
	}

	void CTriggerRecordControllerTestDlg::updateImageCombo()
	{
		m_ImageCombo.ResetContent();
		m_ImageCombo.AddString("Main Image");
		int i = 0;
		for (const auto tool : m_toolList)
		{
			CString text;
			text.Format("%s - %d", tool->getName(), i++);
			m_ImageCombo.AddString(text);
		}
		m_ImageCombo.SetCurSel(0);
	}

	void CTriggerRecordControllerTestDlg::updateButton()
	{
		GetDlgItem(IDC_BUTTON_RESET)->EnableWindow(m_isEdit);
		GetDlgItem(IDC_BUTTON_TRIGGER)->EnableWindow(!m_isEdit);
	}

	bool CTriggerRecordControllerTestDlg::LoadMainImage(SvTRC::ITriggerRecordRWPtr triggerRecord, const CString& rPath)
	{
		bool retValue = false;
		auto& main_image = triggerRecord->createNewImageHandle(0);
		if (nullptr != main_image)
		{
			auto& imageHandle = main_image->getHandle();
			if (nullptr != imageHandle)
			{
				HRESULT hr = SVMatroxBufferInterface::Import(imageHandle->GetBuffer(), std::string(rPath), SVFileBitmap, false);
				assert(S_OK == hr);
				retValue = (S_OK == hr);
			}
		}
		return retValue;
	}

} // namespace SvTRCT