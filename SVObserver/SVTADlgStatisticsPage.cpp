//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVTADlgStatisticsPage
//* .File Name       : $Workfile:   SVTADlgStatisticsPage.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.5  $
//* .Check In Date   : $Date:   16 Dec 2014 18:03:14  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVTADlgStatisticsPage.h"
#include "SVInspectionProcess.h"
#include "SVIPDoc.h"
#include "SVSetupDialogManager.h"
#include "SVToolSet.h"
#include "InspectionCommands/CommandExternalHelper.h"
#include "ObjectSelectorLibrary/ObjectTreeGenerator.h"
#include "ObjectInterfaces\IObjectManager.h"
#include "Operators/SVResult.h"
#include "SVOResource/ConstGlobalSvOr.h"
#include "SVObjectLibrary/SVObjectManagerClass.h"
#include "SVStatusLibrary/ErrorNumbers.h"
#include "SVToolAdjustmentDialogSheetClass.h"
#include "SVUtilityLibrary/StringHelper.h"
#include "Tools/SVStatTool.h"
#pragma endregion Includes

#pragma region Declarations

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
#pragma endregion Declarations

BEGIN_MESSAGE_MAP(SVTADlgStatisticsPage, CPropertyPage)
	//{{AFX_MSG_MAP(SVTADlgStatisticsPage)
	ON_BN_CLICKED(IDC_ADD_BUTTON, OnButtonAdd)
	ON_BN_CLICKED(IDC_REMOVE_BUTTON, OnButtonRemove)
	ON_BN_CLICKED(IDC_SET_RANGE, OnSetRange)
	ON_BN_CLICKED(IDC_PUBLISH_BUTTON, OnPublishButton)
	ON_BN_CLICKED(IDC_BTN_OBJECT_PICKER, OnBtnObjectPicker)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


void SVTADlgStatisticsPage::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	//{{AFX_DATA_MAP(SVTADlgStatisticsPage)
	DDX_Control(pDX, IDC_CHILDREN_LIST, m_lbSelectedList);
	DDX_Control(pDX, IDC_AVAILABLE_CHILDREN_LIST, m_lbAvailableList);
	DDX_Text(pDX, IDC_OCCURANCE_TO_TRACK_EDT, m_strTestValue);
	DDX_Text(pDX, IDC_VARIABLE_TO_MONITOR, m_strVariableToMonitor);
	//}}AFX_DATA_MAP
}

SVTADlgStatisticsPage::SVTADlgStatisticsPage(uint32_t, uint32_t taskObjectId, SVToolAdjustmentDialogSheetClass* pParent) 
: CPropertyPage( SVTADlgStatisticsPage::IDD )
, m_pParent(pParent)
, m_pTool(nullptr)
{
	//{{AFX_DATA_INIT(SVTADlgStatisticsPage)
	m_strTestValue = _T( "" );
	m_strVariableToMonitor = _T("");
	//}}AFX_DATA_INIT

	 m_pTool = dynamic_cast <SvTo::SVStatTool*> (SvOi::getObject(taskObjectId));
}

SVTADlgStatisticsPage::~SVTADlgStatisticsPage()
{
}

BOOL SVTADlgStatisticsPage::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();

	if( m_pTool )
	{
		getParameters();

		SVObjectReference ObjectRef = m_pTool->GetVariableSelected();
		m_strVariableToMonitor = ObjectRef.GetObjectNameToObjectType(SvPb::SVToolSetObjectType, true).c_str();
		m_strFullNameOfVariable = ObjectRef.GetCompleteName(true).c_str();
	}

	UpdateData( FALSE ); // set data to dialog

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX-Eigenschaftenseiten sollten FALSE zurückgeben
}

/////////////////////////////////////////////////////////////////////////////
//
void SVTADlgStatisticsPage::OnButtonAdd() 
{
	DWORD_PTR lAvailableIndex = m_lbAvailableList.GetItemData( m_lbAvailableList.GetCurSel() );

	if( lAvailableIndex != LB_ERR && lAvailableIndex >= 0)
	{
		try
		{
			m_pTool->EnableFeature (static_cast<SvTo::SVStatisticsFeatureEnum> (lAvailableIndex));
		}
		catch (const SvStl::MessageContainer& rExp)
		{
			SvStl::MessageManager Exception(SvStl::MsgType::Log | SvStl::MsgType::Display);
			Exception.setMessage(rExp.getMessage());
		}

		// List of not enabled.
		initListBox( &m_lbAvailableList, _T('0') );

		// List of enabled.
		initListBox( &m_lbSelectedList, _T('1') );

		UpdateData (FALSE);
	}
}

