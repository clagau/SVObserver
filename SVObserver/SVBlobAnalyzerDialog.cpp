//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVBlobAnalyzerDialog
//* .File Name       : $Workfile:   SVBlobAnalyzerDialog.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.7  $
//* .Check In Date   : $Date:   18 Nov 2014 11:13:46  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVBlobAnalyzerDialog.h"

#include "SVBlobAnalyzer.h"
#include "SVInspectionProcess.h"
#include "SVIPDoc.h"
#include "SVResultLong.h"
#include "SVTool.h"
#include "SVResult.h"
#include "SVFillBlobDlg.h"
#include "SVBlobFeaturePropertiesDlg.h"
#include "SVValueObjectImpl.h"
#include "SVSetupDialogManager.h"
#pragma endregion Includes

// e.g. IMPLEMENT_DYNCREATE(<ClassName>, <MFCBaseClass>)

BEGIN_MESSAGE_MAP(SVBlobAnalyzeFeatureDialogClass, CDialog)
	//{{AFX_MSG_MAP(SVBlobAnalyzeFeatureDialogClass)
	ON_BN_CLICKED(IDC_BUTTON1, OnButtonAdd)
	ON_BN_CLICKED(IDC_BUTTON5, OnButtonRemove)
	ON_BN_CLICKED(IDC_BUTTON4, OnButtonSetRange) 
	ON_BN_CLICKED(IDC_SET_SORT_BTN, OnSetSortBtn)
	ON_BN_CLICKED(IDC_ASCENDING, OnAscending)
	ON_WM_DESTROY()
	ON_LBN_SELCHANGE(IDC_LIST2, OnSelchangeList2)
	ON_BN_CLICKED(IDC_NO_OF_BLOBS, OnNbrOfBlobs)
	ON_BN_CLICKED(IDC_EXCLUDE_BLOB, OnExcludeBlob)
	ON_BN_CLICKED(IDC_BTN_FILL_BLOBS, OnBtnFillBlobs)
	ON_BN_CLICKED(IDC_FILL_BLOBS, OnFillBlobs)
	ON_CBN_SELCHANGE(IDC_BLOB_COLOR, OnBlobColor)
	ON_EN_CHANGE(IDC_EDIT_MAX_NBR_BLOBS, OnChangeEditMaxNbrBlobs)
	ON_EN_KILLFOCUS(IDC_EDIT_MAX_NBR_BLOBS, OnKillFocusMaxNbrBlobs)
	ON_BN_CLICKED(IDC_BUTTON_SET_FEATURE_PROPERTIES, OnButtonSetFeatureProperties)
	ON_EN_CHANGE(IDC_EDIT_MAX_BLOB_DATA_ARRAY_SIZE, OnChangeEditMaxBlobDataArraySize)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

////////////////////////////////////////////////////////////////////////////
//
//
SVBlobAnalyzeFeatureDialogClass::SVBlobAnalyzeFeatureDialogClass(  
	SVToolClass* PTool, 
	SVBlobAnalyzerClass* pAnalyzer, 
	SVIPDoc* p_pIPDoc, 
	CWnd* pParent )
	: CDialog( SVBlobAnalyzeFeatureDialogClass::IDD, pParent )
	, m_colorBlobEnum(SV_BLOB_WHITE)
	, m_pIPDoc( p_pIPDoc )
	, m_pCurrentAnalyzer( pAnalyzer )
{
	//{{AFX_DATA_INIT(SVBlobAnalyzeFeatureDialogClass)
	msvSortFeatureEdt = _T("");
	msvAscending = FALSE;
	m_bExclude = FALSE;
	m_lMaxNumberBlobs = 100;
	m_lMaxBlobDataArraySize = 100;
	//}}AFX_DATA_INIT
}

SVBlobAnalyzeFeatureDialogClass::~SVBlobAnalyzeFeatureDialogClass()
{
}

