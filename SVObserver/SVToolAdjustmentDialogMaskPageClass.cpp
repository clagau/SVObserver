//******************************************************************************
//* COPYRIGHT (c) 2005 by SVResearch, Harrisburg
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
//Moved to precompiled header: #include <boost/assign/list_of.hpp>
//Moved to precompiled header: #include <colordlg.h>
#include "SVToolAdjustmentDialogMaskPageClass.h"
#include "SVMaskShapeEditorDlg.h"
#include "SVMFCControls\SVMaskEditor.h"
#include "ObjectInterfaces\NameValueList.h"
#include "SVObserver.h"
#include "SVStatusLibrary\MessageManager.h"
#include "TextDefinesSvO.h"
#include "ObjectInterfaces\ErrorNumbers.h"
#include "SVUtilityLibrary/SVStringConversions.h"
#pragma endregion Includes

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static LPCTSTR NoImageTag = _T("(No Image)");
static LPCSTR EnabledTag = "Enabled";
static LPCSTR MaskTypeTag = "MaskType";
static LPCSTR OperatorTag = "Operator";
static LPCSTR FillAreaTag = "FillArea";
static LPCSTR FillColorTag = "FillColor";
static LPCSTR DrawCriteriaTag = "DrawCriteria";

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

SVToolAdjustmentDialogMaskPageClass::SVToolAdjustmentDialogMaskPageClass(const SVGUID& rInspectionID, const SVGUID& rTaskObjectID) 
: CPropertyPage(SVToolAdjustmentDialogMaskPageClass::IDD)
, m_InspectionID(rInspectionID)
, m_TaskObjectID(rTaskObjectID)
, m_pMaskEditorCtl(nullptr)
, m_ImageController(rInspectionID, rTaskObjectID)
, m_Values(SvOg::BoundValues(rInspectionID, rTaskObjectID, boost::assign::map_list_of
	(EnabledTag, SVMaskEnabledObjectGuid)
	(MaskTypeTag, SVMaskUseImageMaskGuid)
	(OperatorTag, SVMaskOperatorGuid)
	(FillAreaTag, SVMaskFillAreaGuid)
	(FillColorTag, SVMaskFillColorGuid)
	(DrawCriteriaTag, SVDrawCriteriaObjectGuid)))
