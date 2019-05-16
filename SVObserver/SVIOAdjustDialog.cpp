//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVIOAdjustDialogClass
//* .File Name       : $Workfile:   SVIOAdjustDialog.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.6  $
//* .Check In Date   : $Date:   15 May 2014 12:56:58  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"

#include "SVIOAdjustDialog.h"
#include "SVObjectLibrary\SVObjectManagerClass.h"
#include "SVStatusLibrary/SVSVIMStateClass.h"
#include "SVIOLibrary/SVDigitalInputObject.h"
#include "SVIOLibrary/SVDigitalOutputObject.h"
#include "SVPPQObject.h"
#include "SVConfigurationObject.h"
#include "TriggerInformation/SVHardwareManifest.h"
#include "SVStatusLibrary/ErrorNumbers.h"
#include "SVStatusLibrary\MessageManager.h"
#include "TextDefinesSvO.h"
#pragma endregion Includes

#pragma region Declarations
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

#pragma region Constructor
SVIOAdjustDialogClass::SVIOAdjustDialogClass(CWnd* pParent /*=nullptr*/)
: CDialog(SVIOAdjustDialogClass::IDD, pParent)
, m_Items( boost::bind( &( CComboBox::GetItemData ), &IOCombo, _1 ), boost::bind( &( CComboBox::SetItemData ), &IOCombo, _1, _2 ) )
{
	//{{AFX_DATA_INIT(SVIOAdjustDialogClass)
	IOName = _T("");
	IOValue = _T("");
	IsForced = FALSE;
	IsInverted = FALSE;
	IsForcedTrue = FALSE;
	IsForcedFalse = FALSE;
	IsCombined = FALSE;
	IsCombinedACK = FALSE;
	IsCombinedNAK = FALSE;
	//}}AFX_DATA_INIT
	m_pDigInput = nullptr;
	m_pDigOutput = nullptr;
	m_pIODoc = nullptr;
}

SVIOAdjustDialogClass::~SVIOAdjustDialogClass()
{
}
#pragma endregion Constructor

void SVIOAdjustDialogClass::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(SVIOAdjustDialogClass)
	DDX_Control(pDX, IDC_IONAME_COMBO, IOCombo);
	DDX_Text(pDX, IDC_VALUE_STATIC, IOValue);
	DDX_Check(pDX, IDC_FORCE_CHECK, IsForced);
	DDX_Check(pDX, IDC_INVERT_CHECK, IsInverted);
	DDX_Check(pDX, IDC_FORCE_1_RADIO, IsForcedTrue);
	DDX_Check(pDX, IDC_FORCE_0_RADIO, IsForcedFalse);
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
		m_pIOEntry.reset();
	}
}

