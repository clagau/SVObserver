//******************************************************************************
//* COPYRIGHT (c) 2005 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVToolAdjustmentDialogMaskPageClass
//* .File Name       : $Workfile:   SVToolAdjustmentDialogMaskPageClass.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   02 Oct 2013 08:24:42  $
//******************************************************************************

#include "stdafx.h"
#include <colordlg.h>	// for custom color dlg resource #defines
#include "SVToolAdjustmentDialogMaskPageClass.h"

#include "SVObjectLibrary/SVObjectManagerClass.h"
#include "SVRunControlLibrary/SVRunStatus.h"

#include "SVGetObjectDequeByTypeVisitor.h"
#include "SVGlobal.h"
#include "SVMaskEditor.h"
#include "SVMaskShapeEditorDlg.h"
#include "SVObserver.h"
#include "SVUserMaskOperatorClass.h"
#include "SVIPDoc.h"
#include "SVTool.h"
#include "SVToolAdjustmentDialogSheetClass.h"
#include "SVToolSet.h"
#include "SVUnaryImageOperatorList.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

SVToolAdjustmentDialogMaskPageClass* SVToolAdjustmentDialogMaskPageClass::m_pThis = NULL;

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
	ON_NOTIFY(TCN_SELCHANGE, IDC_IMAGE_TAB, OnSelchangeImageTab)
	ON_EN_CHANGE(IDC_EDIT_FILL_COLOR, OnChangeEditFillColor)
	ON_CBN_SELCHANGE(IDC_DRAW_MASK_CRITERIA, OnSelchangeDrawMaskCriteria)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


SVToolAdjustmentDialogMaskPageClass::SVToolAdjustmentDialogMaskPageClass( SVToolAdjustmentDialogSheetClass* Parent ) 
	: CPropertyPage(SVToolAdjustmentDialogMaskPageClass::IDD)
{
	m_pThis = this;
	m_pParentDialog = Parent;
	m_pTool = NULL;
	m_pMask  = NULL;
	m_pUnaryImageOperatorList = NULL;
	m_pMaskEditorCtl = NULL;            // 24 Sep 1999 - frb.

   // begin SES 15-Jan-2001
   // Set SVFileNameClass for Import & Export functions.
   CString strDefExt( 
      TheSVObserverApp.GetStringResource( IDS_FULL_MASKFILE_EXTENSION ) );

   CString strFilter( 
      TheSVObserverApp.GetStringResource( IDS_MASKFILE_DIALOG_FILTER ) );

   m_svfnFileName.SetDefaultFileExtension ( strDefExt );

   m_svfnFileName.SetFileSaveFlags ( OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT );

   m_svfnFileName.SetFileSelectFlags ( OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT );

   m_svfnFileName.SetFileExtensionFilterList ( strFilter );

   // end SES 15-Jan-2001


	if( m_pParentDialog )
	{
		m_pTool = m_pParentDialog->GetTool();

		if (m_pTool)
		{
			SVObjectTypeInfoStruct info;
			info.ObjectType = SVUnaryImageOperatorListObjectType;

			// Get The UnaryImageOperatorList
			m_pUnaryImageOperatorList = dynamic_cast <SVUnaryImageOperatorListClass*>
				 ( reinterpret_cast<SVObjectClass*> ( SVSendMessage( m_pTool, SVM_GETFIRST_OBJECT, NULL, ( DWORD )&info ) ) );

			if( m_pUnaryImageOperatorList )
			{
				info.ObjectType = SVUnaryImageOperatorObjectType;
				info.SubType = SVUserMaskOperatorObjectType;
				m_pMask = dynamic_cast <SVUserMaskOperatorClass*>
					( reinterpret_cast <SVObjectClass*> ( SVSendMessage( m_pUnaryImageOperatorList, SVM_GETFIRST_OBJECT, NULL, ( DWORD )&info ) ) );
			}
		}
	}

	//{{AFX_DATA_INIT(SVToolAdjustmentDialogMaskPageClass)
	m_bActivateMask = FALSE;
	m_iMaskType = -1;
	m_sFillColor = _T("");
	//}}AFX_DATA_INIT

}


