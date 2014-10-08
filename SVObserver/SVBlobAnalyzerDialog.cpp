//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVBlobAnalyzerDialog
//* .File Name       : $Workfile:   SVBlobAnalyzerDialog.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   08 Oct 2014 02:53:40  $
//******************************************************************************

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
{
	//{{AFX_DATA_INIT(SVBlobAnalyzeFeatureDialogClass)
	msvSortFeatureEdt = _T("");
	msvAscending = FALSE;
	m_bExclude = FALSE;
	m_lMaxNumberBlobs = 100;
	m_lMaxBlobDataArraySize = 100;
	//}}AFX_DATA_INIT

	m_pIPDoc = p_pIPDoc;
	m_pCurrentAnalyzer = pAnalyzer;
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
	DDX_Text(pDX, IDC_SORT_FEATURE_EDT, msvSortFeatureEdt);
	DDX_Check(pDX, IDC_ASCENDING, msvAscending);
	DDX_Check(pDX, IDC_EXCLUDE_BLOB, m_bExclude);
	DDX_Text(pDX, IDC_EDIT_MAX_NBR_BLOBS, m_lMaxNumberBlobs);
	DDV_MinMaxLong(pDX, m_lMaxNumberBlobs, 100, 9999);
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
			m_lbAvailableFeatures.init( m_pCurrentAnalyzer, _T('0') );
	//      List of enabled.
			m_lbSelectedFeatures.init( m_pCurrentAnalyzer, _T('1') );

	//      _tcscpy (msvszOriginalFeaturesEnabled, msvszOriginalFeaturesEnabled); 
			m_pCurrentAnalyzer->msvSortAscending.GetValue(msvAscending);
			m_pCurrentAnalyzer->msvbExcludeFailed.GetValue(m_bExclude);
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

			if ( index == SV_BOXX_MAX || index == SV_BOXX_MIN || index == SV_BOXY_MAX || index == SV_BOXY_MIN )
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
		m_lbAvailableFeatures.init( m_pCurrentAnalyzer, _T('0') );

//      List of enabled.
		m_lbSelectedFeatures.init( m_pCurrentAnalyzer, _T('1') );
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
		m_lbAvailableFeatures.init( m_pCurrentAnalyzer, _T('0') );
//      List of enabled.
		m_lbSelectedFeatures.init( m_pCurrentAnalyzer, _T('1') );

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

// Features Box X Max, Box X Min, Box Y Max and Box Y Min should not be removed from the list. 
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

		if ( index == SV_BOXX_MAX || index == SV_BOXX_MIN || index == SV_BOXY_MAX || index == SV_BOXY_MIN )
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
	
	dlg.m_pvoBlobColor = &m_pCurrentAnalyzer->m_evoBlobColor;
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

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVBlobAnalyzerDialog.cpp_v  $
 * 
 *    Rev 1.2   08 Oct 2014 02:53:40   mziegler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  924
 * SCR Title:  Default Parameters in Blob Analyzer Result Number of Blobs
 * Checked in by:  mZiegler;  Marc Ziegler
 * Change Description:  
 *   change max-"Max number of Blobs" from 10000 to 9999
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   01 Oct 2013 12:16:24   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Add x64 platform.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 09:43:34   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.29   04 Sep 2012 14:43:08   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Removed obsolete comment.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.28   02 Jul 2012 16:36:16   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to promote new display functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.27   09 Feb 2012 09:09:28   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  751
 * SCR Title:  Fix validation of desired maximum number of processed blobs
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Fixed validation for Desired Max Number of Processed Blobs.  will now validate when leaving the field.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.26   08 Dec 2010 07:40:18   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to include changes in notification functionality using the Observer Design Pattern.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.25   28 Oct 2010 13:39:22   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  704
 * SCR Title:  Upgrade SVObserver Version for 5.33 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to remove duplicate definition of HRESULT.  The SVHRESULT definition will be removed in place of the Microsoft defined HRESULT.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.24   01 Jun 2010 08:32:38   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  693
 * SCR Title:  Fix Performance Issue with Inspection Process
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to remove duplicate definitions and redundunt containers.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.23   20 Oct 2008 11:00:52   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  635
 * SCR Title:  Blob Analyzer Bug Fixes
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised to remove close box.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.22   21 Jun 2006 14:06:32   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  563
 * SCR Title:  Fix problem with Blob Analyzer changing Max Number of Blobs
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   fixed problem with Max Number of Blobs
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.21   13 Oct 2005 13:59:52   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Removed index 1 from GetValue calls. Updated dialog to handle the change in the fill blob reset functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.20   08 Aug 2005 15:30:06   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated objects and dialogs to use the ActiveX Interface to update inspection data.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.19   28 Jul 2005 13:08:26   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  469
 * SCR Title:  Add array capability to the Blob Analyzer
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   store feature name in feature properties dialog
 * made MaxBlobDataArraySize upper bound be tied to MaxNumberBlobs
 * renamed member variables
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.18   19 Jul 2005 15:17:46   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  469
 * SCR Title:  Add array capability to the Blob Analyzer
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   fixed update bug
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.17   19 Jul 2005 14:38:20   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  469
 * SCR Title:  Add array capability to the Blob Analyzer
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   implemented support for Max Blob Data Array Size
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.16   13 Jul 2005 09:52:38   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  469
 * SCR Title:  Add array capability to the Blob Analyzer
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   changed minimum value for max number of blobs from 100 to 1.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.15   22 Jun 2005 10:39:16   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  464
 * SCR Title:  Add array indexing for value objects
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added m_ prefix to blob analyzer member variable
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.14   20 May 2005 07:37:08   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  487
 * SCR Title:  Add new blob feature for source extents of blob box center
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   changed dialog to add the new features. buttons will be disabled for sort and range for the two new features.  Center X source and CenterY Source
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.13   17 Feb 2005 13:18:14   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  456
 * SCR Title:  Update Image and Tool Objects to use the new Extent Classes
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Update class with new methodologies based on new base object, extents and reset structure.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.12   16 Feb 2004 13:05:26   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  397
 * SCR Title:  Blob Analyzer - Allow Max number of blobs to be set.
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Made change so that 100 is the minimum number of blobs to be processed.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.11   04 Dec 2003 12:58:14   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  397
 * SCR Title:  Blob Analyzer - Allow Max number of blobs to be set.
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Added code for Max Number of Blobs.  The value is now setable
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.10   06 Nov 2003 07:40:30   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  392
 * SCR Title:  Blob Analyzer have option for filling in the blobs
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   changed the layout of the dialog and changed logic based on if fill blobs is turned on
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.9   29 Oct 2003 14:06:54   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  392
 * SCR Title:  Blob Analyzer have option for filling in the blobs
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   added new button on dialog for FillBlobs
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.8   17 Apr 2003 17:08:14   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.7   14 Jan 2003 13:51:08   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  307
 * SCR Title:  Implement the ability to remotely change Tool parameters on the SVIM
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   changed SetValue to send a long
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.6   19 Nov 2002 10:49:02   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  226
 * SCR Title:  Monochrome SVIM configuration compatibility between ViperQUAD and ViperDUAL
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Changed ValueObject semantics to work with buckets
 * Cosmetic change: TheObjectManager -> TheSVObjectManager
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.5   Apr 17 2001 15:14:32   steve
 * Project:  SVObserver
 * Change Request (SCR) nbr:  196
 * SCR Title:  Restructure Scripted Load/Save Procedures Functional Requirement
 * Checked in by:  Steve;  Stephen S. Steffan
 * Change Description:  
 *   Header file chages due to restructuring of include files.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.4   27 Apr 2000 08:37:26   Jim
 * Project:  SVObserver
 * Change Request (SCR) nbr:  131
 * SCR Title:  Bug fixes for blob analyzer
 * Checked in by:  sMarakkath;  Srithaj Marakkath
 * Change Description:  
 *   Added a handler function for the check box 'Exclude Failed Blobs'.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.3   19 Apr 2000 18:27:42   Jim
 * Project:  SVObserver
 * Change Request (SCR) nbr:  128
 * SCR Title:  Suport for 3.11 versioning
 * Checked in by:  JimAdmin;  James A. Brown
 * Change Description:  
 *   The following classes were modified to remove references to SVBlobFeatureClass, and impliment SVBlobFeatureEnum:
 *   SVBlobFeatureDialogClass::OnInitDialog ()
 *   SVBlobFeatureDialogClass::OnButtonAdd ()
 *   SVBlobFeatureDialogClass::OnButtonRemove ()
 *   SVBlobFeatureDialogClass::OnButtonSetRange ()
 *   SVBlobFeatureDialogClass::OnOK ()
 *   SVBlobFeatureDialogClass::OnSetSortBtn ()
 * 
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.2   14 Apr 2000 10:10:02   sMarakkath
 * Project:  SVObserver
 * Change Request (SCR) nbr:  123, 122
 * SCR Title:  Add Grahpical Interface to show all blobs found
 * Checked in by:  sMarakkath;  Srithaj Marakkath
 * Change Description:  
 *   Added an option to exclude or include failed blobs.
 * Added a button to Set Range for Number of Blobs Found.
 * Made the features 'Box X Max', 'Box X Min', 'Box Y Max' and 'Box Y Min' non removable.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.1   Feb 16 2000 16:17:36   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  107
 * SCR Title:  Add More Blob Analyzer Features
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Changed code to add 26 more Blob features that SVObserver can calculate using the MIL 6.0 library.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.0   23 Nov 1999 11:05:42   mike
 * Project:  SVObserver
 * Change Request (SCR) nbr:  61
 * SCR Title:  Update PVCS versioning to version 3.0.
 * Checked in by:  Mike;  Mike McCarl
 * Change Description:  
 *   
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   Nov 17 1999 10:27:20   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  32
 * SCR Title:  Port Blob Analyzer from v2.42
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Save the location of the dialog between 'uses'.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   Nov 10 1999 12:44:52   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  57
 * SCR Title:  Version 3.00 Beta 18 Versioning
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Documents marked as modified if Dialog OK button pressed.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   Nov 09 1999 10:57:18   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  32
 * SCR Title:  Port Blob Analyzer from v2.42
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Remove dead code from SVResult and associated files.
 * SVAvailableFeatureListClass was removed.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   Oct 18 1999 13:04:40   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  32
 * SCR Title:  Port Blob Analyzer from v2.42
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Removed SVBlobAnalyzeResult.h from project.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   13 Oct 1999 13:54:06   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  32
 * SCR Title:  Port Blob Analyzer from v2.42
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Revised to correct problem with sort ascending checkbox.
 * Added logic to Hide/Show Sort Ascending, Set Sort, and
 * Range button based on features selected.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   13 Oct 1999 09:35:30   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  32
 * SCR Title:  Port Blob Analyzer from v2.42
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Revised to set SortFeature correctly when removing features
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   05 Oct 1999 08:50:26   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  32
 * SCR Title:  Port Blob Analyzer from v2.42
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Added CheckBox for Sort Ascending
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   24 Sep 1999 17:25:38   jim
 * Project:  SVObserver
 * Change Request (SCR) nbr:  32
 * SCR Title:  Port Blob Analyzer from v2.42
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   The Blob analyzer is integrated and semi functional.  Still needs debugging.  
 * 
 * Sorting is included.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   15 Sep 1999 22:06:20   jim
 * Project:  SVObserver
 * Change Request (SCR) nbr:  32
 * SCR Title:  Port Blob Analyzer from v2.42
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Mostly commented out and stubbed code for allowing SVBlobAnalyzerClass to compile and run.  Needs work.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   Sep 10 1999 13:57:12   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  32
 * SCR Title:  Port Blob Analyzer from v2.42
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Files extracted from v2.42 code and changed to v3.00 architecture.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