void SVBlobAnalyzeFeatureDialogClass::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(SVBlobAnalyzeFeatureDialogClass)
	DDX_Control(pDX, IDC_EDIT_MAX_BLOB_DATA_ARRAY_SIZE, m_ebMaxBlobDataArraySize);
	DDX_Control(pDX, IDC_EDIT_MAX_NBR_BLOBS, m_ctlMaxNbrBlobs);
	DDX_Control(pDX, IDC_FILL_BLOBS, m_chkFillBlob);
	DDX_Control(pDX, IDC_BTN_FILL_BLOBS, m_btnFillBlobs);
	DDX_Control(pDX, IDC_LIST2, m_lbSelectedFeatures);
	DDX_Control(pDX, IDC_LIST1, m_lbAvailableFeatures);
	DDX_Control(pDX, IDC_BLOB_COLOR, m_cbBlobColor);
	DDX_Text(pDX, IDC_SORT_FEATURE_EDT, msvSortFeatureEdt);
	DDX_Check(pDX, IDC_ASCENDING, msvAscending);
	DDX_Check(pDX, IDC_EXCLUDE_BLOB, m_bExclude);
	DDX_Text(pDX, IDC_EDIT_MAX_NBR_BLOBS, m_lMaxNumberBlobs);
	DDV_MinMaxLong(pDX, m_lMaxNumberBlobs, 1, 10000);
	DDX_Text(pDX, IDC_EDIT_MAX_BLOB_DATA_ARRAY_SIZE, m_lMaxBlobDataArraySize);
	DDV_MinMaxLong(pDX, m_lMaxBlobDataArraySize, 1, m_lMaxNumberBlobs);
	//}}AFX_DATA_MAP
}

//
// Saved location of dialog.
//
static CPoint gPtPosition(0,0);

BOOL SVBlobAnalyzeFeatureDialogClass::OnInitDialog() 
{
	long    lSortFeature;

	CDialog::OnInitDialog();
	
	// Remove Close Button
	ModifyStyle( WS_SYSMENU, 0, SWP_FRAMECHANGED );

	while (1)
	{
		SetTaskObject( m_pCurrentAnalyzer );

		if( m_pCurrentAnalyzer )
		{
	//      List of not enabled.
			initFeatureListBox(m_lbAvailableFeatures, m_pCurrentAnalyzer->getFeatureList(false));

	//      List of enabled.
			initFeatureListBox(m_lbSelectedFeatures, m_pCurrentAnalyzer->getFeatureList(true));

	//      _tcscpy (msvszOriginalFeaturesEnabled, msvszOriginalFeaturesEnabled); 
			m_pCurrentAnalyzer->msvSortAscending.GetValue(msvAscending);
			m_pCurrentAnalyzer->msvbExcludeFailed.GetValue(m_bExclude);
			m_pCurrentAnalyzer->m_colorBlobEnumValue.GetValue(m_colorBlobEnum);
			m_pCurrentAnalyzer->m_lvoBlobSampleSize.GetValue(m_lMaxNumberBlobs);
			m_pCurrentAnalyzer->m_lvoMaxBlobDataArraySize.GetValue(m_lMaxBlobDataArraySize);

			m_ctlMaxNbrBlobs.UpdateData(FALSE);
		}
		else
		{
			SendMessage( WM_CLOSE );
			return FALSE;
		}

		msvpTool = (SVToolClass *) m_pCurrentAnalyzer->GetOwner ();
		if (!msvpTool)
		{
			msvError.msvlErrorCd = -1145;
			SV_TRAP_ERROR_BRK_TSTFIRST (msvError, 1145);
		}
		
		m_pCurrentAnalyzer->msvSortFeature.GetValue (lSortFeature);
		
		if (lSortFeature < 0)
		{
			msvSortFeatureEdt.Empty ();
		}
		else
		{
			msvSortFeatureEdt.Format ("%s", m_pCurrentAnalyzer->msvValue [lSortFeature].GetName ());
		}

		EnableButtons();

		//check to see if the fill blobs is checked.  if so, enable 
		//the button.
		BOOL l_bFillBlobValue;
		m_pCurrentAnalyzer->m_bvoFillBlobs.GetValue( l_bFillBlobValue );

		if ( l_bFillBlobValue )
		{
			m_btnFillBlobs.EnableWindow(TRUE);
			m_chkFillBlob.SetCheck(TRUE);
		}
		else
		{
			m_btnFillBlobs.EnableWindow(FALSE);
			m_chkFillBlob.SetCheck(FALSE);
		}

		if (SV_BLOB_BLACK == m_colorBlobEnum)
		{
			m_cbBlobColor.SetCurSel(0);
		}
		else
		{
			m_cbBlobColor.SetCurSel(1);
		}

		UpdateData (FALSE);

		break;
	}

	//
	// Position this dialog at saved (or initial) location.
	//
	BOOL bResult2 = SetWindowPos(
	                             NULL,          // Z order not used
	                             gPtPosition.x,
	                             gPtPosition.y,
	                             0,0,           // Width and Height Not Used
	                             SWP_NOSIZE | SWP_NOZORDER | SWP_NOREDRAW 
	                            );
	ASSERT(bResult2);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX-Eigenschaftenseiten sollten FALSE zurückgeben
}

