//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVToolAdjustmentDialogStatisticsPageClass
//* .File Name       : $Workfile:   SVTADlgStatisticsPage.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.5  $
//* .Check In Date   : $Date:   16 Dec 2014 18:03:14  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVTADlgStatisticsPage.h"
#include "SVToolSet.h"
#include "SVStatTool.h"
#include "SVResult.h"
#include "SVIPDoc.h"
#include "SVInspectionProcess.h"
#include "SVSetupDialogManager.h"
#include "SVObjectLibrary/SVObjectManagerClass.h"
#include "ObjectSelectorLibrary/ObjectTreeGenerator.h"
#include "ObjectInterfaces\IObjectManager.h"
#include "SVStatusLibrary/ErrorNumbers.h"
#include "SVOGui/NoSelector.h"
#include "SVOGui/ToolSetItemSelector.h"
#include "SVToolAdjustmentDialogSheetClass.h"
#include "SVUtilityLibrary/StringHelper.h"
#pragma endregion Includes

#pragma region Declarations

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
#pragma endregion Declarations

BEGIN_MESSAGE_MAP(SVToolAdjustmentDialogStatisticsPageClass, CPropertyPage)
	//{{AFX_MSG_MAP(SVToolAdjustmentDialogStatisticsPageClass)
	ON_BN_CLICKED(IDC_ADD_BUTTON, OnButtonAdd)
	ON_BN_CLICKED(IDC_REMOVE_BUTTON, OnButtonRemove)
	ON_BN_CLICKED(IDC_SET_RANGE, OnSetRange)
	ON_BN_CLICKED(IDC_PUBLISH_BUTTON, OnPublishButton)
	ON_BN_CLICKED(IDC_BTN_OBJECT_PICKER, OnBtnObjectPicker)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


void SVToolAdjustmentDialogStatisticsPageClass::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	//{{AFX_DATA_MAP(SVToolAdjustmentDialogStatisticsPageClass)
	DDX_Control(pDX, IDC_CHILDREN_LIST, m_lbSelectedList);
	DDX_Control(pDX, IDC_AVAILABLE_CHILDREN_LIST, m_lbAvailableList);
	DDX_Text(pDX, IDC_OCCURANCE_TO_TRACK_EDT, m_strTestValue);
	DDX_Text(pDX, IDC_VARIABLE_TO_MONITOR, m_strVariableToMonitor);
	//}}AFX_DATA_MAP
}

SVToolAdjustmentDialogStatisticsPageClass::SVToolAdjustmentDialogStatisticsPageClass(const SVGUID& rInspectionID, const SVGUID& rTaskObjectID, SVToolAdjustmentDialogSheetClass* pParent) 
: CPropertyPage( SVToolAdjustmentDialogStatisticsPageClass::IDD )
, m_pParent(pParent)
, m_pTool(nullptr)
, m_pToolSet(nullptr)
{
	//{{AFX_DATA_INIT(SVToolAdjustmentDialogStatisticsPageClass)
	m_strTestValue = _T( "" );
	m_strVariableToMonitor = _T("");
	//}}AFX_DATA_INIT

	if( m_pTool = dynamic_cast <SVStatisticsToolClass*> (SvOi::getObject(rTaskObjectID)) )
	{
		m_pToolSet = dynamic_cast <SVToolSetClass*> (m_pTool->GetOwner());
	}
}

SVToolAdjustmentDialogStatisticsPageClass::~SVToolAdjustmentDialogStatisticsPageClass()
{
}

BOOL SVToolAdjustmentDialogStatisticsPageClass::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();

	if( m_pTool )
	{
		getParameters();

		SVObjectReference refObject = m_pTool->GetVariableSelected();
		m_strVariableToMonitor = refObject.GetCompleteObjectNameToObjectType( nullptr, m_pToolSet->GetObjectType() ).c_str();
		m_strFullNameOfVariable = refObject.GetCompleteName().c_str();
	}

	UpdateData( FALSE ); // set data to dialog

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX-Eigenschaftenseiten sollten FALSE zurückgeben
}

