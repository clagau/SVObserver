
// TriggerRecordControllerTestDlg.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "TriggerRecordControllerTestDlg.h"
#include "SVUtilityLibrary\NameGuidList.h"
#include "SVMatroxLibrary\SVMatroxBufferCreateStruct.h"
#include "CopyTool.h"
#include "WindowTool.h"
#include "SVMatroxLibrary\SVMatroxBufferInterface.h"
#include "RotationTool.h"
#include "DeactivedTool.h"
#include "SVProtoBuf/ConverterHelper.h"
#include "SVProtoBuf/TriggerRecordController.h"
#include "SVStatusLibrary/MessageContainer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define ID_GET_CALLBACK (WM_USER + 1)

namespace SvTrcT
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



	CTriggerRecordControllerTestDlg::CTriggerRecordControllerTestDlg(bool isReader, CWnd* pParent /*=NULL*/)
		: CDialogEx(IDD_TRIGGERRECORDCONTROLLERTEST_DIALOG, pParent)
		, m_isReader(isReader)
	{
		m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
		UuidCreateSequential(&m_mainGuid);
	}

	CTriggerRecordControllerTestDlg::~CTriggerRecordControllerTestDlg()
	{
		m_recordController.unregisterResetCallback(m_resetCallbackId);
		m_recordController.unregisterReadyCallback(m_readyCallbackId);
		m_recordController.unregisterNewTrCallback(m_newTrIdCallbackId);
		SvTrc::destroyTriggerRecordController();

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
		DDX_Text(pDX, IDC_TEXT_VALUE, m_valueText);
		DDX_Control(pDX, IDC_TOOLSET_LIST, m_toolListBox);
		DDX_Control(pDX, IDC_COMBO_TOOLTYPE, m_ToolSelectCombo);
		DDX_Control(pDX, IDC_COMBO_IMAGE, m_ImageCombo);
		DDX_Control(pDX, IDC_COMBO_VALUE, m_ValueCombo);
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
		ON_MESSAGE(ID_GET_CALLBACK, OnGetCallback)
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
		if (!m_isReader)
		{
			SvPb::InspectionList inspList;
			auto insp = inspList.add_list();
			insp->set_numberofrecords(m_trNumbers);
			SvTrc::getTriggerRecordControllerRWInstance().setInspections(std::move(inspList));
		}
		else
		{
			m_resetCallbackId = m_recordController.registerResetCallback(std::bind(&CTriggerRecordControllerTestDlg::OnResetUpdate, this));
			m_readyCallbackId = m_recordController.registerReadyCallback(std::bind(&CTriggerRecordControllerTestDlg::OnReadyUpdate, this));
			m_newTrIdCallbackId = m_recordController.registerNewTrCallback(std::bind(&CTriggerRecordControllerTestDlg::OnNewTrId, this, std::placeholders::_1));
			m_isTRCValid = m_recordController.isValid();
		}
		resetController();
		updateControls();
		
		m_ToolSelectCombo.SetCurSel(0);
		m_ImageCombo.SetCurSel(0);
		if (!m_isReader)
		{
			OnToolTypeChanged();
		}
		else
		{
			m_toolDescription.Format("Last reset update: Start");
			UpdateData(false);
		}

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

		if (!m_isReader)
		{
			if (updateState)
			{
				resetController();
				m_isEdit = false;
				updateButton();
			}
		}
		else
		{
			auto comboPos = m_ValueCombo.GetCurSel();
			int id = m_recordController.getLastTrId(0);
			auto triggerRecord = m_recordController.createTriggerRecordObject(0, id);
			if (nullptr != triggerRecord)
			{
				_variant_t value = triggerRecord->getDataValue(comboPos);
				m_valueText.Format("For TrId %d: %s", id, SvUl::createStdString(value).c_str());
			}
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

		SvTrc::IImagePtr pMainImage = LoadMainImage(dlg.GetPathName());
		if (nullptr != pMainImage)
		{
			int lastTRid = m_recordController.getLastTrId(m_inspectionPos);
			auto& lastTriggerRecord = m_recordController.createTriggerRecordObject(m_inspectionPos, lastTRid);
			auto& triggerRecord = SvTrc::getTriggerRecordControllerRWInstance().createTriggerRecordObjectToWrite(m_inspectionPos);
			if (m_isCopyTR && nullptr != lastTriggerRecord)
			{
				triggerRecord->setImages(*(lastTriggerRecord.get()));
			}
			try
			{
				triggerRecord->setImage(0, pMainImage);
			}
			catch (...)
			{	
				assert(false);
			}

			pMainImage.reset();
			int pos = 0;
			for (const auto& tool : m_toolList)
			{
				if (!tool->run(triggerRecord))
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

		m_lastID.Format("%d", m_recordController.getLastTrId(m_inspectionPos));
		UpdateData(false);
	}

	void CTriggerRecordControllerTestDlg::OnSetImage()
	{
		UpdateData();
		int id = m_trIDToDisplay;
		if (-1 == id)
		{
			id = m_recordController.getLastTrId(m_inspectionPos);
			if (m_isReader)
			{
				m_lastID.Format("%d", id);
				UpdateData(false);
			}
		}

		auto triggerRecord = m_recordController.createTriggerRecordObject(m_inspectionPos, id);
		SvTrc::IImagePtr image;
		auto comboPos = m_ImageCombo.GetCurSel();
		if (nullptr != triggerRecord)
		{
			if (!m_isReader)
			{
				int imagePos = 0;

				if (0 < comboPos && m_toolList.size() >= comboPos)
				{
					imagePos = m_toolList[comboPos - 1]->getImagePos();
				}

				image = triggerRecord->getImage(imagePos);
			}
			else
			{
				image = triggerRecord->getImage(comboPos);

			}
		}

		if (nullptr != image)
		{
			m_dialogImage.setImage(image->getHandle());
			m_dialogImage.Refresh();
		}
		else
		{
			m_dialogImage.setImage((IPictureDisp*)nullptr);
			m_dialogImage.Refresh();

			if (m_isReader)
			{
				updateImageCombo();
			}
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
	enum callbackEnum
	{
		reset,
		newTrcId
	};
	void CTriggerRecordControllerTestDlg::OnResetUpdate()
	{
		m_isTRCValid = false;
		PostMessage(ID_GET_CALLBACK, reset, (LPARAM)0);
	}

	void CTriggerRecordControllerTestDlg::OnReadyUpdate()
	{
		m_isTRCValid = true;
		PostMessage(ID_GET_CALLBACK, reset, (LPARAM)0);
	}

	void CTriggerRecordControllerTestDlg::OnNewTrId(SvTrc::TrEventData data)
	{
		if (0 == data.m_inspectionPos)
		{
			PostMessage(ID_GET_CALLBACK, newTrcId, (LPARAM)data.m_trId);
		}
	}

	LRESULT CTriggerRecordControllerTestDlg::OnGetCallback(WPARAM wParam, LPARAM lParam)
	{
		switch (wParam)
		{
			case reset:
				OnResetTRCData();
				break;
			case newTrcId:
				int newId = static_cast<int>(lParam);
				m_lastID.Format("%d", newId);
				UpdateData(false);
				break;
		}
		
		return 0;
	}

	void CTriggerRecordControllerTestDlg::updateControls()
	{
		if (!m_isReader)
		{
			updateToolList();
			GetDlgItem(IDC_COMBO_VALUE)->ShowWindow(false);
			GetDlgItem(IDC_TEXT_VALUE)->ShowWindow(false);
		}
		else
		{
			GetDlgItem(IDC_EDIT_MAINIMAGE_WIDTH)->ShowWindow(false);
			GetDlgItem(IDC_EDIT_MAINIMAGE_HEIGHT)->ShowWindow(false);
			GetDlgItem(IDC_TOOLSET_LIST)->ShowWindow(false);
			GetDlgItem(IDC_BUTTON_ADD)->ShowWindow(false);
			GetDlgItem(IDC_BUTTON_REMOVE)->ShowWindow(false);
			GetDlgItem(IDC_COMBO_TOOLTYPE)->ShowWindow(false);
			GetDlgItem(IDC_EDIT_TRNUMBERS)->ShowWindow(false);
			GetDlgItem(IDC_STATIC_SIZE)->ShowWindow(false);
			GetDlgItem(IDC_STATIC_WIDTH)->ShowWindow(false);
			GetDlgItem(IDC_STATIC_HEIGHT)->ShowWindow(false);
			GetDlgItem(IDC_STATIC_TRNUMBER)->ShowWindow(false);
			GetDlgItem(IDC_CHECK_COPY_IMAGE)->ShowWindow(false);
			GetDlgItem(IDC_STATIC_TOOLDESCRIPTION)->ShowWindow(false);
			updateValueCombo();
		}
		updateImageCombo();
		updateButton();
	}

	void CTriggerRecordControllerTestDlg::resetController()
	{
		if (!m_isReader)
		{
			auto& rRecordControllerRW = SvTrc::getTriggerRecordControllerRWInstance();
			auto inspList = rRecordControllerRW.getInspections();
			inspList.mutable_list(0)->set_numberofrecords(m_trNumbers);
			rRecordControllerRW.setInspections(std::move(inspList));

			SVMatroxBufferCreateStruct bufferStruct;
			bufferStruct.m_lSizeX = m_mainWidth;
			bufferStruct.m_lSizeY = m_mainHeigth;
			bufferStruct.m_eAttribute = SVBufAttImageProc;
			bufferStruct.m_eType = SV8BitUnsigned;
			rRecordControllerRW.startResetTriggerRecordStructure();
			rRecordControllerRW.removeAllImageBuffer(m_mainGuid);
			rRecordControllerRW.addImageBuffer(m_mainGuid, bufferStruct, 1);
			rRecordControllerRW.finishResetTriggerRecordStructure();

			rRecordControllerRW.startResetTriggerRecordStructure(m_inspectionPos);
			rRecordControllerRW.addOrChangeImage(m_mainGuid, bufferStruct);
			int sourcePos = 0;
			GUID sourceImage = m_mainGuid;
			for (const auto& tool : m_toolList)
			{
				tool->reset(sourceImage, sourcePos, bufferStruct, rRecordControllerRW);
				sourcePos = tool->getImagePos();
				sourceImage = tool->getGuid();
				bufferStruct = tool->getBufferOut();
			}
			rRecordControllerRW.finishResetTriggerRecordStructure();
		}
		m_lastID.Format("%d", m_recordController.getLastTrId(m_inspectionPos));
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
		try
		{
			m_ImageCombo.ResetContent();
			if (!m_isReader)
			{
				m_ImageCombo.AddString("Main Image");
				int i = 0;
				for (const auto tool : m_toolList)
				{
					CString text;
					text.Format("%s - %d", tool->getName(), i++);
					m_ImageCombo.AddString(text);
				}
			}
			else if (m_isTRCValid)
			{
				int i = 0;
				auto imageDefList = m_recordController.getImageDefList(0);
				for (auto imageDef : imageDefList.list())
				{
					CString text;
					SVGUID guid = SvPb::GetGuidFromProtoBytes(imageDef.guidid());
					text.Format("%d - %s", i++, guid.ToString().c_str());
					m_ImageCombo.AddString(text);
				}
			}
		}
		catch (const SvStl::MessageContainer& rSve)
		{
			std::string messageStr;
			rSve.Format(messageStr);
			MessageBox(messageStr.c_str());
		}
		m_ImageCombo.SetCurSel(0);
	}

	void CTriggerRecordControllerTestDlg::updateValueCombo()
	{
		try
		{
			m_ValueCombo.ResetContent();
			if (m_isReader && m_isTRCValid)
			{
				int i = 0;
				auto dataDefList = m_recordController.getDataDefList(0);
				for (auto dataDef : dataDefList.list())
				{
					CString text;
					text.Format("%s - %d", dataDef.name().c_str(), i++);
					m_ValueCombo.AddString(text);
				}
			}
		}
		catch (const SvStl::MessageContainer& rSve)
		{
			std::string messageStr;
			rSve.Format(messageStr);
			MessageBox(messageStr.c_str());
		}
		m_ValueCombo.SetCurSel(0);
	}

	void CTriggerRecordControllerTestDlg::updateButton()
	{
		if (!m_isReader)
		{
			GetDlgItem(IDC_BUTTON_RESET)->EnableWindow(m_isEdit);
			GetDlgItem(IDC_BUTTON_TRIGGER)->EnableWindow(!m_isEdit);
		}
		else
		{
			GetDlgItem(IDC_BUTTON_RESET)->EnableWindow(true);
			GetDlgItem(IDC_BUTTON_RESET)->SetWindowText("Read Value");
			GetDlgItem(IDC_BUTTON_TRIGGER)->ShowWindow(false);
		}
	}

	SvTrc::IImagePtr CTriggerRecordControllerTestDlg::LoadMainImage(const CString& rPath)
	{
		SVMatroxBufferCreateStruct bufferStruct;
		bufferStruct.m_lSizeX = m_mainWidth;
		bufferStruct.m_lSizeY = m_mainHeigth;
		bufferStruct.m_eAttribute = SVBufAttImageProc;
		bufferStruct.m_eType = SV8BitUnsigned;
		SvTrc::IImagePtr pImage = SvTrc::getTriggerRecordControllerRWInstance().getImageBuffer(bufferStruct);
		if (nullptr != pImage)
		{
			auto& imageHandle = pImage->getHandle();
			if (nullptr != imageHandle)
			{
				HRESULT hr = SVMatroxBufferInterface::Import(imageHandle->GetBuffer(), std::string(rPath), SVFileBitmap, false);
				assert(S_OK == hr);
			}
		}
		return pImage;
	}

	void CTriggerRecordControllerTestDlg::OnResetTRCData()
	{
		m_isTRCValid = m_recordController.isValid();
		updateImageCombo();
		updateValueCombo();
		std::time_t current_time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
		struct tm locTime;
		localtime_s(&locTime, &current_time);
		char timeBuf[100];
		asctime_s(timeBuf, 100, &locTime);
		m_toolDescription.Format("Last reset (%s): %s", m_isTRCValid?"valid":"invalid", timeBuf);
		UpdateData(false);
	}

} // namespace SvTrcT
