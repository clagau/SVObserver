//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVPPQEntryDialog
//* .File Name       : $Workfile:   SVPPQEntryDialog.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.4  $
//* .Check In Date   : $Date:   15 May 2014 12:32:56  $
//******************************************************************************

#include "stdafx.h"
#include "SVPPQEntryDialog.h"
#include "SVObjectLibrary/SVObjectManagerClass.h"
#include "SVSVIMStateClass.h"
#include "SVVirtualCamera.h"
#include "SVPPQObject.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


//******************************************************************************
//* CLASS METHOD IMPLEMENTATION(S):
//******************************************************************************



//*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/
//* Class Name : SVPPQEntryDialogCameraPageClass
//* Note(s)    : property page
//*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/

IMPLEMENT_DYNCREATE(SVPPQEntryDialogCameraPageClass, CPropertyPage)

SVPPQEntryDialogCameraPageClass::SVPPQEntryDialogCameraPageClass() : CPropertyPage(SVPPQEntryDialogCameraPageClass::IDD)
{
	//{{AFX_DATA_INIT(SVPPQEntryDialogCameraPageClass)
		StrCurPos = _T("");
	//}}AFX_DATA_INIT
    m_bIsTaken = FALSE;
}

SVPPQEntryDialogCameraPageClass::~SVPPQEntryDialogCameraPageClass()
{
}

void SVPPQEntryDialogCameraPageClass::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(SVPPQEntryDialogCameraPageClass)
	DDX_Control(pDX, IDC_SELECTED_LIST, selectedList);
	DDX_Control(pDX, IDC_AVAILABLE_LIST, availableList);
	DDX_Text(pDX, IDC_CURRENT_POSITION, StrCurPos);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(SVPPQEntryDialogCameraPageClass, CPropertyPage)
	//{{AFX_MSG_MAP(SVPPQEntryDialogCameraPageClass)
	ON_BN_CLICKED(IDC_ADD_BUTTON, OnAddButton)
	ON_BN_CLICKED(IDC_REMOVE_BUTTON, OnRemoveButton)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// SVPPQEntryDialogCameraPageClass message handlers

BOOL SVPPQEntryDialogCameraPageClass::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	ASSERT( m_pSheet );

	SVVirtualCamera *pCamera;
	long lPosition;
	int index;

	std::deque< SVVirtualCamera* > l_Cameras;

	m_pSheet->m_pPPQ->GetCameraList( l_Cameras );

	std::deque< SVVirtualCamera* >::iterator l_Iter = l_Cameras.begin();

	while( l_Iter != l_Cameras.end() )
	{
		pCamera = ( *l_Iter );

		m_pSheet->m_pPPQ->GetCameraPPQPosition( lPosition, pCamera );

		// Fill selected box...
		if( m_pSheet->m_lCurrentPosition == lPosition )			
		{
			index = selectedList.AddString( pCamera->GetCompleteObjectName() );
			selectedList.SetItemData( index , reinterpret_cast<DWORD_PTR>(pCamera) );			
		}// end if

		// Fill available box...
		if( -1 == lPosition )			
		{
			index = availableList.AddString( pCamera->GetCompleteObjectName() );
			availableList.SetItemData( index , reinterpret_cast<DWORD_PTR>(pCamera) );			
		}// end if

		GetDlgItem(IDC_ADD_BUTTON)->EnableWindow(m_pSheet->OkToAdd());
		++l_Iter;
	}// end for

	StrCurPos.Format( "Current PPQ\r\nPosition : %d", m_pSheet->m_lCurrentPosition + 1 );

	UpdateData( FALSE );

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX-Eigenschaftenseiten sollten FALSE zurückgeben
}


void SVPPQEntryDialogCameraPageClass::OnAddButton() 
{
	ASSERT( m_pSheet );
	UpdateData( TRUE );

	int index = availableList.GetCurSel();
	if( m_pSheet && index != LB_ERR )
	{
		SVVirtualCamera* pCamera = (SVVirtualCamera*) availableList.GetItemData( index );
		availableList.DeleteString( index );

		if( pCamera )
		{
			index = selectedList.AddString( pCamera->GetName() );
			selectedList.SetItemData( index, reinterpret_cast<DWORD_PTR>(pCamera) );
		}// end if
		
	}// end if

	UpdateData( FALSE );
}

