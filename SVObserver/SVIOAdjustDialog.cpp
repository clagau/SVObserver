//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVIOAdjustDialog
//* .File Name       : $Workfile:   SVIOAdjustDialog.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.6  $
//* .Check In Date   : $Date:   15 May 2014 12:56:58  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVIOAdjustDialog.h"
#include "SVConfigurationObject.h"
#include "SVPPQObject.h"
#include "SVIOLibrary/PlcOutputObject.h"
#include "SVIOLibrary/SVDigitalInputObject.h"
#include "SVIOLibrary/SVDigitalOutputObject.h"
#include "SVObjectLibrary/SVObjectManagerClass.h"
#include "SVStatusLibrary/ErrorNumbers.h"
#include "SVStatusLibrary/MessageManager.h"
#include "SVOLibrary/SVHardwareManifest.h"
#pragma endregion Includes

#pragma region Declarations
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

BEGIN_MESSAGE_MAP(SVIOAdjustDialog, CDialog)
	//{{AFX_MSG_MAP(SVIOAdjustDialog)
	ON_CBN_SELCHANGE(IDC_IONAME_COMBO, OnSelChangeIOCombo)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

SVIOAdjustDialog::SVIOAdjustDialog(CWnd* pParent /*=nullptr*/) : CDialog(SVIOAdjustDialog::IDD, pParent)
{
}

void SVIOAdjustDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(SVIOAdjustDialog)
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

void SVIOAdjustDialog::OnOK()
{
	CDialog::OnOK();

	int curSel = IOCombo.GetCurSel();
	if( curSel == CB_ERR )
	{
		return;
	}

	CString selection;
	IOCombo.GetLBText(curSel, selection);

	const auto iter = m_Items.find(std::string(selection));
	if (m_Items.end() != iter)
	{
		m_pIOEntry = iter->second;

		if(nullptr != m_pDigInput)
		{
			m_pDigInput->Force( IsForced != FALSE, IsForcedTrue != FALSE );
			m_pDigInput->Invert( IsInverted != FALSE );
		}
		else if(nullptr != m_pDigOutput )
		{
			m_pDigOutput->Force( IsForced != FALSE, IsForcedTrue != FALSE );
			m_pDigOutput->Invert( IsInverted != FALSE );
			m_pDigOutput->Combine( IsCombined != FALSE, IsCombinedACK != FALSE );
		}
		else if (nullptr != m_pPlcOutput)
		{
			m_pPlcOutput->Combine(IsCombined != FALSE, IsCombinedACK != FALSE);
		}
	}
	else
	{
		m_pIOEntry.reset();
	}
}

