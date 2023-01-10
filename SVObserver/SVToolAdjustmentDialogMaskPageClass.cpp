//******************************************************************************
//* COPYRIGHT (c) 2005 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVToolAdjustmentDialogMaskPageClass
//* .File Name       : $Workfile:   SVToolAdjustmentDialogMaskPageClass.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.6  $
//* .Check In Date   : $Date:   26 Jun 2014 18:29:28  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
//Moved to precompiled header: #include <colordlg.h>
#include "SVToolAdjustmentDialogMaskPageClass.h"
#include "SVMaskShapeEditorDlg.h"
#include "Definitions/StringTypeDef.h"
#include "Definitions/NameValueVector.h"
#include "SVMFCControls/SVMaskEditor.h"
#include "SVStatusLibrary/MessageManager.h"
#include "SVUtilityLibrary/StringHelper.h"
#pragma endregion Includes

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


static LPCTSTR NoImageTag = _T("(No Image)");

const UINT UISetRGB = RegisterWindowMessage(SETRGBSTRING);
enum ImageTabEnums
{
	SourceTab = 0,
	MaskTab = 1,
	ResultTab = 2
};

SVToolAdjustmentDialogMaskPageClass* SVToolAdjustmentDialogMaskPageClass::m_pThis = nullptr;

BEGIN_MESSAGE_MAP(SVToolAdjustmentDialogMaskPageClass, CPropertyPage)
	//{{AFX_MSG_MAP(SVToolAdjustmentDialogMaskPageClass)
	ON_BN_CLICKED(IDC_BTN_EDIT_STATIC_MASK, OnEditStaticMaskButton)
	ON_BN_CLICKED(IDC_BTN_EDIT_SHAPE_MASK, OnEditShapeMaskButton)
	ON_BN_CLICKED(IDC_BTN_EXPORT_MASK, OnExportMaskButton)
	ON_BN_CLICKED(IDC_BTN_IMPORT_MASK, OnImportMaskButton)
	ON_BN_CLICKED(IDC_CHK_ACTIVATE_MASK, OnActivateMask)
	ON_CBN_SELCHANGE(IDC_MASK_OPERATOR_COMBO, OnSelchangeMaskOperatorCombo)
	ON_BN_CLICKED(IDC_RADIO_IMAGE_MASK, OnRadioImageMask)
	ON_BN_CLICKED(IDC_RADIO_STATIC_MASK, OnRadioStaticMask)
	ON_CBN_SELCHANGE(IDC_COMBO_IMAGE_MASK, OnSelchangeComboImageMask)
	ON_BN_CLICKED(IDC_BTN_FILL_COLOR_MORE, OnButtonFillColorMore)
	ON_BN_CLICKED(IDC_RADIO_SHAPE_MASK, OnRadioShapeMask)
	ON_CBN_SELCHANGE(IDC_COMBO_FILL_OPTIONS, OnSelchangeComboFillOptions)
	ON_EN_CHANGE(IDC_EDIT_FILL_COLOR, OnChangeEditFillColor)
	ON_CBN_SELCHANGE(IDC_DRAW_MASK_CRITERIA, OnSelchangeDrawMaskCriteria)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

