//******************************************************************************
//* COPYRIGHT (c) 2012 by Seidenader Vision, Inc.
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVTADlgRemoteInputToolPage
//* .File Name       : $Workfile:   SVTADlgRemoteInputToolPage.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.3  $
//* .Check In Date   : $Date:   16 Dec 2014 18:01:22  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVTADlgRemoteInputToolPage.h"

#include "SVInspectionProcess.h"
#include "Tools/SVRemoteInputTool.h"
#include "Tools/SVTool.h"
#include "SVToolAdjustmentDialogSheetClass.h"
#include "SVToolset.h"
#include "ObjectSelectorLibrary/ObjectTreeGenerator.h"
#include "InspectionCommands/CommandExternalHelper.h"
#include "Definitions/StringTypeDef.h"
#include "SVOResource/ConstGlobalSvOr.h"
#pragma endregion Includes

#pragma region Declarations

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
#pragma endregion Declarations

SVTADlgRemoteInputToolPage::SVTADlgRemoteInputToolPage( const SVGUID& rInspectionID, const SVGUID& rTaskObjectID, SVToolAdjustmentDialogSheetClass* Parent, int id ) 
: CPropertyPage( id )
, m_pParentDialog( Parent )
, m_pTool( nullptr )
, m_InputName( _T( "" ) )
{
}

SVTADlgRemoteInputToolPage::~SVTADlgRemoteInputToolPage()
{
}

void SVTADlgRemoteInputToolPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_REMOTE_INPUT_NAME, m_RemoteInputNameCtrl);
}


BEGIN_MESSAGE_MAP(SVTADlgRemoteInputToolPage, CPropertyPage)
	ON_BN_CLICKED(IDC_CLEAR_INPUT_BUTTON, &SVTADlgRemoteInputToolPage::OnBnClickedClearInputButton)
	ON_BN_CLICKED(IDC_SELECT_INPUT_BUTTON, &SVTADlgRemoteInputToolPage::OnBnClickedSelectInputButton)
END_MESSAGE_MAP()

BOOL SVTADlgRemoteInputToolPage::OnInitDialog() 
{
	BOOL l_Status = CPropertyPage::OnInitDialog();

	l_Status = l_Status && ( nullptr != m_pParentDialog );

	if( l_Status )
	{
		m_pTool = dynamic_cast<SvTo::SVRemoteInputTool*> (m_pParentDialog->GetTool());
	}

	l_Status = l_Status && ( nullptr != m_pTool );

	if( l_Status )
	{
		m_InputName = m_pTool->GetInputObject().GetCompleteName(true);
	}

	RefreshSelectedInputName();

	UpdateData( FALSE );

	return l_Status;  // return TRUE unless you set the focus to a control
	                  // EXCEPTION: OCX Property Pages should return FALSE
}

void SVTADlgRemoteInputToolPage::OnBnClickedClearInputButton()
{
	m_InputName.clear();

	if( nullptr != m_pTool )
	{
		m_pTool->ClearInputObject();
	}

	RefreshSelectedInputName();
}

void SVTADlgRemoteInputToolPage::OnBnClickedSelectInputButton()
{
	if( nullptr == m_pTool ) { return; }

	SVToolSetClass* pToolSet = dynamic_cast<SVToolSetClass*>( m_pTool->GetAncestor(SvPb::SVToolSetObjectType ) );

	if( nullptr == pToolSet ) { return; }

	SvPb::InspectionCmdMsgs request, response;
	*request.mutable_getobjectselectoritemsrequest() = SvCmd::createObjectSelectorRequest(
		{SvPb::ObjectSelectorType::globalConstantItems, SvPb::ObjectSelectorType::toolsetItems},
		pToolSet->GetInspection()->GetUniqueObjectID(), SvPb::archivable, pToolSet->GetUniqueObjectID());
	SvCmd::InspectionCommandsSynchronous(pToolSet->GetInspection()->GetUniqueObjectID(), &request, &response);

	SvOsl::ObjectTreeGenerator::Instance().setSelectorType( SvOsl::ObjectTreeGenerator::SelectorTypeEnum::TypeSingleObject);
	if (response.has_getobjectselectoritemsresponse())
	{
		SvOsl::ObjectTreeGenerator::Instance().insertTreeObjects(response.getobjectselectoritemsresponse().tree());
	}

	SVObjectReference objectRef(m_pTool->GetInputObject());

	if(nullptr != objectRef.getObject())
	{
		SvDef::StringSet Items;

		Items.insert(objectRef.GetObjectNameBeforeObjectType(SvPb::SVInspectionObjectType, true));
		SvOsl::ObjectTreeGenerator::Instance().setCheckItems(Items);
	}

	std::string ToolsetOutput = SvUl::LoadStdString( IDS_SELECT_TOOLSET_OUTPUT );
	std::string Title = SvUl::Format( _T("%s - %s"), ToolsetOutput.c_str(), m_pTool->GetName() );
	std::string Filter = SvUl::LoadStdString( IDS_FILTER );

	INT_PTR Result = SvOsl::ObjectTreeGenerator::Instance().showDialog( Title.c_str(), ToolsetOutput.c_str(), Filter.c_str(), this );

	if( IDOK == Result )
	{
		SVObjectReference ObjectRef {SvOsl::ObjectTreeGenerator::Instance().getSingleObjectResult()};
		m_InputName = ObjectRef.GetCompleteName(true);

		m_pTool->SetInputObject(ObjectRef.GetGuidAndIndexOneBased());
	}

	RefreshSelectedInputName();
}

void SVTADlgRemoteInputToolPage::RefreshSelectedInputName()
{
	m_RemoteInputNameCtrl.SetWindowText( m_InputName.c_str() );
}

