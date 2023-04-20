//******************************************************************************
//* COPYRIGHT (c) 2008 by Körber Pharma Inspection GmbH. All Rights Reserved
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
#include "SVRemoteOutputGroup.h"
#include "SVConfigurationObject.h"
#include "SVObjectLibrary\SVObjectManagerClass.h"
#include "SVInspectionProcess.h"
#include "SVStatusLibrary\MessageManager.h"
#include "InspectionCommands\CommandExternalHelper.h"
#include "ObjectSelectorLibrary\ObjectTreeGenerator.h"
#pragma endregion Includes

SVRemoteOutputEditDialog::SVRemoteOutputEditDialog(CWnd* pParent /*=nullptr*/)
: CDialog(SVRemoteOutputEditDialog::IDD, pParent)
{
}

SVRemoteOutputEditDialog::~SVRemoteOutputEditDialog()
{
}

void SVRemoteOutputEditDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_INSPECTION, m_inspectionCtrl);
	DDX_Text(pDX, IDC_IONAME, m_IOName);
}


BEGIN_MESSAGE_MAP(SVRemoteOutputEditDialog, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_SELECTOR, OnObjectSelector)
END_MESSAGE_MAP()


BOOL SVRemoteOutputEditDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	SVObjectClass* pObject = SVObjectManagerClass::Instance().GetObjectA( m_InputObjectID );
	if( pObject )
	{
		m_IOName = pObject->GetCompleteName().c_str();
	}

	SVConfigurationObject* pConfig( nullptr );
	SVObjectManagerClass::Instance().GetConfigurationObject( pConfig );

	if( nullptr == pConfig )
	{
		SvStl::MessageManager e(SvStl::MsgType::Log );
		e.setMessage( SVMSG_SVO_55_DEBUG_BREAK_ERROR, SvStl::Tid_ErrorGettingPPQCount, SvStl::SourceFileParams(StdMessageParams));
		DebugBreak();
		return false;
	}

	SVRemoteOutputGroup* l_pRemoteGroup = pConfig->GetRemoteOutputGroup( m_GroupName );
	long lPPQSize = pConfig->GetPPQCount();
	for( int k = 0; k < lPPQSize; k++ )
	{
		SVPPQObject* pPPQ = pConfig->GetPPQ( k );
		if( nullptr == pPPQ )
		{
			SvStl::MessageManager e(SvStl::MsgType::Log );
			e.setMessage( SVMSG_SVO_55_DEBUG_BREAK_ERROR, SvStl::Tid_ErrorGettingPPQ, SvStl::SourceFileParams(StdMessageParams));
			DebugBreak();
			return false;
		}

		std::string l_strPPQName( pPPQ->GetName() );
		if( l_strPPQName == l_pRemoteGroup->GetPPQName() )
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
			break;
		}
	}// end for

	m_inspectionCtrl.SetCurSel(0);

	UpdateData(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
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
	}
	return CDialog::PreTranslateMessage(pMsg);
}

void SVRemoteOutputEditDialog::OnObjectSelector()
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
	searchArea.push_back(SvPb::SearchArea::ppqItems);
	searchArea.push_back(SvPb::SearchArea::toolsetItems);
	*requestCmd.mutable_getobjectselectoritemsrequest() = SvCmd::createObjectSelectorRequest(
		searchArea, inspectionID, SvPb::viewable, SvDef::InvalidObjectId, false, SvPb::allValueObjects, SvPb::GetObjectSelectorItemsRequest::kAttributesAllowed);

	SvCmd::InspectionCommands(inspectionID, requestCmd, &responseCmd);
	SvOsl::ObjectTreeGenerator::Instance().setSelectorType(SvOsl::ObjectTreeGenerator::SelectorTypeEnum::TypeSingleObject);
	if (responseCmd.has_getobjectselectoritemsresponse())
	{
		SvOsl::ObjectTreeGenerator::Instance().insertTreeObjects(responseCmd.getobjectselectoritemsresponse().tree());
	}

	SVObjectReference oldObjectRef {m_InputObjectID};
	SvOsl::ObjectTreeGenerator::Instance().setCheckItems({oldObjectRef.GetObjectNameToObjectType()});
	CString inspectionSelection;
	m_inspectionCtrl.GetLBText(m_inspectionCtrl.GetCurSel(), inspectionSelection);
	std::string ToolsetOutput = SvUl::LoadStdString(IDS_SELECT_TOOLSET_OUTPUT);
	std::string Filter = SvUl::LoadStdString(IDS_FILTER);

	INT_PTR Result = SvOsl::ObjectTreeGenerator::Instance().showDialog(inspectionSelection.GetString(), ToolsetOutput.c_str(), Filter.c_str(), this);

	if (IDOK == Result)
	{
		SVObjectReference ObjectRef {SvOsl::ObjectTreeGenerator::Instance().getSingleObjectResult()};
		m_IOName = ObjectRef.GetCompleteName(true).c_str();
		m_InputObjectID = ObjectRef.getObjectId();
	}
	UpdateData(false);
}