SVToolAdjustmentDialogMaskPageClass::SVToolAdjustmentDialogMaskPageClass(uint32_t inspectionId, uint32_t taskObjectId, uint32_t maskOperatorId)
	: CPropertyPage(SVToolAdjustmentDialogMaskPageClass::IDD)
	, m_InspectionID(inspectionId)
	, m_TaskObjectID(taskObjectId)
	, m_pMaskEditorCtl(nullptr)
	, m_ImageController(inspectionId, taskObjectId)
	, m_Values{ SvOgu::BoundValues{ inspectionId, maskOperatorId } }
	, m_maskController{ inspectionId, taskObjectId, maskOperatorId }
{
	m_pThis = this;

	// Set SvFs::FileHelper for Import & Export functions.
	std::string DefaultExtension(SvUl::LoadStdString(IDS_FULL_MASKFILE_EXTENSION));
	std::string Filter(SvUl::LoadStdString(IDS_MASKFILE_DIALOG_FILTER));

	m_svfnFileName.SetDefaultFileExtension(DefaultExtension.c_str());
	m_svfnFileName.SetFileSaveFlags(OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT);
	m_svfnFileName.SetFileSelectFlags(OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT);
	m_svfnFileName.SetFileExtensionFilterList(Filter.c_str());

	//{{AFX_DATA_INIT(SVToolAdjustmentDialogMaskPageClass)
	m_bActivateMask = false;
	m_iMaskType = -1;
	m_sFillColor = _T("");
	//}}AFX_DATA_INIT
}

SVToolAdjustmentDialogMaskPageClass::~SVToolAdjustmentDialogMaskPageClass()
{
	m_pThis = nullptr;
	if (m_pMaskEditorCtl)
	{
		delete m_pMaskEditorCtl;
		m_pMaskEditorCtl = nullptr;
	}
}

void SVToolAdjustmentDialogMaskPageClass::SetupMaskOperatorComboBox()
{
	const SvDef::NameValueVector& rMaskOperators = m_Values.GetEnumTypes(SvPb::MaskOperatorEId);
	m_cbMaskOperator.SetEnumTypes(rMaskOperators);
	long CurrentSelection = m_Values.Get<long>(SvPb::MaskOperatorEId);
	m_cbMaskOperator.SetCurSelItemData(CurrentSelection);
}

void SVToolAdjustmentDialogMaskPageClass::SetupFillAreaComboBox()
{
	const SvDef::NameValueVector& rFillArea = m_Values.GetEnumTypes(SvPb::MaskFillAreaEId);
	m_cbFillOptions.SetEnumTypes(rFillArea);
	long CurrentSelection = m_Values.Get<long>(SvPb::MaskFillAreaEId);
	m_cbFillOptions.SetCurSelItemData(CurrentSelection);
}

void SVToolAdjustmentDialogMaskPageClass::SetupDrawCriteriaCombo()
{
	const SvDef::NameValueVector& rDrawCriteria = m_Values.GetEnumTypes(SvPb::DrawCriteriaEId);
	m_DrawCriteriaCombo.SetEnumTypes(rDrawCriteria);
	long CurrentSelection = m_Values.Get<long>(SvPb::DrawCriteriaEId);
	m_DrawCriteriaCombo.SetCurSelItemData(CurrentSelection);
}

void SVToolAdjustmentDialogMaskPageClass::SetInspectionData()
{
	UpdateData(true);
	m_Values.Set<bool>(SvPb::MaskEnabledEId, m_bActivateMask ? true : false);
	m_Values.Set<DWORD>(SvPb::MaskUseImageMaskEId, m_iMaskType);

	long Value{ 0L };
	int CurrentSelection = m_cbMaskOperator.GetCurSel();
	if (CB_ERR != CurrentSelection)
	{
		Value = static_cast<long> (m_cbMaskOperator.GetItemData(CurrentSelection));
		m_Values.Set<long>(SvPb::MaskOperatorEId, Value);
	}

	CurrentSelection = m_cbFillOptions.GetCurSel();
	if (CB_ERR != CurrentSelection)
	{
		Value = static_cast<long> (m_cbFillOptions.GetItemData(CurrentSelection));
		m_Values.Set<long>(SvPb::MaskFillAreaEId, Value);
	}

	CurrentSelection = m_DrawCriteriaCombo.GetCurSel();
	if (CB_ERR != CurrentSelection)
	{
		Value = static_cast<long> (m_DrawCriteriaCombo.GetItemData(CurrentSelection));
		m_Values.Set<long>(SvPb::DrawCriteriaEId, Value);
	}

	m_Values.Commit(SvOgu::PostAction::doReset | SvOgu::PostAction::doRunOnce);
}

