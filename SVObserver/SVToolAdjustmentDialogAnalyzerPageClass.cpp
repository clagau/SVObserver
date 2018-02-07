//******************************************************************************
//* COPYRIGHT (c) 2005 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVToolAdjustmentDialogAnalyzerPageClass
//* .File Name       : $Workfile:   SVToolAdjustmentDialogAnalyzerPageClass.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.6  $
//* .Check In Date   : $Date:   16 Dec 2014 18:04:26  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVToolAdjustmentDialogAnalyzerPageClass.h"

#include "SVObjectLibrary/SVObjectManagerClass.h"
#include "SVObjectLibrary/SVObjectSynchronousCommandTemplate.h"

#include "InspectionEngine/SVAnalyzer.h"
#include "SVChildrenSetupDialog.h"
#include "SVGlobal.h"
#include "SVIPDoc.h"
#include "SVInspectionProcess.h"
#include "InspectionEngine/SVTool.h"
#include "SVToolAdjustmentDialogSheetClass.h"
#include "SVToolSet.h"
#include "SVSetupDialogManager.h"
#include "ObjectSelectorLibrary/ObjectTreeGenerator.h"
#include "SVOGui/NoSelector.h"
#include "SVOGui/ToolSetItemSelector.h"
#include "InspectionCommands/GetCreatableObjects.h"
#include "SVStatusLibrary/ErrorNumbers.h"
#include "TextDefinesSvO.h"
#include "SVStatusLibrary/MessageManager.h"
#include "InspectionCommands/CommandFunctionHelper.h"
#pragma endregion Includes

#pragma region Declarations

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
#pragma endregion Declarations

BEGIN_MESSAGE_MAP(SVToolAdjustmentDialogAnalyzerPageClass, CPropertyPage)
	//{{AFX_MSG_MAP(SVToolAdjustmentDialogAnalyzerPageClass)
	ON_BN_CLICKED(IDC_DETAILS_BUTTON, OnButtonDetails)
	ON_CBN_SELCHANGE(IDC_ANALYZER_COMBO, OnSelchangeCurrentAnalyzer)
	ON_BN_CLICKED(IDC_RESULT_BUTTON, OnResultButton)
	ON_BN_CLICKED(IDC_PUBLISH_BUTTON, OnPublishButton)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

SVToolAdjustmentDialogAnalyzerPageClass::SVToolAdjustmentDialogAnalyzerPageClass( const SVGUID& rInspectionID, const SVGUID& rTaskObjectID, SVToolAdjustmentDialogSheetClass* pParent ) 
: CPropertyPage( SVToolAdjustmentDialogAnalyzerPageClass::IDD )
, m_pParentDialog(pParent)
, m_pTool(nullptr)
, m_pCurrentAnalyzer(nullptr)
, m_InspectionID(rInspectionID)
, m_TaskObjectID(rTaskObjectID)
, m_additionalAnalyzerId(GUID_NULL)
{
	if( m_pParentDialog )
	{
		m_pTool = m_pParentDialog->GetTool();

		if (m_pTool)
		{
			SvDef::SVObjectTypeInfoStruct info;
			info.ObjectType = SvDef::SVAnalyzerObjectType;

			m_pCurrentAnalyzer = dynamic_cast<SVAnalyzerClass *>(m_pTool->getFirstObject(info));
		}
	}
}

SVToolAdjustmentDialogAnalyzerPageClass::~SVToolAdjustmentDialogAnalyzerPageClass()
{
}

void SVToolAdjustmentDialogAnalyzerPageClass::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(SVToolAdjustmentDialogAnalyzerPageClass)
	DDX_Control(pDX, IDC_ANALYZER_COMBO, m_availableAnalyzerCombobox);
	DDX_Control(pDX, IDC_DIALOGIMAGE, dialogImage);
	//}}AFX_DATA_MAP
}


BOOL SVToolAdjustmentDialogAnalyzerPageClass::OnSetActive()
{
	setImages();
	return CPropertyPage::OnSetActive();
}

