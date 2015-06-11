//******************************************************************************
//* COPYRIGHT (c) 2008 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVRemoteOutputEditDialog
//* .File Name       : $Workfile:   SVRemoteOutputEditDialog.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.7  $
//* .Check In Date   : $Date:   17 Jul 2014 20:21:28  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVRemoteOutputEditDialog.h"
#include "SVObserver.h"
#include "SVConfigurationObject.h"
#include "SVRemoteOutputDataController.h"
#include "SVObjectLibrary/SVObjectManagerClass.h"
#include "SVInspectionProcess.h"
#include "SVToolSet.h"
#include "ObjectInterfaces\ErrorNumbers.h"
#include "SVStatusLibrary/ExceptionManager.h"
#include "TextDefinesSvO.h"
#pragma endregion Includes

IMPLEMENT_DYNAMIC(SVRemoteOutputEditDialog, CDialog)

SVRemoteOutputEditDialog::SVRemoteOutputEditDialog(CWnd* pParent /*=NULL*/)
: CDialog(SVRemoteOutputEditDialog::IDD, pParent)
, m_strValueObjectSourceName(_T("Invalid"))
, m_Items( boost::bind( &( CComboBox::GetItemData ), &m_ValueObjectNameCombo, _1 ) , boost::bind( &( CComboBox::SetItemData ), &m_ValueObjectNameCombo, _1, _2 ) )
{
	m_TriggerCount = new SVIOEntryHostStruct;
}

SVRemoteOutputEditDialog::~SVRemoteOutputEditDialog()
{
}

void SVRemoteOutputEditDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_VALUE_OBJECT_NAME_COMBO, m_ValueObjectNameCombo);
}


BEGIN_MESSAGE_MAP(SVRemoteOutputEditDialog, CDialog)
	ON_CBN_SELCHANGE(IDC_VALUE_OBJECT_NAME_COMBO, &SVRemoteOutputEditDialog::OnCbnSelchangeValueObjectNameCombo)

END_MESSAGE_MAP()


// SVRemoteOutputEditDialog message handlers

