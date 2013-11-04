//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVPPQEntryDialogDigInPage
//* .File Name       : $Workfile:   SVPPQEntryDialogDigInPage.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.3  $
//* .Check In Date   : $Date:   07 Aug 2013 13:23:12  $
//******************************************************************************

#include "stdafx.h"
#include <boost/config.hpp>
#include <boost/bind.hpp>
#include "SVPPQEntryDialogDigInPage.h"
#include "SVObjectLibrary/SVObjectManagerClass.h"
#include "SVPPQEntryDialog.h"
#include "SVSVIMStateClass.h"
#include "SVDigitalInputObject1.h"
#include "SVInfoStructs.h"
#include "SVPPQObject.h"

//******************************************************************************
//* DEFINITIONS OF MODULE-LOCAL VARIABLES:
//******************************************************************************

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


//******************************************************************************
//* CLASS METHOD IMPLEMENTATION(S):
//******************************************************************************



//*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/
//* Class Name : SVPPQEntryDialogDigInPageClass
//* Note(s)    : property page
//*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/


/////////////////////////////////////////////////////////////////////////////
// SVPPQEntryDialogDigInPageClass property page

IMPLEMENT_DYNCREATE(SVPPQEntryDialogDigInPageClass, CPropertyPage)

SVPPQEntryDialogDigInPageClass::SVPPQEntryDialogDigInPageClass()
: CPropertyPage(SVPPQEntryDialogDigInPageClass::IDD)
, m_AvailableItems( boost::bind( &( CListBox::GetItemData ), &availableInputListCtrl, _1 ) , boost::bind( &( CListBox::SetItemData ), &availableInputListCtrl, _1, _2 ) )
, m_SelectedItems( boost::bind( &( CListBox::GetItemData ), &selectedInputListCtrl, _1 ) , boost::bind( &( CListBox::SetItemData ), &selectedInputListCtrl, _1, _2 ) )
{
	//{{AFX_DATA_INIT(SVPPQEntryDialogDigInPageClass)
	StrCurPos = _T("");
	//}}AFX_DATA_INIT
    m_bIsTaken = FALSE;
}

SVPPQEntryDialogDigInPageClass::~SVPPQEntryDialogDigInPageClass()
{
}

void SVPPQEntryDialogDigInPageClass::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(SVPPQEntryDialogDigInPageClass)
	DDX_Control(pDX, IDC_SELECTED_LIST, selectedInputListCtrl);
	DDX_Control(pDX, IDC_AVAILABLE_LIST, availableInputListCtrl);
	DDX_Text(pDX, IDC_CURRENT_POSITION, StrCurPos);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(SVPPQEntryDialogDigInPageClass, CPropertyPage)
	//{{AFX_MSG_MAP(SVPPQEntryDialogDigInPageClass)
	ON_BN_CLICKED(IDC_ADD_BUTTON, OnAddButton)
	ON_BN_CLICKED(IDC_REMOVE_BUTTON, OnRemoveButton)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// SVPPQEntryDialogDigInPageClass message handlers