SVToolAdjustmentDialogMaskPageClass::~SVToolAdjustmentDialogMaskPageClass()
{
	m_pThis = NULL;
    if(m_pMaskEditorCtl)
    {
        delete m_pMaskEditorCtl;
        m_pMaskEditorCtl = NULL;
    }
}

HRESULT SVToolAdjustmentDialogMaskPageClass::SetInspectionData()
{
	HRESULT l_hrOk = S_FALSE;

	if( m_pMask != NULL )
	{
		UpdateData( TRUE ); // get data from dialog

		l_hrOk = AddInputRequest( &( m_pMask->m_Data.bvoActivated ), m_bActivateMask );

		if( l_hrOk == S_OK )
		{
			l_hrOk = AddInputRequest( &( m_pMask->m_Data.evoCurrentMaskOperator ), static_cast<double>(m_cbMaskOperator.GetItemData( m_cbMaskOperator.GetCurSel() )) );
		}

		if( l_hrOk == S_OK )
		{
			l_hrOk = AddInputRequest( &( m_pMask->m_Data.dwvoMaskType ), m_iMaskType );
		}

		if( l_hrOk == S_OK )
		{
			l_hrOk = AddInputRequest( &( m_pMask->m_Data.lvoFillColor ), m_sFillColor );
		}

		if( l_hrOk == S_OK )
		{
			int iSel = m_cbFillOptions.GetCurSel();
			if ( iSel != CB_ERR )
			{
				long lValue = static_cast<long>(m_cbFillOptions.GetItemData(iSel));

				l_hrOk = AddInputRequest( &( m_pMask->m_Data.evoFillArea ), lValue );
			}
		}

		if( l_hrOk == S_OK )
		{
			int sel = m_DrawCriteriaCombo.GetCurSel();
			if( sel >= 0 )
			{
				long lValue = ( long ) m_DrawCriteriaCombo.GetItemData( sel );

				l_hrOk = AddInputRequest( &( m_pMask->m_Data.evoDrawCriteria ), lValue );
			}
		}

		if( l_hrOk == S_OK )
		{
			l_hrOk = AddInputRequestMarker();
		}

		if( l_hrOk == S_OK )
		{
			l_hrOk = RunOnce( m_pMask->GetTool() );
		}

		UpdateData( FALSE );
	}

	return l_hrOk;
}


void SVToolAdjustmentDialogMaskPageClass::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(SVToolAdjustmentDialogMaskPageClass)
	DDX_Control(pDX, IDC_IMAGE_TAB, m_tabImage);
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

	DisplayImagePage();

	UpdateData( FALSE ); // set data to dialog
}