HRESULT SVBlobAnalyzeFeatureDialogClass::SetInspectionData()
{
	HRESULT l_hrOk = S_FALSE;

	if( m_pCurrentAnalyzer )
	{
		UpdateData(TRUE);

		l_hrOk = AddInputRequest( &( m_pCurrentAnalyzer->msvPersistantFeaturesEnabled ), m_pCurrentAnalyzer->msvszFeaturesEnabled );

		if( l_hrOk == S_OK )
		{
			l_hrOk = AddInputRequest( &( m_pCurrentAnalyzer->msvSortAscending ), msvAscending );
		}

		if( l_hrOk == S_OK )
		{
			l_hrOk = AddInputRequest( &( m_pCurrentAnalyzer->msvbExcludeFailed ), m_bExclude );
		}

		if( l_hrOk == S_OK )
		{
			l_hrOk = AddInputRequest( &( m_pCurrentAnalyzer->m_colorBlobEnumValue ), m_colorBlobEnum );
		}

		if( l_hrOk == S_OK )
		{
			l_hrOk = AddInputRequest( &( m_pCurrentAnalyzer->m_bvoFillBlobs ), m_chkFillBlob.GetCheck() );
		}

		if ( l_hrOk == S_OK)
		{
			l_hrOk = AddInputRequest( &( m_pCurrentAnalyzer->m_lvoBlobSampleSize ), m_lMaxNumberBlobs );
		}

		if( l_hrOk == S_OK )
		{
			l_hrOk = AddInputRequestMarker();
		}

		if( l_hrOk == S_OK )
		{
			l_hrOk = RunOnce( m_pCurrentAnalyzer->GetTool() );
		}
	}

	return l_hrOk;
}

/////////////////////////////////////////////////////////////////////////////
//
//
void SVBlobAnalyzeFeatureDialogClass::EnableButtons()
{
	// Check for features selected
	int count = m_lbSelectedFeatures.GetCount();
	bool l_bSetRange = true;
	
	CWnd* pAscendingCheckBoxWnd = GetDlgItem( IDC_ASCENDING );
	CWnd* pSetSortButtonWnd = GetDlgItem( IDC_SET_SORT_BTN );
	CWnd* pSetRangeButtonWnd = GetDlgItem( IDC_BUTTON4 );

	BOOL state = FALSE;

	if( count ) 
	{
		// Check for the "No Feature" string
		long newIndex = static_cast< long >( m_lbSelectedFeatures.GetItemData( 0 ) );
		if( newIndex != LB_ERR)
		{
			state = TRUE;
// Features Box X Max, Box X Min, Box Y Max and Box Y Min should not be removed from the list. 
// So if the selection is one of these features, disable the remove button.
/*			CString		strSelection;
			m_lbSelectedFeatures.GetText(0, strSelection);
			if(strSelection.Find((LPCTSTR)"Box X") == 0 || 
					strSelection.Find((LPCTSTR)"Box Y") == 0)
				((CWnd *)GetDlgItem(IDC_BUTTON5))->EnableWindow(FALSE);
			else
				((CWnd *)GetDlgItem(IDC_BUTTON5))->EnableWindow(TRUE);
*/
			int iIndex = m_lbSelectedFeatures.GetCurSel();
			SVBlobFeatureEnum index = (SVBlobFeatureEnum) m_lbSelectedFeatures.GetItemData(iIndex);

			if ( index == SV_AREA || index == SV_BOXX_MAX || index == SV_BOXX_MIN || index == SV_BOXY_MAX || index == SV_BOXY_MIN )
				((CWnd *)GetDlgItem(IDC_BUTTON5))->EnableWindow(FALSE);
			else
				((CWnd *)GetDlgItem(IDC_BUTTON5))->EnableWindow(TRUE);

			if ( (index == SV_CENTER_X_SOURCE) || (index == SV_CENTER_Y_SOURCE) )
			{
				((CWnd *)GetDlgItem(IDC_SET_SORT_BTN))->EnableWindow(FALSE);
				((CWnd *)GetDlgItem(IDC_BUTTON4))->EnableWindow(FALSE);
				l_bSetRange = false;
			}
			else
			{
				((CWnd *)GetDlgItem(IDC_SET_SORT_BTN))->EnableWindow(TRUE);
				((CWnd *)GetDlgItem(IDC_BUTTON4))->EnableWindow(TRUE);
			}
		}
	}

	pAscendingCheckBoxWnd->EnableWindow( state );
	pAscendingCheckBoxWnd->ShowWindow( state ? SW_SHOW : SW_HIDE );

//	pSetSortButtonWnd->EnableWindow( state );
//	pSetSortButtonWnd->ShowWindow( state ? SW_SHOW : SW_HIDE );

	if ( l_bSetRange )
	{
		pSetRangeButtonWnd->EnableWindow( state );
		pSetRangeButtonWnd->ShowWindow( state ? SW_SHOW : SW_HIDE );
	}
}

