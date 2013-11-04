//******************************************************************************
//* COPYRIGHT (c) 2005 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVConditionalHistoryBasePage
//* .File Name       : $Workfile:   SVConditionalHistoryBasePage.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 10:00:12  $
//******************************************************************************

#include "stdafx.h"
#include "svobserver.h"
#include "SVConditionalHistoryBasePage.h"
#include "SVObjectLibrary/SVObjectManagerClass.h"
#include "SVConditionalHistorySheet.h"
#include "SVConfigurationObject.h"
#include "SVIPDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// SVConditionalHistoryBasePage property page

BEGIN_MESSAGE_MAP(SVConditionalHistoryBasePage, CPropertyPage)
	//{{AFX_MSG_MAP(SVConditionalHistoryBasePage)
	ON_LBN_SELCHANGE(IDC_LIST_SELECTED, OnSelchangeListSelected_Base)
	ON_CBN_SELCHANGE(IDC_COMBO_INSPECTION, OnSelchangeComboInspection_Base)
	ON_BN_CLICKED(IDC_BTN_CLEAR, OnBtnClear)
	ON_BN_CLICKED(IDC_BTN_CLEAR_ALL, OnBtnClearAll)
	ON_BN_CLICKED(IDC_CHECK_OVERWRITE, OnCheckOverwrite)
	ON_EN_CHANGE(IDC_EDIT_MAX_HISTORY, OnChangeEditMaxHistory)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

SVConditionalHistoryBasePage::SVConditionalHistoryBasePage( SVConditionalHistorySheet* pParent, const CString& szCaption, int id )
: CPropertyPage(id)
{
	//{{AFX_DATA_INIT(SVConditionalHistoryBasePage)
	//m_strInspection = _T("");
	//m_strMaxHistory = _T("");
	//m_bOverwrite = FALSE;
	//}}AFX_DATA_INIT

	m_strCaption = szCaption;
	m_psp.pszTitle = m_strCaption;
	m_psp.dwFlags |= PSP_USETITLE;

	m_pSheet = pParent;
	m_bObjectsChanged = false;
}

SVConditionalHistoryBasePage::~SVConditionalHistoryBasePage()
{
}

void SVConditionalHistoryBasePage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(SVConditionalHistoryBasePage)
	DDX_Control(pDX, IDC_EDIT_MAX_HISTORY, m_ebMaxHistory);
	DDX_Control(pDX, IDC_BTN_CLEAR_ALL, m_btnClearAll);
	DDX_Control(pDX, IDC_BTN_CLEAR, m_btnClear);
	DDX_Control(pDX, IDC_COMBO_INSPECTION, m_cbInspection);
	DDX_Control(pDX, IDC_TREE, m_Tree);
	DDX_Control(pDX, IDC_LIST_SELECTED, m_lbSelected);
//	DDX_CBString(pDX, IDC_COMBO_INSPECTION, m_strInspection);
//	DDX_Text(pDX, IDC_EDIT_MAX_HISTORY, m_strMaxHistory);
//	DDX_Check(pDX, IDC_CHECK_OVERWRITE, m_bOverwrite);
	//}}AFX_DATA_MAP
	DDX_CBString(pDX, IDC_COMBO_INSPECTION, m_pSheet->m_strInspection);
	DDX_Text(pDX, IDC_EDIT_MAX_HISTORY, m_pSheet->m_strMaxHistory);
	DDX_Check(pDX, IDC_CHECK_OVERWRITE, m_pSheet->m_bOverwrite);
}