BOOL SVToolAdjustmentDialogMaskPageClass::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();

	//
	// See if we can 'connect' to the Mask Editor Active X control.
	//
	ASSERT(m_pMaskEditorCtl == NULL);
	
	SetTaskObject( m_pTool );

	if( m_pTool )
	{
		ASSERT( m_pMask );
		if( m_pMask )
		{
			m_pMask->m_Data.bvoActivated.GetValue( m_bActivateMask );

			m_ePage = VIEW_RESULT;
			DisplayImagePage();

			// Fill Arithmetic Operator Combo...
			size_t i=0;
			SVEnumerateVector vec;
			m_pMask->m_Data.evoCurrentMaskOperator.GetEnumTypes( vec );
			for ( i = 0; i < vec.size(); i++ )
			{
				m_cbMaskOperator.SetItemData( m_cbMaskOperator.AddString(vec[i].first), vec[i].second );
			}
			
			RefreshComboBox(m_pMask->m_Data.evoCurrentMaskOperator, &m_cbMaskOperator);


			// Fill the Fill Area combo
			vec.clear();
			m_pMask->m_Data.evoFillArea.GetEnumTypes( vec );
			for ( i = 0; i < vec.size(); i++ )
			{
				m_cbFillOptions.SetItemData( m_cbFillOptions.AddString(vec[i].first), vec[i].second );
			}
			
			RefreshComboBox(m_pMask->m_Data.evoFillArea, &m_cbFillOptions);


			long lColor=0;
			m_pMask->m_Data.lvoFillColor.GetValue(lColor);
			m_sFillColor = AsString(lColor);


			DWORD dwMaskValue=0;
			m_pMask->m_Data.dwvoMaskType.GetValue( dwMaskValue );
			m_iMaskType = (int) dwMaskValue;
			BOOL l_bImageMask = FALSE;

			ShowControls( m_iMaskType );

			if ( dwMaskValue == MASK_TYPE_IMAGE )
			{
				SetupImageMaskCombo();
			}

			// Refresh dialog image...
			m_dialogImage.refresh();

			UpdateData( FALSE ); // set data to dialog

			// Populate Draw Criteria combo...
			CString strEnumList;
			m_pMask->m_Data.evoDrawCriteria.GetEnumTypes( strEnumList );
			m_DrawCriteriaCombo.SetEnumTypes( strEnumList );

			long l_lValue;
			m_pMask->m_Data.evoDrawCriteria.GetValue( l_lValue );

			// Set Combos Current Value
			CString strEnum;
			if( m_pMask->m_Data.evoDrawCriteria.GetValue( strEnum ) == S_OK )
			{
				// Set cur sel in combo box...
				m_DrawCriteriaCombo.SelectString( -1, strEnum );
			}

			UpdateData( FALSE ); // set data to dialog

		}

		CRect rect;
		m_tabImage.GetWindowRect(&rect);
		m_tabImage.SetMinTabWidth( rect.Width() / 3 - 5 );

		TCITEM tcitem;
		tcitem.mask = TCIF_PARAM;

		m_tabImage.InsertItem(0, _T("Tool Input"));
		tcitem.lParam = VIEW_SOURCE;
		m_tabImage.SetItem(0, &tcitem);

		m_tabImage.InsertItem(1, _T("Mask"));
		tcitem.lParam = VIEW_MASK;
		m_tabImage.SetItem(1, &tcitem);

		m_tabImage.InsertItem(2, _T("Tool Result"));
		tcitem.lParam = VIEW_RESULT;
		m_tabImage.SetItem(2, &tcitem);

		m_tabImage.SetCurSel( 2 );	// default to Result


		OnActivateMask();

		return TRUE;  // return TRUE unless you set the focus to a control
				      // EXCEPTION: OCX-Eigenschaftenseiten sollten FALSE zurückgeben
	}
	

	// Not valid call...
	if( GetParent() )
		GetParent()->SendMessage( WM_CLOSE );
	else
		SendMessage( WM_CLOSE );

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX-Eigenschaftenseiten sollten FALSE zurückgeben
}

void SVToolAdjustmentDialogMaskPageClass::RefreshComboBox(SVValueObjectClass& p_rValueObject, CComboBox* p_pComboBox)
{
	ASSERT( p_pComboBox != NULL );
	double dValue = 0;
	p_rValueObject.GetValue(dValue);
	long lValue = static_cast<long>(dValue);

	for( int i = 0; i < p_pComboBox->GetCount(); ++ i )
	{
		if( lValue == ( long ) p_pComboBox->GetItemData( i ) )
		{
			p_pComboBox->SetCurSel( i );
			break;
		}
	}
}