void SVPPQEntryDialogCameraPageClass::OnRemoveButton() 
{
	ASSERT( m_pSheet );
	UpdateData( TRUE );

	int index = selectedList.GetCurSel();
	if( m_pSheet && index != LB_ERR )
	{
		SVVirtualCamera* pCamera = (SVVirtualCamera*) selectedList.GetItemData( index );
		selectedList.DeleteString( index );

		if( pCamera )
		{
			index = availableList.AddString( pCamera->GetName() );
			availableList.SetItemData( index, reinterpret_cast<DWORD_PTR>(pCamera) );
		}// end if
		
	}// end if

	UpdateData( FALSE );
}

void SVPPQEntryDialogCameraPageClass::OnOK() 
{
	UpdateData( TRUE );
	ASSERT( m_pSheet );

	SVVirtualCamera* pCamera;
	
	m_bIsTaken = ( selectedList.GetCount() > 0 );

	// Check selected camera list ( remove deselected items )...
	int k = selectedList.GetCount() - 1;
	for( ; k >= 0; -- k )
	{
		pCamera = (SVVirtualCamera*) selectedList.GetItemData( k );
		if( pCamera )
		{
			m_pSheet->m_pPPQ->SetCameraPPQPosition( m_pSheet->m_lCurrentPosition, pCamera );
			selectedList.DeleteString( k );
		}// end if

	}// end for

	// And now add new items...( remainder in selected list box control )
	for( int i = availableList.GetCount() - 1; i >= 0; -- i )
	{
		pCamera = (SVVirtualCamera*) availableList.GetItemData( i );
		if( pCamera )
		{
			m_pSheet->m_pPPQ->SetCameraPPQPosition( -1, pCamera );
			availableList.DeleteString( k );
		}// end if

	}// end for

	SVSVIMStateClass::AddState( SV_STATE_MODIFIED );
	
	CPropertyPage::OnOK();

}// end OnOk


//*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/
//* Class Name : SVPPQEntryDialogPropertySheetClass
//* Note(s)    : property sheet
//*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/

IMPLEMENT_DYNAMIC(SVPPQEntryDialogPropertySheetClass, CPropertySheet)