BOOL SVConditionalHistoryBasePage::OnInitDialog() 
{
	size_t i( 0 );

	CPropertyPage::OnInitDialog();

	m_cbInspection.ShowWindow( SW_HIDE );

	m_Tree.SetCanSelectObjectCallback( SVObjectTreeCanSelectObjectCallbackFn(this, &SVConditionalHistoryBasePage::CanSelectObjectCallback) );
	
	SVConfigurationObject* l_pConfig = NULL;
	SVObjectManagerClass::Instance().GetConfigurationObject( l_pConfig );

	// load inspection list
	SVInspectionProcessPtrList vecInspections;
	l_pConfig->GetInspections( vecInspections );
	int iCurInspection = CB_ERR;
	for ( i=0; i < vecInspections.size(); ++i )
	{
		SVInspectionProcess* pInspection = vecInspections[i];
		CString strName = pInspection->GetName();
		int iIndex = m_cbInspection.AddString(strName);
		m_cbInspection.SetItemDataPtr( iIndex, pInspection );
		if ( pInspection == m_pSheet->m_pInspection )
			iCurInspection = iIndex;
	}

	// select current inspection
	if ( iCurInspection != CB_ERR )
		m_cbInspection.SetCurSel( iCurInspection );

	// initialize page with current inspection data
	InitPage( m_pSheet->m_pInspection );

	// fill the list
	CString strPrefix = m_pSheet->m_pInspection->GetName();
	strPrefix += _T(".Tool Set.");

	std::vector<SVObjectReference> vecObjects;
	std::vector<int> vecIndexes;
	m_Tree.GetSelectedObjectsInTreeOrder( vecObjects, vecIndexes );
	for ( i = 0; i < vecObjects.size(); ++i )
	{
		SVObjectReference refObject = vecObjects.at(i);
		CString strName = refObject.GetCompleteObjectName();
		if ( strName.GetLength() > 0 )
			strName = strName.Mid( strPrefix.GetLength() );
		int iIndex = m_lbSelected.AddString( strName );
		m_lbSelected.SetItemData( iIndex, vecIndexes.at(i) );
	}
	


	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

bool SVConditionalHistoryBasePage::CanSelectObjectCallback( SVObjectReference refObject, bool bCurrentState, int p_iIndex )
{
	UpdateData();

	CString strPrefix = m_pSheet->m_pInspection->GetName();
	strPrefix += _T(".Tool Set.");

	CString strName = refObject.GetCompleteObjectName();

	if ( strName.GetLength() > 0 )
		strName = strName.Mid( strPrefix.GetLength() );

	if ( bCurrentState == false )// want to select
	{
		bool bInserted = false;
		for ( int i=0; i < m_lbSelected.GetCount(); ++i )
		{
			if ( static_cast<int>(m_lbSelected.GetItemData( i )) > p_iIndex )
			{
				m_lbSelected.InsertString( i, strName );
				m_lbSelected.SetItemData( i, p_iIndex );
				bInserted = true;
				break;
			}
		}
		if ( !bInserted )
		{
			int iIndex = m_lbSelected.AddString( strName );
			m_lbSelected.SetItemData( iIndex, p_iIndex );
		}
	}
	else // want to deselect
	{
		int iIndex = m_lbSelected.FindString( 0, strName );
		if ( iIndex != LB_ERR )
			m_lbSelected.DeleteString( iIndex );
	}

	return true;
}

bool SVConditionalHistoryBasePage::QueryAllowExit()
{
	return true;
}

void SVConditionalHistoryBasePage::OnSelchangeListSelected_Base() 
{
	OnSelchangeListSelected();
}

void SVConditionalHistoryBasePage::OnSelchangeComboInspection_Base() 
{
	OnSelchangeComboInspection();
}

void SVConditionalHistoryBasePage::OnBtnClear() 
{
	std::vector<int> vecIndexes;
	if ( m_lbSelected.GetCount() > 0 )
	{
		vecIndexes.resize(m_lbSelected.GetCount());
		int iItems = m_lbSelected.GetSelItems(m_lbSelected.GetCount(), &vecIndexes[0]);
		for ( int i = iItems-1; i >= 0; --i )
		{
			int iIndex = m_lbSelected.GetItemData(vecIndexes.at(i));
			m_Tree.SelectItemByIndex( iIndex, false );
		}
	}
	// edit list
	// send ppq -> set_ch_list
	// call ppq -> get_ch_list
}

void SVConditionalHistoryBasePage::OnBtnClearAll() 
{
	int iItems = m_lbSelected.GetCount();
	for ( int i = iItems-1; i >= 0; --i )
	{
		int iIndex = m_lbSelected.GetItemData(i);
		m_Tree.SelectItemByIndex( iIndex, false );
	}
	// send ppq -> set_ch_list
	// call ppq -> get_ch_list
}

void SVConditionalHistoryBasePage::OnCheckOverwrite() 
{
	// send ppq -> set_ch_prop
	// call ppq -> get_ch_prop
//	UpdateData();
}

void SVConditionalHistoryBasePage::OnChangeEditMaxHistory() 
{
	// send ppq -> set_ch_prop
	// call ppq -> get_ch_prop
//	UpdateData();
}


BOOL SVConditionalHistoryBasePage::OnSetActive() 
{
//	m_bOverwrite    = m_pSheet->m_bOverwrite;
//	m_strMaxHistory = m_pSheet->m_strMaxHistory;
//	m_strInspection = m_pSheet->m_strInspection;

	UpdateData(FALSE);

	return CPropertyPage::OnSetActive();
}

BOOL SVConditionalHistoryBasePage::OnKillActive() 
{
	UpdateData(TRUE);
	
//	m_pSheet->m_bOverwrite    = m_bOverwrite;
//	m_pSheet->m_strMaxHistory = m_strMaxHistory;
//	m_pSheet->m_strInspection = m_strInspection;

	return CPropertyPage::OnKillActive();
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVConditionalHistoryBasePage.cpp_v  $
 * 
 *    Rev 1.0   23 Apr 2013 10:00:12   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   18 Jun 2012 18:15:46   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  744
 * SCR Title:  Add Shared Memory and Socket Functionality for Run Page Web Server
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Centralized the location of the configuration object identifier and removed synchronization problems.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   19 Dec 2011 15:40:30   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  744
 * SCR Title:  Add Shared Memory and Socket Functionality for Run Page Web Server
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated the evaluation process of conditional history.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   08 Dec 2010 07:45:20   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to include changes in notification functionality using the Observer Design Pattern.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   21 Jun 2007 12:01:24   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  598
 * SCR Title:  Upgrade SVObserver to compile using vc++ in VS2005
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   These changes include modification based on fixing compiler-based and project-based differences between VC6 and VC8.  These changes mainly include casting issues, but some include type conversion and assignment of new compiler controlling defines.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   25 Jan 2006 15:11:08   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  529
 * SCR Title:  Add Conditional Product History
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Hide inspection combo box
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   25 Jan 2006 12:36:32   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  529
 * SCR Title:  Add Conditional Product History
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Added object list functionality
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   17 Jan 2006 14:55:06   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  529
 * SCR Title:  Add Conditional Product History
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   fixed property updating
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   17 Jan 2006 09:19:12   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  529
 * SCR Title:  Add Conditional Product History
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   first revision
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