void SVToolAdjustmentDialogMaskPageClass::OnEditStaticMaskButton() 
{
	if( m_pMaskEditorCtl == NULL )
	{
		m_pMaskEditorCtl = new CSVMaskEditor;

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

		if(!bResult)
		{
			delete m_pMaskEditorCtl;
			m_pMaskEditorCtl = NULL;
			
			AfxMessageBox(_T("Static Mask Editor OCX Failed to Start"));
			return;
		}
	}

	// Get the Image for this tool
	SVImageClass* pImage = NULL;
	SVImageInfoClass* pImageInfo = ( SVImageInfoClass* ) ::SVSendMessage( m_pTool, SVM_GETFIRST_IMAGE_INFO, NULL, NULL );
	if( pImageInfo )
	{
		pImageInfo->GetOwnerImage( pImage );
	}

	if( m_pMask && m_pUnaryImageOperatorList && pImage && m_pMaskEditorCtl )
	{
		UpdateData( TRUE );

		/*
		// Hide mask, we need image buffer without mask...
		BOOL bState;
		m_pMask->m_Data.bvoActivated.GetValue( bState );
		m_pMask->m_Data.bvoActivated.SetValue( 1, FALSE );

		// Run complete tool, since we work also with 
		// in place process image operator lists and we need the image buffer
		// without mask...RO_22Mar2000
		if( m_pTool )
		{
			SVRunStatusClass runStatus;
			runStatus.ClearAll();
			runStatus.m_lResultDataIndex  = 1;
			runStatus.m_lResultImageIndex = 1;
			m_pTool->Run( runStatus );
		}
		//*/

		//
		// Create bitmap of output buffer...
		//
		ASSERT(m_pTool);
		
		HBITMAP hBM = SVMilBufferToBitmap2( pImage );
		
		m_pMaskEditorCtl->SetImageDIBHandle( ( long ) hBM );
		
		/*
		// Recover mask state...
		m_pMask->m_Data.bvoActivated.SetValue( 1, bState );
		//*/

		// Set mask Graphix data...
		m_pMaskEditorCtl->SetMaskData( ( long ) m_pMask->m_graphixObject.GetGraphixData() );

		// Set current mask operator...
		long lMaskOperator;
		m_pMask->m_Data.evoCurrentMaskOperator.GetValue( lMaskOperator );

		m_pMaskEditorCtl->SetMaskOperator( lMaskOperator );

		//
		// Edit...Run the Mask Editor OCX dialog
		//
		if( m_pMaskEditorCtl->DoEditMaskModal() == IDOK )
		{
			// Get mask Graphix data...
			m_pMask->m_graphixObject.SetGraphixData( ( HGLOBAL ) m_pMaskEditorCtl->GetMaskData() );
			m_pMask->Refresh();

			// Update combo...
			lMaskOperator = m_pMaskEditorCtl->GetMaskOperator();

			m_pMask->m_Data.evoCurrentMaskOperator.GetValue( lMaskOperator );
			for( int i = 0; i < m_cbMaskOperator.GetCount(); ++ i )
			{
				if( lMaskOperator == ( long ) m_cbMaskOperator.GetItemData( i ) )
				{
					m_cbMaskOperator.SetCurSel( i );
					break;
				}
			}

			UpdateData( FALSE );
		}

		// Refresh Dialog...
		initMask();
	}
}


void SVToolAdjustmentDialogMaskPageClass::OnExportMaskButton() 
{
	if( m_pTool && m_pMask )
	{
		UpdateData( TRUE );

		// begin SES 15-Jan-2001
		// Use SVFileNameClass for browsing
		if( m_svfnFileName.SaveFile())
		{
			CString strExt = TheSVObserverApp.GetStringResource( IDS_FULL_MASKFILE_EXTENSION );

			if (m_svfnFileName.GetExtension() != strExt)
				m_svfnFileName.SetExtension(strExt);

			CString strPathName = m_svfnFileName.GetFullFileName();

			CFile maskFile; 
			if( maskFile.Open( strPathName, CFile::modeCreate | CFile::modeWrite ) )
			{
				CArchive archive( &maskFile, CArchive::store );
				m_pMask->m_graphixObject.Serialize( archive );
				archive.Close();
				maskFile.Close();
			}
			else
				AfxMessageBox( "Cannot open file!" );
		}
		// Refresh image view...
		initMask();
	}
}

void SVToolAdjustmentDialogMaskPageClass::OnImportMaskButton() 
{
	if( m_pMask )
	{
		UpdateData( TRUE );

		// begin SES 15-Jan-2001
		// Use SVFileNameClass for browsing.
		if (m_svfnFileName.SelectFile())
		{
			CFile maskFile; 

			if( maskFile.Open( m_svfnFileName.GetFullFileName (), CFile::modeRead ) )
			{
				CArchive archive( &maskFile, CArchive::load );
				m_pMask->m_graphixObject.Serialize( archive );
				archive.Close();
				maskFile.Close();

				m_pMask->Refresh();
			}
			else
				AfxMessageBox( "Cannot open file!" );

		}
      // end SES 15-Jan-2001

		// Refresh image view...
		initMask();
	}
}

