//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVIOAdjustDialogClass
//* .File Name       : $Workfile:   SVIOAdjustDialog.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.4  $
//* .Check In Date   : $Date:   07 Aug 2013 13:27:30  $
//******************************************************************************

#include "stdafx.h"
#include "SVObserver.h"

#include "SVIODoc.h"

#include "SVIOAdjustDialog.h"

#include "SVObjectLibrary/SVObjectManagerClass.h"

#include "SVSVIMStateClass.h"
#include "SVDigitalInputObject1.h"
#include "SVDigitalOutputObject1.h"
#include "SVPPQObject.h"
#include "SVConfigurationObject.h"
#include "SVIOController.h"
#include "SVHardwareManifest.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld SVIOAdjustDialogClass 


SVIOAdjustDialogClass::SVIOAdjustDialogClass(CWnd* pParent /*=NULL*/)
: CDialog(SVIOAdjustDialogClass::IDD, pParent)
, m_Items( boost::bind( &( CComboBox::GetItemData ), &IOCombo, _1 ) , boost::bind( &( CComboBox::SetItemData ), &IOCombo, _1, _2 ) ) 
{
	//{{AFX_DATA_INIT(SVIOAdjustDialogClass)
	StrIOName = _T("");
	StrIOValue = _T("");
	IsForced = FALSE;
	IsInverted = FALSE;
	IsForcedTrue = FALSE;
	IsForcedFalse = FALSE;
	IsCombined = FALSE;
	IsCombinedACK = FALSE;
	IsCombinedNAK = FALSE;
	//}}AFX_DATA_INIT
	m_pDigInput		= NULL;
	m_pDigOutput	= NULL;
	m_pIODoc		= NULL;
}

SVIOAdjustDialogClass::~SVIOAdjustDialogClass()
{
}

void SVIOAdjustDialogClass::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(SVIOAdjustDialogClass)
	DDX_Control(pDX, IDC_IONAME_COMBO, IOCombo);
	DDX_Text(pDX, IDC_VALUE_STATIC, StrIOValue);
	DDX_Check(pDX, IDC_FORCE_CHECK, IsForced);
	DDX_Check(pDX, IDC_INVERT_CHECK, IsInverted);
	DDX_Check(pDX, IDC_FORCE_1_RADIO, IsForcedTrue );
	DDX_Check(pDX, IDC_FORCE_0_RADIO, IsForcedFalse );
	DDX_Check(pDX, IDC_COMBINE_CHECK, IsCombined);
	DDX_Check(pDX, IDC_COMBINE_ACK_RADIO, IsCombinedACK);
	DDX_Check(pDX, IDC_COMBINE_NAK_RADIO, IsCombinedNAK);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(SVIOAdjustDialogClass, CDialog)
	//{{AFX_MSG_MAP(SVIOAdjustDialogClass)
	ON_CBN_SELCHANGE(IDC_IONAME_COMBO, OnSelChangeIOCombo)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten SVIOAdjustDialogClass 

void SVIOAdjustDialogClass::OnOK() 
{
	DWORD dwData;

	CDialog::OnOK();

	dwData = IOCombo.GetCurSel();
	if( dwData == CB_ERR )
		return;

	SVDataItemManager::iterator l_Iter = m_Items.GetItemData( dwData );

	if( l_Iter != m_Items.end() )
	{
		m_pIOEntry = l_Iter->second;

		if( m_bInputMode )
		{
			m_pDigInput->Force( IsForced != FALSE, IsForcedTrue != FALSE );
			m_pDigInput->Invert( IsInverted != FALSE );
		}
		else if( m_pDigOutput )
		{
			m_pDigOutput->Force( IsForced != FALSE, IsForcedTrue != FALSE );
			m_pDigOutput->Invert( IsInverted != FALSE );
			m_pDigOutput->Combine( IsCombined != FALSE, IsCombinedACK != FALSE );
		}
	}
	else
	{
		m_pIOEntry.clear();
	}
}