/////////////////////////////////////////////////////////////////////////////
//
void SVTADlgStatisticsPage::OnButtonRemove() 
{

	int index = ( int ) m_lbSelectedList.GetItemData( m_lbSelectedList.GetCurSel() );

	if( index != LB_ERR && index >= 0 )
	{
        m_pTool->DisableFeature (static_cast<SvTo::SVStatisticsFeatureEnum> (index));

		// List of not enabled.
		initListBox( &m_lbAvailableList, _T('0') );
		
		// List of enabled.
		initListBox( &m_lbSelectedList, _T('1') );

        UpdateData (FALSE);
	}
}

/////////////////////////////////////////////////////////////////////////////
//
void SVTADlgStatisticsPage::initListBox(CListBox* pListBox, TCHAR atcTestChar)
{
	std::string FeatureString;
	FeatureString = m_pTool->GetFeatureString();

	pListBox->ResetContent();

	for (int i = SvTo::SV_STATS_MIN_VALUE; i < SvTo::SV_STATS_TOPOF_LIST; i++)
	{
		if (FeatureString[i] == atcTestChar)
		{
			long lReturned = pListBox->AddString(m_pTool->GetFeatureName(i).c_str());

			if (lReturned == LB_ERR || lReturned == LB_ERRSPACE)
			{
				break;
			}

			lReturned = pListBox->SetItemData(lReturned, static_cast<DWORD_PTR>(i));
			if (lReturned == LB_ERR)
			{
				break;
			}
		}
	}

	pListBox->SetCurSel(0);
}

/////////////////////////////////////////////////////////////////////////////
//
void SVTADlgStatisticsPage::getParameters()
{
	// Get Occurence Value
	if( nullptr != m_pTool )
	{
		m_strTestValue = m_pTool->GetOccurenceTestValue().c_str();
	}

	// List of not enabled.
	initListBox( &m_lbAvailableList, _T('0') );

	// List of enabled.
	initListBox( &m_lbSelectedList, _T('1') );
}

/////////////////////////////////////////////////////////////////////////////
//
void SVTADlgStatisticsPage::UpdateStatisticsParameters()
{
	if( m_pTool )
	{
		// Get the Occurence Value
		m_pTool->SetOccurenceTestValue( std::string(m_strTestValue) );

		// Get selected variable
		m_pTool->SetVariableSelected( std::string(m_strFullNameOfVariable) );
	}
}

/////////////////////////////////////////////////////////////////////////////
//
void SVTADlgStatisticsPage::OnOK()
{
	UpdateData(TRUE);

	UpdateStatisticsParameters();
}

BOOL SVTADlgStatisticsPage::OnSetActive() 
{
	return CPropertyPage::OnSetActive();
}

BOOL SVTADlgStatisticsPage::OnKillActive() 
{
	UpdateData(TRUE);

	UpdateStatisticsParameters();
	
	return CPropertyPage::OnKillActive();
}

/////////////////////////////////////////////////////////////////////////////
//
void SVTADlgStatisticsPage::OnSetRange() 
{
	// Get Selected feature
	int item = m_lbSelectedList.GetCurSel();
	if( item != LB_ERR )
	{
		DWORD_PTR index = m_lbSelectedList.GetItemData( item );
		SvOp::SVResult* pResult = m_pTool->GetResult(static_cast<SvTo::SVStatisticsFeatureEnum> (index));

		if (nullptr == pResult)
		{
			SvStl::MessageManager MesMan(SvStl::MsgType::Log );
			MesMan.setMessage( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvStl::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_16089);
		}
		else if (S_OK != SVSetupDialogManager::Instance().SetupDialog( pResult->GetClassID(), pResult->getObjectId(), this ))
		{
			SvStl::MessageManager MesMan(SvStl::MsgType::Log );
			MesMan.setMessage( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvStl::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_16090);
		}
    }
}

