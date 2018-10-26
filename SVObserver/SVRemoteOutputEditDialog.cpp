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
#include "SVStatusLibrary/ErrorNumbers.h"
#include "SVStatusLibrary\MessageManager.h"
#include "TextDefinesSvO.h"
#pragma endregion Includes

IMPLEMENT_DYNAMIC(SVRemoteOutputEditDialog, CDialog)

SVRemoteOutputEditDialog::SVRemoteOutputEditDialog(CWnd* pParent /*=nullptr*/)
: CDialog(SVRemoteOutputEditDialog::IDD, pParent)
, m_ValueObjectSourceName(_T("Invalid"))
, m_Items( boost::bind( &( CComboBox::GetItemData ), &m_ValueObjectNameCombo, _1 ) , boost::bind( &( CComboBox::SetItemData ), &m_ValueObjectNameCombo, _1, _2 ) )
{
	m_TriggerCount = SVIOEntryHostStructPtr{ new SVIOEntryHostStruct };
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
	SVIOEntryHostStructPtrVector ppIOEntries;
	
	int iCurrentSel = 0;

	SVObjectClass* l_pObject = SVObjectManagerClass::Instance().GetObjectA( m_InputObjectGUID );
	if( l_pObject )
	{
		m_ValueObjectSourceName = l_pObject->GetCompleteName().c_str();
	}

	SVConfigurationObject* pConfig( nullptr );
	SVObjectManagerClass::Instance().GetConfigurationObject( pConfig );

	if( nullptr == pConfig )
	{
		SvStl::MessageMgrStd e(SvStl::MsgType::Log );
		e.setMessage( SVMSG_SVO_55_DEBUG_BREAK_ERROR, SvStl::Tid_ErrorGettingPPQCount, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_17048_ErrorGettingPPQCount );
		DebugBreak();
	}

	SVRemoteOutputGroup* l_pRemoteGroup = pConfig->GetRemoteOutputGroup( m_GroupName );
	long lPPQSize = pConfig->GetPPQCount();
	for( int k = 0; k < lPPQSize; k++ )
	{
		pPPQ = pConfig->GetPPQ( k );
		if( nullptr == pPPQ )
		{
			SvStl::MessageMgrStd e(SvStl::MsgType::Log );
			e.setMessage( SVMSG_SVO_55_DEBUG_BREAK_ERROR, SvStl::Tid_ErrorGettingPPQ, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_17049_ErrorGettingPPQ );
			DebugBreak();
		}

		std::string l_strPPQName( pPPQ->GetName() );
		if( l_strPPQName == l_pRemoteGroup->GetPPQName() )
		{
			long lSize = 0;
			// Get list of available outputs
			pPPQ->GetAllOutputs(ppIOEntries);

			lSize = static_cast<long>(ppIOEntries.size());

			// Put the Trigger Count in the list.
			SVObjectClass* pCurrentObject = &pPPQ->m_voTriggerCount;
			int iIndex = m_ValueObjectNameCombo.AddString( pCurrentObject->GetCompleteName().c_str() );
			m_TriggerCount->setObject(pCurrentObject);
			m_TriggerCount->m_DeleteValueObject = false;
			m_Items.SetItemData( iIndex, m_TriggerCount );

			pCurrentObject = SVObjectManagerClass::Instance().GetObject( m_InputObjectGUID);

			// Init IO combo from m_ppIOEntries;
			for( int i = 0; i < lSize; i++ )
			{
				SVIOEntryHostStructPtr pIOEntry = ppIOEntries[i];
				SVIOEntryHostStruct EntryStruct;
				
				int iIndex=0;

				if(nullptr != pIOEntry)
				{
					if( nullptr != pIOEntry->getObject() && std::string::npos == pIOEntry->getObject()->GetCompleteName().find( _T( "PPQ" ) ) )
					{
						iIndex = m_ValueObjectNameCombo.AddString( pIOEntry->getObject()->GetCompleteName().c_str() );
						m_Items.SetItemData( iIndex, pIOEntry );
					}

					// Set current selection if object matches.
					if( pCurrentObject == pIOEntry->getObject() )
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

	if(nullptr != l_pIOEntry)
	{
		SvDef::SVObjectTypeEnum l_PCDataType = l_pIOEntry->getObject()->GetObjectType();
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

		if(nullptr != l_pIOEntry && ( nullptr != l_pIOEntry->getObject() ) )
		{
			CString Name;
			m_ValueObjectNameCombo.GetLBText( l_iSel, Name  );
			m_ValueObjectSourceName = Name;
			m_InputObjectGUID = l_pIOEntry->getObject()->GetUniqueObjectID();
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