BOOL SVIOAdjustDialogClass::OnInitDialog() 
{
	CDialog::OnInitDialog();

	SVPPQObject* pPPQ;
	SVIOEntryHostStructPtrList ppIOEntries;
	SVIOEntryHostStructPtr pIOEntry;
	long lPPQSize;
	long lSize;
	int nIndex = 0;
	int i;
	int k;

	// Add an empty item...
	if( !( m_pIOEntry.empty() ) )
		IOCombo.SetItemData( IOCombo.AddString( _T( "" ) ), (DWORD) NULL );

	if( m_bInputMode )
	{
		nIndex = IOCombo.AddString( m_pDigInput->GetName() );
		m_Items.SetItemData( nIndex, m_pIOEntry );
		IOCombo.SetCurSel( nIndex );
		OnSelChangeIOCombo();
	}
	else if( m_pDigOutput )
	{
		nIndex = IOCombo.AddString( m_pDigOutput->GetName() );
		m_Items.SetItemData( nIndex, m_pIOEntry );
		IOCombo.SetCurSel( nIndex );
		OnSelChangeIOCombo();
	}
	else
	{
		// disable Force unit...
		CWnd* pWnd = GetDlgItem( IDC_FORCE_0_RADIO );
		if( pWnd )
			pWnd->EnableWindow( FALSE );
		pWnd = GetDlgItem( IDC_FORCE_1_RADIO );
		if( pWnd )
			pWnd->EnableWindow( FALSE );
		pWnd = GetDlgItem( IDC_FORCE_CHECK );
		if( pWnd )
			pWnd->EnableWindow( FALSE );
		
		// disable Invert unit...
		pWnd = GetDlgItem( IDC_INVERT_CHECK );
		if( pWnd )
			pWnd->EnableWindow( FALSE );
	}// end else

	if( !m_bInputMode && !SVSVIMStateClass::CheckState( SV_STATE_RUNNING | SV_STATE_TEST ) )
	{
		SVConfigurationObject* pConfig = NULL;
		SVObjectManagerClass::Instance().GetConfigurationObject( pConfig );

		// Get the number of PPQs
		if( !pConfig->GetPPQCount( lPPQSize ) )
			DebugBreak();

		// Check Module Ready first
		if( m_pIODoc )
		{
			pIOEntry = pConfig->GetModuleReady();

			if( ( pIOEntry->m_IOId.empty() ) &&  SV_IS_KIND_OF( pIOEntry->m_pValueObject, SVBoolValueObjectClass ) )
			{
				nIndex = IOCombo.AddString( pIOEntry->m_pValueObject->GetCompleteObjectName() );
				m_Items.SetItemData( nIndex, pIOEntry );
			}// end if

		}// end if

		if ( m_pIODoc )
		{
			if ( SVHardwareManifest::IsProductTypeRAID( pConfig->GetProductType() ) )
			{
				pIOEntry = pConfig->GetRaidErrorBit();
				if( ( pIOEntry->m_IOId.empty() ) && SV_IS_KIND_OF( pIOEntry->m_pValueObject, SVBoolValueObjectClass ) )
				{
					nIndex = IOCombo.AddString( pIOEntry->m_pValueObject->GetCompleteObjectName() );
					m_Items.SetItemData( nIndex, pIOEntry );
				}// end if
			}
		}

		for( k = 0; k < lPPQSize; k++ )
		{
			// Get the number of PPQs
			if( !pConfig->GetPPQ( k, &pPPQ ) )
				DebugBreak();
			
			// Get list of available outputs
			if( !pPPQ->GetAllOutputs( ppIOEntries ) )
				DebugBreak();

			lSize = ppIOEntries.size();

			// Init IO combo from m_ppIOEntries;
			for( i = 0; i < lSize; i++ )
			{
				pIOEntry = ppIOEntries[i];
				if( ( pIOEntry->m_IOId.empty() ) && SV_IS_KIND_OF( pIOEntry->m_pValueObject, SVBoolValueObjectClass ) &&
					 pIOEntry->m_ObjectType == IO_DIGITAL_OUTPUT )
				{
					nIndex = IOCombo.AddString( pIOEntry->m_pValueObject->GetCompleteObjectName() );
					m_Items.SetItemData( nIndex, pIOEntry );
				}// end if

			}// end for

		}// end for
		
	}// end if
	else
	{
		// Current selection should not be changed...
		// ...deactivate combo...
		IOCombo.EnableWindow( FALSE );
	}

	UpdateData( FALSE );

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX-Eigenschaftenseiten sollten FALSE zurückgeben
}// end OnInitDialog