BOOL SVToolAdjustmentDialogAnalyzerPageClass::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	ASSERT( m_pTool );
	if( m_pTool )
	{
		const SvDef::SVObjectTypeInfoStruct& rToolType = m_pTool->GetObjectInfo().m_ObjectTypeInfo;
		CWnd* pWnd;
		// Set Result/Publish button...
		switch( rToolType.SubType )
		{
			case SvDef::SVLinearToolObjectType:
				if( pWnd = GetDlgItem( IDC_PUBLISH_BUTTON ) )
				{
					pWnd->EnableWindow( FALSE );
					pWnd->ShowWindow( SW_HIDE );
				}
				break;

		case SvDef::SVWindowToolObjectType:  // fall through...
			default:
				if( pWnd = GetDlgItem( IDC_RESULT_BUTTON ) )
				{
					pWnd->EnableWindow( FALSE );
					pWnd->ShowWindow( SW_HIDE );
				}

				if( !m_pCurrentAnalyzer )
				{
					if( pWnd = GetDlgItem( IDC_PUBLISH_BUTTON ) )
					{
						pWnd->EnableWindow( FALSE );
					}
				}
				break;
		}

		dialogImage.AddTab(_T("Tool Result"));

		// Get the Image for this tool
			setImages();


		// get index of Current Analyzer
		std::string analyzerSelection = "";
		if (m_pCurrentAnalyzer)
		{
			analyzerSelection = m_pCurrentAnalyzer->GetObjectName();
		}

		
		typedef SvCmd::GetCreatableObjects Command;
		typedef std::shared_ptr<Command> CommandPtr;

		SvUl::NameGuidList availableList;
		CommandPtr commandPtr{ new Command(m_TaskObjectID, SvDef::SVObjectTypeInfoStruct(SvDef::SVAnalyzerObjectType, SvDef::SVNotSetSubObjectType)) };
		SVObjectSynchronousCommandTemplate<CommandPtr> cmd(m_InspectionID, commandPtr);
		HRESULT hr = cmd.Execute(TWO_MINUTE_CMD_TIMEOUT);
		if (S_OK == hr)
		{
			availableList = commandPtr->AvailableObjects();
		}

		//This is to add the analyzer due to MIL License restrictions.
		if ( m_pCurrentAnalyzer)
		{
			SvUl::NameGuidList::const_iterator iter = std::find_if(availableList.begin(), availableList.end(), [&](const SvUl::NameGuidPair& rVal)->bool 
			{ 
				return (!rVal.first.empty() && 0 == rVal.first.compare(m_pCurrentAnalyzer->GetObjectName())); 
			} );
			if ( availableList.end() == iter )
			{
				availableList.push_back(SvUl::NameGuidPair(m_pCurrentAnalyzer->GetObjectName(), m_pCurrentAnalyzer->GetClassID()));
				m_additionalAnalyzerId = m_pCurrentAnalyzer->GetClassID();
			}
		}

		m_availableAnalyzerCombobox.Init(availableList, analyzerSelection, _T( "(No Analyzer Available)" ), _T( "(No Analyzer)" ) );

		updateButtons();

		UpdateData( FALSE ); // set data to dialog
	}
	else
		// Not valid call...
		if( GetParent() )
			GetParent()->SendMessage( WM_CLOSE );
		else
			SendMessage( WM_CLOSE );

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX-Eigenschaftenseiten sollten FALSE zur�ckgeben
}

////////////////////////////////////////////////////////////////////////////////
// .Title       : OnButtonDetails
////////////////////////////////////////////////////////////////////////////////
void SVToolAdjustmentDialogAnalyzerPageClass::OnButtonDetails()
{
	if( m_pCurrentAnalyzer )
	{
		// Save GUID
		GUID analyzerGuid = m_pCurrentAnalyzer->GetUniqueObjectID();

		// Show Dialog
		SVSetupDialogManager::Instance().SetupDialog( m_pCurrentAnalyzer->GetClassID(), m_pCurrentAnalyzer->GetUniqueObjectID(), this );

		// Restore the pointer (in case of Cancel)
		m_pCurrentAnalyzer = ( SVAnalyzerClass* )SVObjectManagerClass::Instance().GetObject( analyzerGuid);

		if( nullptr != m_pTool )
		{
			SVObjectClass* pInspection( m_pTool->GetInspection() );
			ASSERT( nullptr != pInspection );
			
			if(nullptr != pInspection )
			{
				SVGUID l_ToolId;

				if( nullptr != m_pTool )
				{
					l_ToolId = m_pTool->GetUniqueObjectID();
				}

				SvPB::InspectionRunOnceRequest requestMessage;
				SvPB::SetGuidInMessage(requestMessage.mutable_inspectionid(), pInspection->GetUniqueObjectID());
				SvPB::SetGuidInMessage(requestMessage.mutable_taskid(), l_ToolId);
				SvCmd::InspectionCommandsSynchronous(pInspection->GetUniqueObjectID(), &requestMessage, nullptr);
			}
		}
	}
	else
	{
		SvStl::MessageMgrStd Msg( SvStl::LogAndDisplay );
		Msg.setMessage( SVMSG_SVO_94_GENERAL_Informational, SvStl::Tid_Error_NoAnalyzerDetails, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_10210 );
	}
}

