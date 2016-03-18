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
#include "GuiCommands/InspectionRunOnce.h"

#include "SVAnalyzer.h"
#include "SVChildrenSetupDialog.h"
#include "SVGlobal.h"
#include "SVIPDoc.h"
#include "SVInspectionProcess.h"
#include "SVTool.h"
#include "SVToolAdjustmentDialogSheetClass.h"
#include "SVToolSet.h"
#include "SVSetupDialogManager.h"
#include "ObjectSelectorLibrary/ObjectTreeGenerator.h"
#include "SVOGui/NoSelector.h"
#include "SVOGui/ToolSetItemSelector.h"
#include "GuiCommands/GetCreatableObjects.h"
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
, m_additionalAnalyzerId(SVInvalidGUID)
{
	if( m_pParentDialog )
	{
		m_pTool = m_pParentDialog->GetTool();

		if (m_pTool)
		{
			SVObjectTypeInfoStruct info;
			info.ObjectType = SVAnalyzerObjectType;

			m_pCurrentAnalyzer = reinterpret_cast<SVAnalyzerClass *>(SVSendMessage( m_pTool, SVM_GETFIRST_OBJECT, NULL, reinterpret_cast<DWORD_PTR>(&info) ));
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
		const SVObjectTypeInfoStruct& rToolType = m_pTool->GetObjectInfo().ObjectTypeInfo;
		CWnd* pWnd;
		// Set Result/Publish button...
		switch( rToolType.SubType )
		{
			case SVLinearToolObjectType:
				if( pWnd = GetDlgItem( IDC_PUBLISH_BUTTON ) )
				{
					pWnd->EnableWindow( FALSE );
					pWnd->ShowWindow( SW_HIDE );
				}
				break;

		case SVWindowToolObjectType:  // fall through...
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
		SVString analyzerSelection = "";
		if (m_pCurrentAnalyzer)
		{
			analyzerSelection = m_pCurrentAnalyzer->GetObjectName();
		}

		
		typedef GuiCmd::GetCreatableObjects Command;
		typedef SVSharedPtr<Command> CommandPtr;

		SvUl::NameGuidList availableList;
		CommandPtr commandPtr = new Command(m_TaskObjectID, SVObjectTypeInfoStruct(SVAnalyzerObjectType, SVNotSetSubObjectType));
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
	              // EXCEPTION: OCX-Eigenschaftenseiten sollten FALSE zurückgeben
}

////////////////////////////////////////////////////////////////////////////////
// .Title       : OnButtonDetails
////////////////////////////////////////////////////////////////////////////////
// .History
//	 Date		Author		Comment
//  :27.05.1997 RO			First Implementation
//	:28.08.1999 RO			Replaced DoDetailDialog() with SetupDialog().
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

		if( m_pTool != NULL )
		{
			SVInspectionProcess* pInspection( m_pTool->GetInspection() );
			ASSERT( nullptr != pInspection );
			
			if(nullptr != pInspection )
			{
				SVGUID l_ToolId;

				if( m_pTool != NULL )
				{
					l_ToolId = m_pTool->GetUniqueObjectID();
				}

				GuiCmd::InspectionRunOncePtr l_CommandPtr = new GuiCmd::InspectionRunOnce( pInspection->GetUniqueObjectID(), l_ToolId );
				SVObjectSynchronousCommandTemplate< GuiCmd::InspectionRunOncePtr > l_Command( pInspection->GetUniqueObjectID(), l_CommandPtr );

				l_Command.Execute( TWO_MINUTE_CMD_TIMEOUT );
			}
		}
	}
	else
		AfxMessageBox( IDS_USER_INFORMATION_NO_ANALYZER_DETAILS, MB_ICONINFORMATION );
}

void SVToolAdjustmentDialogAnalyzerPageClass::OnSelchangeCurrentAnalyzer()
{
	CWaitCursor l_cwcMouse;

	UpdateData( TRUE ); // get data from dialog

	SVGUID classGUID = m_availableAnalyzerCombobox.getSelectedGUID();

	// Check for valid selection
	if( SVInvalidGUID != classGUID )
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
				//::SVSendMessage( pCurrentAnalyzer, SVM_CONNECT_ALL_INPUTS, NULL, NULL );

				// Fix to ensure Friends get connections as well
				::SVSendMessage( m_pTool, SVM_CONNECT_ALL_INPUTS, NULL, NULL );

				// And last - Create (initialize) it

				if( ! m_pCurrentAnalyzer->IsCreated() )
				{
					// SEJ
					// And finally try to create the child object...
					if( ::SVSendMessage( m_pTool, SVM_CREATE_CHILD_OBJECT, reinterpret_cast<DWORD_PTR>(m_pCurrentAnalyzer), SVMFSetDefaultInputs | SVMFResetInspection ) != SVMR_SUCCESS )
					{
						AfxMessageBox("Creation of Analyzer Failed");

						// What should we really do here ??? SEJ

						// Remove it from the Tool TaskObjectList ( Destruct it )
						GUID objectID = m_pCurrentAnalyzer->GetUniqueObjectID();
						if( objectID != SVInvalidGUID )
							m_pTool->Delete( objectID );
						else
							delete m_pCurrentAnalyzer;

						m_pCurrentAnalyzer = NULL;
					}
				}
			}
			else
			{
				AfxMessageBox("Instantiation of Analyzer Failed");
			}
		}
	}
	else // means No Analyzer
	{
		// if we had an Analyzer - delete it
		DestroyAnalyzer();
	}

	if( m_pTool != NULL )
	{
		SVInspectionProcess* pInspection( m_pTool->GetInspection() );
		ASSERT(nullptr != pInspection);

		if(nullptr != pInspection )
		{
			SVGUID l_ToolId;

			if( m_pTool != NULL )
			{
				l_ToolId = m_pTool->GetUniqueObjectID();
			}

			GuiCmd::InspectionRunOncePtr l_CommandPtr = new GuiCmd::InspectionRunOnce( pInspection->GetUniqueObjectID(), l_ToolId );
			SVObjectSynchronousCommandTemplate< GuiCmd::InspectionRunOncePtr > l_Command( pInspection->GetUniqueObjectID(), l_CommandPtr );

			l_Command.Execute( TWO_MINUTE_CMD_TIMEOUT );
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
		if ( SVInvalidGUID != m_additionalAnalyzerId && m_pCurrentAnalyzer->GetClassID() == m_additionalAnalyzerId )
		{
			m_availableAnalyzerCombobox.remove(m_pCurrentAnalyzer->GetObjectName());
			m_additionalAnalyzerId = SVInvalidGUID;
		}

		::SVSendMessage( m_pTool, SVMSGID_DISCONNECT_IMAGE_OBJECT, reinterpret_cast<DWORD_PTR>(m_pCurrentAnalyzer), NULL );
		// Close, Disconnect and Delete the Object
		::SVSendMessage( m_pTool, SVM_DESTROY_CHILD_OBJECT, reinterpret_cast<DWORD_PTR>(m_pCurrentAnalyzer), SVMFSetDefaultInputs | SVMFResetInspection );

		m_pCurrentAnalyzer = NULL;
	}
}