void SVIOAdjustDialogClass::OnSelChangeIOCombo() 
{
	DWORD_PTR dwData = IOCombo.GetCurSel();
	CWnd *pWnd;

	if( dwData != CB_ERR )
	{
		SVIOEntryHostStructPtr pIOEntry;
		SVDataItemManager::iterator l_Iter = m_Items.GetItemData( dwData );

		if( l_Iter != m_Items.end() )
		{
			pIOEntry = l_Iter->second;
		}

		if( !( pIOEntry.empty() ) )
		{
			if( m_bInputMode )
			{
				IsForced	  = m_pDigInput->IsForced();
				IsInverted	  = m_pDigInput->IsInverted();
				IsForcedFalse = !m_pDigInput->GetForcedValue();
				IsForcedTrue  = m_pDigInput->GetForcedValue();
			}// end if
			else
			{
				IsForced	  = m_pDigOutput->IsForced();
				IsInverted	  = m_pDigOutput->IsInverted();
				IsForcedFalse = !m_pDigOutput->GetForcedValue();
				IsForcedTrue  = m_pDigOutput->GetForcedValue();

				IsCombined	  = m_pDigOutput->IsCombined();
				IsCombinedACK = m_pDigOutput->GetCombinedValue();
				IsCombinedNAK = !IsCombinedACK;
			}// end else

			// enable Force unit...
			pWnd = GetDlgItem( IDC_FORCE_0_RADIO );
			if( pWnd )
				pWnd->EnableWindow( TRUE );
			pWnd = GetDlgItem( IDC_FORCE_1_RADIO );
			if( pWnd )
				pWnd->EnableWindow( TRUE );
			pWnd = GetDlgItem( IDC_FORCE_CHECK );
			if( pWnd )
				pWnd->EnableWindow( TRUE );

			// enable Invert unit...
			pWnd = GetDlgItem( IDC_INVERT_CHECK );
			if( pWnd )
				pWnd->EnableWindow( TRUE );

			// You can't combine inputs or non inspection results
			if( m_bInputMode || SV_IS_KIND_OF( pIOEntry->m_pValueParent, SVPPQObject ) )
			{
				// disable Combine unit...
				pWnd = GetDlgItem( IDC_COMBINE_ACK_RADIO );
				if( pWnd )
					pWnd->EnableWindow( FALSE );
				pWnd = GetDlgItem( IDC_COMBINE_NAK_RADIO );
				if( pWnd )
					pWnd->EnableWindow( FALSE );
				pWnd = GetDlgItem( IDC_COMBINE_CHECK );
				if( pWnd )
					pWnd->EnableWindow( FALSE );

				// Clear the current status of combine if it is leftover from a previous output type
				IsCombined = FALSE;
			}// end if
			else
			{
				// enable Combine unit...
				pWnd = GetDlgItem( IDC_COMBINE_ACK_RADIO );
				if( pWnd )
					pWnd->EnableWindow( TRUE );
				pWnd = GetDlgItem( IDC_COMBINE_NAK_RADIO );
				if( pWnd )
					pWnd->EnableWindow( TRUE );
				pWnd = GetDlgItem( IDC_COMBINE_CHECK );
				if( pWnd )
					pWnd->EnableWindow( TRUE );
			}// end else

		}// end if
		else
		{
			// disable Force unit...
			pWnd = GetDlgItem( IDC_FORCE_0_RADIO );
			if( pWnd )
				pWnd->EnableWindow( FALSE );
			pWnd = GetDlgItem( IDC_FORCE_1_RADIO );
			if( pWnd )
				pWnd->EnableWindow( FALSE );
			pWnd = GetDlgItem( IDC_FORCE_CHECK );
			if( pWnd )
				pWnd->EnableWindow( FALSE );

			// disable Invert unit...
			pWnd = GetDlgItem( IDC_INVERT_CHECK );
			if( pWnd )
				pWnd->EnableWindow( FALSE );

			// disable Combine unit...
			pWnd = GetDlgItem( IDC_COMBINE_ACK_RADIO );
			if( pWnd )
				pWnd->EnableWindow( FALSE );
			pWnd = GetDlgItem( IDC_COMBINE_NAK_RADIO );
			if( pWnd )
				pWnd->EnableWindow( FALSE );
			pWnd = GetDlgItem( IDC_COMBINE_CHECK );
			if( pWnd )
				pWnd->EnableWindow( FALSE );
		}// end else

	}// end if

	UpdateData( FALSE );

}// end OnSelChangeIOCombo

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVIOAdjustDialog.cpp_v  $
 * 
 *    Rev 1.4   07 Aug 2013 13:27:30   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  809
 * SCR Title:  Non I/O SVIM
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Renamed SVIOEntryHostStructPtrVector to SVIOEntryHostStructPtrList.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   15 May 2013 19:46:30   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 016.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
      Rev 3.25   09 May 2013 15:54:04   tbair
   Project:  SVObserver
   Change Request (SCR) nbr:  826
   SCR Title:  Fix Memory Leaks in I/O Adjustment Dialogs
   Checked in by:  tBair;  Tom Bair
   Change Description:  
     Change code to GetItem in OnOk and OnSelChange for the SVDataItemManagerTemplate.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
 *    Rev 1.2   13 May 2013 16:29:26   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 015.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
      Rev 3.24   08 May 2013 09:35:20   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  826
   SCR Title:  Fix Memory Leaks in I/O Adjustment Dialogs
   Checked in by:  tBair;  Tom Bair
   Change Description:  
     Used new GetItemData method that combined the gui get and the data get into one method.  This method returns an iterator to the data container for extraction and updates(if non-const) of data element.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
 *    Rev 1.1   08 May 2013 16:16:36   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 012.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
      Rev 3.23   24 Apr 2013 13:53:38   tbair
   Project:  SVObserver
   Change Request (SCR) nbr:  826
   SCR Title:  Fix Memory Leaks in I/O Adjustment Dialogs
   Checked in by:  tBair;  Tom Bair
   Change Description:  
     Add SVDataItemManagerTemplate to manage item data.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
 *    Rev 1.0   23 Apr 2013 11:06:20   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.22   16 Jan 2013 16:06:24   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  802
   SCR Title:  Add new product type GD1A
   Checked in by:  bWalter;  Ben Walter
   Change Description:  
     Migrating branch code into primary code base.  
   
   Used hardware manifest object to do configuration look-up.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.21   15 Aug 2012 16:58:02   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  779
   SCR Title:  Fix Problems with DDE Inputs and DDE Outputs
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated code to remove compiler warnings.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.20   15 Aug 2012 14:37:46   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  779
   SCR Title:  Fix Problems with DDE Inputs and DDE Outputs
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated source code to use the new Singleton version of the SVIOConfigurationInterfaceClass.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.19   09 Jul 2012 14:03:52   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  769
   SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated source code to change the include for SVIM State.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.18   19 Jun 2012 12:54:34   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  744
   SCR Title:  Add Shared Memory and Socket Functionality for Run Page Web Server
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Optimized the application state functionality.  The application state class was altered to simplify its usage and elements were added to object manager to separate tracking states.
   Centralized the location of the configuration object identifier and removed synchronization problems.
   
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.17   30 Jan 2012 11:12:40   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  748
   SCR Title:  Add Remote Output Steams to SVObserver
   Checked in by:  jSpila;  Joseph Spila
   Change Description:  
     Modified source code to consolidate PLC and New Output stream functionality to I/O Controller.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.16   20 Jan 2012 11:03:20   tbair
   Project:  SVObserver
   Change Request (SCR) nbr:  749
   SCR Title:  Add logic to IO page to prevent combine setting with PPQ variables.
   Checked in by:  tBair;  Tom Bair
   Change Description:  
     Modified OnSelChangeIOCombo to clear combine flag when object type is a PPQ object.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.15   06 Jun 2011 13:06:18   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  722
   SCR Title:  Fix a problem with Raid Error Information on X2 systems
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated source code to correct issues with the RAID error information associated with the X2 products.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.14   19 Apr 2011 16:17:06   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  707
   SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated code to fix issues with IO Assignment.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.13   18 Apr 2011 10:24:12   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  707
   SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated code to move the shared data for the IO Entry data to a different class to fix issues with PPQ and Inspection shared data.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.12   27 Jan 2011 14:44:26   ryoho
   Project:  SVObserver
   Change Request (SCR) nbr:  707
   SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     fixed issue with clearing an output
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.11   21 Jan 2011 14:43:38   ryoho
   Project:  SVObserver
   Change Request (SCR) nbr:  707
   SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     fixed issue with selecting an output and it was not being cleared from the list
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.10   15 Dec 2010 10:00:18   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  707
   SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated source code to fix connections and synchronization issues with the IO Sub-system.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.9   08 Dec 2010 12:51:58   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  707
   SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated source code to include changes in notification functionality using the Observer Design Pattern.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.8   09 Apr 2007 11:54:50   ryoho
   Project:  SVObserver
   Change Request (SCR) nbr:  588
   SCR Title:  Add the RAID Error Bit for the digital IO
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
     check to determine if the product has the raid bit
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.7   22 Jan 2007 10:35:44   ryoho
   Project:  SVObserver
   Change Request (SCR) nbr:  588
   SCR Title:  Add the RAID Error Bit for the digital IO
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
     Added the new RaidErrorBit 
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.6   25 Sep 2003 14:34:00   rschock
   Project:  SVObserver
   Change Request (SCR) nbr:  381
   SCR Title:  Combine PPQ Data to Inspection Data for a Digital Output
   Checked in by:  rSchock;  Rosco Schock
   Change Description:  
     Add code to support the combining of digital outputs with PPQ values.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.5   07 May 2003 15:09:42   rschock
   Project:  SVObserver
   Change Request (SCR) nbr:  341
   SCR Title:  DDE Input and Outputs are no longer working correctly
   Checked in by:  rSchock;  Rosco Schock
   Change Description:  
     Modified code so that some inputs that were being exposed as outputs were removed.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.4   22 Apr 2003 10:31:12   rschock
   Project:  SVObserver
   Change Request (SCR) nbr:  346
   SCR Title:  Update SVObserver to Version 4.21 Release
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
   
   /////////////////////////////////////////////////////////////////////////////////////
*/
