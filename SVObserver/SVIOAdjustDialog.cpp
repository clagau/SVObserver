//******************************************************************************
//* COPYRIGHT (c) 2003 by Körber Pharma Inspection GmbH. All Rights Reserved
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
#include "InspectionCommands/CommandExternalHelper.h"
#include "ObjectSelectorLibrary/ObjectTreeGenerator.h"
#include "SVIOLibrary/PlcOutputObject.h"
#include "SVIOLibrary/SVDigitalInputObject.h"
#include "SVIOLibrary/SVDigitalOutputObject.h"
#include "SVObjectLibrary/SVObjectManagerClass.h"
#include "SVOLibrary/SVHardwareManifest.h"
#include "SVStatusLibrary/MessageManager.h"
#pragma endregion Includes

#pragma region Declarations
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

BEGIN_MESSAGE_MAP(SVIOAdjustDialog, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_SELECTOR, OnObjectSelector)
	ON_CBN_SELCHANGE(IDC_COMBO_INSPECTION, OnSelchangeInspection)
END_MESSAGE_MAP()

SVIOAdjustDialog::SVIOAdjustDialog(const std::vector<std::string>& rUsedOutputList, CWnd* pParent /*=nullptr*/) : CDialog(SVIOAdjustDialog::IDD, pParent)
, m_rUsedOuputList{rUsedOutputList}
{
}

void SVIOAdjustDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(SVIOAdjustDialog)
	DDX_Control(pDX, IDC_COMBO_INSPECTION, m_inspectionCtrl);
	DDX_Text(pDX, IDC_IONAME, m_IOName);
	DDX_Text(pDX, IDC_VALUE_STATIC, m_IOValue);
	DDX_Check(pDX, IDC_FORCE_CHECK, m_isForced);
	DDX_Check(pDX, IDC_INVERT_CHECK, m_isInverted);
	DDX_Check(pDX, IDC_FORCE_1_RADIO, m_isForcedTrue);
	DDX_Check(pDX, IDC_FORCE_0_RADIO, m_isForcedFalse);
	DDX_Check(pDX, IDC_COMBINE_CHECK, m_sCombined);
	DDX_Check(pDX, IDC_COMBINE_ACK_RADIO, m_isCombinedACK);
	DDX_Check(pDX, IDC_COMBINE_NAK_RADIO, m_isCombinedNAK);
	//}}AFX_DATA_MAP
}

void SVIOAdjustDialog::OnOK()
{
	CDialog::OnOK();

	if(nullptr != m_pDigInput)
	{
		m_pDigInput->Force( m_isForced != FALSE, m_isForcedTrue != FALSE );
		m_pDigInput->Invert( m_isInverted != FALSE );
	}
	else if(nullptr != m_pDigOutput )
	{
		m_pDigOutput->Force( m_isForced != FALSE, m_isForcedTrue != FALSE );
		m_pDigOutput->Invert( m_isInverted != FALSE );
		m_pDigOutput->Combine( m_sCombined != FALSE, m_isCombinedACK != FALSE );
	}
	else if (nullptr != m_pPlcOutput)
	{
		m_pPlcOutput->Combine(m_sCombined != FALSE, m_isCombinedACK != FALSE);
	}
}

BOOL SVIOAdjustDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	SVConfigurationObject* pConfig = nullptr;
	SVObjectManagerClass::Instance().GetConfigurationObject(pConfig);
	if (nullptr != pConfig)
	{
		for (long i = 0; i < pConfig->GetPPQCount(); ++i)
		{
			//For NEO display only the valid PPQ index
			if (-1 != m_PpqIndex && i != m_PpqIndex)
			{
				continue;
			}
			SVPPQObject* pPPQ = pConfig->GetPPQ(i);
			if (nullptr != pPPQ)
			{
				for (long j = 0; j < pPPQ->GetInspectionCount(); ++j)
				{
					SVInspectionProcess* pInspection(nullptr);
					pPPQ->GetInspection(j, pInspection);
					if (nullptr != pInspection)
					{
						std::string name {pPPQ->GetName()};
						name += ": ";
						name += pInspection->GetName();
						int index = m_inspectionCtrl.AddString(name.c_str());
						m_inspectionCtrl.SetItemData(index, pInspection->getObjectId());
					}
				}
			}
		}
	}
	m_inspectionCtrl.SetCurSel(0);
	OnSelchangeInspection();

	UpdateGroups();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX-Eigenschaftenseiten sollten FALSE zurückgeben
}// end OnInitDialog