void SVToolAdjustmentDialogAnalyzerPageClass::OnSelchangeCurrentAnalyzer()
{
	CWaitCursor l_cwcMouse;

	UpdateData( TRUE ); // get data from dialog

	SVGUID classGUID = m_availableAnalyzerCombobox.getSelectedGUID();

	// Check for valid selection
	if( GUID_NULL != classGUID )
	{
		// Check if its the same Analyzer
		if( m_pCurrentAnalyzer && m_pCurrentAnalyzer->GetClassID() != classGUID )
		{
			// if the Tool has an Analyzer - Close it and Delete it
			// Why? because this dialog is currently only used for selection of a single analyzer
			if( m_pCurrentAnalyzer )
			{
				DestroyAnalyzer();
			}
		}

		// if an Analyzer does not exist - create one
		if( !m_pCurrentAnalyzer )
		{
			// and now Instantiate a new Object
			SVObjectManagerClass::Instance().ConstructObject( classGUID, m_pCurrentAnalyzer );

			if( m_pCurrentAnalyzer )
			{
				m_pTool->Add( m_pCurrentAnalyzer );

				// Ensure this Object's inputs get connected
				// Fix to ensure Friends get connections as well
				m_pTool->ConnectAllInputs();

				// And last - Create (initialize) it

				if( ! m_pCurrentAnalyzer->IsCreated() )
				{
					// And finally try to create the child object...
					if( !m_pTool->CreateChildObject(m_pCurrentAnalyzer, SvDef::SVMFSetDefaultInputs | SvDef::SVMFResetInspection ) )
					{
						SvStl::MessageMgrStd Msg( SvStl::LogAndDisplay );
						Msg.setMessage( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_Error_AnalyzerCreationFailed, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_10211 );

						// Remove it from the Tool TaskObjectList ( Destruct it )
						GUID objectID = m_pCurrentAnalyzer->GetUniqueObjectID();
						if( GUID_NULL != objectID )
						{
							m_pTool->Delete( objectID );
						}
						else
						{
							delete m_pCurrentAnalyzer;
						}

						m_pCurrentAnalyzer = nullptr;
					}
				}
			}
			else
			{
				SvStl::MessageMgrStd Msg( SvStl::LogAndDisplay );
				Msg.setMessage( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_Error_AnalyzerInstantiationFailed, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_10212 );
			}
		}
	}
	else // means No Analyzer
	{
		// if we had an Analyzer - delete it
		DestroyAnalyzer();
	}

	if( nullptr != m_pTool )
	{
		SVObjectClass* pInspection( m_pTool->GetInspection() );
		ASSERT(nullptr != pInspection);

		if(nullptr != pInspection )
		{
			SVGUID l_ToolId;

			if( nullptr != m_pTool )
			{
				l_ToolId = m_pTool->GetUniqueObjectID();
			}

			SvPB::InspectionRunOnceRequest requestMessage;
			SvPB::SetGuidInMessage(requestMessage.mutable_inspectionid(), pInspection->GetUniqueObjectID());
			SvPB::SetGuidInMessage(requestMessage.mutable_taskid(), l_ToolId);
			SvCmd::InspectionCommandsSynchronous(pInspection->GetUniqueObjectID(), &requestMessage, nullptr);
		}
	}

	// Update the Buttons
	updateButtons();
}

void SVToolAdjustmentDialogAnalyzerPageClass::DestroyAnalyzer()
{
	if( m_pCurrentAnalyzer )
	{
		//This is to remove the analyzer due to MIL License restrictions.
		if ( GUID_NULL != m_additionalAnalyzerId && m_pCurrentAnalyzer->GetClassID() == m_additionalAnalyzerId )
		{
			m_availableAnalyzerCombobox.remove(m_pCurrentAnalyzer->GetObjectName());
			m_additionalAnalyzerId = GUID_NULL;
		}

		m_pCurrentAnalyzer->DisconnectImages();

		// Close, Disconnect and Delete the Object
		m_pTool->DestroyChildObject(m_pCurrentAnalyzer, SvDef::SVMFSetDefaultInputs | SvDef::SVMFResetInspection );

		m_pCurrentAnalyzer = nullptr;
	}
}

BOOL SVToolAdjustmentDialogAnalyzerPageClass::setImages()
{
	// Get the Image for this tool
	const SVImageInfoClass* pImageInfo = m_pTool->getFirstImageInfo();
	if( pImageInfo )
	{
		SVImageClass* l_pImage = nullptr;

		pImageInfo->GetOwnerImage( l_pImage );

		dialogImage.setImage(l_pImage);
		dialogImage.Refresh();
		return TRUE;
	}
	else
		return FALSE;
}