, m_maskController(rInspectionID, rTaskObjectID)
{
	m_pThis = this;

   // Set SVFileNameClass for Import & Export functions.
   SVString DefaultExtension(SvUl_SF::LoadSVString(IDS_FULL_MASKFILE_EXTENSION));
   SVString Filter(SvUl_SF::LoadSVString(IDS_MASKFILE_DIALOG_FILTER));

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

void SVToolAdjustmentDialogMaskPageClass::SetData()
{
	UpdateData(true);
	m_Values.Set<bool>(EnabledTag, m_bActivateMask ? true : false);
	m_Values.Set<DWORD>(MaskTypeTag, m_iMaskType);

	int sel = m_cbMaskOperator.GetCurSel();
	if (CB_ERR != sel)
	{
		CString maskOperator;
		m_cbMaskOperator.GetLBText(sel, maskOperator);
		m_Values.Set<CString>(OperatorTag, maskOperator);
	}

	sel = m_cbFillOptions.GetCurSel();
	if (CB_ERR != sel)
	{
		CString fillArea;
		m_cbFillOptions.GetLBText(sel, fillArea);
		m_Values.Set<CString>(FillAreaTag, fillArea);
	}

	sel = m_DrawCriteriaCombo.GetCurSel();
	if (CB_ERR != sel)
	{
		CString drawCriteria;
		m_DrawCriteriaCombo.GetLBText(sel, drawCriteria);
		m_Values.Set<CString>(DrawCriteriaTag, drawCriteria);
	}
}

void SVToolAdjustmentDialogMaskPageClass::GetData()
{
	m_bActivateMask = m_Values.Get<bool>(EnabledTag);
	long lColor = m_Values.Get<long>(FillColorTag);
	m_sFillColor = SvUl::AsString(lColor).c_str();

	m_iMaskType = m_Values.Get<int>(MaskTypeTag);

	UpdateData(false);
}

void SVToolAdjustmentDialogMaskPageClass::SetupMaskOperatorComboBox()
{
	// Fill Arithmetic Operator Combo...
	m_cbMaskOperator.ResetContent();
	CString maskOperator = m_Values.Get<CString>(OperatorTag);
	const SvOi::NameValueList& maskOperators = m_Values.GetEnumTypes(OperatorTag);
	for (SvOi::NameValueList::const_iterator it = maskOperators.begin(); it != maskOperators.end();++it)
	{
		m_cbMaskOperator.SetItemData(m_cbMaskOperator.AddString(it->first.c_str()), it->second);
	}
	RefreshComboBox(maskOperator, m_cbMaskOperator);
}

void SVToolAdjustmentDialogMaskPageClass::SetupFillAreaComboBox()
{
	// Fill the Fill Area combo
	m_cbFillOptions.ResetContent();
	CString fillAreaValue = m_Values.Get<CString>(FillAreaTag);
	const SvOi::NameValueList& fillArea = m_Values.GetEnumTypes(FillAreaTag);
	for (SvOi::NameValueList::const_iterator it = fillArea.begin(); it != fillArea.end();++it)
	{
		m_cbFillOptions.SetItemData(m_cbFillOptions.AddString(it->first.c_str()), it->second);
	}
	RefreshComboBox(fillAreaValue, m_cbFillOptions);
}

void SVToolAdjustmentDialogMaskPageClass::SetupDrawCriteriaCombo()
{
	// Populate Draw Criteria combo...
	m_DrawCriteriaCombo.ResetContent();
	CString drawCriteriaValue = m_Values.Get<CString>(DrawCriteriaTag);
	const SvOi::NameValueList& drawCriteria = m_Values.GetEnumTypes(DrawCriteriaTag);
	// Set Combos Current Value
	for (SvOi::NameValueList::const_iterator it = drawCriteria.begin(); it != drawCriteria.end();++it)
	{
		m_DrawCriteriaCombo.SetItemData(m_DrawCriteriaCombo.AddString(it->first.c_str()), it->second);
	}
	RefreshComboBox(drawCriteriaValue, m_DrawCriteriaCombo);
}

void SVToolAdjustmentDialogMaskPageClass::SetupComboBoxes()
{
	SetupMaskOperatorComboBox();
	SetupFillAreaComboBox();
	SetupDrawCriteriaCombo();
}

HRESULT SVToolAdjustmentDialogMaskPageClass::SetInspectionData()
{
	HRESULT hr = S_OK;
	SetData();
	m_Values.Commit(true);
	GetData();
	return hr;
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
	DDX_Radio(pDX, IDC_RADIO_STATIC_MASK, m_iMaskType);
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
	ASSERT(nullptr == m_pMaskEditorCtl);
	
	m_ImageController.Init();
	m_Values.Init();
	m_maskController.Init();

	RetreiveCurrentlySelectedImageNames();
	RetreiveResultImageNames();

	m_dialogImage.AddTab(_T("Tool Input")); 
	m_dialogImage.AddTab(_T("Mask")); 
	m_dialogImage.AddTab(_T("Tool Result")); 

	m_dialogImage.SelectTab(2);

	// get Values
	GetData();

	SetupComboBoxes();

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

void SVToolAdjustmentDialogMaskPageClass::RefreshComboBox(const CString& rValue, CComboBox& rComboBox)
{
	for (int i = 0; i < rComboBox.GetCount(); ++i)
	{
		CString entry;
		rComboBox.GetLBText(i, entry);
		if (rValue == entry)
		{
			rComboBox.SetCurSel(i);
			break;
		}
	}
}

void SVToolAdjustmentDialogMaskPageClass::OnEditStaticMaskButton() 
{
	if( nullptr == m_pMaskEditorCtl )
	{
		m_pMaskEditorCtl = new SvMc::CSVMaskEditor;

		CRect r(0,0,1,1);
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
			
			SvStl::MessageMgrStd Msg( SvStl::LogAndDisplay );
			Msg.setMessage( SVMSG_SVO_93_GENERAL_WARNING, SvOi::Tid_MaskPage_StartOCXFailed, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_10215 );
			return;
		}
	}

	HGLOBAL hGlobal = m_maskController.GetMaskData();
	// Set mask Graphix data...
	CComPtr<IStream> pStream;
	HRESULT hr = CreateStreamOnHGlobal(hGlobal, true, &pStream);
	if (S_OK == hr)
	{
		m_pMaskEditorCtl->SetMaskData(pStream);
	}
	else
	{
		ASSERT(false);
	}

	if (m_pMaskEditorCtl)
	{
		UpdateData(true);

		IPictureDisp* pSourceImage = m_maskController.GetReferenceImage();
		m_pMaskEditorCtl->SetImageDIBHandle(pSourceImage);

		// Set Mask Graphix data...
		CComPtr<IStream> pStream;
		hr = CreateStreamOnHGlobal(m_maskController.GetMaskData(), true, &pStream);
		if (S_OK == hr)
		{
			m_pMaskEditorCtl->SetMaskData(pStream);
		}
		else
		{
			ASSERT(false);
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
			CComPtr<IStream> pStream;
			hr = pUnk->QueryInterface(IID_IStream, reinterpret_cast<void**>(&pStream));
			if (S_OK == hr)
			{
				if (nullptr != pStream )
				{
					hr = GetHGlobalFromStream(pStream, &hg);
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

	// Use SVFileNameClass for browsing
	if (m_svfnFileName.SaveFile())
	{
		SVString Extension = SvUl_SF::LoadSVString(IDS_FULL_MASKFILE_EXTENSION);

		if (m_svfnFileName.GetExtension() != Extension)
		{
			m_svfnFileName.SetExtension(Extension.c_str());
		}

		HRESULT hr = m_maskController.ExportMask( m_svfnFileName.GetFullFileName() );
		if (!SUCCEEDED(hr))
		{
			SvStl::MessageMgrStd Msg( SvStl::LogAndDisplay );
			SVStringVector msgList;
			msgList.push_back( m_svfnFileName.GetFullFileName() );
			Msg.setMessage( SVMSG_SVO_93_GENERAL_WARNING, SvOi::Tid_Error_CannotOpenFile, msgList, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_10216 );
		}
	}
	// Refresh image view...
	initMask();
}

void SVToolAdjustmentDialogMaskPageClass::OnImportMaskButton() 
{
	UpdateData(true);
		
	// Use SVFileNameClass for browsing.
	if (m_svfnFileName.SelectFile())
	{
		HRESULT hr = m_maskController.ImportMask(m_svfnFileName.GetFullFileName());
		if (!SUCCEEDED(hr))
		{
			SvStl::MessageMgrStd Msg( SvStl::LogAndDisplay );
			SVStringVector msgList;
			msgList.push_back( SVString(m_svfnFileName.GetFullFileName()) );
			Msg.setMessage( SVMSG_SVO_93_GENERAL_WARNING, SvOi::Tid_Error_CannotOpenFile, msgList, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_10217 );
		}
	}
	// Refresh image view...
	initMask();
}

void SVToolAdjustmentDialogMaskPageClass::OnEditShapeMaskButton() 
{
	m_Values.Commit(true);

	SVMaskShapeEditorDlg dlg(m_InspectionID, m_TaskObjectID, m_maskController.GetInstanceID());

	long selTab = 2; 
	m_dialogImage.GetSelectedTab(&selTab);
	dlg.setSelectedTab(selTab);

	// remember cancel data
	dlg.CheckPoint();

	if (IDOK == dlg.DoModal())
	{
		m_Values.Init();
		GetData();

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
	const SvUl::NameGuidList& rAvailableImageList = m_ImageController.GetAvailableImageList();
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

UINT_PTR CALLBACK SVToolAdjustmentDialogMaskPageClass::ColorDlgHookFn( HWND hdlg, UINT uiMsg, WPARAM wParam, LPARAM lParam )
{
	int iReturnCode = 0;	// by default allow color dlg to process message

#if defined (TRACE_THEM_ALL) || defined (TRACE_OTHER)
	TRACE("ColorDlgHookFn - MSG: %08X, WPARAM: %08X, LPARAM: %08X\n", uiMsg, wParam, lParam);
#endif

	CWnd* pwndTmp = CWnd::FromHandle(hdlg);
	ASSERT(pwndTmp);
	CColorDialog* pDlg = dynamic_cast <CColorDialog*> (pwndTmp);

	switch (uiMsg)
	{
		case WM_INITDIALOG:
		{
			CWnd* pwndTmp = CWnd::FromHandle(hdlg);
			ASSERT(pwndTmp);
			if (pwndTmp)
			{
				pwndTmp->GetDlgItem(COLOR_RAINBOW)->ShowWindow(SW_HIDE);
				pwndTmp->GetDlgItem(COLOR_BOX1)->ShowWindow(SW_HIDE);
				pwndTmp->GetDlgItem(COLOR_CUSTOM1)->ShowWindow(SW_HIDE);
				pwndTmp->GetDlgItem(COLOR_SOLID)->ShowWindow(SW_HIDE);
				
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
			CWnd* pWndRed = pDlg->GetDlgItem(COLOR_RED);
			ASSERT(pWndRed);
			if (pWndRed)
			{
				CString sText;
				pWndRed->GetWindowText(sText);

				m_pThis->m_Values.Set<CString>(FillColorTag, sText);
				m_pThis->m_Values.Commit(true);
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
	IPictureDisp* pResultImage = m_ImageController.GetImage(m_resultImageID.ToGUID());
	IPictureDisp* pMaskImage = m_maskController.GetMaskImage();

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
	const SvUl::InputNameGuidPairList& rImageList = m_ImageController.GetConnectedImageList(m_maskController.GetInstanceID(), 1);

	SvUl::InputNameGuidPairList::const_iterator it = rImageList.begin();
	if (it != rImageList.end())
	{
		m_imageInputName = it->first;
		m_imageName = it->second.first;
	}
}

void SVToolAdjustmentDialogMaskPageClass::RetreiveResultImageNames()
{
	const SvUl::NameGuidList& rImageList = m_ImageController.GetResultImages();

	SvUl::NameGuidList::const_iterator it = rImageList.begin();
	if (it != rImageList.end())
	{
		m_resultImageName = it->first;
		m_resultImageID = it->second;
	}
}

