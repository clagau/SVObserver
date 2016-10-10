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
#include "SVObjectLibrary\SVObjectManagerClass.h"
#include "SVInspectionProcess.h"
#include "SVToolSet.h"
#include "ObjectInterfaces\ErrorNumbers.h"
#include "SVStatusLibrary\MessageManager.h"
#include "TextDefinesSvO.h"
#pragma endregion Includes

IMPLEMENT_DYNAMIC(SVRemoteOutputEditDialog, CDialog)

SVRemoteOutputEditDialog::SVRemoteOutputEditDialog(CWnd* pParent /*=nullptr*/)
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
		SvStl::MessageMgrStd e( SvStl::LogOnly );
		e.setMessage( SVMSG_SVO_55_DEBUG_BREAK_ERROR, SvOi::Tid_ErrorGettingPPQCount, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_17048_ErrorGettingPPQCount );
		DebugBreak();
	}

	SVRemoteOutputGroup* l_pRemoteGroup = pConfig->GetRemoteOutputGroup( m_strGroupName );
	long lPPQSize = pConfig->GetPPQCount();
	for( int k = 0; k < lPPQSize; k++ )
	{
		pPPQ = pConfig->GetPPQ( k );
		if( nullptr == pPPQ )
		{
			SvStl::MessageMgrStd e( SvStl::LogOnly );
			e.setMessage( SVMSG_SVO_55_DEBUG_BREAK_ERROR, SvOi::Tid_ErrorGettingPPQ, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_17049_ErrorGettingPPQ );
			DebugBreak();
		}

		SVString l_strPPQName( pPPQ->GetName() );
		if( l_strPPQName == l_pRemoteGroup->GetPPQName() )
		{
			long lSize = 0;
			// Get list of available outputs
			if( !pPPQ->GetAllOutputs( ppIOEntries ) )
			{
				SvStl::MessageMgrStd e( SvStl::LogOnly );
				e.setMessage( SVMSG_SVO_55_DEBUG_BREAK_ERROR, SvOi::Tid_ErrorGettingOutputs, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_17050_ErrorGettingOutputs );
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
					if( ( nullptr != pIOEntry->m_pValueObject ) 
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

		if( !( l_pIOEntry.empty() ) && ( nullptr != l_pIOEntry->m_pValueObject ) )
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