void SVBlobAnalyzeFeatureDialogClass::OnButtonAdd() 
{
	long lAvailableIndex = static_cast< long >( m_lbAvailableFeatures.GetItemData( m_lbAvailableFeatures.GetCurSel() ) );
	long lCurrentIndex = static_cast< long >( m_lbSelectedFeatures.GetItemData( m_lbSelectedFeatures.GetCurSel() ) );

	if( lAvailableIndex != LB_ERR &&
		lAvailableIndex >= 0)
	{
		if (lCurrentIndex == LB_ERR)
		{
			m_pCurrentAnalyzer->msvSortFeature.SetValue (1, lAvailableIndex);
			msvSortFeatureEdt.Format ("%s", m_pCurrentAnalyzer->msvValue [lAvailableIndex].GetName ());
		}

		m_pCurrentAnalyzer->msvszFeaturesEnabled [lAvailableIndex] = _T('1');
		m_pCurrentAnalyzer->EnableFeature ((SVBlobFeatureEnum) lAvailableIndex);

//      List of not enabled.
		initFeatureListBox(m_lbAvailableFeatures, m_pCurrentAnalyzer->getFeatureList(false));

//      List of enabled.
		initFeatureListBox(m_lbSelectedFeatures, m_pCurrentAnalyzer->getFeatureList(true));
	}
	EnableButtons();

	UpdateData (FALSE);
}

void SVBlobAnalyzeFeatureDialogClass::OnButtonRemove() 
{
	long index = static_cast< long >( m_lbSelectedFeatures.GetItemData( 
		m_lbSelectedFeatures.GetCurSel() ) );

	if( index != LB_ERR && 
		index >= 0 )
	{
		m_pCurrentAnalyzer->msvszFeaturesEnabled [index] = _T('0');
		m_pCurrentAnalyzer->DisableFeature ((SVBlobFeatureEnum) index);

		//      List of not enabled.
		initFeatureListBox(m_lbAvailableFeatures, m_pCurrentAnalyzer->getFeatureList(false));

		//      List of enabled.
		initFeatureListBox(m_lbSelectedFeatures, m_pCurrentAnalyzer->getFeatureList(true));

		// need to reset the sort index
		long sortIndex;
		m_pCurrentAnalyzer->msvSortFeature.GetValue( sortIndex );

		int count = m_lbSelectedFeatures.GetCount();

		if( index == sortIndex )
		{
			// Only one Entry and it may be the "No Feature" string
			if( count )
			{
				long newIndex = static_cast< long >( m_lbSelectedFeatures.GetItemData( 0 ) );
				if( newIndex == LB_ERR)
				{
					m_pCurrentAnalyzer->msvSortFeature.SetValue (1, -1L);
					msvSortFeatureEdt.Empty ();
				}
				else
				{
					m_pCurrentAnalyzer->msvSortFeature.SetValue (1, newIndex);
					msvSortFeatureEdt.Format ("%s", m_pCurrentAnalyzer->msvValue [newIndex].GetName ());
				}
			}
		}
		else
		{
			if( !count ) // truly empty list
			{
				m_pCurrentAnalyzer->msvSortFeature.SetValue (1, -1L);
				msvSortFeatureEdt.Empty ();
			}
			// Only one Entry and it may be the "No Feature" string
			else if( count == 1 )
			{
				long newIndex = static_cast< long >( m_lbSelectedFeatures.GetItemData( 0 ) );
				if( newIndex == LB_ERR)
				{
					m_pCurrentAnalyzer->msvSortFeature.SetValue (1, -1L);
					msvSortFeatureEdt.Empty ();
				}
			}
		}
	}
	
	EnableButtons();

	UpdateData (FALSE);
}