void SVToolAdjustmentDialogMaskPageClass::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(SVToolAdjustmentDialogMaskPageClass)
	DDX_Control(pDX, IDC_BTN_IMPORT_MASK, m_btnImportMask);
	DDX_Control(pDX, IDC_BTN_EXPORT_MASK, m_btnExportMask);
	DDX_Control(pDX, IDC_BTN_EDIT_STATIC_MASK, m_btnEditStaticMask);
	DDX_Control(pDX, IDC_BTN_EDIT_SHAPE_MASK, m_btnEditShapeMask);
	DDX_Control(pDX, IDC_EDIT_FILL_COLOR, m_ebFillColor);
	DDX_Control(pDX, IDC_COMBO_FILL_OPTIONS, m_cbFillOptions);
	DDX_Control(pDX, IDC_BTN_FILL_COLOR_MORE, m_btnFillColorMore);
	DDX_Control(pDX, IDC_COMBO_IMAGE_MASK, m_cbAvailableSourceImageList);
	DDX_Control(pDX, IDC_MASK_OPERATOR_COMBO, m_cbMaskOperator);
	DDX_Control(pDX, IDC_DIALOGIMAGE, m_dialogImage);
	DDX_Check(pDX, IDC_CHK_ACTIVATE_MASK, m_bActivateMask);
	DDX_Radio(pDX, IDC_RADIO_STATIC_MASK, m_iMaskType);
	DDX_Text(pDX, IDC_EDIT_FILL_COLOR, m_sFillColor);
	DDX_Control(pDX, IDC_DRAW_MASK_CRITERIA, m_DrawCriteriaCombo);
	//}}AFX_DATA_MAP
}

BOOL SVToolAdjustmentDialogMaskPageClass::OnSetActive()
{
	setImages();
	return CPropertyPage::OnSetActive();
}

////////////////////////////////////////////////////////////////////////////////
// .Title       : initMask
////////////////////////////////////////////////////////////////////////////////
void SVToolAdjustmentDialogMaskPageClass::initMask()
{
	SetInspectionData();
	setImages();
	UpdateData(false); // set data to dialog
}

BOOL SVToolAdjustmentDialogMaskPageClass::OnInitDialog()
{
	CPropertyPage::OnInitDialog();
	// See if we can 'connect' to the Mask Editor Active X control.
	Log_Assert(nullptr == m_pMaskEditorCtl);

	m_ImageController.Init();
	m_Values.Init();
	m_maskController.Init();

	RetreiveCurrentlySelectedImageNames();

	m_dialogImage.AddTab(_T("Tool Input"));
	m_dialogImage.AddTab(_T("Mask"));
	m_dialogImage.AddTab(_T("Tool Result"));

	m_dialogImage.SelectTab(2);

	m_bActivateMask = m_Values.Get<bool>(SvPb::MaskEnabledEId);
	long lColor = m_Values.Get<long>(SvPb::MaskFillColorEId);
	m_sFillColor = SvUl::AsString(lColor).c_str();
	m_iMaskType = m_Values.Get<int>(SvPb::MaskUseImageMaskEId);

	SetupMaskOperatorComboBox();
	SetupFillAreaComboBox();
	SetupDrawCriteriaCombo();

	ShowControls(m_iMaskType);

	if (MASK_TYPE_IMAGE == m_iMaskType)
	{
		SetupImageMaskCombo();
	}

	// Refresh dialog image...
	m_dialogImage.Refresh();

	UpdateData(false); // set data to dialog

	OnActivateMask();

	return TRUE;  // return TRUE unless you set the focus to a control
					// EXCEPTION: OCX-Eigenschaftenseiten sollten FALSE zurückgeben
}