////////////////////////////////////////////////////////////////////////////////
// updateButtons - Enable/Disable Buttons
////////////////////////////////////////////////////////////////////////////////
void SVToolAdjustmentDialogAnalyzerPageClass::updateButtons()
{
	BOOL state = FALSE;

	if( m_pCurrentAnalyzer )
		state = TRUE;

	CWnd* pWnd;

	if( ( pWnd = GetDlgItem( IDC_DETAILS_BUTTON ) ) )
		pWnd->EnableWindow( state );

	if( ( pWnd = GetDlgItem( IDC_RESULT_BUTTON ) ) )
	{
		if( pWnd->IsWindowVisible() )
			pWnd->EnableWindow( state );
	}

	if( ( pWnd = GetDlgItem( IDC_PUBLISH_BUTTON ) ) )
	{
		if( pWnd->IsWindowVisible() )
			pWnd->EnableWindow( state );
	}
}

void SVToolAdjustmentDialogAnalyzerPageClass::OnResultButton()
{
	if( m_pCurrentAnalyzer )
	{
		// Get Available Results...
		SVClassInfoStructVector	availableResults;
		SvDef::SVObjectTypeInfoStruct		resultTypeInfo;
		resultTypeInfo.ObjectType = SvDef::SVResultObjectType;
		//@TODO[MZA][7.40][14.10.2016] The getAvailableObject method should be replaced by the SvCmd::GetAvailableObjects
		m_pCurrentAnalyzer->getAvailableObjects(&availableResults, &resultTypeInfo);

		// Get Dialog Title...
		std::string Title = SvUl::LoadStdString( IDS_RESULT_ADJUSTMENT_DIALOG_TITLE );
		// Get Complete Name up to the tool level...
		Title = m_pCurrentAnalyzer->GetObjectNameToObjectType() + _T(" ") + Title;

		SVIPDoc* l_pIPDoc = nullptr;

		if( nullptr != m_pCurrentAnalyzer->GetInspection() )
		{
			l_pIPDoc = m_pParentDialog->GetIPDoc();
		}

		// Prepare result adjustment dialog...
		SVChildrenSetupDialogClass dlg;
		dlg.m_pDocument = l_pIPDoc;
		dlg.m_AllowMultipleChildrenInstances = FALSE;
		dlg.m_pParentObject					= m_pCurrentAnalyzer;
		dlg.m_pAvailableChildrenList		= &availableResults;
		dlg.m_Title							= Title;

		// Save Guid
		GUID analyzerGuid = m_pCurrentAnalyzer->GetUniqueObjectID();

		// Call dialog...
		dlg.DoModal();

		// Restore the pointer (in case of Cancel)
		m_pCurrentAnalyzer = ( SVAnalyzerClass* )SVObjectManagerClass::Instance().GetObject( analyzerGuid );
	}
}

void SVToolAdjustmentDialogAnalyzerPageClass::OnPublishButton()
{
	if( nullptr == m_pCurrentAnalyzer || nullptr == m_pTool ) { return; }


	SVInspectionProcess* pInspection = dynamic_cast<SVInspectionProcess*>(m_pTool->GetInspection());
	if( nullptr == pInspection ) { return; }

	SvOsl::ObjectTreeGenerator::Instance().setSelectorType( SvOsl::ObjectTreeGenerator::SelectorTypeEnum::TypeSetAttributes );
	SvOsl::ObjectTreeGenerator::Instance().setLocationFilter( SvOsl::ObjectTreeGenerator::FilterInput, std::string(m_pTool->GetCompleteName()), std::string( _T("") ) );

	SvOsl::SelectorOptions BuildOptions( pInspection->GetUniqueObjectID(), SvDef::SV_PUBLISHABLE, m_pCurrentAnalyzer->GetUniqueObjectID() );
	SvOsl::ObjectTreeGenerator::Instance().BuildSelectableItems<SvOg::NoSelector, SvOg::NoSelector, SvOg::ToolSetItemSelector<>>( BuildOptions );

	std::string PublishableResults = SvUl::LoadStdString( IDS_PUBLISHABLE_RESULTS );
	std::string Title = SvUl::Format( _T("%s - %s"), PublishableResults.c_str(), m_pTool->GetName() );
	std::string Filter = SvUl::LoadStdString( IDS_FILTER );

	INT_PTR Result = SvOsl::ObjectTreeGenerator::Instance().showDialog( Title.c_str(), PublishableResults.c_str(), Filter.c_str(), this );

	if( IDOK == Result )
	{
		SVPublishListClass& PublishList = pInspection->GetPublishList();
		PublishList.Refresh( static_cast<SVTaskObjectClass*>(pInspection->GetToolSet()) );

		SVIPDoc* pIPDoc = m_pParentDialog->GetIPDoc();

		if( nullptr != pIPDoc )
		{
			pIPDoc->SetModifiedFlag();
		}
	}
}