/////////////////////////////////////////////////////////////////////////////
//
//
void SVBlobAnalyzeFeatureDialogClass::OnButtonSetRange() 
{
	SVErrorClass  error;
	
	SVBlobFeatureEnum index;
	SVResultClass *pAnalyzerResult;
	
	pAnalyzerResult = NULL;
	
	while (1)
	{
		index = (SVBlobFeatureEnum) m_lbSelectedFeatures.GetItemData(
			m_lbSelectedFeatures.GetCurSel());
		
		if (index == LB_ERR || index < 0)
		{
			error.msvlErrorCd = -1141;
			SV_TRAP_ERROR_BRK_TSTFIRST(error, 1141)
		}
		
		pAnalyzerResult = m_pCurrentAnalyzer->GetResultObject( index );
		
		if (!pAnalyzerResult)
		{
			error.msvlErrorCd = -1142;
			SV_TRAP_ERROR_BRK_TSTFIRST(error, 1142)
		}
		
		SVSetupDialogManager::Instance().SetupDialog( pAnalyzerResult->GetClassID(), pAnalyzerResult->GetUniqueObjectID(), this );
		break;
	}
}

void SVBlobAnalyzeFeatureDialogClass::OnOK() 
{
	SetInspectionData();

	m_pIPDoc->SetModifiedFlag();

	// Incase the Dimensional Data changed...
	SVSendMessage ( m_pCurrentAnalyzer->GetInspection(), SVM_RESET_ALL_OBJECTS, NULL, NULL );

	// Rebuild results list/view
	// what about published results list ?
	m_pIPDoc->UpdateAllViews( NULL );

	CDialog::OnOK();
}

void SVBlobAnalyzeFeatureDialogClass::OnCancel() 
{
	CDialog::OnCancel();
}

void SVBlobAnalyzeFeatureDialogClass::OnSetSortBtn() 
{
	long lCurrentIndex = static_cast< long >( m_lbSelectedFeatures.GetItemData( m_lbSelectedFeatures.GetCurSel() ) );

	if( lCurrentIndex != LB_ERR && lCurrentIndex >= 0)
	{
		m_pCurrentAnalyzer->msvSortFeature.SetValue (1, lCurrentIndex);
		msvSortFeatureEdt.Format ("%s", m_pCurrentAnalyzer->msvValue [lCurrentIndex].GetName ());
	}
	else
	{
		m_pCurrentAnalyzer->msvSortFeature.SetValue (1, -1L);
		msvSortFeatureEdt.Empty();
	}
	UpdateData (FALSE);
}

void SVBlobAnalyzeFeatureDialogClass::OnAscending() 
{
	UpdateData( TRUE );
}

void SVBlobAnalyzeFeatureDialogClass::OnDestroy() 
{
	CDialog::OnDestroy();
	
	//
	// Save the current location of the property sheet.
	//
	CRect rectWindow;
	GetWindowRect(&rectWindow);
	gPtPosition.x = rectWindow.left;
	gPtPosition.y = rectWindow.top;

	// TODO: Add your message handler code here
}

// Features Area, Box X Max, Box X Min, Box Y Max and Box Y Min should not be removed from the list. 
// So if the selection is one of these features, disable the remove button.
void SVBlobAnalyzeFeatureDialogClass::OnSelchangeList2() 
{
	long iIndex = m_lbSelectedFeatures.GetCurSel();
	SVBlobFeatureEnum index;// = (SVBlobFeatureEnum) iIndex;

	if( iIndex != LB_ERR )
	{
/*		CString		strSelection;
		m_lbSelectedFeatures.GetText(index, strSelection);
		if(strSelection.Find((LPCTSTR)"Box X") == 0 || 
				strSelection.Find((LPCTSTR)"Box Y") == 0)*/

		index = (SVBlobFeatureEnum) m_lbSelectedFeatures.GetItemData(iIndex);

		if ( index == SV_AREA || index == SV_BOXX_MAX || index == SV_BOXX_MIN || index == SV_BOXY_MAX || index == SV_BOXY_MIN )
			((CWnd *)GetDlgItem(IDC_BUTTON5))->EnableWindow(FALSE);
		else
			((CWnd *)GetDlgItem(IDC_BUTTON5))->EnableWindow(TRUE);

		if ( (index == SV_CENTER_X_SOURCE) || (index == SV_CENTER_Y_SOURCE) )
		{
			((CWnd *)GetDlgItem(IDC_SET_SORT_BTN))->EnableWindow(FALSE);
			((CWnd *)GetDlgItem(IDC_BUTTON4))->EnableWindow(FALSE);
		}
		else
		{
			((CWnd *)GetDlgItem(IDC_SET_SORT_BTN))->EnableWindow(TRUE);
			((CWnd *)GetDlgItem(IDC_BUTTON4))->EnableWindow(TRUE);
		}
	}
}