void SVToolAdjustmentDialogMaskPageClass::OnEditStaticMaskButton()
{
	if (nullptr == m_pMaskEditorCtl)
	{
		m_pMaskEditorCtl = new SvMc::SVMaskEditor;

		CRect r(0, 0, 1, 1);
		BOOL bResult = m_pMaskEditorCtl->Create(
			"MaskEditorCtlWnd",  //LPCTSTR lpszWindowName, 
			WS_POPUP,  // | WS_VISIBLE,  //DWORD dwStyle,
			r,           //const RECT& rect, 
			this,        //CWnd* pParentWnd, 
			2           //UINT nID,
		);

		if (!bResult)
		{
			delete m_pMaskEditorCtl;
			m_pMaskEditorCtl = nullptr;

			SvStl::MessageManager Msg(SvStl::MsgType::Log | SvStl::MsgType::Display);
			Msg.setMessage(SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_MaskPage_StartOCXFailed, SvStl::SourceFileParams(StdMessageParams));
			return;
		}
	}

	if (m_pMaskEditorCtl)
	{
		UpdateData(true);

		IPictureDisp* pSourceImage = m_maskController.GetReferenceImage();
		m_pMaskEditorCtl->SetImageDIBHandle(pSourceImage);

		// Set Mask Graphix data...
		CComPtr<IStream> pStreamMaskEditor;
		HRESULT hr = CreateStreamOnHGlobal(m_maskController.GetMaskData(), true, &pStreamMaskEditor);
		if (S_OK == hr)
		{
			m_pMaskEditorCtl->SetMaskData(pStreamMaskEditor);
		}
		else
		{
			Log_Assert(false);
		}
		// Set current mask operator...
		int item = m_cbMaskOperator.GetCurSel();
		if (item != CB_ERR)
		{
			long lMaskOperator = static_cast<long>(m_cbMaskOperator.GetItemData(item));
			m_pMaskEditorCtl->SetMaskOperator(lMaskOperator);
		}

		// Edit...Run the Mask Editor OCX dialog
		if (IDOK == m_pMaskEditorCtl->DoEditMaskModal())
		{
			// Get Mask Graphix data...
			HGLOBAL hg;
			CComPtr<IUnknown> pUnk;
			pUnk.Attach(m_pMaskEditorCtl->GetMaskData());
			CComPtr<IStream> pStreamMaskController;
			hr = pUnk->QueryInterface(IID_IStream, reinterpret_cast<void**>(&pStreamMaskController));
			if (S_OK == hr)
			{
				if (nullptr != pStreamMaskController)
				{
					/*hr = */GetHGlobalFromStream(pStreamMaskController, &hg);
					m_maskController.SetMaskData(hg);
				}
			}

			// Update combo...
			long lMaskOperator = m_pMaskEditorCtl->GetMaskOperator();

			for (int i = 0; i < m_cbMaskOperator.GetCount(); ++i)
			{
				if (lMaskOperator == static_cast<long>(m_cbMaskOperator.GetItemData(i)))
				{
					m_cbMaskOperator.SetCurSel(i);
					break;
				}
			}
			UpdateData(false);
		}
		// Refresh Dialog...
		initMask();
	}
}

void SVToolAdjustmentDialogMaskPageClass::OnExportMaskButton()
{
	UpdateData(true);

	// Use SvFs::FileHelper for browsing
	if (m_svfnFileName.SaveFile())
	{
		std::string Extension = SvUl::LoadStdString(IDS_FULL_MASKFILE_EXTENSION);

		if (m_svfnFileName.GetExtension() != Extension)
		{
			m_svfnFileName.SetExtension(Extension.c_str());
		}

		HRESULT hr = m_maskController.ExportMask(m_svfnFileName.GetFullFileName());
		if (!SUCCEEDED(hr))
		{
			SvStl::MessageManager Msg(SvStl::MsgType::Log | SvStl::MsgType::Display);
			SvDef::StringVector msgList;
			msgList.push_back(m_svfnFileName.GetFullFileName());
			Msg.setMessage(SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_Error_CannotOpenFile, msgList, SvStl::SourceFileParams(StdMessageParams));
		}
	}
	// Refresh image view...
	initMask();
}