BOOL SVIOAdjustDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	SVIOObjectType outputType{SVIOObjectType::IO_INVALID_OBJECT};

	// Add an empty item...
	if(nullptr != m_pIOEntry)
	{
		IOCombo.SetItemData( IOCombo.AddString( _T( "" ) ), 0 );
	}

	if(nullptr != m_pDigInput)
	{
		std::string name{m_pDigInput->GetName()};
		IOCombo.AddString(name.c_str());
		m_Items[name] = m_pIOEntry;
		if(false == name.empty())
		{
			IOCombo.SelectString(0, name.c_str());
		}
		else
		{
			IOCombo.SetCurSel(0);
		}
		OnSelChangeIOCombo();
	}
	else if (nullptr != m_pDigOutput)
	{
		outputType = IO_DIGITAL_OUTPUT;
		std::string name {m_pDigOutput->GetName()};
		IOCombo.AddString(name.c_str());
		m_Items[name] = m_pIOEntry;
		if (false == name.empty())
		{
			IOCombo.SelectString(0, name.c_str());
		}
		else
		{
			IOCombo.SetCurSel(0);
		}
		OnSelChangeIOCombo();
	}
	else if (nullptr != m_pPlcOutput)
	{
		outputType = IO_PLC_OUTPUT;
		std::string name {m_pPlcOutput->GetName()};
		IOCombo.AddString(name.c_str());
		m_Items[name] = m_pIOEntry;
		if (false == name.empty())
		{
			IOCombo.SelectString(0, name.c_str());
		}
		else
		{
			IOCombo.SetCurSel(0);
		}
		OnSelChangeIOCombo();
	}
	else
	{
		showForcedGroup(SW_HIDE);
		showInvertGroup(SW_HIDE);
	}

	if(nullptr != m_pDigOutput || nullptr != m_pPlcOutput)
	{
		SVConfigurationObject* pConfig = nullptr;
		SVObjectManagerClass::Instance().GetConfigurationObject( pConfig );

		// Get the number of PPQs
		if( nullptr == pConfig )
		{
			SvStl::MessageManager e(SvStl::MsgType::Log );
			e.setMessage( SVMSG_SVO_55_DEBUG_BREAK_ERROR, SvStl::Tid_ErrorGettingPPQCount, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_17029_ErrorGettingPPQCount );
			DebugBreak();
		}

		// Check Module Ready first
		SVIOEntryHostStructPtr pIOModuleReady = pConfig->GetModuleReady();

		if (nullptr != pIOModuleReady && SvDef::InvalidObjectId == pIOModuleReady->m_IOId && SV_IS_KIND_OF(pIOModuleReady->getObject(), SvVol::SVBoolValueObjectClass) )
		{
			std::string name{pIOModuleReady->getObject()->GetCompleteName()};
			IOCombo.AddString(name.c_str());
			m_Items[name] = pIOModuleReady;
		}// end if

		if ( SVHardwareManifest::IsProductTypeRAID( pConfig->GetProductType() ) )
		{
			SVIOEntryHostStructPtr pIORaid = pConfig->GetRaidErrorBit();
			if(nullptr != pIORaid && SvDef::InvalidObjectId == pIORaid->m_IOId && SV_IS_KIND_OF(pIORaid->getObject(), SvVol::SVBoolValueObjectClass ) )
			{
				std::string name{pIORaid->getObject()->GetCompleteName()};
				IOCombo.AddString(name.c_str());
				m_Items[name] = pIORaid;
			}// end if
		}

		for(int i = 0; i <  pConfig->GetPPQCount(); ++i )
		{
			SVIOEntryHostStructPtrVector IOEntryVector;
			SVPPQObject* pPPQ = pConfig->GetPPQ(i);
			if( nullptr == pPPQ )
			{
				SvStl::MessageManager e(SvStl::MsgType::Log );
				e.setMessage( SVMSG_SVO_55_DEBUG_BREAK_ERROR, SvStl::Tid_ErrorGettingPPQ, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_17030_ErrorGettingPPQ );
				DebugBreak();
			}
			else
			{
				// Get list of available outputs
				pPPQ->GetAllOutputs(IOEntryVector);
			}


			// Init IO combo from m_ppIOEntries;
			for(const auto& pIOEntry : IOEntryVector)
			{
				///For PLC Output only insert items from the same PPQ
				if(nullptr != m_pPlcOutput && i != m_PpqIndex)
				{
					continue;
				}
				///Note entries with IO_INVALID_OBJECT have not yet been set and shall either become IO_DIGITAL_OUPUT or IO_PLC_OUTPUT
				if(SvDef::InvalidObjectId == pIOEntry->m_IOId && SV_IS_KIND_OF( pIOEntry->getObject(), SvVol::SVBoolValueObjectClass ) &&
					(pIOEntry->m_ObjectType == outputType))
				{
					std::string name {pIOEntry->getObject()->GetCompleteName()};
					IOCombo.AddString(name.c_str());
					m_Items[name] = pIOEntry;
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

void SVIOAdjustDialog::OnSelChangeIOCombo()
{
	int curSel = IOCombo.GetCurSel();
	if(curSel != CB_ERR)
	{
		CString selection;
		IOCombo.GetLBText(curSel, selection);
		SVIOEntryHostStructPtr pIOEntry;
		const auto iter = m_Items.find(std::string(selection));
		if (m_Items.end() != iter)
		{
			pIOEntry = iter->second;
		}

		if(nullptr != pIOEntry)
		{
			if(nullptr != m_pDigInput)
			{
				IsForced	  = m_pDigInput->IsForced();
				IsInverted	  = m_pDigInput->IsInverted();
				IsForcedFalse = !m_pDigInput->GetForcedValue();
				IsForcedTrue  = m_pDigInput->GetForcedValue();
			}
			else if(nullptr != m_pDigOutput)
			{
				IsForced	  = m_pDigOutput->IsForced();
				IsInverted	  = m_pDigOutput->IsInverted();
				IsForcedFalse = !m_pDigOutput->GetForcedValue();
				IsForcedTrue  = m_pDigOutput->GetForcedValue();

				IsCombined	  = m_pDigOutput->isCombined();
				IsCombinedACK = m_pDigOutput->isAndACK();
				IsCombinedNAK = !IsCombinedACK;
			}
			else if (nullptr != m_pPlcOutput)
			{
				IsCombined = m_pPlcOutput->isCombined();
				IsCombinedACK = m_pPlcOutput->isAndACK();
				IsCombinedNAK = !IsCombinedACK;
			}

			switch(pIOEntry->m_ObjectType)
			{
				case SVIOObjectType::IO_DIGITAL_INPUT:
				{
					showForcedGroup(SW_SHOW);
					showInvertGroup(SW_SHOW);
					showCombinedGroup(SW_HIDE);
					break;
				}
				case SVIOObjectType::IO_DIGITAL_OUTPUT:
				{
					showForcedGroup(SW_SHOW);
					showInvertGroup(SW_SHOW);
					showCombinedGroup(SV_IS_KIND_OF(pIOEntry->getObject()->GetParent(), SVPPQObject) ? SW_HIDE : SW_SHOW);
					break;
				}
				case SVIOObjectType::IO_PLC_OUTPUT:
				{
					showForcedGroup(SW_HIDE);
					showInvertGroup(SW_HIDE);
					showCombinedGroup(SV_IS_KIND_OF(pIOEntry->getObject()->GetParent(), SVPPQObject) ? SW_HIDE : SW_SHOW);
					break;
				}
				default:
					break;
			}
		}// end if
		else
		{
			showForcedGroup(SW_HIDE);
			showInvertGroup(SW_HIDE);
			showCombinedGroup(SW_HIDE);
		}
	}

	UpdateData( FALSE );

}// end OnSelChangeIOCombo

void SVIOAdjustDialog::showForcedGroup(int showState)
{
	 CWnd* pWnd = GetDlgItem(IDC_FORCE_GROUP);
	if (pWnd) { pWnd->ShowWindow(showState); }
	pWnd = GetDlgItem(IDC_FORCE_0_RADIO);
	if (pWnd) { pWnd->ShowWindow(showState); }
	pWnd = GetDlgItem(IDC_FORCE_1_RADIO);
	if (pWnd) { pWnd->ShowWindow(showState); }
	pWnd = GetDlgItem(IDC_FORCE_CHECK);
	if (pWnd) { pWnd->ShowWindow(showState); }
}

void SVIOAdjustDialog::showInvertGroup(int showState)
{
	CWnd* pWnd = GetDlgItem(IDC_INVERT_GROUP);
	if (pWnd) { pWnd->ShowWindow(showState); }
	pWnd = GetDlgItem(IDC_INVERT_CHECK);
	if (pWnd) { pWnd->ShowWindow(showState); }
}

void SVIOAdjustDialog::showCombinedGroup(int showState)
{
	CWnd* pWnd = GetDlgItem(IDC_COMBINE_GROUP);
	if (pWnd) { pWnd->ShowWindow(showState); }
	pWnd = GetDlgItem(IDC_COMBINE_ACK_RADIO);
	if (pWnd) { pWnd->ShowWindow(showState); }
	pWnd = GetDlgItem(IDC_COMBINE_NAK_RADIO);
	if (pWnd) { pWnd->ShowWindow(showState); }
	pWnd = GetDlgItem(IDC_COMBINE_CHECK);
	if (pWnd) { pWnd->ShowWindow(showState); }
}