SVPPQEntryDialogPropertySheetClass::SVPPQEntryDialogPropertySheetClass(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(nIDCaption, pParentWnd, iSelectPage)
{
	m_lCurrentPosition	= -1;
	m_bIsTaken			= FALSE;

	addPages();
}

SVPPQEntryDialogPropertySheetClass::SVPPQEntryDialogPropertySheetClass(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(pszCaption, pParentWnd, iSelectPage)
{
	m_lCurrentPosition	= -1;
	m_bIsTaken			= FALSE;

	addPages();
}

SVPPQEntryDialogPropertySheetClass::~SVPPQEntryDialogPropertySheetClass()
{
}

void SVPPQEntryDialogPropertySheetClass::addPages()
{
	m_oCameraPage.m_pSheet = this;
	AddPage( &m_oCameraPage );
	m_oDigitalPage.m_pSheet = this;
	AddPage( &m_oDigitalPage );
	m_oRemotePage.m_pSheet = this;
	AddPage( &m_oRemotePage );
}

BEGIN_MESSAGE_MAP(SVPPQEntryDialogPropertySheetClass, CPropertySheet)
	//{{AFX_MSG_MAP(SVPPQEntryDialogPropertySheetClass)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// SVPPQEntryDialogPropertySheetClass message handlers

INT_PTR SVPPQEntryDialogPropertySheetClass::DoModal() 
{
	INT_PTR nRetVal = CPropertySheet::DoModal();

	// Check IsTaken state of pages...
	m_bIsTaken = m_oCameraPage.m_bIsTaken;

	return nRetVal;
}// end DoModal

////////////////////////////////////////////////////////////////////////////////
// This function will check that the position is not the last in next trigger mode.
// Adding inputs to the last position will result in a NAK in next trigger mode.
BOOL SVPPQEntryDialogPropertySheetClass::OkToAdd()
{
	BOOL bOK = TRUE;
	SVPPQOutputModeEnum eMode = SVPPQUnknownMode;
	m_pPPQ->GetPPQOutputMode( eMode );
	if( eMode == SVPPQNextTriggerMode )
	{
		long iLen;
		m_pPPQ->GetPPQLength(iLen);
		if( m_lCurrentPosition+1 >= iLen)
		{
			bOK = FALSE;
		}
	}
	return bOK;
}

BOOL SVPPQEntryDialogCameraPageClass::OnApply() 
{
	// Set is taken flag...
	m_bIsTaken = ( selectedList.GetCount() > 0 );
	
	return CPropertyPage::OnApply();
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVPPQEntryDialog.cpp_v  $
 * 
 *    Rev 1.4   15 May 2014 12:32:56   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changed SetItemData to cast values to DWORD_PTR.
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   28 Feb 2014 08:31:08   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  888
 * SCR Title:  Prevent adding inputs to the last position of the PPQ in Next Trigger (e116)
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added function OkToAdd to enable/disable add button.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   02 Oct 2013 07:01:54   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Add x64 platform.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   06 May 2013 20:11:54   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 010.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
      Rev 1.1.1.21   16 Apr 2013 15:35:46   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  822
   SCR Title:  Remove DDE Input and Output functionality from SVObserver
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Removed DDE functionality from the source code.
   
   /////////////////////////////////////////////////////////////////////////////////////
 *    Rev 1.0   23 Apr 2013 13:31:10   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.1.1.20   09 Jul 2012 14:11:56   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  769
   SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated source code to change the include for SVIM State.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.1.1.19   19 Jun 2012 13:49:38   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  744
   SCR Title:  Add Shared Memory and Socket Functionality for Run Page Web Server
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Optimized the application state functionality.  The application state class was altered to simplify its usage and elements were added to object manager to separate tracking states.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.1.1.18   16 Sep 2011 16:18:06   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  730
   SCR Title:  Adjust SVObserver to fix issues with Inspection resource handshaking
   Checked in by:  tBair;  Tom Bair
   Change Description:  
     Updated objects with new device and resource management functionality.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.1.1.17   19 Apr 2011 16:22:10   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  707
   SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated code to fix issues with IO Assignment.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.1.1.16   18 Apr 2011 10:59:40   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  707
   SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated code to move the shared data for the IO Entry data to a different class to fix issues with PPQ and Inspection shared data.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.1.1.15   14 Feb 2011 14:26:08   ryoho
   Project:  SVObserver
   Change Request (SCR) nbr:  707
   SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     fixed Add/Remove logic to not overwrite the index
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.1.1.14   15 Dec 2010 10:36:00   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  707
   SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated source code to fix connections and synchronization issues with the IO Sub-system.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.1.1.13   13 Dec 2010 11:28:48   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  707
   SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Change the usage of _variant_t to clear the variable after use to eliminate invalid variant type for assignment.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.1.1.12   08 Dec 2010 13:26:00   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  707
   SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated source code to include changes in notification functionality using the Observer Design Pattern.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.1.1.11   21 Jun 2007 14:41:06   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  598
   SCR Title:  Upgrade SVObserver to compile using vc++ in VS2005
   Checked in by:  jSpila;  Joseph Spila
   Change Description:  
     These changes include modification based on fixing compiler-based and project-based differences between VC6 and VC8.  These changes mainly include casting issues, but some include type conversion and assignment of new compiler controlling defines.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.1.1.10   22 Aug 2005 10:27:02   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  504
   SCR Title:  Fix lock-up problem when pending product count is larger than PPQ size
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated methods to include changes to SVProductInfoStruct and supporting classes to handle proper assignment and copy construction.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.1.1.9   25 Apr 2003 11:25:52   rschock
   Project:  SVObserver
   Change Request (SCR) nbr:  341
   SCR Title:  DDE Input and Outputs are no longer working correctly
   Checked in by:  rSchock;  Rosco Schock
   Change Description:  
     Fixed several problems so that the DDE inputs and outputs will work like they used to in pre SVO 4.00 versions.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.1.1.8   22 Apr 2003 14:07:52   rschock
   Project:  SVObserver
   Change Request (SCR) nbr:  346
   SCR Title:  Update SVObserver to Version 4.21 Release
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.1.1.7   13 Jan 2003 13:00:10   rschock
   Project:  SVObserver
   Change Request (SCR) nbr:  307
   SCR Title:  Implement the ability to remotely change Tool parameters on the SVIM
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
     Added new code to fully enable Remote Inputs in the system.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.1.1.6   20 Nov 2002 10:46:58   ryoho
   Project:  SVObserver
   Change Request (SCR) nbr:  272
   SCR Title:  Outputs do not always get set correctly.
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
     Removed COM support
   Many changes related to the new architecture
   
   
   /////////////////////////////////////////////////////////////////////////////////////
*/