BOOL SVPPQEntryDialogDigInPageClass::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();	
	ASSERT( m_pSheet );

	long lSize;
	int j;
	int nIndex;
	SVIOEntryHostStructPtrList ppIOEntries;
	SVIOEntryHostStructPtr pIOEntry;

	// Get list of available inputs
	if( !m_pSheet->m_pPPQ->GetAllInputs( ppIOEntries ) )
		DebugBreak();

	lSize = ppIOEntries.size();

	for( j = 0; j < lSize; ++ j )
	{
		pIOEntry = ppIOEntries[j];

		if( pIOEntry->m_ObjectType != IO_DIGITAL_INPUT )
			continue;

		// Fill selected input list box...
		if( pIOEntry->m_PPQIndex == m_pSheet->m_lCurrentPosition )
		{
			SVObjectClass* l_pObject = SVObjectManagerClass::Instance().GetObject( pIOEntry->m_IOId );

			nIndex = selectedInputListCtrl.AddString( l_pObject->GetName() );
			m_SelectedItems.SetItemData( nIndex, pIOEntry );
		}// end if

		// Fill available input list box...
		if( pIOEntry->m_PPQIndex == -1 )
		{
			SVObjectClass* l_pObject = SVObjectManagerClass::Instance().GetObject( pIOEntry->m_IOId );

			nIndex = availableInputListCtrl.AddString( l_pObject->GetName() );
			m_AvailableItems.SetItemData( nIndex, pIOEntry );
		}// end if

	}// end for

	StrCurPos.Format( "Current PPQ\r\nPosition : %d", m_pSheet->m_lCurrentPosition + 1 );

	UpdateData( FALSE );
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void SVPPQEntryDialogDigInPageClass::OnAddButton() 
{
	ASSERT( m_pSheet );
	UpdateData( TRUE );

	int index = availableInputListCtrl.GetCurSel();
	if( m_pSheet && index >= 0 )
	{
		SVIOEntryHostStructPtr pIOEntry;
		SVDataItemManager::iterator l_Iter = m_AvailableItems.GetItemData( index );

		if( l_Iter != m_AvailableItems.end() )
		{
			pIOEntry = l_Iter->second;
		}

		m_AvailableItems.ClearItemData( index );
		availableInputListCtrl.DeleteString( index );

		if( !( pIOEntry.empty() ) )
		{
			SVObjectClass* l_pObject = SVObjectManagerClass::Instance().GetObject( pIOEntry->m_IOId );

			index = selectedInputListCtrl.AddString( l_pObject->GetName() );
			m_SelectedItems.SetItemData( index, pIOEntry );
		}
	}

	UpdateData( FALSE );
}

void SVPPQEntryDialogDigInPageClass::OnRemoveButton() 
{
	ASSERT( m_pSheet );
	UpdateData( TRUE );

	int index = selectedInputListCtrl.GetCurSel();
	if( m_pSheet && index >= 0 )
	{
		SVIOEntryHostStructPtr pIOEntry;
		SVDataItemManager::iterator l_Iter = m_SelectedItems.GetItemData( index );

		if( l_Iter != m_SelectedItems.end() )
		{
			pIOEntry = l_Iter->second;
		}

		m_SelectedItems.ClearItemData( index );
		selectedInputListCtrl.DeleteString( index );

		if( !( pIOEntry.empty() ) )
		{
			SVObjectClass* l_pObject = SVObjectManagerClass::Instance().GetObject( pIOEntry->m_IOId );

			index = availableInputListCtrl.AddString( l_pObject->GetName() );
			m_AvailableItems.SetItemData( index, pIOEntry );
		}
	}

	UpdateData( FALSE );
}

void SVPPQEntryDialogDigInPageClass::OnOK() 
{
	UpdateData( TRUE );
	ASSERT( m_pSheet );

	m_bIsTaken = ( selectedInputListCtrl.GetCount() > 0 );

	// Check selected input list ( remove deselected items )...
	for( int k = selectedInputListCtrl.GetCount() - 1; k >= 0; -- k )
	{
		SVIOEntryHostStructPtr pIOEntry;
		SVDataItemManager::iterator l_Iter = m_SelectedItems.GetItemData( k );

		if( l_Iter != m_SelectedItems.end() )
		{
			pIOEntry = l_Iter->second;
		}

		m_SelectedItems.ClearItemData( k );
		selectedInputListCtrl.DeleteString( k );

		if( !( pIOEntry.empty() ) )
		{
			pIOEntry->m_Enabled = TRUE;
			pIOEntry->m_PPQIndex = m_pSheet->m_lCurrentPosition;
		}
	}
	
	// And now add new items...( remainder in selected list box control )
	for( int i = availableInputListCtrl.GetCount() - 1; i >= 0;  -- i )
	{
		SVIOEntryHostStructPtr pIOEntry;
		SVDataItemManager::iterator l_Iter = m_AvailableItems.GetItemData( i );

		if( l_Iter != m_AvailableItems.end() )
		{
			pIOEntry = l_Iter->second;
		}

		m_AvailableItems.ClearItemData( i );
		availableInputListCtrl.DeleteString( i );

		if( !( pIOEntry.empty() ) )
		{
			pIOEntry->m_Enabled = FALSE;
			pIOEntry->m_PPQIndex = -1;
		}
	}

	SVSVIMStateClass::AddState( SV_STATE_MODIFIED );

	CPropertyPage::OnOK();
}