void SVIOAdjustDialog::OnObjectSelector()
{
	int inspectionIndex {m_inspectionCtrl.GetCurSel()};

	if (CB_ERR == inspectionIndex)
	{
		return;
	}
	uint32_t inspectionID {static_cast<uint32_t> (m_inspectionCtrl.GetItemData(inspectionIndex))};
	SvPb::InspectionCmdRequest requestCmd;
	SvPb::InspectionCmdResponse responseCmd;
	std::vector<SvPb::SearchArea> searchArea;
	searchArea.reserve(3);
	if (IO_DIGITAL_OUTPUT == m_ioObjectType)
	{
		//This is required to be able to select Module Ready
		searchArea.push_back(SvPb::SearchArea::globalConstantItems);
	}
	searchArea.push_back(SvPb::SearchArea::ppqItems);
	searchArea.push_back(SvPb::SearchArea::toolsetItems);
	*requestCmd.mutable_getobjectselectoritemsrequest() = SvCmd::createObjectSelectorRequest(
		searchArea, inspectionID, SvPb::useable, SvDef::InvalidObjectId, false, SvPb::boolValueObjects, SvPb::GetObjectSelectorItemsRequest::kAttributesAllowed);

	SvCmd::InspectionCommands(inspectionID, requestCmd, &responseCmd);
	SvOsl::ObjectTreeGenerator::Instance().setSelectorType(SvOsl::ObjectTreeGenerator::SelectorTypeEnum::TypeSingleObject);
	if (responseCmd.has_getobjectselectoritemsresponse())
	{
		SvOsl::ObjectTreeGenerator::Instance().insertTreeObjects(responseCmd.getobjectselectoritemsresponse().tree());
	}

	if (nullptr != m_pLinkedObject)
	{
		SvOsl::ObjectTreeGenerator::Instance().setCheckItems({m_pLinkedObject->GetObjectNameToObjectType()});
	}
	CString inspectionSelection;
	m_inspectionCtrl.GetLBText(m_inspectionCtrl.GetCurSel(), inspectionSelection);
	std::string ToolsetOutput = SvUl::LoadStdString(IDS_SELECT_TOOLSET_OUTPUT);
	std::string Filter = SvUl::LoadStdString(IDS_FILTER);

	INT_PTR Result = SvOsl::ObjectTreeGenerator::Instance().showDialog(inspectionSelection.GetString(), ToolsetOutput.c_str(), Filter.c_str(), this);

	if (IDOK == Result)
	{
		SVObjectClass* pPrevLinkedObject {m_pLinkedObject};
		std::string prevName{m_IOName.GetString()};
		SVObjectReference ObjectRef {SvOsl::ObjectTreeGenerator::Instance().getSingleObjectResult()};
		m_pLinkedObject = ObjectRef.getObject();
		m_IOName = ObjectRef.GetCompleteName(true).c_str();

		if (m_rUsedOuputList.end() != std::find(m_rUsedOuputList.begin(), m_rUsedOuputList.end(), std::string(m_IOName.GetString())))
		{
			SvStl::MessageManager e(SvStl::MsgType::Display);
			e.setMessage(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_OutputAlreadyUsed, {m_IOName.GetString()}, SvStl::SourceFileParams(StdMessageParams));
			m_pLinkedObject = pPrevLinkedObject;
			m_IOName = prevName.c_str();
		}
		if (IO_PLC_OUTPUT == m_ioObjectType)
		{
			SVInspectionProcess* pInspection = dynamic_cast<SVInspectionProcess*> (SVObjectManagerClass::Instance().GetObject(inspectionID));
			if (nullptr != pInspection)
			{
				DWORD objectIdIndex {pInspection->GetObjectIdIndex()};
				if (objectIdIndex < SvDef::cObjectIndexMaxNr)
				{
					m_pObjectLinkList[objectIdIndex] = m_pLinkedObject;
				}
			}
		}
	}
	UpdateGroups();
}