void SVToolAdjustmentDialogMaskPageClass::OnImportMaskButton()
{
	UpdateData(true);

	// Use SvFs::FileHelper for browsing.
	if (m_svfnFileName.SelectFile())
	{
		HRESULT hr = m_maskController.ImportMask(m_svfnFileName.GetFullFileName());
		if (!SUCCEEDED(hr))
		{
			SvStl::MessageManager Msg(SvStl::MsgType::Log | SvStl::MsgType::Display);
			SvDef::StringVector msgList;
			msgList.push_back(std::string(m_svfnFileName.GetFullFileName()));
			Msg.setMessage(SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_Error_CannotOpenFile, msgList, SvStl::SourceFileParams(StdMessageParams));
		}
	}
	// Refresh image view...
	initMask();
}

void SVToolAdjustmentDialogMaskPageClass::OnEditShapeMaskButton()
{
	m_Values.Commit(SvOgu::PostAction::doReset | SvOgu::PostAction::doRunOnce);

	SVMaskShapeEditorDlg dlg(m_InspectionID, m_TaskObjectID, m_maskController.GetInstanceID(), m_maskController.GetShapeMaskHelperID());

	long selTab = 2;
	m_dialogImage.GetSelectedTab(&selTab);
	dlg.setSelectedTab(selTab);

	if (IDOK == dlg.DoModal())
	{
		m_Values.Init();

		m_bActivateMask = m_Values.Get<bool>(SvPb::MaskEnabledEId);
		long lColor = m_Values.Get<long>(SvPb::MaskFillColorEId);
		m_sFillColor = SvUl::AsString(lColor).c_str();
		m_iMaskType = m_Values.Get<int>(SvPb::MaskUseImageMaskEId);

		SetupMaskOperatorComboBox();
		SetupFillAreaComboBox();

		UpdateData(false);

		long tabNumber = dlg.getSelectedTab();
		m_dialogImage.SelectTab(tabNumber);
	}
	else
	{
		dlg.Revert(); // reset to previous
	}
	initMask();
}

void SVToolAdjustmentDialogMaskPageClass::OnActivateMask()
{
	initMask();

	GetDlgItem(IDC_RADIO_STATIC_MASK)->EnableWindow(m_bActivateMask);
	GetDlgItem(IDC_RADIO_IMAGE_MASK)->EnableWindow(m_bActivateMask);
	GetDlgItem(IDC_RADIO_SHAPE_MASK)->EnableWindow(m_bActivateMask);
	GetDlgItem(IDC_MASK_OPERATOR_COMBO)->EnableWindow(m_bActivateMask);
	GetDlgItem(IDC_COMBO_IMAGE_MASK)->EnableWindow(m_bActivateMask);
	GetDlgItem(IDC_EDIT_FILL_COLOR)->EnableWindow(m_bActivateMask);
	GetDlgItem(IDC_BTN_FILL_COLOR_MORE)->EnableWindow(m_bActivateMask);
	GetDlgItem(IDC_BTN_IMPORT_MASK)->EnableWindow(m_bActivateMask);
	GetDlgItem(IDC_BTN_EXPORT_MASK)->EnableWindow(m_bActivateMask);
	GetDlgItem(IDC_BTN_EDIT_SHAPE_MASK)->EnableWindow(m_bActivateMask);
	GetDlgItem(IDC_BTN_EDIT_STATIC_MASK)->EnableWindow(m_bActivateMask);
	GetDlgItem(IDC_COMBO_FILL_OPTIONS)->EnableWindow(m_bActivateMask);
	GetDlgItem(IDC_DRAW_MASK_CRITERIA)->EnableWindow(m_bActivateMask);
}

void SVToolAdjustmentDialogMaskPageClass::OnSelchangeMaskOperatorCombo()
{
	initMask();
}