BOOL SVPPQEntryDialogDigInPageClass::OnApply() 
{
	// Set is taken flag...
	m_bIsTaken = ( selectedInputListCtrl.GetCount() > 0 );
	
	return CPropertyPage::OnApply();
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVPPQEntryDialogDigInPage.cpp_v  $
 * 
 *    Rev 1.3   07 Aug 2013 13:23:12   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  809
 * SCR Title:  Non I/O SVIM
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Renamed SVIOEntryHostStructPtrVector to SVIOEntryHostStructPtrList.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   13 May 2013 16:29:36   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 015.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.13   08 May 2013 09:35:22   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  826
 * SCR Title:  Fix Memory Leaks in I/O Adjustment Dialogs
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Used new GetItemData method that combined the gui get and the data get into one method.  This method returns an iterator to the data container for extraction and updates(if non-const) of data element.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   08 May 2013 16:30:58   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 012.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.12   24 Apr 2013 14:19:12   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  826
 * SCR Title:  Fix Memory Leaks in I/O Adjustment Dialogs
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added SVDataItemManagerTemplate to manage Item Data.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 13:31:34   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.11   09 Jul 2012 14:11:56   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to change the include for SVIM State.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.10   19 Jun 2012 13:49:38   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  744
 * SCR Title:  Add Shared Memory and Socket Functionality for Run Page Web Server
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Optimized the application state functionality.  The application state class was altered to simplify its usage and elements were added to object manager to separate tracking states.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.9   19 Apr 2011 16:22:10   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to fix issues with IO Assignment.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.8   18 Apr 2011 10:59:40   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to move the shared data for the IO Entry data to a different class to fix issues with PPQ and Inspection shared data.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.7   15 Dec 2010 10:36:02   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix connections and synchronization issues with the IO Sub-system.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.6   13 Dec 2010 11:28:50   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Change the usage of _variant_t to clear the variable after use to eliminate invalid variant type for assignment.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.5   08 Dec 2010 13:26:02   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to include changes in notification functionality using the Observer Design Pattern.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.4   22 Apr 2003 14:07:26   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.3   20 Nov 2002 10:59:38   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  272
 * SCR Title:  Outputs do not always get set correctly.
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   OnApply added
 * Cosmetic changes (member variable names)
 * Changed to accommodate new architecture
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.2   10 May 2001 15:51:42   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  197
 * SCR Title:  Fix lock-ups in application when loading configuration from SVFocusNT
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Changed SVObserver state code in:
 * OnOK
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.1   17 May 2000 17:46:56   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  141
 * SCR Title:  View DDE Inputs/Outputs on Results View
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised OnOk method to update Viewed PPQ Data lists in
 * the documents.
 * Revised OnRemoveButton to reset uObjectAttributesSet.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.0   23 Nov 1999 13:29:12   mike
 * Project:  SVObserver
 * Change Request (SCR) nbr:  61
 * SCR Title:  Update PVCS versioning to version 3.0.
 * Checked in by:  Mike;  Mike McCarl
 * Change Description:  
 *   
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   Sep 16 1999 12:06:36   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  34
 * SCR Title:  Add Build Reference Tool
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Provides Digital Input Selection Page for PQ Tab Bar.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