void SVIOAdjustDialog::OnSelchangeInspection()
{
	if (IO_PLC_OUTPUT == m_ioObjectType)
	{
		int inspectionIndex {m_inspectionCtrl.GetCurSel()};

		if (CB_ERR == inspectionIndex)
		{
			return;
		}
		uint32_t inspectionID {static_cast<uint32_t> (m_inspectionCtrl.GetItemData(inspectionIndex))};
		SVInspectionProcess* pInspection = dynamic_cast<SVInspectionProcess*> (SVObjectManagerClass::Instance().GetObject(inspectionID));
		if (nullptr != pInspection)
		{
			DWORD objectIdIndex {pInspection->GetObjectIdIndex()};
			if (objectIdIndex < SvDef::cObjectIndexMaxNr)
			{
				m_pLinkedObject = m_pObjectLinkList[objectIdIndex];
			}
		}
		m_IOName = (nullptr != m_pLinkedObject) ? m_pLinkedObject->GetCompleteName().c_str() : _T("");
		UpdateData(false);
	}
}

void SVIOAdjustDialog::UpdateGroups()
{
	if (nullptr != m_pDigInput)
	{
		m_isForced = m_pDigInput->IsForced();
		m_isInverted = m_pDigInput->IsInverted();
		m_isForcedFalse = !m_pDigInput->GetForcedValue();
		m_isForcedTrue = m_pDigInput->GetForcedValue();
	}
	else if (nullptr != m_pDigOutput)
	{
		m_isForced = m_pDigOutput->IsForced();
		m_isInverted = m_pDigOutput->IsInverted();
		m_isForcedFalse = !m_pDigOutput->GetForcedValue();
		m_isForcedTrue = m_pDigOutput->GetForcedValue();

		m_sCombined = m_pDigOutput->isCombined();
		m_isCombinedACK = m_pDigOutput->isAndACK();
		m_isCombinedNAK = !m_isCombinedACK;
	}
	else if (nullptr != m_pPlcOutput)
	{
		m_sCombined = m_pPlcOutput->isCombined();
		m_isCombinedACK = m_pPlcOutput->isAndACK();
		m_isCombinedNAK = !m_isCombinedACK;
	}

	auto* pParent = (nullptr != m_pLinkedObject && nullptr != m_pLinkedObject->GetParent()) ? m_pLinkedObject->GetParent() : nullptr;
	SvPb::SVObjectTypeEnum objectType =  (nullptr != pParent) ? pParent->GetObjectType() : SvPb::SVObjectTypeEnum::SVNotSetObjectType;
	switch (m_ioObjectType)
	{
		case SVIOObjectType::IO_DIGITAL_INPUT:
		{
			showForcedGroup(SW_SHOW);
			showInvertGroup(SW_SHOW);
			showCombinedGroup(SW_HIDE);
			m_inspectionCtrl.ShowWindow(SW_HIDE);
			GetDlgItem(IDC_BUTTON_SELECTOR)->ShowWindow(SW_HIDE);
			break;
		}
		case SVIOObjectType::IO_DIGITAL_OUTPUT:
		{
			showForcedGroup(SW_SHOW);
			showInvertGroup(SW_SHOW);
			showCombinedGroup(SvPb::SVObjectTypeEnum::SVPPQObjectType == objectType ? SW_HIDE : SW_SHOW);
			if (SvPb::SVObjectTypeEnum::SVPPQObjectType == objectType)
			{
				m_sCombined = FALSE;
			}
			break;
		}
		case SVIOObjectType::IO_PLC_OUTPUT:
		{
			showForcedGroup(SW_HIDE);
			showInvertGroup(SW_HIDE);
			showCombinedGroup(SvPb::SVObjectTypeEnum::SVPPQObjectType == objectType ? SW_HIDE : SW_SHOW);
			if (SvPb::SVObjectTypeEnum::SVPPQObjectType == objectType)
			{
				m_sCombined = FALSE;
			}
			break;
		}
		default:
		{
			break;
		}
	}

	UpdateData(false);
}

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
