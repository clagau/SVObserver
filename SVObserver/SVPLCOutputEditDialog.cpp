//******************************************************************************
//* COPYRIGHT (c) 2008 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVPLCOutputEditDialog
//* .File Name       : $Workfile:   SVPLCOutputEditDialog.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.5  $
//* .Check In Date   : $Date:   17 Jul 2014 20:04:58  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#ifndef _WIN64
#include "SVObserver.h"
#include "SVConfigurationObject.h"
#include "SVPLCOutputEditDialog.h"
#include "SVObjectLibrary/SVObjectManagerClass.h"
#include "SVInspectionProcess.h"
#include "SVToolSet.h"
#include "SVPLCValidateReasons.h"
#include "ObjectInterfaces\ErrorNumbers.h"
#include "SVStatusLibrary/ExceptionManager.h"
#include "TextDefinesSvO.h"
#pragma endregion Includes

IMPLEMENT_DYNAMIC(SVPLCOutputEditDialog, CDialog)

SVPLCOutputEditDialog::SVPLCOutputEditDialog(CWnd* pParent /*=NULL*/)
: CDialog(SVPLCOutputEditDialog::IDD, pParent)
, m_strOutputSourceName(_T("Invalid"))
, m_lElement(0)
, m_lSize(1)
, m_bDisableElement(false)
, m_bDisableObject(false)
, m_lBitNum(0)
, m_bUseBitSize(FALSE)
, m_lOriginalIndex(-1)
, m_bNewObject(false)
, m_Items( boost::bind( &( CComboBox::GetItemData ), &m_ValueObjectNameCombo, _1 ) , boost::bind( &( CComboBox::SetItemData ), &m_ValueObjectNameCombo, _1, _2 ) )
{
	m_TriggerCount = new SVIOEntryHostStruct;
}

SVPLCOutputEditDialog::~SVPLCOutputEditDialog()
{
}

void SVPLCOutputEditDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_SIZE_EDIT, m_lSize);
	DDX_Control(pDX, IDC_VALUE_OBJECT_NAME_COMBO, m_ValueObjectNameCombo);
	DDX_Text(pDX, IDC_PLC_ELEMENT_EDIT, m_lElement);
	DDX_Text(pDX, IDC_BIT_EDIT, m_lBitNum);
	DDX_Check(pDX, IDC_USE_BIT_SIZE, m_bUseBitSize);
}


BEGIN_MESSAGE_MAP(SVPLCOutputEditDialog, CDialog)
	ON_CBN_SELCHANGE(IDC_VALUE_OBJECT_NAME_COMBO, &SVPLCOutputEditDialog::OnCbnSelchangeValueObjectNameCombo)
END_MESSAGE_MAP()


// SVPLCOutputEditDialog message handlers