BOOL SVToolAdjustmentDialogAnalyzerPageClass::setImages()
{
	// Get the Image for this tool
	SVImageInfoClass* pImageInfo = reinterpret_cast<SVImageInfoClass*>( ::SVSendMessage( m_pTool, SVM_GETFIRST_IMAGE_INFO, NULL, NULL ));
	if( pImageInfo )
	{
		SVImageClass* l_pImage = NULL;

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
		SVClassInfoStructListClass	availableResults;
		SVObjectTypeInfoStruct		resultTypeInfo;
		resultTypeInfo.ObjectType = SVResultObjectType;
		::SVSendMessage( m_pCurrentAnalyzer, SVM_GETAVAILABLE_OBJECTS, reinterpret_cast<DWORD_PTR>(&availableResults), reinterpret_cast<DWORD_PTR>(&resultTypeInfo) );

		// Get Dialog Title...
		CString strTitle;
		strTitle.LoadString( IDS_RESULT_ADJUSTMENT_DIALOG_TITLE );
		// Get Complete Name up to the tool level...
		strTitle = m_pCurrentAnalyzer->GetCompleteObjectNameToObjectType( NULL, SVToolSetObjectType ) + SV_TSTR_SPACE + strTitle;

		SVIPDoc* l_pIPDoc = NULL;

		if( m_pCurrentAnalyzer->GetInspection() != NULL )
		{
			l_pIPDoc = SVObjectManagerClass::Instance().GetIPDoc( m_pCurrentAnalyzer->GetInspection()->GetUniqueObjectID() );
		}

		// Prepare result adjustment dialog...
		SVChildrenSetupDialogClass dlg;
		dlg.m_pDocument = l_pIPDoc;
		dlg.m_AllowMultipleChildrenInstances = FALSE;
		dlg.m_pParentObject					= m_pCurrentAnalyzer;
		dlg.m_pAvailableChildrenList		= &availableResults;
		dlg.StrTitle						= strTitle;

		// Save Guid
		GUID analyzerGuid = m_pCurrentAnalyzer->GetUniqueObjectID();

		// Call dialog...
		dlg.DoModal();

		// if dialog was cancelled - get current analyzer pointer...
			//pCurrentAnalyzer = ( SVAnalyzerClass* ) dlg.PParentObject;

		// Restore the pointer (in case of Cancel)
		m_pCurrentAnalyzer = ( SVAnalyzerClass* )SVObjectManagerClass::Instance().GetObject( analyzerGuid );
	}
}