BOOL SVRemoteOutputEditDialog::OnInitDialog()
{
	CDialog::OnInitDialog();
	SVPPQObject* pPPQ( nullptr );
	SVIOEntryHostStructPtrList ppIOEntries;
	
	int iCurrentSel = 0;

	SVObjectClass* l_pObject = SVObjectManagerClass::Instance().GetObjectA( m_InputObjectGUID );
	if( l_pObject )
	{
		m_strValueObjectSourceName = l_pObject->GetCompleteObjectName();
	}

	SVConfigurationObject* pConfig( nullptr );
	SVObjectManagerClass::Instance().GetConfigurationObject( pConfig );

	if( nullptr == pConfig )
	{
		SvStl::ExceptionMgr1 e; // The default constructor sets the type to LogOnly.
		e.setMessage( SVMSG_SVO_55_DEBUG_BREAK_ERROR, SvO::ErrorGettingPPQCount, StdExceptionParams, SvOi::Err_17048_ErrorGettingPPQCount );
		DebugBreak();
	}

	SVRemoteOutputGroup* l_pRemoteGroup = pConfig->GetRemoteOutputGroup( m_strGroupName );
	long lPPQSize = pConfig->GetPPQCount();
	for( int k = 0; k < lPPQSize; k++ )
	{
		CString l_strPPQName;
		pPPQ = pConfig->GetPPQ( k );
		if( nullptr == pPPQ )
		{
			SvStl::ExceptionMgr1 e; // The default constructor sets the type to LogOnly.
			e.setMessage( SVMSG_SVO_55_DEBUG_BREAK_ERROR, SvO::ErrorGettingPPQ, StdExceptionParams, SvOi::Err_17049_ErrorGettingPPQ );
			DebugBreak();
		}

		l_strPPQName = pPPQ->GetName();
		if( l_strPPQName == l_pRemoteGroup->GetPPQName() )
		{
			long lSize = 0;
			// Get list of available outputs
			if( !pPPQ->GetAllOutputs( ppIOEntries ) )
			{
				SvStl::ExceptionMgr1 e; // The default constructor sets the type to LogOnly.
				e.setMessage( SVMSG_SVO_55_DEBUG_BREAK_ERROR, SvO::ErrorGettingOutputs, StdExceptionParams, SvOi::Err_17050_ErrorGettingOutputs );
				DebugBreak();
			}

			lSize = static_cast<long>(ppIOEntries.size());

			// Put the Trigger Count in the list.
			SVValueObjectClass* l_pCurrentObject = &pPPQ->m_voTriggerCount;
			CString l_strName = l_pCurrentObject->GetCompleteObjectName();
			int iIndex = m_ValueObjectNameCombo.AddString( l_strName );
			m_TriggerCount->m_pValueObject = l_pCurrentObject;
			m_TriggerCount->m_DeleteValueObject = false;
			m_Items.SetItemData( iIndex, m_TriggerCount );

			l_pCurrentObject = dynamic_cast<SVValueObjectClass*>(SVObjectManagerClass::Instance().GetObject( m_InputObjectGUID));

			// Init IO combo from m_ppIOEntries;
			for( int i = 0; i < lSize; i++ )
			{
				SVIOEntryHostStructPtr pIOEntry = ppIOEntries[i];
				SVIOEntryHostStruct EntryStruct;
				
				int iIndex=0;

				if( !( pIOEntry.empty() ) )
				{
					if( ( pIOEntry->m_pValueObject != NULL ) 
						&& ( pIOEntry->m_pValueObject->GetCompleteObjectName().Find( _T( "PPQ" ) ) == -1 ) )
					{
						iIndex = m_ValueObjectNameCombo.AddString( pIOEntry->m_pValueObject->GetCompleteObjectName() );
						m_Items.SetItemData( iIndex, pIOEntry );
					}

					// Set current selection if object matches.
					if( l_pCurrentObject == pIOEntry->m_pValueObject )
					{
						iCurrentSel = iIndex;
					}
				}

			}// end for

			break;
		}
	}// end for

	// Search for the pObject and set the current selection to this.
	m_ValueObjectNameCombo.SetCurSel( iCurrentSel );


	UpdateData(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
}



void SVRemoteOutputEditDialog::OnOK()
{

	UpdateData();
	UpdateValueObjectFromCombo();
	CDialog::OnOK();
}
void SVRemoteOutputEditDialog::OnCancel()
{

	UpdateData();
	UpdateValueObjectFromCombo();
	CDialog::OnCancel();
}

void SVRemoteOutputEditDialog::OnCbnSelchangeValueObjectNameCombo()
{
	int l_iSel = m_ValueObjectNameCombo.GetCurSel();

	SVIOEntryHostStructPtr l_pIOEntry;
	SVDataItemManager::iterator l_Iter = m_Items.GetItemData( l_iSel );

	if( l_Iter != m_Items.end() )
	{
		l_pIOEntry = l_Iter->second;
	}

	if( !( l_pIOEntry.empty() ) )
	{
		SVValueObjectClass* l_pObject = l_pIOEntry->m_pValueObject;
		SVObjectTypeEnum l_PCDataType = l_pObject->GetObjectType();
		UpdateData(FALSE);
	}
}

void SVRemoteOutputEditDialog::UpdateValueObjectFromCombo()
{
	int l_iSel = m_ValueObjectNameCombo.GetCurSel();
	if( l_iSel >= 0 )
	{
		SVIOEntryHostStructPtr l_pIOEntry;
		SVDataItemManager::iterator l_Iter = m_Items.GetItemData( l_iSel );

		if( l_Iter != m_Items.end() )
		{
			l_pIOEntry = l_Iter->second;
		}

		if( !( l_pIOEntry.empty() ) && ( l_pIOEntry->m_pValueObject != NULL ) )
		{
			CString l_strName;
			m_ValueObjectNameCombo.GetLBText( l_iSel, m_strValueObjectSourceName );
			m_InputObjectGUID = l_pIOEntry->m_pValueObject->GetUniqueObjectID();
		}
	}
}

BOOL SVRemoteOutputEditDialog::PreTranslateMessage(MSG* pMsg)
{
	if(pMsg->message==WM_KEYDOWN)
    {
		if(pMsg->wParam== VK_DOWN )
		{
			pMsg->wParam = VK_TAB;
		}
		if(pMsg->wParam== VK_UP )
		{
			pMsg->wParam = VK_SHIFT ;
			PostMessage( WM_KEYDOWN, VK_TAB );
			PostMessage( WM_KEYUP, VK_TAB );
		}
		if( GetFocus()->GetDlgCtrlID() != IDOK && pMsg->wParam == VK_RETURN )
		{
			pMsg->wParam = VK_TAB;
		}
		//SendInput( nInputs , INPUT*, cbsize )
	}
	return CDialog::PreTranslateMessage(pMsg);
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVRemoteOutputEditDialog.cpp_v  $
 * 
 *    Rev 1.7   17 Jul 2014 20:21:28   gramseier
 * Project:  SVObserver
 * Change Request (SCR) nbr:  909
 * SCR Title:  Object Selector replacing Result Picker and Output Selector SVO-72, 40, 130
 * Checked in by:  gRamseier;  Guido Ramseier
 * Change Description:  
 *   Removed namespaces and code review changes
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   02 Oct 2013 07:58:46   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Add x64 platform.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   07 Aug 2013 13:21:48   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  809
 * SCR Title:  Non I/O SVIM
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Renamed SVIOEntryHostStructPtrVector to SVIOEntryHostStructPtrList.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   05 Jul 2013 15:01:30   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 021.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   11 Jun 2013 13:14:50   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  822
 * SCR Title:  Remove DDE Input and Output functionality from SVObserver
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Removed filtering Object Type from the OnInitDialog so digital output types will also be available to the remote outputs.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 *    Rev 1.3   13 May 2013 16:29:42   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 015.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   08 May 2013 09:35:20   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  826
 * SCR Title:  Fix Memory Leaks in I/O Adjustment Dialogs
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Used new GetItemData method that combined the gui get and the data get into one method.  This method returns an iterator to the data container for extraction and updates(if non-const) of data element.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   08 May 2013 16:31:28   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 012.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   24 Apr 2013 14:25:30   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  826
 * SCR Title:  Fix Memory Leaks in I/O Adjustment Dialogs
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added SVDataItemManagerTemplate to manage Item Data.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   06 May 2013 20:12:06   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 010.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   16 Apr 2013 15:35:48   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  822
 * SCR Title:  Remove DDE Input and Output functionality from SVObserver
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Removed DDE functionality from the source code.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 14:51:14   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   23 Aug 2012 10:27:02   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  779
 * SCR Title:  Fix Problems with DDE Inputs and DDE Outputs
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fix issue with available outputs for dee and discretes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   18 Jul 2012 14:25:00   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Removed obsolete methods assiciated with overlay drawling and fixed problem with copying data to byte buffer have wrong format.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/