void SVToolAdjustmentDialogMaskPageClass::OnEditShapeMaskButton() 
{
	SVMaskShapeEditorDlg dlg;
	dlg.m_pTool = m_pTool;
	dlg.m_pMask = m_pMask;

	int iSel = m_tabImage.GetCurSel();
	TCITEM tcitem;
	tcitem.mask = TCIF_PARAM;
	m_tabImage.GetItem(iSel, &tcitem);
	dlg.m_ePage = static_cast<SVMaskShapeEditorDlg::ViewImageEnum> (tcitem.lParam);

	// remember cancel data
	SVInputRequestStructMap mapCancelData;
	dlg.GetCancelData( mapCancelData );

	if ( dlg.DoModal() == IDOK )
	{
		RefreshComboBox(m_pMask->m_Data.evoCurrentMaskOperator, &m_cbMaskOperator);
		RefreshComboBox(m_pMask->m_Data.evoFillArea, &m_cbFillOptions);
		long lColor=0;
		m_pMask->m_Data.lvoFillColor.GetValue(lColor);
		m_sFillColor = AsString(lColor);

		UpdateData(FALSE);

		m_ePage = static_cast<ViewImageEnum> (dlg.m_ePage);
		m_tabImage.SetCurSel( m_ePage );
	}
	else
	{
		AddInputRequest( mapCancelData );
		RunOnce( m_pMask->GetTool() );
	}

	// Remove this ResetObject when SVMaskShapeEditorDlg uses ActiveX interface
	//m_pMask->ResetObject();
	//*************************************************************************

	initMask();

}

void SVToolAdjustmentDialogMaskPageClass::OnActivateMask() 
{
	initMask();

	GetDlgItem(IDC_RADIO_STATIC_MASK)->EnableWindow( m_bActivateMask );
	GetDlgItem(IDC_RADIO_IMAGE_MASK)->EnableWindow( m_bActivateMask );
	GetDlgItem(IDC_RADIO_SHAPE_MASK)->EnableWindow( m_bActivateMask );
	GetDlgItem(IDC_MASK_OPERATOR_COMBO)->EnableWindow( m_bActivateMask );
	GetDlgItem(IDC_COMBO_IMAGE_MASK)->EnableWindow( m_bActivateMask );
	GetDlgItem(IDC_EDIT_FILL_COLOR)->EnableWindow( m_bActivateMask );
	GetDlgItem(IDC_BTN_FILL_COLOR_MORE)->EnableWindow( m_bActivateMask );
	GetDlgItem(IDC_BTN_IMPORT_MASK)->EnableWindow( m_bActivateMask );
	GetDlgItem(IDC_BTN_EXPORT_MASK)->EnableWindow( m_bActivateMask );
	GetDlgItem(IDC_BTN_EDIT_SHAPE_MASK)->EnableWindow( m_bActivateMask );
	GetDlgItem(IDC_BTN_EDIT_STATIC_MASK)->EnableWindow( m_bActivateMask );
	GetDlgItem(IDC_COMBO_FILL_OPTIONS)->EnableWindow( m_bActivateMask );
	GetDlgItem(IDC_DRAW_MASK_CRITERIA)->EnableWindow( m_bActivateMask );
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
	GetDlgItem(IDC_EDIT_FILL_COLOR)->ShowWindow( p_iMaskType == MASK_TYPE_SHAPE );
	GetDlgItem(IDC_BTN_FILL_COLOR_MORE)->ShowWindow( p_iMaskType == MASK_TYPE_SHAPE );
	GetDlgItem(IDC_COMBO_FILL_OPTIONS)->ShowWindow( p_iMaskType == MASK_TYPE_SHAPE );
	GetDlgItem(IDC_STATIC_FILL_COLOR)->ShowWindow( p_iMaskType == MASK_TYPE_SHAPE );
	GetDlgItem(IDC_STATIC_FILL_OPTIONS)->ShowWindow( p_iMaskType == MASK_TYPE_SHAPE );
	
	// Static
	GetDlgItem(IDC_BTN_IMPORT_MASK)->ShowWindow( p_iMaskType == MASK_TYPE_STATIC );
	GetDlgItem(IDC_BTN_EXPORT_MASK)->ShowWindow( p_iMaskType == MASK_TYPE_STATIC );
	GetDlgItem(IDC_BTN_EDIT_STATIC_MASK)->ShowWindow( p_iMaskType == MASK_TYPE_STATIC );

	// Shape
	GetDlgItem(IDC_BTN_EDIT_SHAPE_MASK)->ShowWindow( p_iMaskType == MASK_TYPE_SHAPE );
	
	// Image
	GetDlgItem(IDC_COMBO_IMAGE_MASK)->ShowWindow( p_iMaskType == MASK_TYPE_IMAGE );
	GetDlgItem(IDC_STATIC_MASK_IMAGE_SOURCE)->ShowWindow( p_iMaskType == MASK_TYPE_IMAGE );

}

