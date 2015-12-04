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
#include <boost/assign/list_of.hpp>
#include <colordlg.h>	// for custom color dlg resource #defines
#include "SVToolAdjustmentDialogMaskPageClass.h"
#include "SVMaskShapeEditorDlg.h"
#include "SVMFCControls\SVMaskEditor.h"
#include "ObjectInterfaces\NameValueList.h"
#include "SVObserver.h"
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
   CString strDefExt(TheSVObserverApp.GetStringResource(IDS_FULL_MASKFILE_EXTENSION));
   CString strFilter(TheSVObserverApp.GetStringResource(IDS_MASKFILE_DIALOG_FILTER));

   m_svfnFileName.SetDefaultFileExtension(strDefExt);
   m_svfnFileName.SetFileSaveFlags(OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT);
   m_svfnFileName.SetFileSelectFlags(OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT);
   m_svfnFileName.SetFileExtensionFilterList(strFilter);

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
	m_sFillColor = AsString(lColor);

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
		m_cbMaskOperator.SetItemData(m_cbMaskOperator.AddString(it->first.ToString()), it->second);
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
		m_cbFillOptions.SetItemData(m_cbFillOptions.AddString(it->first.ToString()), it->second);
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
		m_DrawCriteriaCombo.SetItemData(m_DrawCriteriaCombo.AddString(it->first.ToString()), it->second);
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
// .History
//	 Date		Author		Comment                                       
//  :17.04.1999 RO			First Implementation
//	:20.03.2000 RO			Revised to update dialog image right.
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
	if( NULL == m_pMaskEditorCtl )
	{
		m_pMaskEditorCtl = new SvMc::CSVMaskEditor;

		CRect r(0,0,1,1);
		BOOL bResult = m_pMaskEditorCtl->Create(
			"MaskEditorCtlWnd",  //LPCTSTR lpszWindowName, 
			WS_POPUP,  // | WS_VISIBLE,  //DWORD dwStyle,
			r,           //const RECT& rect, 
			this,        //CWnd* pParentWnd, 
			2           //UINT nID,
			//CFile* pPersist = NULL, BOOL bStorage = FALSE,
			//BSTR bstrLicKey = NULL
			);

		if (!bResult)
		{
			delete m_pMaskEditorCtl;
			m_pMaskEditorCtl = nullptr;
			
			AfxMessageBox(_T("Static Mask Editor OCX Failed to Start"));
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
				if (pStream != nullptr)
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
		CString strExt = TheSVObserverApp.GetStringResource(IDS_FULL_MASKFILE_EXTENSION);

		if (m_svfnFileName.GetExtension() != strExt)
		{
			m_svfnFileName.SetExtension(strExt);
		}
		CString strPathName = m_svfnFileName.GetFullFileName();

		HRESULT hr = m_maskController.ExportMask(strPathName);
		if (!SUCCEEDED(hr))
		{
			AfxMessageBox("Cannot open file!");
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
			AfxMessageBox("Cannot open file!");
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
		m_sFillColor = AsString(selectedColor);
	}
	else
	{
		m_sFillColor = AsString(lOriginalVal);
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

	TRACE("ColorDlgHookFn - MSG: %08X, WPARAM: %08X, LPARAM: %08X\n", uiMsg, wParam, lParam);

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
					UINT uiSetRGB = RegisterWindowMessage(SETRGBSTRING);
					::SendMessage(hdlg, uiSetRGB, 0, (LPARAM)RGB(pColor->lCustData, pColor->lCustData, pColor->lCustData));
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

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVToolAdjustmentDialogMaskPageClass.cpp_v  $
 * 
 *    Rev 1.6   26 Jun 2014 18:29:28   mziegler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  885
 * SCR Title:  Replace image display in TA-dialogs with activeX SVPictureDisplay
 * Checked in by:  mZiegler;  Marc Ziegler
 * Change Description:  
 *   use SVPictureDisplay-control
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   03 Jun 2014 13:40:30   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Modified Interface to use IPictureDisp and IStream for bitmaps and mask data.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   15 May 2014 14:36:32   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Revised SVSendMessage to use DWORD_PTR
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   01 Feb 2014 12:22:00   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changed SVSendmessage and processmessage to use LONG_PTR instead of DWORD.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   02 Oct 2013 08:24:42   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Add x64 platform.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   13 Aug 2013 10:40:50   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Included SVUserMaskOperatorClass.h instead of SVOperator.h.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 15:37:04   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.25   30 Jul 2012 13:08:34   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated the parameter change for the visitor fucntionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.24   27 Jul 2012 09:05:12   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fixed include information and updated overlay collection functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.23   02 Jul 2012 17:48:12   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to promote new display functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.22   19 Jun 2012 14:23:22   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  744
 * SCR Title:  Add Shared Memory and Socket Functionality for Run Page Web Server
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Protected Object Information structure attributes.  This change will add accessor methods to get and set attributes.
 * Converted GUID to SVGUID.   This allows the use of standard operators for comparison and methods for conversion.
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.21   07 Apr 2011 16:41:42   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated SVDlgImageClass to change the method init to UpdateImageInfo to make it trackable throughout the system.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.20   16 Mar 2011 13:59:12   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated extent functionality and renamed extent methods to correct extent issues and add clarity to the logical flow.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.19   25 Feb 2011 12:25:46   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated classes needed to consolidate SVImageClass and SVImageObjectClass updating calls to one method name.  Fixed updating problems with child buffers.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.18   10 Feb 2011 15:15:36   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to now use shared pointer instead of untracked structure.  This fixes destruction, tracking and rebuilding issues.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.17   27 Jan 2011 12:03:00   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix issues with Matrox Buffer Management.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.16   04 Nov 2010 14:15:34   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix issues with image processing and display image processing classes and associated includes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.15   16 Dec 2009 13:20:12   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  677
 * SCR Title:  Fix problem in camera notify thread
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fix issues with includes and comments.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.14   03 Sep 2009 10:44:32   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated files to fix moved file includes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.13   30 Jul 2009 13:09:12   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Merged branched changes into current code branch with appropriate updates.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.12   25 Jul 2007 07:48:24   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  597
 * SCR Title:  Upgrade Matrox Imaging Library to version 8.0
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Upgrade to MIL 8.0
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.11   21 Jun 2007 15:03:04   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  598
 * SCR Title:  Upgrade SVObserver to compile using vc++ in VS2005
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   These changes include modification based on fixing compiler-based and project-based differences between VC6 and VC8.  These changes mainly include casting issues, but some include type conversion and assignment of new compiler controlling defines.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.10   21 Jun 2006 13:51:06   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  566
 * SCR Title:  Fix problem with Image Mask
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to move find image input from tool class to task object.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   19 Jan 2006 11:22:54   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  534
 * SCR Title:  Add Image Select for Auxiliary Extent Source
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Update code to not process Aux Extent when a gage tool is in the toolset.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   13 Oct 2005 14:04:06   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Removed index 1 from GetValue calls.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   07 Sep 2005 12:32:22   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  482
 * SCR Title:  New vector based mask
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Draw Criteria Button will be grayed out when Mask is not Activated
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   06 Sep 2005 14:15:56   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Used new method in SVTaskObjectClass to handle the image connections properly.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   25 Aug 2005 14:10:16   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  482
 * SCR Title:  New vector based mask
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   fixed fill color update
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   17 Aug 2005 14:20:06   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   fixed dlg updating mask operator
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   17 Aug 2005 10:36:30   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   implemented Input Request Queue tool updating
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   16 Aug 2005 09:58:06   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Removed un-necessary casting from AddInputRequest method.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   15 Aug 2005 11:35:10   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   fixed problem with buttons not displaying properly when changing mask type
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   09 Aug 2005 08:37:36   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated objects and dialogs to use the ActiveX Interface to update inspection data.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
   
	  Rev 3.49   28 Jul 2005 13:26:58   ebeyeler
   Project:  SVObserver
   Change Request (SCR) nbr:  482
   SCR Title:  New vector based mask
   Checked in by:  eBeyeler;  Eric Beyeler
   Change Description:  
	 make fill options only available with shape masks
   
   /////////////////////////////////////////////////////////////////////////////////////
   
	  Rev 3.48   19 Jul 2005 14:49:30   ebeyeler
   Project:  SVObserver
   Change Request (SCR) nbr:  464
   SCR Title:  Add array indexing for value objects
   Checked in by:  eBeyeler;  Eric Beyeler
   Change Description:  
	 implemented new value object Get/Set
   
   /////////////////////////////////////////////////////////////////////////////////////
   
	  Rev 3.47   19 Jul 2005 10:25:50   ryoho
   Project:  SVObserver
   Change Request (SCR) nbr:  482
   SCR Title:  New vector based mask
   Checked in by:  eBeyeler;  Eric Beyeler
   Change Description:  
	 Cosmetic changes for the Mask dialogs
   
   
   /////////////////////////////////////////////////////////////////////////////////////
   
	  Rev 3.46   27 Jun 2005 09:54:14   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  497
   SCR Title:  Remove Freeze Set from SVObserver
   Checked in by:  Joe;  Joe Spila
   Change Description:  
	 Removed Freeze Tool from configurable tools.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
	  Rev 3.45   22 Jun 2005 12:37:48   ebeyeler
   Project:  SVObserver
   Change Request (SCR) nbr:  464
   SCR Title:  Add array indexing for value objects
   Checked in by:  eBeyeler;  Eric Beyeler
   Change Description:  
	 value objects use SVHRESULT
   
   /////////////////////////////////////////////////////////////////////////////////////
   
	  Rev 3.44   22 Jun 2005 10:36:20   ebeyeler
   Project:  SVObserver
   Change Request (SCR) nbr:  464
   SCR Title:  Add array indexing for value objects
   Checked in by:  eBeyeler;  Eric Beyeler
   Change Description:  
	 object attributes use accessor functions
   
   /////////////////////////////////////////////////////////////////////////////////////
   
	  Rev 3.43   19 May 2005 15:02:24   ebeyeler
   Project:  SVObserver
   Change Request (SCR) nbr:  482
   SCR Title:  New vector based mask
   Checked in by:  eBeyeler;  Eric Beyeler
   Change Description:  
	 implemented Shape Mask
   
   /////////////////////////////////////////////////////////////////////////////////////
   
	  Rev 3.42   18 May 2005 14:10:22   tbair
   Project:  SVObserver
   Change Request (SCR) nbr:  484
   SCR Title:  Display Mask on Source Image
   Checked in by:  tBair;  Tom Bair
   Change Description:  
	 Added support for mask draw criteria combo.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
	  Rev 3.41   09 Mar 2005 13:13:28   tbair
   Project:  SVObserver
   Change Request (SCR) nbr:  478
   SCR Title:  Add a new Perspective Warp Tool
   Checked in by:  tBair;  Tom Bair
   Change Description:  
	 Allow the PerspectiveWarp Tool to be added from the add menu.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
	  Rev 3.40   09 Mar 2005 12:29:06   ebeyeler
   Project:  SVObserver
   Change Request (SCR) nbr:  477
   SCR Title:  Add Cylindrical warp Tool
   Checked in by:  eBeyeler;  Eric Beyeler
   Change Description:  
	 added SVToolCylindricalObjectType and #include
   
   /////////////////////////////////////////////////////////////////////////////////////
   
	  Rev 3.39   18 Feb 2005 11:38:22   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  262
   SCR Title:  Improve performance when loading a configuration in SVObserver
   Checked in by:  Joe;  Joe Spila
   Change Description:  
	 Changed method of tracking values objects from full object manager rebuild to a value object registration methodology.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
	  Rev 3.38   18 Feb 2005 11:10:00   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  463
   SCR Title:  Add new Linear Measurement Tool to SVObserver
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
	 Added new linear tool to allowable tools to configure.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
	  Rev 3.37   18 Feb 2005 07:26:12   tbair
   Project:  SVObserver
   Change Request (SCR) nbr:  456
   SCR Title:  Update Image and Tool Objects to use the new Extent Classes
   Checked in by:  Joe;  Joe Spila
   Change Description:  
	 Changes to support new Reset methodology.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
	  Rev 3.36   16 Feb 2005 15:21:30   ryoho
   Project:  SVObserver
   Change Request (SCR) nbr:  440
   SCR Title:  Create Internal Tool Object to Managing Tool Extents and Result Extents
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
	 updated header files
   
   /////////////////////////////////////////////////////////////////////////////////////
   
	  Rev 3.35   20 Oct 2004 15:15:32   ryoho
   Project:  SVObserver
   Change Request (SCR) nbr:  458
   SCR Title:  Deleting a filter on a tool on certain configuration causes a Dr. Watson
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
	 fixed problem with deleting filters.  
   
   /////////////////////////////////////////////////////////////////////////////////////
   
	  Rev 3.34   12 Oct 2004 12:58:42   ebeyeler
   Project:  SVObserver
   Change Request (SCR) nbr:  448
   SCR Title:  Fix External Tool Change DLL functionality
   Checked in by:  eBeyeler;  Eric Beyeler
   Change Description:  
	 added support for ISVPropertyPageDialog interface
   
   /////////////////////////////////////////////////////////////////////////////////////
   
	  Rev 3.33   07 Sep 2004 09:56:02   tbair
   Project:  SVObserver
   Change Request (SCR) nbr:  446
   SCR Title:  Add new filters to SVObserver
   Checked in by:  tBair;  Tom Bair
   Change Description:  
	 Add  filters button properties for Watershed, Thinning, and Thickening. OnSelChangeList1() and OnButtonProperties()
   
   /////////////////////////////////////////////////////////////////////////////////////
   
	  Rev 3.32   10 Aug 2004 16:18:34   tbair
   Project:  SVObserver
   Change Request (SCR) nbr:  406
   SCR Title:  Implement External Tool
   Checked in by:  tBair;  Tom Bair
   Change Description:  
	 Fixed Bug in External Tool where output images still show up in image lists generated by the SVToolAdjustmentDialogImagePageClass.  Added check to uObjectAttributesAllowed not SV_HIDDEN.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
	  Rev 3.31   21 Jul 2004 09:00:30   ebeyeler
   Project:  SVObserver
   Change Request (SCR) nbr:  406
   SCR Title:  Implement External Tool
   Checked in by:  eBeyeler;  Eric Beyeler
   Change Description:  
	 added CREATE_ALL_CLOSED_OBJECTS message to SVToolAdjustmentDialogSheetClass::OnOK()
   
   /////////////////////////////////////////////////////////////////////////////////////
   
	  Rev 3.30   12 Jul 2004 12:03:44   ebeyeler
   Project:  SVObserver
   Change Request (SCR) nbr:  406
   SCR Title:  Implement External Tool
   Checked in by:  eBeyeler;  Eric Beyeler
   Change Description:  
	 implement External Tool
   
   /////////////////////////////////////////////////////////////////////////////////////
   
	  Rev 3.29   04 Nov 2003 14:40:04   rschock
   Project:  SVObserver
   Change Request (SCR) nbr:  394
   SCR Title:  Implement new RankingFilter for WindowTool that will also be remotely settable
   Checked in by:  rSchock;  Rosco Schock
   Change Description:  
	 Added new ranking filter class that has a 1-7x1-7 customizable neighborhood.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
	  Rev 3.28   04 Nov 2003 14:12:14   rschock
   Project:  SVObserver
   Change Request (SCR) nbr:  378
   SCR Title:  Implement new CustomFilter for WindowTools that will also be remotely settable
   Checked in by:  rSchock;  Rosco Schock
   Change Description:  
	 Update the custom filter class to be 1-7x1-7 customizable kernel instead of a static 3x3 one.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
	  Rev 3.27   07 Aug 2003 12:03:48   ryoho
   Project:  SVObserver
   Change Request (SCR) nbr:  380
   SCR Title:  Add Image Mask option
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
	 added new methods for the new mask that is on the dialog
   
   /////////////////////////////////////////////////////////////////////////////////////
   
	  Rev 3.26   08 Jul 2003 10:15:52   rschock
   Project:  SVObserver
   Change Request (SCR) nbr:  378
   SCR Title:  Implement new CustomFilter for WindowTools that will also be remotely settable
   Checked in by:  rSchock;  Rosco Schock
   Change Description:  
	 Added new code and classes for the new custom filters objects and dialogs.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
	  Rev 3.25   22 Apr 2003 16:47:28   rschock
   Project:  SVObserver
   Change Request (SCR) nbr:  346
   SCR Title:  Update SVObserver to Version 4.21 Release
   Checked in by:  Joe;  Joe Spila
   Change Description:  
	 Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
	  Rev 3.24   31 Jan 2003 08:59:58   joe
   Project:  SVObserver
   Change Request (SCR) nbr:  301
   SCR Title:  Implement Result Image Circle Buffer
   Checked in by:  Joe;  Joe Spila
   Change Description:  
	 Updated OnEditMaskButton and OnInitDialog methods to use the new image circle buffer methodology.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
	  Rev 3.23   20 Nov 2002 13:59:22   ryoho
   Project:  SVObserver
   Change Request (SCR) nbr:  226
   SCR Title:  Monochrome SVIM configuration compatibility between ViperQUAD and ViperDUAL
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
	 theSVObjectManager
	ValueObject Data Indexes
	Removed old Automation
   
   /////////////////////////////////////////////////////////////////////////////////////
   
	  Rev 3.22   30 Jan 2002 16:15:28   rschock
   Project:  SVObserver
   Change Request (SCR) nbr:  278
   SCR Title:  Error message is displayed when removing result objects used by other tools
   Checked in by:  rSchock;  Rosco Schock
   Change Description:  
	 Modified ::refresh and ::initMask and ::initThreshold to validate the toolset before blindly running it.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
	  Rev 3.21   03 Oct 2001 08:43:20   ed
   Project:  SVObserver
   Change Request (SCR) nbr:  225
   SCR Title:  Color SVIM, Incorrect default image for tools
   Checked in by:  Ed;  Ed Chobanoff
   Change Description:  
	 Modified function, 
   SVToolAdjustmentDialogImagePageClass::OnInitDialog().
   
   
   
   /////////////////////////////////////////////////////////////////////////////////////
   
	  Rev 3.20   13 Jun 2001 10:24:28   ed
   Project:  SVObserver
   Change Request (SCR) nbr:  200
   SCR Title:  Update code to implement SVIM COM Server
   Checked in by:  Ed;  Ed Chobanoff
   Change Description:  
	 Revised file header.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
	  Rev 3.19   19 Apr 2001 20:24:52   Steve
   Project:  SVObserver
   Change Request (SCR) nbr:  196
   SCR Title:  Restructure Scripted Load/Save Procedures Functional Requirement
   Checked in by:  Steve;  Stephen E. Steffan
   Change Description:  
	 Header file changes due to include file restructuring.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
	  Rev 3.18   06 Feb 2001 13:47:52   cSchmittinger
   Project:  SVObserver
   Change Request (SCR) nbr:  191
   SCR Title:  Restructure File Management
   Checked in by:  cSchmittinger;  Carl Schmittinger
   Change Description:  
	 Modified source code to use new file management classes.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
	  Rev 3.17   01 Feb 2001 11:55:16   Steve
   Project:  SVObserver
   Change Request (SCR) nbr:  191
   SCR Title:  Restructure File Management
   Checked in by:  Steve;  Stephen S. Steffan
   Change Description:  
	 Modified files to use the SVFileNameClass and the SVFileNameManagerClass if applicable, if the object refers to a filename.  The SVFileNameManagerClass is used if the Object is a persistant file object, that is, if the file object is saved as part of the configuration data.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
	  Rev 3.16.1.0   16 May 2001 14:12:06   rschock
   Project:  SVObserver
   Change Request (SCR) nbr:  202
   SCR Title:  Operator, delete and cut should not be available in File Open/Save Dialogs
   Checked in by:  rSchock;  Rosco Schock
   Change Description:  
	 Changed all occurences of CFileDialog to CSVFileDialog to enable the new security features and privileges
   
   /////////////////////////////////////////////////////////////////////////////////////
   
	  Rev 3.16   24 Oct 2000 15:51:36   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  187
   SCR Title:  Color SVIM Problem when Adding Monochrome Tools
   Checked in by:  Joe;  Joe Spila
   Change Description:  
	 These changes remove the ability to add a monochrome tool to the tool set until a color tool gets added.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
	  Rev 3.15   06 Sep 2000 16:37:12   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  179
   SCR Title:  Create Color SVObserver
   Checked in by:  Joe;  Joe Spila
   Change Description:  
	 Updated files to include color SVIM changes.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
	  Rev 3.14   14 Jun 2000 16:22:16   sjones
   Project:  SVObserver
   Change Request (SCR) nbr:  159
   SCR Title:  Incorrect placement of Profile Tool
   Checked in by:  sJones;  Steve Jones
   Change Description:  
	 Revised OnSelchangedCombo1 to Send the message
   SVM_IMAGE_SOURCE_CHANGED to the tool.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
	  Rev 3.13   19 May 2000 16:06:00   sjones
   Project:  SVObserver
   Change Request (SCR) nbr:  145
   SCR Title:  Half field images and rotation
   Checked in by:  sJones;  Steve Jones
   Change Description:  
	 Moved SVToolAdjustmentDialogTwoImagePageClass to
   it's own header and implementation file (
   SVToolAdjustmentDialogTwoImagePage.h and 
   SVToolAdjustmentDialogTwoImagePage.cpp
   
   /////////////////////////////////////////////////////////////////////////////////////
   
	  Rev 3.12   26 Apr 2000 14:57:28   sjones
   Project:  SVObserver
   Change Request (SCR) nbr:  129
   SCR Title:  Angular Profile Tool
   Checked in by:  sJones;  Steve Jones
   Change Description:  
	 Revised OnSelchangeCurrentAnalyzer to Send 
   SVM_CONNECT_ALL_INPUTS message to the Tool, so
   the friends get their connections as well.
   Revised destructor for 
   SVToolAdjustmentDialogSpecialPropertiesPageClass
   to delete propertyArray.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
	  Rev 3.11   23 Mar 2000 16:48:38   sjones
   Project:  SVObserver
   Change Request (SCR) nbr:  118
   SCR Title:  Integration of LUT Operator
   Checked in by:  sJones;  Steve Jones
   Change Description:  
	 Added new LUT Dialog page.
   Revised to work with new In Place Image Operator List.
   Revised to run Tool instead or OperatorList.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
	  Rev 3.10   03 Mar 2000 16:03:20   sjones
   Project:  SVObserver
   Change Request (SCR) nbr:  113
   SCR Title:  Integrate new Polar Unwrap Tool
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
	 Revised addPages(): Polar Transform (Polar Unwarp) Tool
   now has a standard image source page.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
	  Rev 3.9   Mar 01 2000 13:05:46   Nick
   Project:  SVObserver
   Change Request (SCR) nbr:  113
   SCR Title:  Integrate new Polar Unwrap Tool
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
	 Revised to support new polar transform tool ( Polar Unwrap ) and
   his new SVTADlgPolarTransformPage.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
	  Rev 3.8   29 Feb 2000 14:28:36   sjones
   Project:  SVObserver
   Change Request (SCR) nbr:  106
   SCR Title:  Math Tool - List processing
   Checked in by:  sJones;  Steve Jones
   Change Description:  
	 Removed Close Button on caption.
   Added Handler for WM_SYSCOMMAND to restrict SC_CLOSE message.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
	  Rev 3.7   Feb 24 2000 12:06:38   Nick
   Project:  SVObserver
   Change Request (SCR) nbr:  111
   SCR Title:  Integrate Conditional Tool Set/Tool Drawing
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
	 Added new general page.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
	  Rev 3.6   21 Feb 2000 15:22:16   sjones
   Project:  SVObserver
   Change Request (SCR) nbr:  104
   SCR Title:  New Build Reference Tool
   Checked in by:  sJones;  Steve Jones
   Change Description:  
	 Revised 	SVToolAdjustmentDialogImagePageClass
   constructor to use the id as an argument.
   Added include files for
   SVToolImage.h
   SVTransformationTool.h
   SVTADlgTransformImagePage.h
   SVTADlgTranslationPage.h
   SVTADlgRotationPage.h
   SVTADlgTransformationLearnPage.h
   
   Revised SVToolAdjustmentDialogSheetClass::addPages()
   to add property pages for editing the Transformation Tool.
   
   
   
   /////////////////////////////////////////////////////////////////////////////////////
   
	  Rev 3.5   Feb 21 2000 10:53:02   Nick
   Project:  SVObserver
   Change Request (SCR) nbr:  109
   SCR Title:  Freeze on Reject for v3.1
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
	 Introduced new freeze page and freeze tool.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
	  Rev 3.4   07 Feb 2000 22:51:10   robert
   Project:  SVObserver
   Change Request (SCR) nbr:  93
   SCR Title:  Integrate Multiple Camera Image Input per IPD
   Checked in by:  Robert;  Robert Massinger
   Change Description:  
	 Changed to handle new Page of the Acq. Tool.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
	  Rev 3.3   Jan 14 2000 15:48:02   Nick
   Project:  SVObserver
   Change Request (SCR) nbr:  85
   SCR Title:  Version 3.04 Beta 1 versioning changes.
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
	 Fix memory leak for PMaskEditorCtrl pointer.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
	  Rev 3.2   Jan 03 2000 14:41:00   Nick
   Project:  SVObserver
   Change Request (SCR) nbr:  73
   SCR Title:  Mask Editor Mil 5.0 to Mil 6.0 repairs.
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
	 Changes so bitmap image is created when necessary.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
	  Rev 3.1   Nov 30 1999 15:36:18   Nick
   Project:  SVObserver
   Change Request (SCR) nbr:  65
   SCR Title:  Remove obsolute class SVToolSetViewClass
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
	 Removed reference to SVToolSetViewClass.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
	  Rev 3.0   23 Nov 1999 13:46:08   mike
   Project:  SVObserver
   Change Request (SCR) nbr:  61
   SCR Title:  Update PVCS versioning to version 3.0.
   Checked in by:  Mike;  Mike McCarl
   Change Description:  
	 
   
   /////////////////////////////////////////////////////////////////////////////////////
   
	  Rev 1.40   Nov 17 1999 15:06:30   Nick
   Project:  SVObserver
   Change Request (SCR) nbr:  4
   SCR Title:  Add "Load image" option
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
	 RO change for image arithmetic.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
	  Rev 1.39   Nov 17 1999 07:25:18   Nick
   Project:  SVObserver
   Change Request (SCR) nbr:  15
   SCR Title:  Integrate archive tool
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
	 Added check for unique results archive file paths.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
	  Rev 1.38   Nov 10 1999 12:38:40   Nick
   Project:  SVObserver
   Change Request (SCR) nbr:  57
   SCR Title:  Version 3.00 Beta 18 Versioning
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
	 Documents marked as changed when Dialog OK buttons pressed.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
	  Rev 1.37   Nov 09 1999 10:47:24   Nick
   Project:  SVObserver
   Change Request (SCR) nbr:  21
   SCR Title:  Port Gage Tool code from program version 2.42.
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
	 Remove Gage Tool Image possible image selection for tools.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
	  Rev 1.36   08 Nov 1999 10:58:22   sjones
   Project:  SVObserver
   Change Request (SCR) nbr:  16
   SCR Title:  Integrate Math Tool
   Checked in by:  sJones;  Steve Jones
   Change Description:  
	 Revised to use IDS_INVALID_FORMULA (string table).
   Revised to use IDS_INVALID_CONDITION_FORMULA 
   (string table).
   
   /////////////////////////////////////////////////////////////////////////////////////
   
	  Rev 1.35   Oct 28 1999 11:31:48   Nick
   Project:  SVObserver
   Change Request (SCR) nbr:  52
   SCR Title:  Version 3.00 Beta 14 Versioning
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
	 Tool/Tool set conditional dialog adjustment.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
	  Rev 1.34   19 Oct 1999 17:42:42   sjones
   Project:  SVObserver
   Change Request (SCR) nbr:  48
   SCR Title:  Threshold Page Formula
   Checked in by:  sJones;  Steve Jones
   Change Description:  
	 Moved SVTADlgThresholdPageClass to it's own file(s)
   
   /////////////////////////////////////////////////////////////////////////////////////
   
	  Rev 1.33   14 Oct 1999 12:06:16   sjones
   Project:  SVObserver
   Change Request (SCR) nbr:  32
   SCR Title:  Port Blob Analyzer from v2.42
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
	 Revised not to recreate the same analyzer when picked
   from the dropdown list of available analyzers
   
   /////////////////////////////////////////////////////////////////////////////////////
   
	  Rev 1.32   08 Oct 1999 16:29:48   sjones
   Project:  SVObserver
   Change Request (SCR) nbr:  16
   SCR Title:  Integrate Math Tool
   Checked in by:  sJones;  Steve Jones
   Change Description:  
	 Corrected problem in SVToolAdjustmentDialogAnalyzerPageClass
   which would crash when no analyzer selected and the
   publish button was pressed.
   Added call to SVToolAdjustmentDialogPassFailPageClass::
   OnOK()
   
   /////////////////////////////////////////////////////////////////////////////////////
   
	  Rev 1.31   08 Oct 1999 10:58:50   sjones
   Project:  SVObserver
   Change Request (SCR) nbr:  9
   SCR Title:  Integr. a common object interface to handle and to identify obj. in-/outputs
   Checked in by:  sJones;  Steve Jones
   Change Description:  
	 Added Call to SVIPDoc::SetDefaultInputs() after sending
   SVM_DESTROY_CHILD_OBJECT message to refresh
   dependencies.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
	  Rev 1.30   07 Oct 1999 17:48:38   sjones
   Project:  SVObserver
   Change Request (SCR) nbr:  39
   SCR Title:  Integrate Statistics Tool
   Checked in by:  sJones;  Steve Jones
   Change Description:  
	 Added Call to SVToolAdjustmentStatisticsPageClass::OnOk()
   
   /////////////////////////////////////////////////////////////////////////////////////
   
	  Rev 1.29   05 Oct 1999 13:15:48   sjones
   Project:  SVObserver
   Change Request (SCR) nbr:  39
   SCR Title:  Integrate Statistics Tool
   Checked in by:  sJones;  Steve Jones
   Change Description:  
	 Added entry in AddPages for SVStatisticsToolClass
   
   /////////////////////////////////////////////////////////////////////////////////////
   
	  Rev 1.28   Oct 04 1999 10:04:10   Nick
   Project:  SVObserver
   Change Request (SCR) nbr:  4
   SCR Title:  Add "Load image" option
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
	 Image source fix for profile and others.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
	  Rev 1.27   29 Sep 1999 13:14:50   sjones
   Project:  SVObserver
   Change Request (SCR) nbr:  9
   SCR Title:  Integr. a common object interface to handle and to identify obj. in-/outputs
   Checked in by:  sJones;  Steve Jones
   Change Description:  
	 Disabled and Hid IDCANCEL button.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
	  Rev 1.25   24 Sep 1999 19:04:32   sjones
   Project:  SVObserver
   Change Request (SCR) nbr:  9
   SCR Title:  Integr. a common object interface to handle and to identify obj. in-/outputs
   Checked in by:  sJones;  Steve Jones
   Change Description:  
	 Removed call to RemoveExternalDependencies
   (Fix to restore inputs via script)
   
   /////////////////////////////////////////////////////////////////////////////////////
   
	  Rev 1.24   Sep 24 1999 13:58:48   Nick
   Project:  SVObserver
   Change Request (SCR) nbr:  4
   SCR Title:  Add "Load image" option
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
	 Mask Editor OCX access is corrected.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
	  Rev 1.23   Sep 24 1999 10:39:06   Nick
   Project:  SVObserver
   Change Request (SCR) nbr:  4
   SCR Title:  Add "Load image" option
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
	 Load Image Tool changes.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
	  Rev 1.22   Sep 23 1999 13:59:26   Nick
   Project:  SVObserver
   Change Request (SCR) nbr:  37
   SCR Title:  Version 3.00 Beta 3 Versioning
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
	 Bug fixes in threshold edit control code for threshold values.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
	  Rev 1.21   Sep 22 1999 18:12:58   Nick
   Project:  SVObserver
   Change Request (SCR) nbr:  33
   SCR Title:  Port Profile Tool from v2.42
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
	 Added a SVDoubleValue for auto thresholding value on Threshold page.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
	  Rev 1.20   Sep 20 1999 15:35:26   Nick
   Project:  SVObserver
   Change Request (SCR) nbr:  35
   SCR Title:  PPQ mode to wait for input data
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
	 Image source Page Bug fix..
   
   /////////////////////////////////////////////////////////////////////////////////////
   
	  Rev 1.19   20 Sep 1999 11:01:14   sjones
   Project:  SVObserver
   Change Request (SCR) nbr:  9
   SCR Title:  Integr. a common object interface to handle and to identify obj. in-/outputs
   Checked in by:  sJones;  Steve Jones
   Change Description:  
	 Reincorporate changes from version 1.16.
   Corrected bug - restore pointer to current analyzer after 
   calling setupDialog and Result Dialog
   
   
   /////////////////////////////////////////////////////////////////////////////////////
   
	  Rev 1.18   Sep 20 1999 09:51:14   Nick
   Project:  SVObserver
   Change Request (SCR) nbr:  35
   SCR Title:  PPQ mode to wait for input data
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
	 Fixes in image source page.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
	  Rev 1.17   Sep 15 1999 17:24:00   Nick
   Project:  SVObserver
   Change Request (SCR) nbr:  15
   SCR Title:  Integrate archive tool
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
	 Added 'page' for Archive tool adjustments.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
	  Rev 1.16   15 Sep 1999 17:11:34   sjones
   Project:  SVObserver
   Change Request (SCR) nbr:  9
   SCR Title:  Integr. a common object interface to handle and to identify obj. in-/outputs
   Checked in by:  sJones;  Steve Jones
   Change Description:  
	 Revised for SVUserMaskOperatorClass Embedded Objects.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
	  Rev 1.15   Sep 14 1999 17:11:14   Nick
   Project:  SVObserver
   Change Request (SCR) nbr:  34
   SCR Title:  Add Build Reference Tool
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
	 Added Build Reference Page.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
	  Rev 1.14   14 Sep 1999 17:06:28   sjones
   Project:  SVObserver
   Change Request (SCR) nbr:  16
   SCR Title:  Integrate Math Tool
   Checked in by:  sJones;  Steve Jones
   Change Description:  
	 Added equation validation in OnOk method
   
   /////////////////////////////////////////////////////////////////////////////////////
   
	  Rev 1.13   14 Sep 1999 13:21:24   sjones
   Project:  SVObserver
   Change Request (SCR) nbr:  9
   SCR Title:  Integr. a common object interface to handle and to identify obj. in-/outputs
   Checked in by:  sJones;  Steve Jones
   Change Description:  
	 Revised to set pointer to SVIPDoc in SVChildrenSetupDialog
   
   /////////////////////////////////////////////////////////////////////////////////////
   
	  Rev 1.12   Sep 13 1999 15:22:56   Nick
   Project:  SVObserver
   Change Request (SCR) nbr:  34
   SCR Title:  Add Build Reference Tool
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
	 Remove unnececessary DDX_Control line for dialogImage.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
	  Rev 1.11   Sep 13 1999 14:01:10   Nick
   Project:  SVObserver
   Change Request (SCR) nbr:  33
   SCR Title:  Port Profile Tool from v2.42
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
	 Ported TwoImagePage
   
   /////////////////////////////////////////////////////////////////////////////////////
   
	  Rev 1.10   11 Sep 1999 19:54:04   sjones
   Project:  SVObserver
   Change Request (SCR) nbr:  16
   SCR Title:  Integrate Math Tool
   Checked in by:  sJones;  Steve Jones
   Change Description:  
	 Removed old math tool adjust dialogs includes.
   Added include for SVFormulaEditorPage.
   Added Conditional dialog page to all tools.
   Revised Dialog pages for the MathTool.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
	  Rev 1.9   Sep 10 1999 18:46:16   Nick
   Project:  SVObserver
   Change Request (SCR) nbr:  33
   SCR Title:  Port Profile Tool from v2.42
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
	 Added profile pages.
   Modified Analyzer page to support ObjectTypes.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
	  Rev 1.8   08 Sep 1999 09:20:44   sjones
   Project:  SVObserver
   Change Request (SCR) nbr:  9
   SCR Title:  Integr. a common object interface to handle and to identify obj. in-/outputs
   Checked in by:  sJones;  Steve Jones
   Change Description:  
	 Removed include of SVPublishResultDialog.
   Revised OnPublishButton to refresh Published List.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
	  Rev 1.7   07 Sep 1999 08:21:32   sjones
   Project:  SVObserver
   Change Request (SCR) nbr:  9
   SCR Title:  Integr. a common object interface to handle and to identify obj. in-/outputs
   Checked in by:  sJones;  Steve Jones
   Change Description:  
	 Revised all calls to Run() to new Run(RunStatus)
   
   /////////////////////////////////////////////////////////////////////////////////////
   
	  Rev 1.6   30 Aug 1999 20:11:58   jim
   Project:  SVObserver
   Change Request (SCR) nbr:  14
   SCR Title:  Integrate luminance analyzer into window tool.
   Checked in by:  Jim;  James A. Brown
   Change Description:  
	 Integrating Pixel Analyzer;
   Modified OnPublishButton() to use new result picker.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
	  Rev 1.5   28 Aug 1999 14:27:56   robert
   Project:  SVObserver
   Change Request (SCR) nbr:  21
   SCR Title:  Port Gage Tool code from program version 2.42.
   Checked in by:  Robert;  Robert Massinger
   Change Description:  
	 Replaced DoDetailDialog() call with SetupDialog() call.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
	  Rev 1.4   25 Aug 1999 12:21:16   robert
   Project:  SVObserver
   Change Request (SCR) nbr:  21
   SCR Title:  Port Gage Tool code from program version 2.42.
   Checked in by:  Robert Massinger
   Change Description:  
	 Removed old Multiple Analyzer Page.
   Adapted new object type enumerations into SVToolAdjustmentDialogSheet::addPage().
   
   /////////////////////////////////////////////////////////////////////////////////////
*/