BOOL SVIOAdjustDialogClass::OnInitDialog()
{
	CDialog::OnInitDialog();

	SVPPQObject* pPPQ( nullptr );
	SVIOEntryHostStructPtrVector ppIOEntries;
	SVIOEntryHostStructPtr pIOEntry;
	long lPPQSize;
	long lSize;
	int nIndex = 0;
	int i;
	int k;

	// Add an empty item...
	if(nullptr != m_pIOEntry)
	{
		IOCombo.SetItemData( IOCombo.AddString( _T( "" ) ), 0 );
	}

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
		if( pWnd ) { pWnd->EnableWindow( FALSE ); }
		pWnd = GetDlgItem( IDC_FORCE_1_RADIO );
		if( pWnd ) { pWnd->EnableWindow( FALSE ); }
		pWnd = GetDlgItem( IDC_FORCE_CHECK );
		if( pWnd ) { pWnd->EnableWindow( FALSE ); }

		// disable Invert unit...
		pWnd = GetDlgItem( IDC_INVERT_CHECK );
		if( pWnd ) { pWnd->EnableWindow( FALSE ); }
	}// end else

	if( !m_bInputMode && !SVSVIMStateClass::CheckState( SV_STATE_RUNNING | SV_STATE_TEST ) )
	{
		SVConfigurationObject* pConfig = nullptr;
		SVObjectManagerClass::Instance().GetConfigurationObject( pConfig );

		// Get the number of PPQs
		if( nullptr == pConfig )
		{
			SvStl::MessageMgrStd e(SvStl::MsgType::Log );
			e.setMessage( SVMSG_SVO_55_DEBUG_BREAK_ERROR, SvStl::Tid_ErrorGettingPPQCount, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_17029_ErrorGettingPPQCount );
			DebugBreak();
		}

		lPPQSize = pConfig->GetPPQCount( );

		// Check Module Ready first
		if( m_pIODoc )
		{
			pIOEntry = pConfig->GetModuleReady();

			if( ( pIOEntry->m_IOId.empty() ) &&  SV_IS_KIND_OF( pIOEntry->getObject(), SvVol::SVBoolValueObjectClass ) )
			{
				nIndex = IOCombo.AddString( pIOEntry->getObject()->GetCompleteName().c_str() );
				m_Items.SetItemData( nIndex, pIOEntry );
			}// end if
		}// end if

		if ( m_pIODoc )
		{
			if ( SvTi::SVHardwareManifest::IsProductTypeRAID( pConfig->GetProductType() ) )
			{
				pIOEntry = pConfig->GetRaidErrorBit();
				if( ( pIOEntry->m_IOId.empty() ) && SV_IS_KIND_OF( pIOEntry->getObject(), SvVol::SVBoolValueObjectClass ) )
				{
					nIndex = IOCombo.AddString( pIOEntry->getObject()->GetCompleteName().c_str() );
					m_Items.SetItemData( nIndex, pIOEntry );
				}// end if
			}
		}

		for( k = 0; k < lPPQSize; k++ )
		{
			pPPQ = pConfig->GetPPQ( k );
			ppIOEntries.clear();
			if( nullptr == pPPQ )
			{
				SvStl::MessageMgrStd e(SvStl::MsgType::Log );
				e.setMessage( SVMSG_SVO_55_DEBUG_BREAK_ERROR, SvStl::Tid_ErrorGettingPPQ, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_17030_ErrorGettingPPQ );
				DebugBreak();
			}
			else
			{
				// Get list of available outputs
				pPPQ->GetAllOutputs(ppIOEntries);
			}


			lSize = static_cast< long >( ppIOEntries.size() );

			// Init IO combo from m_ppIOEntries;
			for( i = 0; i < lSize; i++ )
			{
				pIOEntry = ppIOEntries[i];
				if( ( pIOEntry->m_IOId.empty() ) && SV_IS_KIND_OF( pIOEntry->getObject(), SvVol::SVBoolValueObjectClass ) &&
					pIOEntry->m_ObjectType == IO_DIGITAL_OUTPUT )
				{
					nIndex = IOCombo.AddString( pIOEntry->getObject()->GetCompleteName().c_str() );
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
	CWnd* pWnd;

	if( dwData != CB_ERR )
	{
		SVIOEntryHostStructPtr pIOEntry;
		SVDataItemManager::iterator l_Iter = m_Items.GetItemData( static_cast< int >( dwData ) );

		if( l_Iter != m_Items.end() )
		{
			pIOEntry = l_Iter->second;
		}

		if(nullptr != pIOEntry)
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
			if( pWnd ) { pWnd->EnableWindow( TRUE ); }
			pWnd = GetDlgItem( IDC_FORCE_1_RADIO );
			if( pWnd ) { pWnd->EnableWindow( TRUE ); }
			pWnd = GetDlgItem( IDC_FORCE_CHECK );
			if( pWnd ) { pWnd->EnableWindow( TRUE ); }

			// enable Invert unit...
			pWnd = GetDlgItem( IDC_INVERT_CHECK );
			if( pWnd ) { pWnd->EnableWindow( TRUE ); }

			// You can't combine inputs or non inspection results
			if( m_bInputMode || SV_IS_KIND_OF( pIOEntry->getObject()->GetParent(), SVPPQObject ) )
			{
				// disable Combine unit...
				pWnd = GetDlgItem( IDC_COMBINE_ACK_RADIO );
				if( pWnd ) { pWnd->EnableWindow( FALSE ); }
				pWnd = GetDlgItem( IDC_COMBINE_NAK_RADIO );
				if( pWnd ) { pWnd->EnableWindow( FALSE ); }
				pWnd = GetDlgItem( IDC_COMBINE_CHECK );
				if( pWnd ) { pWnd->EnableWindow( FALSE ); }

				// Clear the current status of combine if it is leftover from a previous output type
				IsCombined = FALSE;
			}// end if
			else
			{
				// enable Combine unit...
				pWnd = GetDlgItem( IDC_COMBINE_ACK_RADIO );
				if( pWnd ) { pWnd->EnableWindow( TRUE ); }
				pWnd = GetDlgItem( IDC_COMBINE_NAK_RADIO );
				if( pWnd ) { pWnd->EnableWindow( TRUE ); }
				pWnd = GetDlgItem( IDC_COMBINE_CHECK );
				if( pWnd ) { pWnd->EnableWindow( TRUE ); }
			}// end else
		}// end if
		else
		{
			// disable Force unit...
			pWnd = GetDlgItem( IDC_FORCE_0_RADIO );
			if( pWnd ) { pWnd->EnableWindow( FALSE ); }
			pWnd = GetDlgItem( IDC_FORCE_1_RADIO );
			if( pWnd ) { pWnd->EnableWindow( FALSE ); }
			pWnd = GetDlgItem( IDC_FORCE_CHECK );
			if( pWnd ) { pWnd->EnableWindow( FALSE ); }

			// disable Invert unit...
			pWnd = GetDlgItem( IDC_INVERT_CHECK );
			if( pWnd ) { pWnd->EnableWindow( FALSE ); }

			// disable Combine unit...
			pWnd = GetDlgItem( IDC_COMBINE_ACK_RADIO );
			if( pWnd ) { pWnd->EnableWindow( FALSE ); }
			pWnd = GetDlgItem( IDC_COMBINE_NAK_RADIO );
			if( pWnd ) { pWnd->EnableWindow( FALSE ); }
			pWnd = GetDlgItem( IDC_COMBINE_CHECK );
			if( pWnd ) { pWnd->EnableWindow( FALSE ); }
		}// end else
	}// end if

	UpdateData( FALSE );

}// end OnSelChangeIOCombo