void SVToolAdjustmentDialogMaskPageClass::OnRadioImageMask()
{
	UpdateData();
	ShowControls(m_iMaskType);
	initMask();

	SetupImageMaskCombo();
}

void SVToolAdjustmentDialogMaskPageClass::OnRadioStaticMask()
{
	UpdateData();
	ShowControls(m_iMaskType);
	initMask();
}

void SVToolAdjustmentDialogMaskPageClass::OnRadioShapeMask()
{
	UpdateData();
	ShowControls(m_iMaskType);
	initMask();
}

void SVToolAdjustmentDialogMaskPageClass::ShowControls(int p_iMaskType)
{
	// Fill
	GetDlgItem(IDC_EDIT_FILL_COLOR)->ShowWindow(MASK_TYPE_SHAPE == p_iMaskType);
	GetDlgItem(IDC_BTN_FILL_COLOR_MORE)->ShowWindow(MASK_TYPE_SHAPE == p_iMaskType);
	GetDlgItem(IDC_COMBO_FILL_OPTIONS)->ShowWindow(MASK_TYPE_SHAPE == p_iMaskType);
	GetDlgItem(IDC_STATIC_FILL_COLOR)->ShowWindow(MASK_TYPE_SHAPE == p_iMaskType);
	GetDlgItem(IDC_STATIC_FILL_OPTIONS)->ShowWindow(MASK_TYPE_SHAPE == p_iMaskType);

	// Static
	GetDlgItem(IDC_BTN_IMPORT_MASK)->ShowWindow(MASK_TYPE_STATIC == p_iMaskType);
	GetDlgItem(IDC_BTN_EXPORT_MASK)->ShowWindow(MASK_TYPE_STATIC == p_iMaskType);
	GetDlgItem(IDC_BTN_EDIT_STATIC_MASK)->ShowWindow(MASK_TYPE_STATIC == p_iMaskType);

	// Shape
	GetDlgItem(IDC_BTN_EDIT_SHAPE_MASK)->ShowWindow(MASK_TYPE_SHAPE == p_iMaskType);

	// Image
	GetDlgItem(IDC_COMBO_IMAGE_MASK)->ShowWindow(MASK_TYPE_IMAGE == p_iMaskType);
	GetDlgItem(IDC_STATIC_MASK_IMAGE_SOURCE)->ShowWindow(MASK_TYPE_IMAGE == p_iMaskType);
}

void SVToolAdjustmentDialogMaskPageClass::SetupImageMaskCombo()
{
	const SvUl::NameObjectIdList& rAvailableImageList = m_ImageController.GetAvailableImageList();
	m_cbAvailableSourceImageList.Init(rAvailableImageList, m_imageName, NoImageTag);
}

void SVToolAdjustmentDialogMaskPageClass::OnSelchangeComboImageMask()
{
	UpdateData(true); // get data from dialog

	int index = m_cbAvailableSourceImageList.GetCurSel();
	if (CB_ERR != index)
	{
		CString name;
		m_cbAvailableSourceImageList.GetLBText(index, name);
		m_ImageController.ConnectToImage(m_imageInputName, name.GetString(), m_maskController.GetInstanceID());
	}
	initMask();
}

void SVToolAdjustmentDialogMaskPageClass::OnButtonFillColorMore()
{
	UpdateData();
	CColorDialog dlg;
	dlg.m_cc.Flags |= CC_FULLOPEN;

	dlg.m_cc.Flags |= CC_ENABLETEMPLATE;
	dlg.m_cc.lpTemplateName = MAKEINTRESOURCE(IDD_CHOOSEGRAYCOLOR);
	dlg.m_cc.hInstance = (HWND)::AfxGetResourceHandle();

	dlg.m_cc.Flags |= CC_ENABLEHOOK;
	dlg.m_cc.lpfnHook = ColorDlgHookFn;

	dlg.m_cc.lCustData = atoi(m_sFillColor);

	long lOriginalVal = atol(m_sFillColor);

	if (IDOK == dlg.DoModal())
	{
		COLORREF rgb = dlg.GetColor();
		long selectedColor = static_cast<long>(GetRValue(rgb));
		m_sFillColor = SvUl::AsString(selectedColor).c_str();
	}
	else
	{
		m_sFillColor = SvUl::AsString(lOriginalVal).c_str();
	}

	UpdateData(false);
	initMask();
}