void SVToolAdjustmentDialogAnalyzerPageClass::OnPublishButton()
{
	if( nullptr == m_pCurrentAnalyzer || nullptr == m_pTool ) { return; }

	msvError.ClearLastErrorCd();

	SVInspectionProcess* pInspection = m_pTool->GetInspection();
	if( nullptr == pInspection ) { return; }

	SvOsl::ObjectTreeGenerator::Instance().setSelectorType( SvOsl::ObjectTreeGenerator::SelectorTypeEnum::TypeSetAttributes );
	SvOsl::ObjectTreeGenerator::Instance().setLocationFilter( SvOsl::ObjectTreeGenerator::FilterInput, SVString(m_pTool->GetCompleteObjectName()), SVString( _T("") ) );

	SvOsl::SelectorOptions BuildOptions( pInspection->GetUniqueObjectID(), SV_PUBLISHABLE, m_pCurrentAnalyzer->GetUniqueObjectID() );
	SvOsl::ObjectTreeGenerator::Instance().BuildSelectableItems<SvOg::NoSelector, SvOg::NoSelector, SvOg::ToolSetItemSelector<>>( BuildOptions );

	CString Title;
	CString PublishableResults;
	CString Filter;
	PublishableResults.LoadString( IDS_PUBLISHABLE_RESULTS );
	Title.Format( _T("%s - %s"), PublishableResults, m_pTool->GetName() );
	Filter.LoadString( IDS_FILTER );

	INT_PTR Result = SvOsl::ObjectTreeGenerator::Instance().showDialog( Title, PublishableResults, Filter, this );

	if( IDOK == Result )
	{
		SVPublishListClass& PublishList = pInspection->GetPublishList();
		PublishList.Refresh( static_cast<SVTaskObjectClass*>(pInspection->GetToolSet()) );

		SVIPDoc* pIPDoc = SVObjectManagerClass::Instance().GetIPDoc( pInspection->GetUniqueObjectID() );

		if( nullptr != pIPDoc )
		{
			pIPDoc->SetModifiedFlag();
		}
	}
}