void SVToolAdjustmentDialogMaskPageClass::SetupImageMaskCombo()
{
	//initialize combo box of available images
	SVToolSetClass *pToolSet;
	SVToolClass* m_pTool = m_pParentDialog->GetTool();
	SVIPDoc* l_pIPDoc = m_pParentDialog->GetIPDoc();

	if( l_pIPDoc != NULL && m_pTool != NULL )
	{
		pToolSet = ( SVToolSetClass* ) m_pTool->GetAncestor( SVToolSetObjectType );

		if ( (pToolSet) && (pToolSet->getCurrentImage()) )
		{
			m_imageList.RemoveAll();

			SVInObjectInfoStruct* l_psvImageInfo = NULL;
			SVInObjectInfoStruct* l_psvLastImageInfo = NULL;

			while( l_psvImageInfo == NULL && m_pMask->FindNextInputImageInfo( l_psvImageInfo, l_psvLastImageInfo ) == S_OK )
			{
				if( l_psvImageInfo != NULL )
				{
					m_pImageInputInfo = l_psvImageInfo;

					if( l_psvImageInfo->IsConnected() )
					{
						m_pMask->GetObjectInfo().PObject->ConnectObjectInput( l_psvImageInfo );

						m_pCurrentSourceImage = dynamic_cast <SVImageClass*> (l_psvImageInfo->GetInputObjectInfo().PObject);
					}
				}
				else
				{
					l_psvLastImageInfo = l_psvImageInfo;
					l_psvImageInfo = NULL;
				}
			}

			SVObjectTypeInfoStruct imageObjectInfo;
			imageObjectInfo.ObjectType = SVImageObjectType;
			
			SVGetObjectDequeByTypeVisitor l_Visitor( imageObjectInfo );

			SVObjectManagerClass::Instance().VisitElements( l_Visitor, pToolSet->GetUniqueObjectID() );

			SVGetObjectDequeByTypeVisitor::SVObjectPtrDeque::const_iterator l_Iter;

			for( l_Iter = l_Visitor.GetObjects().begin(); l_Iter != l_Visitor.GetObjects().end(); ++l_Iter )
			{
				SVImageClass* pImage = dynamic_cast< SVImageClass* >( const_cast< SVObjectClass* >( *l_Iter ) );

				if( pImage != NULL )
				{
					SVToolClass* pImageOwnerTool = ( SVToolClass* ) pImage->GetAncestor( SVToolObjectType );

					if( pImageOwnerTool != NULL )
					{
						if( (!( l_pIPDoc->IsToolPreviousToSelected( pImageOwnerTool->GetUniqueObjectID() ) )) ||
							(m_pTool == pImageOwnerTool) )
						{
							break;
						}
					}

					SVImageInfoClass ImageInfo = pImage->GetImageInfo();

					long l_lBandNumber = 1;
					long l_lPixelDepth = 8;

					ImageInfo.GetImageProperty( SVImagePropertyBandNumber, l_lBandNumber );
					ImageInfo.GetImageProperty( SVImagePropertyPixelDepth, l_lPixelDepth );

					pImageOwnerTool = dynamic_cast<SVToolClass *>( ImageInfo.GetOwner() );

					if ( pImageOwnerTool )
					{
						if ( l_lBandNumber == 1 )
						{
							if ( l_lPixelDepth == 8 )
							{
								m_imageList.Add(pImage);
							}
						}
					}
					else
					{
						if ( l_lBandNumber == 1 )
						{
							if ( l_lPixelDepth == 8 )
							{
								m_imageList.Add(pImage);
							}
						}
					}
				}
			}
			/*
			SVImageClass *pImage = (SVImageClass*) ::SVSendMessage( pToolSet, SVM_GETFIRST_OBJECT,NULL,(DWORD)&imageObjectInfo);
			while ( pImage )
			{
				SVImageInfoClass ImageInfo = pImage->GetImageInfo();

				long l_lBandNumber = 1;
				long l_lPixelDepth = 8;

				ImageInfo.GetImageProperty( SVImagePropertyBandNumber, l_lBandNumber );
				ImageInfo.GetImageProperty( SVImagePropertyPixelDepth, l_lPixelDepth );

				SVToolClass *pImageOwnerTool = dynamic_cast<SVToolClass *>( ImageInfo.GetOwner() );
				if ( pImageOwnerTool )
				{
					if ( l_lBandNumber == 1 )
					{
						if ( l_lPixelDepth == 8 )
						{
							m_imageList.Add(pImage);
						}
					}
				}
				else
				{
					if ( l_lBandNumber == 1 )
					{
						if ( l_lPixelDepth == 8 )
						{
							m_imageList.Add(pImage);
						}
					}
				}

				// Search for next image...
				pImage = ( SVImageClass *) ::SVSendMessage( pToolSet, SVM_GETNEXT_OBJECT, ( DWORD ) pImage, ( DWORD ) &imageObjectInfo );

				// Ensure only image sources which are produced by tools above the current tool....
				if( pImage )
				{
					SVToolClass* pImageOwnerTool = ( SVToolClass* ) pImage->GetAncestor( SVToolObjectType );

					if( pImageOwnerTool != NULL )
					{
						if( !( l_pIPDoc->IsToolPreviousToSelected( pImageOwnerTool->GetUniqueObjectID() ) ) )
						{
							break;
						}
					}
				}
			}
			*/
		}
	}
	// add image from the mask

	m_cbAvailableSourceImageList.init( &m_imageList, m_pCurrentSourceImage );
}



