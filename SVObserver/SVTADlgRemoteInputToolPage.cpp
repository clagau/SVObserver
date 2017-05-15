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
#include "SVRemoteInputTool.h"
#include "SVOCore/SVTool.h"
#include "SVToolAdjustmentDialogSheetClass.h"
#include "SVToolset.h"
#include "ObjectSelectorLibrary/ObjectTreeGenerator.h"
#include "SVOGui/GlobalSelector.h"
#include "SVOGui/NoSelector.h"
#include "SVOGui/ToolSetItemSelector.h"
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
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_REMOTE_INPUT_NAME, m_RemoteInputNameCtrl);
}


BEGIN_MESSAGE_MAP(SVTADlgRemoteInputToolPage, CDialog)
	ON_BN_CLICKED(IDC_CLEAR_INPUT_BUTTON, &SVTADlgRemoteInputToolPage::OnBnClickedClearInputButton)
	ON_BN_CLICKED(IDC_SELECT_INPUT_BUTTON, &SVTADlgRemoteInputToolPage::OnBnClickedSelectInputButton)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// SVCylindricalWarpDlg message handlers

BOOL SVTADlgRemoteInputToolPage::OnInitDialog() 
{
	BOOL l_Status = CPropertyPage::OnInitDialog();

	l_Status = l_Status && ( nullptr != m_pParentDialog );

	if( l_Status )
	{
		m_pTool = dynamic_cast< SVRemoteInputTool* >( m_pParentDialog->GetTool() );
	}

	l_Status = l_Status && ( nullptr != m_pTool );

	if( l_Status )
	{
		SVObjectClass* l_pObject = m_pTool->GetInputObject();

		l_Status = ( nullptr != l_pObject );

		if( l_Status )
		{
			m_InputName = l_pObject->GetCompleteName();
		}
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

	SVToolSetClass* pToolSet = dynamic_cast<SVToolSetClass*>( m_pTool->GetAncestor( SVToolSetObjectType ) );

	if( nullptr == pToolSet ) { return; }

	SVString InspectionName( pToolSet->GetInspection()->GetName() );

	SvOsl::ObjectTreeGenerator::Instance().setSelectorType( SvOsl::ObjectTreeGenerator::SelectorTypeEnum::TypeSingleObject );
	SvOsl::ObjectTreeGenerator::Instance().setLocationFilter( SvOsl::ObjectTreeGenerator::FilterInput, InspectionName, SVString( _T("") ) );

	SvOsl::SelectorOptions BuildOptions( pToolSet->GetInspection()->GetUniqueObjectID(), SvOi::SV_ARCHIVABLE, pToolSet->GetUniqueObjectID() );
	SvOsl::ObjectTreeGenerator::Instance().BuildSelectableItems<SvOg::GlobalSelector, SvOg::NoSelector, SvOg::ToolSetItemSelector<>>( BuildOptions );

	if( nullptr !=  m_pTool->GetInputObject() )
	{
		SVStringSet Items;

		SvOsl::SelectorItem InsertItem;
		SVObjectReference ObjectRef( m_pTool->GetInputObject() );

		InsertItem.setName( ObjectRef.GetName().c_str() );
		InsertItem.setLocation( ObjectRef.GetCompleteOneBasedObjectName().c_str() );

		SVString Location = SvOsl::ObjectTreeGenerator::Instance().convertObjectArrayName( InsertItem );
		Items.insert( Location );
		SvOsl::ObjectTreeGenerator::Instance().setCheckItems( Items );
	}

	SVString ToolsetOutput = SvUl_SF::LoadSVString( IDS_SELECT_TOOLSET_OUTPUT );
	SVString Title = SvUl_SF::Format( _T("%s - %s"), ToolsetOutput.c_str(), m_pTool->GetName() );
	SVString Filter = SvUl_SF::LoadSVString( IDS_FILTER );

	INT_PTR Result = SvOsl::ObjectTreeGenerator::Instance().showDialog( Title.c_str(), ToolsetOutput.c_str(), Filter.c_str(), this );

	if( IDOK == Result )
	{
		m_InputName = SvOsl::ObjectTreeGenerator::Instance().getSingleObjectResult().getLocation();

		SVGUID ObjectGuid(SvOsl::ObjectTreeGenerator::Instance().getSingleObjectResult().getItemKey());
		m_pTool->SetInputObject( ObjectGuid );
	}

	RefreshSelectedInputName();
}

void SVTADlgRemoteInputToolPage::RefreshSelectedInputName()
{
	m_RemoteInputNameCtrl.SetWindowText( m_InputName.c_str() );
}