/////////////////////////////////////////////////////////////////////////////
//
void SVTADlgStatisticsPage::OnPublishButton()
{
	if( nullptr == m_pTool ) { return; }

	SVInspectionProcess* pInspection = dynamic_cast<SVInspectionProcess*>(m_pTool->GetInspection());
	if( nullptr == pInspection ) { return; }

	SvPb::InspectionCmdRequest requestCmd;
	SvPb::InspectionCmdResponse responseCmd;
	*requestCmd.mutable_getobjectselectoritemsrequest() = SvCmd::createObjectSelectorRequest(
		{SvPb::SearchArea::toolsetItems}, pInspection->getObjectId(), SvPb::publishable, m_pTool->getObjectId());
	
	SvCmd::InspectionCommands(pInspection->getObjectId(), requestCmd, &responseCmd);
	SvOsl::ObjectTreeGenerator::Instance().setSelectorType( SvOsl::ObjectTreeGenerator::SelectorTypeEnum::TypeMultipleObject, IDD_PUBLISHED_RESULTS + SvOr::HELPFILE_DLG_IDD_OFFSET);
	if (responseCmd.has_getobjectselectoritemsresponse())
	{
		SvOsl::ObjectTreeGenerator::Instance().insertTreeObjects(responseCmd.getobjectselectoritemsresponse().tree());
	}

	std::string PublishableResults = SvUl::LoadStdString( IDS_PUBLISHABLE_RESULTS );
	std::string Title = SvUl::Format( _T("%s - %s"), PublishableResults.c_str(), m_pTool->GetName() );
	std::string Filter = SvUl::LoadStdString( IDS_FILTER );
	
	INT_PTR Result = SvOsl::ObjectTreeGenerator::Instance().showDialog( Title.c_str(), PublishableResults.c_str(), Filter.c_str(), this );

	if( IDOK == Result )
	{
		for (auto const& rEntry : SvOsl::ObjectTreeGenerator::Instance().getModifiedObjects())
		{
			SVObjectReference ObjectRef {rEntry};
			bool previousState = SvPb::publishable == (SvPb::publishable & ObjectRef.ObjectAttributesSet());
			SvOi::SetAttributeType attributeType = previousState ? SvOi::SetAttributeType::RemoveAttribute : SvOi::SetAttributeType::AddAttribute;
			ObjectRef.SetObjectAttributesSet(SvPb::publishable, attributeType);
		}

		SVPublishList& PublishList = pInspection->GetPublishList();
		PublishList.Refresh( static_cast<SvIe::SVTaskObjectClass*>(pInspection->GetToolSet()) );

		if (nullptr != m_pParent)
		{
			m_pParent->markDocumentAsDirty();
		}
	}
}

void SVTADlgStatisticsPage::OnBtnObjectPicker()
{
	if( nullptr == m_pTool) { return; }

	SVObjectClass* pInspection( m_pTool->GetInspection() );
	if( nullptr == pInspection ) { return; }

	SvPb::InspectionCmdRequest requestCmd;
	SvPb::InspectionCmdResponse responseCmd;
	*requestCmd.mutable_getobjectselectoritemsrequest() = SvCmd::createObjectSelectorRequest(
		{SvPb::SearchArea::toolsetItems}, pInspection->getObjectId(), SvPb::selectableForStatistics, SvDef::InvalidObjectId, false, SvPb::attributesAllowed, SvPb::allNumberValueObjects);

	SvCmd::InspectionCommands(pInspection->getObjectId(), requestCmd, &responseCmd);
	SvOsl::ObjectTreeGenerator::Instance().setSelectorType(SvOsl::ObjectTreeGenerator::SelectorTypeEnum::TypeSingleObject);
	if (responseCmd.has_getobjectselectoritemsresponse())
	{
		SvOsl::ObjectTreeGenerator::Instance().insertTreeObjects(responseCmd.getobjectselectoritemsresponse().tree());
	}

	SVObjectReference objectRef(m_pTool->GetVariableSelected());
	if (nullptr != objectRef.getObject())
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
		SVObjectReference ObjectRef{SvOsl::ObjectTreeGenerator::Instance().getSingleObjectResult()};
		m_strVariableToMonitor = ObjectRef.GetObjectNameToObjectType(SvPb::SVToolSetObjectType, true).c_str();
		m_pTool->SetVariableSelected(ObjectRef.GetCompleteName(true));
		m_strFullNameOfVariable = ObjectRef.GetCompleteName(true).c_str();

		UpdateData( FALSE );
	}
}