BOOL SVPLCOutputEditDialog::OnInitDialog()
{
	CDialog::OnInitDialog();
	SVPPQObject* pPPQ( nullptr );
	SVIOEntryHostStructPtrList ppIOEntries;
	long lPPQSize=0;
	int iCurrentSel = 0;

	SVObjectClass* l_pObject = SVObjectManagerClass::Instance().GetObjectA( m_InputObjectGUID );
	if( l_pObject )
	{
		m_strOutputSourceName = l_pObject->GetCompleteObjectName();
	}

	if( m_bDisableElement )
	{
		GetDlgItem(IDC_PLC_ELEMENT_EDIT)->EnableWindow( FALSE );
	}

	if( m_bDisableObject )
	{
		GetDlgItem(IDC_VALUE_OBJECT_NAME_COMBO)->EnableWindow( FALSE );
	}

	SVConfigurationObject* pConfig( nullptr );
	SVObjectManagerClass::Instance().GetConfigurationObject( pConfig );

	if( nullptr == pConfig || !pConfig->GetPPQCount( lPPQSize ) )
	{
		SvStl::ExceptionMgr1 e; // The default constructor sets the type to LogOnly.
		e.setMessage( SVMSG_SVO_55_DEBUG_BREAK_ERROR, SvO::ErrorGettingPPQCount, StdExceptionParams, SvOi::Err_17034_ErrorGettingPPQCount );
		DebugBreak();
	}

	for( int k = 0; k < lPPQSize; k++ )
	{
		CString l_strPLCName;
		// Get the number of PPQs
		if( !pConfig->GetPPQ( k, &pPPQ ) )
		{
			SvStl::ExceptionMgr1 e; // The default constructor sets the type to LogOnly.
			e.setMessage( SVMSG_SVO_55_DEBUG_BREAK_ERROR, SvO::ErrorGettingPPQ, StdExceptionParams, SvOi::Err_17035_ErrorGettingPPQ );
			DebugBreak();
		}

		pPPQ->GetPLCName(l_strPLCName);
		if( l_strPLCName == m_strPLCId )
		{
			long lSize=0;
			// Get list of available outputs
			if( !pPPQ->GetAllOutputs( ppIOEntries ) )
			{
				SvStl::ExceptionMgr1 e; // The default constructor sets the type to LogOnly.
				e.setMessage( SVMSG_SVO_55_DEBUG_BREAK_ERROR, SvO::ErrorGettingOutputs, StdExceptionParams, SvOi::Err_17036_ErrorGettingOutputs );
				DebugBreak();
			}

			lSize = ppIOEntries.size();

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

				int iIndex=0;
				if( !( pIOEntry.empty() ) )
				{
					if( pIOEntry->m_pValueObject != NULL )
					{
						CString l_strObjectName = pIOEntry->m_pValueObject->GetCompleteObjectName();
						if( m_ValueObjectNameCombo.FindString(-1, l_strObjectName ) == CB_ERR )
						{
							iIndex = m_ValueObjectNameCombo.AddString( l_strObjectName );
							m_Items.SetItemData( iIndex, pIOEntry );
						}
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

	if( m_bNewObject )
	{
		OnCbnSelchangeValueObjectNameCombo();
	}

	UpdateData(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
}



void SVPLCOutputEditDialog::OnOK()
{
	UpdateData();
	UpdateValueObjectFromCombo();

	HRESULT l_hr = S_OK;
	if( !m_validateFunc.empty() )
	{
		long l_lBitNum = m_lBitNum;
		if( m_bUseBitSize == FALSE )
		{
			l_lBitNum = -1;
		}
		l_hr = m_validateFunc(m_strPLCId, m_InputObjectGUID, m_lElement, m_lSize, m_lOriginalIndex, l_lBitNum );
		if( l_hr == INPUT_ALREADY_USED )
		{
			AfxMessageBox( _T("SVObserver Object Already Used"));
		}
		if( l_hr == ALREADY_USED )
		{
			AfxMessageBox( _T("Element is Already Used"), MB_OK );
		}
		if( l_hr == OVERLAPPING )
		{
			AfxMessageBox( _T("Element + Size overlaps other Elements"), MB_OK );
		}
		if( l_hr == BITS_OVERLAP )
		{
			AfxMessageBox( _T("Bit Elements Overlap"), MB_OK );
		}
		if( l_hr == TOO_LARGE )
		{
			AfxMessageBox( _T("Size is larger than the Data Table size" ));
		}
	}
	if( l_hr == S_OK )
	{
		CDialog::OnOK();
	}
}

bool SVPLCOutputEditDialog::GetUseBitSize()
{
	return m_bUseBitSize ? true : false;
}

void SVPLCOutputEditDialog::SetUseBitSize( bool p_bUseBitSize )
{
	m_bUseBitSize = p_bUseBitSize ? TRUE : FALSE ;
}

void SVPLCOutputEditDialog::OnCbnSelchangeValueObjectNameCombo()
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
		// Since we have the type, 
		// ask the data controller to give us an element size.
		if( !m_CalcNewSizeCallBack.empty() )
		{
			SVValueObjectClass* l_pObject = l_pIOEntry->m_pValueObject;
			SVObjectTypeEnum l_PCDataType = l_pObject->GetObjectType();
			m_lSize = m_CalcNewSizeCallBack( m_strPLCId, l_PCDataType );
			UpdateData(FALSE);
		}
	}
}

void SVPLCOutputEditDialog::UpdateValueObjectFromCombo()
{
	int l_iSel = m_ValueObjectNameCombo.GetCurSel();

	SVIOEntryHostStructPtr l_pIOEntry;
	SVDataItemManager::iterator l_Iter = m_Items.GetItemData( l_iSel );

	if( l_Iter != m_Items.end() )
	{
		l_pIOEntry = l_Iter->second;
	}

	if( !( l_pIOEntry.empty() ) && ( l_pIOEntry->m_pValueObject != NULL ) )
	{
		CString l_strName;
		m_ValueObjectNameCombo.GetLBText(l_iSel, m_strOutputSourceName );
		m_InputObjectGUID = l_pIOEntry->m_pValueObject->GetUniqueObjectID();
	}
}

BOOL SVPLCOutputEditDialog::PreTranslateMessage(MSG* pMsg)
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

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVPLCOutputEditDialog.cpp_v  $
 * 
 *    Rev 1.5   17 Jul 2014 20:04:58   gramseier
 * Project:  SVObserver
 * Change Request (SCR) nbr:  909
 * SCR Title:  Object Selector replacing Result Picker and Output Selector SVO-72, 40, 130
 * Checked in by:  gRamseier;  Guido Ramseier
 * Change Description:  
 *   Removed namespaces and code review changes
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   30 Oct 2013 11:00:30   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added #ifndef _WIN64 to remove deprecated code from the 64bit solution.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   07 Aug 2013 13:23:10   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  809
 * SCR Title:  Non I/O SVIM
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Renamed SVIOEntryHostStructPtrVector to SVIOEntryHostStructPtrList.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   13 May 2013 16:29:34   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 015.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.14   08 May 2013 09:35:20   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  826
 * SCR Title:  Fix Memory Leaks in I/O Adjustment Dialogs
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Used new GetItemData method that combined the gui get and the data get into one method.  This method returns an iterator to the data container for extraction and updates(if non-const) of data element.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   08 May 2013 16:30:54   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 012.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.13   24 Apr 2013 14:19:10   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  826
 * SCR Title:  Fix Memory Leaks in I/O Adjustment Dialogs
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added SVDataItemManagerTemplate to manage Item Data.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 13:26:28   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.12   07 Sep 2012 11:44:34   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  783
 * SCR Title:  Update SVObserver to Validate PLC Sub-system Before Entering a Running Mode
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Fixed issue where duplicate published objects show up in the plc outputs picker dialog.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.11   02 Jul 2012 17:30:16   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to promote new display functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.10   20 Jun 2012 09:47:44   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  744
 * SCR Title:  Add Shared Memory and Socket Functionality for Run Page Web Server
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Centralized the location of the configuration object identifier and removed synchronization problems.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   19 Jun 2012 13:58:54   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  744
 * SCR Title:  Add Shared Memory and Socket Functionality for Run Page Web Server
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Centralized the location of the configuration object identifier and removed synchronization problems.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   19 Jun 2012 13:54:12   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  744
 * SCR Title:  Add Shared Memory and Socket Functionality for Run Page Web Server
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Centralized the location of the configuration object identifier and removed synchronization problems.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   30 Jan 2012 11:47:30   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  748
 * SCR Title:  Add Remote Output Steams to SVObserver
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   Modified source code to consolidate PLC and New Output stream functionality to I/O Controller.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   19 Apr 2011 16:22:08   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to fix issues with IO Assignment.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   18 Apr 2011 10:59:38   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to move the shared data for the IO Entry data to a different class to fix issues with PPQ and Inspection shared data.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   08 Dec 2010 13:25:58   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to include changes in notification functionality using the Observer Design Pattern.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   16 Dec 2009 12:25:16   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  677
 * SCR Title:  Fix problem in camera notify thread
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fix issues with includes and comments.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   19 Jan 2009 08:54:56   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  615
 * SCR Title:  Integrate PLC Classes into SVObserver Outputs
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changed behavior of return key in PLC Edit Dialogs
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   14 Jan 2009 16:12:04   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  615
 * SCR Title:  Integrate PLC Classes into SVObserver Outputs
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Modified PLC GUI to match Functional Requirement.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   14 Nov 2008 14:09:16   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  615
 * SCR Title:  Integrate PLC Classes into SVObserver Outputs
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Initial Check-in
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/