void SVToolAdjustmentDialogMaskPageClass::OnSelchangeComboImageMask() 
{
	UpdateData( TRUE ); // get data from dialog

	int index = ( int ) m_cbAvailableSourceImageList.GetItemData( m_cbAvailableSourceImageList.GetCurSel() );

	if( index != LB_ERR && index >= 0 && index < m_imageList.GetSize() )
	{
		m_pCurrentSourceImage = m_imageList.GetAt( index );

		m_pTool->ConnectToImage( m_pImageInputInfo, m_pCurrentSourceImage );

		m_pCurrentSourceImage = dynamic_cast<SVImageClass*>( m_pImageInputInfo->GetInputObjectInfo().PObject );
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
	dlg.m_cc.hInstance = (HWND)AfxGetApp()->m_hInstance;

	dlg.m_cc.Flags |= CC_ENABLEHOOK;
	dlg.m_cc.lpfnHook = ColorDlgHookFn;
	
	dlg.m_cc.lCustData = atoi(m_sFillColor);

	long lOriginalVal = atol(m_sFillColor);

	if ( dlg.DoModal() == IDOK )
	{
		COLORREF rgb = dlg.GetColor();
		long earlGrayTea = (long) GetRValue(rgb);// + GetGValue(rgb) + GetBValue(rgb)) / 3.0 );

		m_sFillColor = AsString(earlGrayTea);
	}
	else
	{
		m_sFillColor = AsString( lOriginalVal );
	}

	UpdateData(FALSE);

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
			if ( pwndTmp )
			{
				pwndTmp->GetDlgItem(COLOR_RAINBOW)->ShowWindow(SW_HIDE);
				pwndTmp->GetDlgItem(COLOR_BOX1)->ShowWindow(SW_HIDE);
				pwndTmp->GetDlgItem(COLOR_CUSTOM1)->ShowWindow(SW_HIDE);
				pwndTmp->GetDlgItem(COLOR_SOLID)->ShowWindow(SW_HIDE);
				
				CHOOSECOLOR* pColor = reinterpret_cast<CHOOSECOLOR*> (lParam);

				//(dynamic_cast<CColorDialog*>(pwndTmp))->SetCurrentColor(RGB(pColor->lCustData,pColor->lCustData,pColor->lCustData));
				UINT uiSetRGB;
				uiSetRGB = RegisterWindowMessage(SETRGBSTRING);
				::SendMessage(hdlg, uiSetRGB, 0, (LPARAM) RGB(pColor->lCustData,pColor->lCustData,pColor->lCustData)); 
			}
			iReturnCode = 1;	// don't have color dlg handle this again (it has already handled it)
			break;
		}

		/*  // we just make the control read-only and circumvent these update problems
		case WM_COMMAND:
		{
			int iCtrlID = LOWORD(wParam);
			int iNotifyCode = HIWORD(wParam);
			switch (iCtrlID)
			{
				case COLOR_RED:	// 706
				{
					CWnd* pwndTmp = CWnd::FromHandle(hdlg);
					ASSERT(pwndTmp);
					if ( pwndTmp )
					{
						CString sText;
						pwndTmp->GetDlgItem(COLOR_RED)->GetWindowText(sText);
						pwndTmp->GetDlgItem(COLOR_GREEN)->SetWindowText(sText);
						pwndTmp->GetDlgItem(COLOR_BLUE)->SetWindowText(sText);
					}					
					iReturnCode = 1;
					break;
				}
			}
			break;
		}
		*/

		//*
		case WM_PAINT:
		{
			CWnd* pWndRed = pDlg->GetDlgItem(COLOR_RED);
			ASSERT( pWndRed );
			if ( pWndRed )
			{
				CString sText;
				pWndRed->GetWindowText( sText );
				long earlGrayTea = atol( sText );
				//m_pThis->m_pMask->m_Data.lvoFillColor.SetValue(1, earlGrayTea);
				m_pThis->AddInputRequest( SVValueObjectReference( &m_pThis->m_pMask->m_Data.lvoFillColor ), sText );
				m_pThis->AddInputRequestMarker();
				m_pThis->RunOnce( m_pThis->m_pMask->GetTool() );
				m_pThis->DisplayImagePage();
			}
		}
		//*/

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

void SVToolAdjustmentDialogMaskPageClass::OnSelchangeImageTab(NMHDR* pNMHDR, LRESULT* pResult) 
{
	CTabCtrl* pTab = dynamic_cast<CTabCtrl*> ( CWnd::FromHandle( pNMHDR->hwndFrom ) );
	ASSERT( pTab );
	if ( pTab )
	{
		int iSel = pTab->GetCurSel();
		TCITEM tcitem;
		tcitem.mask = TCIF_PARAM;
		pTab->GetItem(iSel, &tcitem);
		m_ePage = static_cast<ViewImageEnum> (tcitem.lParam);
		DisplayImagePage();
	}

	*pResult = 0;
}

void SVToolAdjustmentDialogMaskPageClass::DisplayImagePage()
{
	switch ( m_ePage )
	{
		case VIEW_RESULT:
		{
			// Get the Image for the Dialog
			// Get the Image for this tool
			SVImageInfoClass* pImageInfo = reinterpret_cast < SVImageInfoClass*> (::SVSendMessage( m_pTool, SVM_GETFIRST_IMAGE_INFO, NULL, NULL ) );
			if( pImageInfo )
			{
				SVImageClass* l_pImage = NULL;

				pImageInfo->GetOwnerImage( l_pImage );

				m_dialogImage.UpdateImageInfo( l_pImage );
				m_dialogImage.refresh();
			}
			break;
		}// end case VIEW_RESULT:

		case VIEW_MASK:
		{
			m_dialogImage.UpdateImageInfo( &(m_pMask->m_MaskBufferInfo),  m_pMask->m_MaskBufferHandlePtr );
			m_dialogImage.refresh();
			break;
		}// end case VIEW_MASK:

		case VIEW_SOURCE:
		{
			m_dialogImage.UpdateImageInfo( m_pMask->getReferenceImage(), true );
			m_dialogImage.refresh();
			break;
		}// end case VIEW_SOURCE:

	}// end switch ( m_ePage )
}

void SVToolAdjustmentDialogMaskPageClass::OnSelchangeDrawMaskCriteria() 
{
	initMask();
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVToolAdjustmentDialogMaskPageClass.cpp_v  $
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