void SVToolAdjustmentDialogMaskPageClass::OnChangeEditFillColor()
{
	initMask();
}

UINT_PTR CALLBACK SVToolAdjustmentDialogMaskPageClass::ColorDlgHookFn(HWND hdlg, UINT uiMsg, WPARAM, LPARAM lParam)
{
	int iReturnCode = 0;	// by default allow color dlg to process message

#if defined (TRACE_THEM_ALL) || defined (TRACE_OTHER)
	TRACE("ColorDlgHookFn - MSG: %08X, LPARAM: %08X\n", uiMsg, lParam);
#endif

	CWnd* pWnd = CWnd::FromHandle(hdlg);
	Log_Assert(nullptr != pWnd);
	switch (uiMsg)
	{
	case WM_INITDIALOG:
	{
		if (pWnd)
		{
			pWnd->GetDlgItem(COLOR_RAINBOW)->ShowWindow(SW_HIDE);
			pWnd->GetDlgItem(COLOR_BOX1)->ShowWindow(SW_HIDE);
			pWnd->GetDlgItem(COLOR_CUSTOM1)->ShowWindow(SW_HIDE);
			pWnd->GetDlgItem(COLOR_SOLID)->ShowWindow(SW_HIDE);

			CHOOSECOLOR* pColor = reinterpret_cast<CHOOSECOLOR*>(lParam);
			if (pColor)
			{

				::SendMessage(hdlg, UISetRGB, 0, (LPARAM)RGB(pColor->lCustData, pColor->lCustData, pColor->lCustData));
			}
		}
		iReturnCode = 1;	// don't have color dlg handle this again (it has already handled it)
		break;
	}

	case WM_PAINT:
	{
		CWnd* pWndRed = pWnd->GetDlgItem(COLOR_RED);
		Log_Assert(nullptr != pWndRed);
		if (nullptr != pWndRed)
		{
			CString sText;
			pWndRed->GetWindowText(sText);

			m_pThis->m_Values.Set<CString>(SvPb::MaskFillColorEId, sText);
			m_pThis->m_Values.Commit(SvOgu::PostAction::doReset | SvOgu::PostAction::doRunOnce);
			m_pThis->setImages();
		}
	}

	default:
	{
		break;
	}
	}
	return iReturnCode;
}

void SVToolAdjustmentDialogMaskPageClass::OnSelchangeComboFillOptions()
{
	initMask();
}

void SVToolAdjustmentDialogMaskPageClass::setImages()
{
	RetreiveCurrentlySelectedImageNames();

	IPictureDisp* pSourceImage = m_maskController.GetReferenceImage();
	IPictureDisp* pMaskImage = m_maskController.GetMaskImage();
	IPictureDisp* pResultImage = m_maskController.GetResultImage();

	m_dialogImage.setImage(pSourceImage, SourceTab);
	m_dialogImage.setImage(pMaskImage, MaskTab);
	m_dialogImage.setImage(pResultImage, ResultTab);

	m_dialogImage.Refresh();
}

void SVToolAdjustmentDialogMaskPageClass::OnSelchangeDrawMaskCriteria()
{
	initMask();
}

void SVToolAdjustmentDialogMaskPageClass::RetreiveCurrentlySelectedImageNames()
{
	const auto& rImageList = m_ImageController.GetInputImageList(m_maskController.GetInstanceID(), 1);

	auto it = rImageList.begin();
	if (it != rImageList.end())
	{
		m_imageInputName = it->inputname();
		m_imageName = it->connected_objectdottedname();
	}
}
