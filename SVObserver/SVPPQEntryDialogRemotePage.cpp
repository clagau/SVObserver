//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVPPQEntryDialogRemotePageClass
//* .File Name       : $Workfile:   SVPPQEntryDialogRemotePage.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.4  $
//* .Check In Date   : $Date:   02 Oct 2013 07:12:20  $
//******************************************************************************

#include "stdafx.h"
#include <boost/config.hpp>
#include <boost/bind.hpp>
#include "SVPPQEntryDialogRemotePage.h"
#include "SVObjectLibrary/SVObjectManagerClass.h"
#include "SVPPQEntryDialog.h"
#include "SVSVIMStateClass.h"
#include "SVRemoteInputObject.h"
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
//* Class Name : SVPPQEntryDialogRemotePageClass
//* Note(s)    : property page
//*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/


/////////////////////////////////////////////////////////////////////////////
// SVPPQEntryDialogRemotePageClass property page

IMPLEMENT_DYNCREATE(SVPPQEntryDialogRemotePageClass, CPropertyPage)

SVPPQEntryDialogRemotePageClass::SVPPQEntryDialogRemotePageClass()
: CPropertyPage(SVPPQEntryDialogRemotePageClass::IDD)
, m_AvailableItems( boost::bind( &( CListBox::GetItemData ), &availableInputListCtrl, _1 ) , boost::bind( &( CListBox::SetItemData ), &availableInputListCtrl, _1, _2 ) )
, m_SelectedItems( boost::bind( &( CListBox::GetItemData ), &selectedInputListCtrl, _1 ) , boost::bind( &( CListBox::SetItemData ), &selectedInputListCtrl, _1, _2 ) )
{
	//{{AFX_DATA_INIT(SVPPQEntryDialogRemotePageClass)
	StrCurPos = _T("");
	//}}AFX_DATA_INIT
    m_bIsTaken = FALSE;
}

SVPPQEntryDialogRemotePageClass::~SVPPQEntryDialogRemotePageClass()
{
}

void SVPPQEntryDialogRemotePageClass::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(SVPPQEntryDialogRemotePageClass)
	DDX_Control(pDX, IDC_SELECTED_REMOTE_LIST, selectedInputListCtrl);
	DDX_Control(pDX, IDC_AVAILABLE_REMOTE_LIST, availableInputListCtrl);
	DDX_Text(pDX, IDC_CURRENT_POSITION, StrCurPos);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(SVPPQEntryDialogRemotePageClass, CPropertyPage)
	//{{AFX_MSG_MAP(SVPPQEntryDialogRemotePageClass)
	ON_BN_CLICKED(IDC_ADD_REMOTE_BUTTON, OnAddButton)
	ON_BN_CLICKED(IDC_REMOVE_REMOTE_BUTTON, OnRemoveButton)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// SVPPQEntryDialogRemotePageClass message handlers

BOOL SVPPQEntryDialogRemotePageClass::OnInitDialog() 
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

	lSize = static_cast<long>(ppIOEntries.size());

	for( j = 0; j < lSize; ++ j )
	{
		pIOEntry = ppIOEntries[j];

		if( pIOEntry->m_ObjectType != IO_REMOTE_INPUT )
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

void SVPPQEntryDialogRemotePageClass::OnAddButton() 
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
	}// end if

	UpdateData( FALSE );
}

void SVPPQEntryDialogRemotePageClass::OnRemoveButton() 
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
	}// end if

	UpdateData( FALSE );
}

void SVPPQEntryDialogRemotePageClass::OnOK() 
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
		}// end if
	
	}// end for

	SVSVIMStateClass::AddState( SV_STATE_MODIFIED );

	CPropertyPage::OnOK();

}// end OnOK

BOOL SVPPQEntryDialogRemotePageClass::OnApply() 
{
	// Set is taken flag...
	m_bIsTaken = ( selectedInputListCtrl.GetCount() > 0 );

	return CPropertyPage::OnApply();
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVPPQEntryDialogRemotePage.cpp_v  $
 * 
 *    Rev 1.4   02 Oct 2013 07:12:20   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Add x64 platform.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
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
 *    Rev 1.2   13 May 2013 16:29:38   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 015.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.11   08 May 2013 09:35:22   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  826
 * SCR Title:  Fix Memory Leaks in I/O Adjustment Dialogs
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Used new GetItemData method that combined the gui get and the data get into one method.  This method returns an iterator to the data container for extraction and updates(if non-const) of data element.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   08 May 2013 16:31:04   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 012.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.10   24 Apr 2013 14:19:12   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  826
 * SCR Title:  Fix Memory Leaks in I/O Adjustment Dialogs
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added SVDataItemManagerTemplate to manage Item Data.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 13:29:02   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   09 Jul 2012 14:11:58   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to change the include for SVIM State.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   19 Jun 2012 13:49:38   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  744
 * SCR Title:  Add Shared Memory and Socket Functionality for Run Page Web Server
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Optimized the application state functionality.  The application state class was altered to simplify its usage and elements were added to object manager to separate tracking states.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   19 Apr 2011 16:22:12   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to fix issues with IO Assignment.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   18 Apr 2011 10:59:42   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to move the shared data for the IO Entry data to a different class to fix issues with PPQ and Inspection shared data.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   15 Dec 2010 10:36:02   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix connections and synchronization issues with the IO Sub-system.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   13 Dec 2010 11:28:50   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Change the usage of _variant_t to clear the variable after use to eliminate invalid variant type for assignment.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   08 Dec 2010 13:26:02   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to include changes in notification functionality using the Observer Design Pattern.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   22 Apr 2003 14:07:30   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   13 Jan 2003 13:00:08   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  307
 * SCR Title:  Implement the ability to remotely change Tool parameters on the SVIM
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Added new code to fully enable Remote Inputs in the system.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   15 Nov 2002 13:51:56   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  272
 * SCR Title:  Outputs do not always get set correctly.
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   This is the first revision of this file.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