/////////////////////////////////////////////////////////////////////////////
//
void SVToolAdjustmentDialogStatisticsPageClass::OnButtonAdd() 
{
	DWORD_PTR lAvailableIndex = m_lbAvailableList.GetItemData( m_lbAvailableList.GetCurSel() );
	DWORD_PTR lCurrentIndex = m_lbSelectedList.GetItemData( m_lbSelectedList.GetCurSel() );

	if( lAvailableIndex != LB_ERR && lAvailableIndex >= 0)
	{
		try
		{
			m_pTool->EnableFeature ((SVStatisticsFeatureEnum) lAvailableIndex);
		}
		catch (const SvStl::MessageContainer& rExp)
		{
			SvStl::MessageMgrStd Exception(SvStl::LogAndDisplay);
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
void SVToolAdjustmentDialogStatisticsPageClass::OnButtonRemove() 
{

	int index = ( int ) m_lbSelectedList.GetItemData( m_lbSelectedList.GetCurSel() );

	if( index != LB_ERR && index >= 0 )
	{
        m_pTool->DisableFeature ((SVStatisticsFeatureEnum) index);

		// List of not enabled.
		initListBox( &m_lbAvailableList, _T('0') );
		
		// List of enabled.
		initListBox( &m_lbSelectedList, _T('1') );

        UpdateData (FALSE);
	}
}

/////////////////////////////////////////////////////////////////////////////
//
void SVToolAdjustmentDialogStatisticsPageClass::initListBox(CListBox* pListBox, TCHAR atcTestChar)
{

    long lEmpty;
	long lReturned;
    long lError;

    lError = 0;

    while (1)
    {
		std::string FeatureString;
		FeatureString = m_pTool->GetFeatureString();
		
        pListBox->ResetContent();

        lEmpty = TRUE;

		for (int i = SV_STATS_MIN_VALUE; i < SV_STATS_TOPOF_LIST; i++ )
		{	
            if( FeatureString[i] == atcTestChar)
            {
				lReturned = pListBox->AddString( m_pTool->GetFeatureName( i ).c_str() );
                
                if (lReturned == LB_ERR || lReturned == LB_ERRSPACE)
                {
                    lError = -SvStl::Err_15005;
                    break;
                }

    			lReturned = pListBox->SetItemData( lReturned, static_cast<DWORD_PTR>(i) );
                if (lReturned == LB_ERR)
                {
                    lError = -SvStl::Err_15006;
                    break;
                }
                lEmpty = FALSE;
	        }
		}

		pListBox->SetCurSel( 0 );

        break;
    }
}

/////////////////////////////////////////////////////////////////////////////
//
void SVToolAdjustmentDialogStatisticsPageClass::getParameters()
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
void SVToolAdjustmentDialogStatisticsPageClass::UpdateStatisticsParameters()
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
void SVToolAdjustmentDialogStatisticsPageClass::OnOK()
{
	UpdateData(TRUE);

	UpdateStatisticsParameters();
}

BOOL SVToolAdjustmentDialogStatisticsPageClass::OnSetActive() 
{
	return CPropertyPage::OnSetActive();
}

BOOL SVToolAdjustmentDialogStatisticsPageClass::OnKillActive() 
{
	UpdateData(TRUE);

	UpdateStatisticsParameters();
	
	return CPropertyPage::OnKillActive();
}

/////////////////////////////////////////////////////////////////////////////
//
void SVToolAdjustmentDialogStatisticsPageClass::OnSetRange() 
{
    SVResultClass* pResult;

		// Get Selected feature
		int item = m_lbSelectedList.GetCurSel();
		if( item != LB_ERR )
		{
			DWORD_PTR index = m_lbSelectedList.GetItemData( item );
			pResult = m_pTool->GetResult( (SVStatisticsFeatureEnum)index );

			if (nullptr == pResult)
			{
				SvStl::MessageMgrStd MesMan( SvStl::LogOnly );
				MesMan.setMessage( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvStl::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_16089);
				

			}
			else if (S_OK != SVSetupDialogManager::Instance().SetupDialog( pResult->GetClassID(), pResult->GetUniqueObjectID(), this ))
			{
				
				SvStl::MessageMgrStd MesMan( SvStl::LogOnly );
				MesMan.setMessage( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvStl::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_16090);
			}
    }
}

/////////////////////////////////////////////////////////////////////////////
//
void SVToolAdjustmentDialogStatisticsPageClass::OnPublishButton()
{
	if( nullptr == m_pTool ) { return; }

	SVInspectionProcess* pInspection = dynamic_cast<SVInspectionProcess*>(m_pTool->GetInspection());
	if( nullptr == pInspection ) { return; }

	SvOsl::ObjectTreeGenerator::Instance().setSelectorType( SvOsl::ObjectTreeGenerator::SelectorTypeEnum::TypeSetAttributes );
	SvOsl::ObjectTreeGenerator::Instance().setLocationFilter( SvOsl::ObjectTreeGenerator::FilterInput, std::string(pInspection->GetToolSet()->GetCompleteName()), std::string( _T("") ) );

	SvOsl::SelectorOptions BuildOptions( pInspection->GetUniqueObjectID(), SvDef::SV_PUBLISHABLE, m_pTool->GetUniqueObjectID() );
	SvOsl::ObjectTreeGenerator::Instance().BuildSelectableItems<SvOg::NoSelector, SvOg::NoSelector, SvOg::ToolSetItemSelector<>>(BuildOptions);

	std::string PublishableResults = SvUl::LoadStdString( IDS_PUBLISHABLE_RESULTS );
	std::string Title = SvUl::Format( _T("%s - %s"), PublishableResults.c_str(), m_pTool->GetName() );
	std::string Filter = SvUl::LoadStdString( IDS_FILTER );
	
	INT_PTR Result = SvOsl::ObjectTreeGenerator::Instance().showDialog( Title.c_str(), PublishableResults.c_str(), Filter.c_str(), this );

	if( IDOK == Result )
	{
		SVPublishListClass& PublishList = pInspection->GetPublishList();
		PublishList.Refresh( static_cast<SVTaskObjectClass*>(pInspection->GetToolSet()) );
		if (m_pParent)
		{
			SVIPDoc* l_pIPDoc = m_pParent->GetIPDoc();

			if( nullptr != l_pIPDoc )
			{
				l_pIPDoc->SetModifiedFlag();
			}
		}
	}
}

void SVToolAdjustmentDialogStatisticsPageClass::OnBtnObjectPicker()
{
	if( nullptr == m_pTool || nullptr == m_pToolSet ) { return; }

	SVObjectClass* pInspection( m_pTool->GetInspection() );
	if( nullptr == pInspection ) { return; }

	m_pTool->UpdateTaskObjectOutputListAttributes();

	std::string InspectionName( pInspection->GetName() );

	SvOsl::ObjectTreeGenerator::SelectorTypeEnum SelectorType;
	SelectorType = static_cast<SvOsl::ObjectTreeGenerator::SelectorTypeEnum>(SvOsl::ObjectTreeGenerator::SelectorTypeEnum::TypeSetAttributes | SvOsl::ObjectTreeGenerator::SelectorTypeEnum::TypeSingleObject);
	SvOsl::ObjectTreeGenerator::Instance().setSelectorType( SelectorType );
	SvOsl::ObjectTreeGenerator::Instance().setLocationFilter( SvOsl::ObjectTreeGenerator::FilterInput, InspectionName, std::string( _T("") ) );

	SvOsl::SelectorOptions BuildOptions( pInspection->GetUniqueObjectID(), SvDef::SV_SELECTABLE_FOR_STATISTICS, m_pToolSet->GetUniqueObjectID() );
	SvOsl::ObjectTreeGenerator::Instance().BuildSelectableItems<SvOg::NoSelector, SvOg::NoSelector, SvOg::ToolSetItemSelector<>>( BuildOptions );

	std::string ToolsetOutput = SvUl::LoadStdString( IDS_SELECT_TOOLSET_OUTPUT );
	std::string Title = SvUl::Format( _T("%s - %s"), ToolsetOutput.c_str(), m_pTool->GetName() );
	std::string Filter = SvUl::LoadStdString( IDS_FILTER );

	INT_PTR Result = SvOsl::ObjectTreeGenerator::Instance().showDialog( Title.c_str(), ToolsetOutput.c_str(), Filter.c_str(), this );

	if( IDOK == Result )
	{
		m_strVariableToMonitor = SvOsl::ObjectTreeGenerator::Instance().getSingleObjectResult().getDisplayLocation().c_str();

		SVGUID ResultObjectGuid( SvOsl::ObjectTreeGenerator::Instance().getSingleObjectResult().getItemKey() );
		SVObjectClass* pResultObject = nullptr;
		SVObjectManagerClass::Instance().GetObjectByIdentifier( ResultObjectGuid,  pResultObject);
		if( nullptr != pResultObject )
		{
			m_pTool->SetVariableSelected( std::string(pResultObject->GetCompleteName()) );
			m_strFullNameOfVariable = pResultObject->GetCompleteName().c_str();
		}

		UpdateData( FALSE );
	}
}