void SVBlobAnalyzeFeatureDialogClass::OnNbrOfBlobs() 
{
	SVLongResultClass* pAnalyzerResult = m_pCurrentAnalyzer->m_pResultBlob;

	SVSetupDialogManager::Instance().SetupDialog( pAnalyzerResult->GetClassID(), pAnalyzerResult->GetUniqueObjectID(), this );
}

void SVBlobAnalyzeFeatureDialogClass::OnExcludeBlob() 
{
	m_bExclude = (BOOL)((CButton *)GetDlgItem(IDC_EXCLUDE_BLOB))->GetCheck();
}

void SVBlobAnalyzeFeatureDialogClass::OnBtnFillBlobs() 
{
	SVFillBlobDlg dlg;
	
	dlg.m_pvoBlobFillColor = &m_pCurrentAnalyzer->m_evoBlobFillColor;
	dlg.m_pvoBlobType = &m_pCurrentAnalyzer->m_evoBlobType;

	dlg.DoModal();
}

void SVBlobAnalyzeFeatureDialogClass::OnFillBlobs() 
{
	if ( m_chkFillBlob.GetCheck() )
	{
		m_btnFillBlobs.EnableWindow(TRUE);
	}
	else
	{
		m_btnFillBlobs.EnableWindow(FALSE);
	}
	
	SetInspectionData();
}

void SVBlobAnalyzeFeatureDialogClass::OnBlobColor()
{
	UpdateData(TRUE);
	m_colorBlobEnum = m_cbBlobColor.GetCurSel();
	SetInspectionData();
}

void SVBlobAnalyzeFeatureDialogClass::OnChangeEditMaxNbrBlobs() 
{
	int i = 1;

	CString sTmp;
	m_ctlMaxNbrBlobs.GetWindowText(sTmp);

	m_lMaxNumberBlobs = atol(sTmp);


//	m_pCurrentAnalyzer->m_lvoBlobSampleSize.SetValue(1,m_lMaxNumberBlobs);
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
}

void SVBlobAnalyzeFeatureDialogClass::OnKillFocusMaxNbrBlobs()
{
	UpdateData(TRUE);
}

void SVBlobAnalyzeFeatureDialogClass::OnChangeEditMaxBlobDataArraySize() 
{
	UpdateData();

	m_pCurrentAnalyzer->m_lvoMaxBlobDataArraySize.SetValue(1,m_lMaxBlobDataArraySize);
}

void SVBlobAnalyzeFeatureDialogClass::OnButtonSetFeatureProperties() 
{
	if ( m_lbSelectedFeatures.GetCurSel() != LB_ERR )
	{
		SVBlobFeatureEnum eFeature = static_cast <SVBlobFeatureEnum> (m_lbSelectedFeatures.GetItemData( m_lbSelectedFeatures.GetCurSel()) );
		SVBlobFeaturePropertiesDlg dlg;
		HRESULT hr = m_pCurrentAnalyzer->msvValue[ eFeature ].GetDefaultValue(dlg.m_dDefaultValue);
		dlg.m_strFeatureName = m_pCurrentAnalyzer->msvValue[ eFeature ].GetName();
		if ( dlg.DoModal() == IDOK )
		{
			hr = m_pCurrentAnalyzer->msvValue[ eFeature ].SetDefaultValue( dlg.m_dDefaultValue, false );
		}
	}
}

void SVBlobAnalyzeFeatureDialogClass::initFeatureListBox(CListBox& listBox, const SvOi::NameValueList& list)
{
	listBox.ResetContent();

	for (SvOi::NameValueList::const_iterator it = list.cbegin(); list.cend() != it; it++)
	{
		if (!it->first.empty())
		{
			listBox.SetItemData( listBox.AddString( it->first.c_str() ), static_cast<DWORD_PTR>(it->second) );
		}
	}

	if (0 >= listBox.GetCount())
	{
		listBox.SetItemData( listBox.AddString( _T( "(No Feature)" ) ), LB_ERR );
	}

	listBox.SetCurSel( 0 );
}

