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
#include "SVOGui/PublishSelector.h"
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
{
	if( m_pParentDialog )
	{
		m_pTool = m_pParentDialog->GetTool();

		if (m_pTool)
		{
			SVObjectTypeInfoStruct info;
			info.ObjectType = SVAnalyzerObjectType;

			SVSendMessage( m_pTool, SVM_GETAVAILABLE_OBJECTS, reinterpret_cast<DWORD_PTR>(&availableAnalyzers), reinterpret_cast<DWORD_PTR>(&info) );

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
	DDX_Control(pDX, IDC_ANALYZER_COMBO, availableAnalyzerListBox);
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
		case SVToolProfileObjectType:  // fall through...
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
		int currentAnalyzerIndex = -1;
		if (m_pCurrentAnalyzer)
		{
			currentAnalyzerIndex = availableAnalyzers.Find( m_pCurrentAnalyzer->GetClassID() );
		}
		oldIndex = availableAnalyzerListBox.init( &availableAnalyzers, currentAnalyzerIndex );

		//This is to add the analyzer due to MIL License restrictions.
		if ( currentAnalyzerIndex == -1 && m_pCurrentAnalyzer)
		{
			availableAnalyzerListBox.add(m_pCurrentAnalyzer->GetObjectName());
		}

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

	int index = ( int ) availableAnalyzerListBox.GetItemData( availableAnalyzerListBox.GetCurSel() );

	// Check for valid selection
	if( index != LB_ERR && index >= 0 && index < availableAnalyzers.GetSize() )
	{
		// Check if its the same Analyzer
		if( m_pCurrentAnalyzer && m_pCurrentAnalyzer->GetClassID() != availableAnalyzers.GetAt( index ).ClassId )
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
			SVObjectManagerClass::Instance().ConstructObject( availableAnalyzers.GetAt( index ).ClassId, m_pCurrentAnalyzer );

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
	else if( index == LB_ERR ) // means No Analyzer
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
		int currentAnalyzerIndex = availableAnalyzers.Find( m_pCurrentAnalyzer->GetClassID() );
		if ( currentAnalyzerIndex == -1 )
		{
			availableAnalyzerListBox.remove(m_pCurrentAnalyzer->GetObjectName());
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
	SvOsl::ObjectTreeGenerator::Instance().setAttributeFilters( SV_PUBLISHABLE );
	SvOsl::ObjectTreeGenerator::Instance().setLocationFilter( SvOsl::ObjectTreeGenerator::FilterInput, m_pTool->GetCompleteObjectName(), SVString( _T("") ) );

	SvOg::PublishSelector(pInspection->GetUniqueObjectID(), m_pCurrentAnalyzer->GetUniqueObjectID());

	CString PublishableResults;
	PublishableResults.LoadString( IDS_PUBLISHABLE_RESULTS );
	SVString Title;
	SVString ToolName( m_pTool->GetName() );
	Title.Format( _T("%s - %s"), PublishableResults, ToolName.c_str() );
	SVString mainTabTitle = PublishableResults;
	CString Filter;
	Filter.LoadString( IDS_FILTER );
	SVString filterTabTitle = Filter;
	INT_PTR Result = SvOsl::ObjectTreeGenerator::Instance().showDialog( Title, mainTabTitle, filterTabTitle, this );

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

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVToolAdjustmentDialogAnalyzerPageClass.cpp_v  $
 * 
 *    Rev 1.6   16 Dec 2014 18:04:26   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  933
 * SCR Title:  Add Filter Tab to Object Selector (SVO-377)
 * Checked in by:  mZiegler;  Marc Ziegler
 * Change Description:  
 *   Changed the OnPublishButton method to specify the title of the Object Selector's Filter Page.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   17 Jul 2014 20:59:58   gramseier
 * Project:  SVObserver
 * Change Request (SCR) nbr:  909
 * SCR Title:  Object Selector replacing Result Picker and Output Selector SVO-72, 40, 130
 * Checked in by:  gRamseier;  Guido Ramseier
 * Change Description:  
 *   Replace ResultPicker Dialog with Object Selector Dialog
 * Code review changes
 * Changed methods: OnPublishButton
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   26 Jun 2014 18:29:24   mziegler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  885
 * SCR Title:  Replace image display in TA-dialogs with activeX SVPictureDisplay
 * Checked in by:  mZiegler;  Marc Ziegler
 * Change Description:  
 *   use SVPictureDisplay-control
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 *
 *    Rev 1.3   15 May 2014 14:36:12   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Revised SVSendMessage to use DWORD_PTR
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   01 Feb 2014 12:18:28   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changed SVSendmessage and processmessage to use LONG_PTR instead of DWORD.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   02 Oct 2013 08:24:40   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Add x64 platform.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 15:33:18   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.10   05 Mar 2013 13:39:42   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  796
 * SCR Title:  Add a License Manager that will handle the Matrox Licenses
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Added DestroyAnalyszer method and changed the InitDialog to check to see if an analyzer was excluded because of MIL License.  It will add the string so the user can see what analyzer it is, but if changed it will remove it from the list
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   01 Aug 2012 12:41:06   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added new command functionality to allow for synchronous command execution through the inspection thread.  This will fix problems with race conditions with the inspection process.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   27 Jul 2012 09:05:10   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fixed include information and updated overlay collection functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   02 Jul 2012 17:48:10   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to promote new display functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   07 Apr 2011 16:41:40   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated SVDlgImageClass to change the method init to UpdateImageInfo to make it trackable throughout the system.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   27 Jan 2011 12:02:58   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix issues with Matrox Buffer Management.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   08 Dec 2010 13:45:12   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to include changes in notification functionality using the Observer Design Pattern.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   16 Dec 2009 13:20:10   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  677
 * SCR Title:  Fix problem in camera notify thread
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fix issues with includes and comments.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   30 Jul 2009 13:09:12   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Merged branched changes into current code branch with appropriate updates.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   11 Dec 2008 10:14:08   jhanebach
 * Project:  SVObserver
 * Change Request (SCR) nbr:  629
 * SCR Title:  Add Histogram analyzer
 * Checked in by:  jHanebach;  Jack Hanebach
 * Change Description:  
 *   Added message for analyzer to clean up the images
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   09 Aug 2005 08:37:08   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated objects and dialogs to use the ActiveX Interface to update inspection data.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.49   28 Jul 2005 13:26:58   ebeyeler
   Project:  SVObserver
   Change Request (SCR) nbr:  482
   SCR Title:  New vector based mask
   Checked in by:  eBeyeler;  Eric Beyeler
   Change Description:  
     make fill options only available with shape masks
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.48   19 Jul 2005 14:49:30   ebeyeler
   Project:  SVObserver
   Change Request (SCR) nbr:  464
   SCR Title:  Add array indexing for value objects
   Checked in by:  eBeyeler;  Eric Beyeler
   Change Description:  
     implemented new value object Get/Set
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.47   19 Jul 2005 10:25:50   ryoho
   Project:  SVObserver
   Change Request (SCR) nbr:  482
   SCR Title:  New vector based mask
   Checked in by:  eBeyeler;  Eric Beyeler
   Change Description:  
     Cosmetic changes for the Mask dialogs
   
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.46   27 Jun 2005 09:54:14   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  497
   SCR Title:  Remove Freeze Set from SVObserver
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Removed Freeze Tool from configurable tools.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.45   22 Jun 2005 12:37:48   ebeyeler
   Project:  SVObserver
   Change Request (SCR) nbr:  464
   SCR Title:  Add array indexing for value objects
   Checked in by:  eBeyeler;  Eric Beyeler
   Change Description:  
     value objects use SVHRESULT
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.44   22 Jun 2005 10:36:20   ebeyeler
   Project:  SVObserver
   Change Request (SCR) nbr:  464
   SCR Title:  Add array indexing for value objects
   Checked in by:  eBeyeler;  Eric Beyeler
   Change Description:  
     object attributes use accessor functions
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.43   19 May 2005 15:02:24   ebeyeler
   Project:  SVObserver
   Change Request (SCR) nbr:  482
   SCR Title:  New vector based mask
   Checked in by:  eBeyeler;  Eric Beyeler
   Change Description:  
     implemented Shape Mask
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.42   18 May 2005 14:10:22   tbair
   Project:  SVObserver
   Change Request (SCR) nbr:  484
   SCR Title:  Display Mask on Source Image
   Checked in by:  tBair;  Tom Bair
   Change Description:  
     Added support for mask draw criteria combo.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.41   09 Mar 2005 13:13:28   tbair
   Project:  SVObserver
   Change Request (SCR) nbr:  478
   SCR Title:  Add a new Perspective Warp Tool
   Checked in by:  tBair;  Tom Bair
   Change Description:  
     Allow the PerspectiveWarp Tool to be added from the add menu.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.40   09 Mar 2005 12:29:06   ebeyeler
   Project:  SVObserver
   Change Request (SCR) nbr:  477
   SCR Title:  Add Cylindrical warp Tool
   Checked in by:  eBeyeler;  Eric Beyeler
   Change Description:  
     added SVToolCylindricalObjectType and #include
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.39   18 Feb 2005 11:38:22   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  262
   SCR Title:  Improve performance when loading a configuration in SVObserver
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Changed method of tracking values objects from full object manager rebuild to a value object registration methodology.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.38   18 Feb 2005 11:10:00   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  463
   SCR Title:  Add new Linear Measurement Tool to SVObserver
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
     Added new linear tool to allowable tools to configure.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.37   18 Feb 2005 07:26:12   tbair
   Project:  SVObserver
   Change Request (SCR) nbr:  456
   SCR Title:  Update Image and Tool Objects to use the new Extent Classes
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Changes to support new Reset methodology.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.36   16 Feb 2005 15:21:30   ryoho
   Project:  SVObserver
   Change Request (SCR) nbr:  440
   SCR Title:  Create Internal Tool Object to Managing Tool Extents and Result Extents
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
     updated header files
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.35   20 Oct 2004 15:15:32   ryoho
   Project:  SVObserver
   Change Request (SCR) nbr:  458
   SCR Title:  Deleting a filter on a tool on certain configuration causes a Dr. Watson
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
     fixed problem with deleting filters.  
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.34   12 Oct 2004 12:58:42   ebeyeler
   Project:  SVObserver
   Change Request (SCR) nbr:  448
   SCR Title:  Fix External Tool Change DLL functionality
   Checked in by:  eBeyeler;  Eric Beyeler
   Change Description:  
     added support for ISVPropertyPageDialog interface
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.33   07 Sep 2004 09:56:02   tbair
   Project:  SVObserver
   Change Request (SCR) nbr:  446
   SCR Title:  Add new filters to SVObserver
   Checked in by:  tBair;  Tom Bair
   Change Description:  
     Add  filters button properties for Watershed, Thinning, and Thickening. OnSelChangeList1() and OnButtonProperties()
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.32   10 Aug 2004 16:18:34   tbair
   Project:  SVObserver
   Change Request (SCR) nbr:  406
   SCR Title:  Implement External Tool
   Checked in by:  tBair;  Tom Bair
   Change Description:  
     Fixed Bug in External Tool where output images still show up in image lists generated by the SVToolAdjustmentDialogImagePageClass.  Added check to uObjectAttributesAllowed not SV_HIDDEN.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.31   21 Jul 2004 09:00:30   ebeyeler
   Project:  SVObserver
   Change Request (SCR) nbr:  406
   SCR Title:  Implement External Tool
   Checked in by:  eBeyeler;  Eric Beyeler
   Change Description:  
     added CREATE_ALL_CLOSED_OBJECTS message to SVToolAdjustmentDialogSheetClass::OnOK()
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.30   12 Jul 2004 12:03:44   ebeyeler
   Project:  SVObserver
   Change Request (SCR) nbr:  406
   SCR Title:  Implement External Tool
   Checked in by:  eBeyeler;  Eric Beyeler
   Change Description:  
     implement External Tool
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.29   04 Nov 2003 14:40:04   rschock
   Project:  SVObserver
   Change Request (SCR) nbr:  394
   SCR Title:  Implement new RankingFilter for WindowTool that will also be remotely settable
   Checked in by:  rSchock;  Rosco Schock
   Change Description:  
     Added new ranking filter class that has a 1-7x1-7 customizable neighborhood.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.28   04 Nov 2003 14:12:14   rschock
   Project:  SVObserver
   Change Request (SCR) nbr:  378
   SCR Title:  Implement new CustomFilter for WindowTools that will also be remotely settable
   Checked in by:  rSchock;  Rosco Schock
   Change Description:  
     Update the custom filter class to be 1-7x1-7 customizable kernel instead of a static 3x3 one.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.27   07 Aug 2003 12:03:48   ryoho
   Project:  SVObserver
   Change Request (SCR) nbr:  380
   SCR Title:  Add Image Mask option
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
     added new methods for the new mask that is on the dialog
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.26   08 Jul 2003 10:15:52   rschock
   Project:  SVObserver
   Change Request (SCR) nbr:  378
   SCR Title:  Implement new CustomFilter for WindowTools that will also be remotely settable
   Checked in by:  rSchock;  Rosco Schock
   Change Description:  
     Added new code and classes for the new custom filters objects and dialogs.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.25   22 Apr 2003 16:47:28   rschock
   Project:  SVObserver
   Change Request (SCR) nbr:  346
   SCR Title:  Update SVObserver to Version 4.21 Release
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.24   31 Jan 2003 08:59:58   joe
   Project:  SVObserver
   Change Request (SCR) nbr:  301
   SCR Title:  Implement Result Image Circle Buffer
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated OnEditMaskButton and OnInitDialog methods to use the new image circle buffer methodology.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.23   20 Nov 2002 13:59:22   ryoho
   Project:  SVObserver
   Change Request (SCR) nbr:  226
   SCR Title:  Monochrome SVIM configuration compatibility between ViperQUAD and ViperDUAL
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
     theSVObjectManager
    ValueObject Data Indexes
    Removed old Automation
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.22   30 Jan 2002 16:15:28   rschock
   Project:  SVObserver
   Change Request (SCR) nbr:  278
   SCR Title:  Error message is displayed when removing result objects used by other tools
   Checked in by:  rSchock;  Rosco Schock
   Change Description:  
     Modified ::refresh and ::initMask and ::initThreshold to validate the toolset before blindly running it.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.21   03 Oct 2001 08:43:20   ed
   Project:  SVObserver
   Change Request (SCR) nbr:  225
   SCR Title:  Color SVIM, Incorrect default image for tools
   Checked in by:  Ed;  Ed Chobanoff
   Change Description:  
     Modified function, 
   SVToolAdjustmentDialogImagePageClass::OnInitDialog().
   
   
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.20   13 Jun 2001 10:24:28   ed
   Project:  SVObserver
   Change Request (SCR) nbr:  200
   SCR Title:  Update code to implement SVIM COM Server
   Checked in by:  Ed;  Ed Chobanoff
   Change Description:  
     Revised file header.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.19   19 Apr 2001 20:24:52   Steve
   Project:  SVObserver
   Change Request (SCR) nbr:  196
   SCR Title:  Restructure Scripted Load/Save Procedures Functional Requirement
   Checked in by:  Steve;  Stephen E. Steffan
   Change Description:  
     Header file changes due to include file restructuring.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.18   06 Feb 2001 13:47:52   cSchmittinger
   Project:  SVObserver
   Change Request (SCR) nbr:  191
   SCR Title:  Restructure File Management
   Checked in by:  cSchmittinger;  Carl Schmittinger
   Change Description:  
     Modified source code to use new file management classes.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.17   01 Feb 2001 11:55:16   Steve
   Project:  SVObserver
   Change Request (SCR) nbr:  191
   SCR Title:  Restructure File Management
   Checked in by:  Steve;  Stephen S. Steffan
   Change Description:  
     Modified files to use the SVFileNameClass and the SVFileNameManagerClass if applicable, if the object refers to a filename.  The SVFileNameManagerClass is used if the Object is a persistant file object, that is, if the file object is saved as part of the configuration data.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.16.1.0   16 May 2001 14:12:06   rschock
   Project:  SVObserver
   Change Request (SCR) nbr:  202
   SCR Title:  Operator, delete and cut should not be available in File Open/Save Dialogs
   Checked in by:  rSchock;  Rosco Schock
   Change Description:  
     Changed all occurences of CFileDialog to CSVFileDialog to enable the new security features and privileges
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.16   24 Oct 2000 15:51:36   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  187
   SCR Title:  Color SVIM Problem when Adding Monochrome Tools
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     These changes remove the ability to add a monochrome tool to the tool set until a color tool gets added.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.15   06 Sep 2000 16:37:12   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  179
   SCR Title:  Create Color SVObserver
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated files to include color SVIM changes.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.14   14 Jun 2000 16:22:16   sjones
   Project:  SVObserver
   Change Request (SCR) nbr:  159
   SCR Title:  Incorrect placement of Profile Tool
   Checked in by:  sJones;  Steve Jones
   Change Description:  
     Revised OnSelchangedCombo1 to Send the message
   SVM_IMAGE_SOURCE_CHANGED to the tool.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.13   19 May 2000 16:06:00   sjones
   Project:  SVObserver
   Change Request (SCR) nbr:  145
   SCR Title:  Half field images and rotation
   Checked in by:  sJones;  Steve Jones
   Change Description:  
     Moved SVToolAdjustmentDialogTwoImagePageClass to
   it's own header and implementation file (
   SVToolAdjustmentDialogTwoImagePage.h and 
   SVToolAdjustmentDialogTwoImagePage.cpp
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.12   26 Apr 2000 14:57:28   sjones
   Project:  SVObserver
   Change Request (SCR) nbr:  129
   SCR Title:  Angular Profile Tool
   Checked in by:  sJones;  Steve Jones
   Change Description:  
     Revised OnSelchangeCurrentAnalyzer to Send 
   SVM_CONNECT_ALL_INPUTS message to the Tool, so
   the friends get their connections as well.
   Revised destructor for 
   SVToolAdjustmentDialogSpecialPropertiesPageClass
   to delete propertyArray.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.11   23 Mar 2000 16:48:38   sjones
   Project:  SVObserver
   Change Request (SCR) nbr:  118
   SCR Title:  Integration of LUT Operator
   Checked in by:  sJones;  Steve Jones
   Change Description:  
     Added new LUT Dialog page.
   Revised to work with new In Place Image Operator List.
   Revised to run Tool instead or OperatorList.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.10   03 Mar 2000 16:03:20   sjones
   Project:  SVObserver
   Change Request (SCR) nbr:  113
   SCR Title:  Integrate new Polar Unwrap Tool
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
     Revised addPages(): Polar Transform (Polar Unwarp) Tool
   now has a standard image source page.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.9   Mar 01 2000 13:05:46   Nick
   Project:  SVObserver
   Change Request (SCR) nbr:  113
   SCR Title:  Integrate new Polar Unwrap Tool
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
     Revised to support new polar transform tool ( Polar Unwrap ) and
   his new SVTADlgPolarTransformPage.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.8   29 Feb 2000 14:28:36   sjones
   Project:  SVObserver
   Change Request (SCR) nbr:  106
   SCR Title:  Math Tool - List processing
   Checked in by:  sJones;  Steve Jones
   Change Description:  
     Removed Close Button on caption.
   Added Handler for WM_SYSCOMMAND to restrict SC_CLOSE message.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.7   Feb 24 2000 12:06:38   Nick
   Project:  SVObserver
   Change Request (SCR) nbr:  111
   SCR Title:  Integrate Conditional Tool Set/Tool Drawing
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
     Added new general page.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.6   21 Feb 2000 15:22:16   sjones
   Project:  SVObserver
   Change Request (SCR) nbr:  104
   SCR Title:  New Build Reference Tool
   Checked in by:  sJones;  Steve Jones
   Change Description:  
     Revised 	SVToolAdjustmentDialogImagePageClass
   constructor to use the id as an argument.
   Added include files for
   SVToolImage.h
   SVTransformationTool.h
   SVTADlgTransformImagePage.h
   SVTADlgTranslationPage.h
   SVTADlgRotationPage.h
   SVTADlgTransformationLearnPage.h
   
   Revised SVToolAdjustmentDialogSheetClass::addPages()
   to add property pages for editing the Transformation Tool.
   
   
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.5   Feb 21 2000 10:53:02   Nick
   Project:  SVObserver
   Change Request (SCR) nbr:  109
   SCR Title:  Freeze on Reject for v3.1
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
     Introduced new freeze page and freeze tool.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.4   07 Feb 2000 22:51:10   robert
   Project:  SVObserver
   Change Request (SCR) nbr:  93
   SCR Title:  Integrate Multiple Camera Image Input per IPD
   Checked in by:  Robert;  Robert Massinger
   Change Description:  
     Changed to handle new Page of the Acq. Tool.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.3   Jan 14 2000 15:48:02   Nick
   Project:  SVObserver
   Change Request (SCR) nbr:  85
   SCR Title:  Version 3.04 Beta 1 versioning changes.
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
     Fix memory leak for PMaskEditorCtrl pointer.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.2   Jan 03 2000 14:41:00   Nick
   Project:  SVObserver
   Change Request (SCR) nbr:  73
   SCR Title:  Mask Editor Mil 5.0 to Mil 6.0 repairs.
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
     Changes so bitmap image is created when necessary.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.1   Nov 30 1999 15:36:18   Nick
   Project:  SVObserver
   Change Request (SCR) nbr:  65
   SCR Title:  Remove obsolute class SVToolSetViewClass
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
     Removed reference to SVToolSetViewClass.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.0   23 Nov 1999 13:46:08   mike
   Project:  SVObserver
   Change Request (SCR) nbr:  61
   SCR Title:  Update PVCS versioning to version 3.0.
   Checked in by:  Mike;  Mike McCarl
   Change Description:  
     
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.40   Nov 17 1999 15:06:30   Nick
   Project:  SVObserver
   Change Request (SCR) nbr:  4
   SCR Title:  Add "Load image" option
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
     RO change for image arithmetic.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.39   Nov 17 1999 07:25:18   Nick
   Project:  SVObserver
   Change Request (SCR) nbr:  15
   SCR Title:  Integrate archive tool
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
     Added check for unique results archive file paths.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.38   Nov 10 1999 12:38:40   Nick
   Project:  SVObserver
   Change Request (SCR) nbr:  57
   SCR Title:  Version 3.00 Beta 18 Versioning
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
     Documents marked as changed when Dialog OK buttons pressed.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.37   Nov 09 1999 10:47:24   Nick
   Project:  SVObserver
   Change Request (SCR) nbr:  21
   SCR Title:  Port Gage Tool code from program version 2.42.
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
     Remove Gage Tool Image possible image selection for tools.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.36   08 Nov 1999 10:58:22   sjones
   Project:  SVObserver
   Change Request (SCR) nbr:  16
   SCR Title:  Integrate Math Tool
   Checked in by:  sJones;  Steve Jones
   Change Description:  
     Revised to use IDS_INVALID_FORMULA (string table).
   Revised to use IDS_INVALID_CONDITION_FORMULA 
   (string table).
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.35   Oct 28 1999 11:31:48   Nick
   Project:  SVObserver
   Change Request (SCR) nbr:  52
   SCR Title:  Version 3.00 Beta 14 Versioning
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
     Tool/Tool set conditional dialog adjustment.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.34   19 Oct 1999 17:42:42   sjones
   Project:  SVObserver
   Change Request (SCR) nbr:  48
   SCR Title:  Threshold Page Formula
   Checked in by:  sJones;  Steve Jones
   Change Description:  
     Moved SVTADlgThresholdPageClass to it's own file(s)
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.33   14 Oct 1999 12:06:16   sjones
   Project:  SVObserver
   Change Request (SCR) nbr:  32
   SCR Title:  Port Blob Analyzer from v2.42
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
     Revised not to recreate the same analyzer when picked
   from the dropdown list of available analyzers
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.32   08 Oct 1999 16:29:48   sjones
   Project:  SVObserver
   Change Request (SCR) nbr:  16
   SCR Title:  Integrate Math Tool
   Checked in by:  sJones;  Steve Jones
   Change Description:  
     Corrected problem in SVToolAdjustmentDialogAnalyzerPageClass
   which would crash when no analyzer selected and the
   publish button was pressed.
   Added call to SVToolAdjustmentDialogPassFailPageClass::
   OnOK()
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.31   08 Oct 1999 10:58:50   sjones
   Project:  SVObserver
   Change Request (SCR) nbr:  9
   SCR Title:  Integr. a common object interface to handle and to identify obj. in-/outputs
   Checked in by:  sJones;  Steve Jones
   Change Description:  
     Added Call to SVIPDoc::SetDefaultInputs() after sending
   SVM_DESTROY_CHILD_OBJECT message to refresh
   dependencies.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.30   07 Oct 1999 17:48:38   sjones
   Project:  SVObserver
   Change Request (SCR) nbr:  39
   SCR Title:  Integrate Statistics Tool
   Checked in by:  sJones;  Steve Jones
   Change Description:  
     Added Call to SVToolAdjustmentStatisticsPageClass::OnOk()
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.29   05 Oct 1999 13:15:48   sjones
   Project:  SVObserver
   Change Request (SCR) nbr:  39
   SCR Title:  Integrate Statistics Tool
   Checked in by:  sJones;  Steve Jones
   Change Description:  
     Added entry in AddPages for SVStatisticsToolClass
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.28   Oct 04 1999 10:04:10   Nick
   Project:  SVObserver
   Change Request (SCR) nbr:  4
   SCR Title:  Add "Load image" option
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
     Image source fix for profile and others.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.27   29 Sep 1999 13:14:50   sjones
   Project:  SVObserver
   Change Request (SCR) nbr:  9
   SCR Title:  Integr. a common object interface to handle and to identify obj. in-/outputs
   Checked in by:  sJones;  Steve Jones
   Change Description:  
     Disabled and Hid IDCANCEL button.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.25   24 Sep 1999 19:04:32   sjones
   Project:  SVObserver
   Change Request (SCR) nbr:  9
   SCR Title:  Integr. a common object interface to handle and to identify obj. in-/outputs
   Checked in by:  sJones;  Steve Jones
   Change Description:  
     Removed call to RemoveExternalDependencies
   (Fix to restore inputs via script)
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.24   Sep 24 1999 13:58:48   Nick
   Project:  SVObserver
   Change Request (SCR) nbr:  4
   SCR Title:  Add "Load image" option
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
     Mask Editor OCX access is corrected.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.23   Sep 24 1999 10:39:06   Nick
   Project:  SVObserver
   Change Request (SCR) nbr:  4
   SCR Title:  Add "Load image" option
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
     Load Image Tool changes.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.22   Sep 23 1999 13:59:26   Nick
   Project:  SVObserver
   Change Request (SCR) nbr:  37
   SCR Title:  Version 3.00 Beta 3 Versioning
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
     Bug fixes in threshold edit control code for threshold values.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.21   Sep 22 1999 18:12:58   Nick
   Project:  SVObserver
   Change Request (SCR) nbr:  33
   SCR Title:  Port Profile Tool from v2.42
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
     Added a SVDoubleValue for auto thresholding value on Threshold page.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.20   Sep 20 1999 15:35:26   Nick
   Project:  SVObserver
   Change Request (SCR) nbr:  35
   SCR Title:  PPQ mode to wait for input data
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
     Image source Page Bug fix..
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.19   20 Sep 1999 11:01:14   sjones
   Project:  SVObserver
   Change Request (SCR) nbr:  9
   SCR Title:  Integr. a common object interface to handle and to identify obj. in-/outputs
   Checked in by:  sJones;  Steve Jones
   Change Description:  
     Reincorporate changes from version 1.16.
   Corrected bug - restore pointer to current analyzer after 
   calling setupDialog and Result Dialog
   
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.18   Sep 20 1999 09:51:14   Nick
   Project:  SVObserver
   Change Request (SCR) nbr:  35
   SCR Title:  PPQ mode to wait for input data
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
     Fixes in image source page.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.17   Sep 15 1999 17:24:00   Nick
   Project:  SVObserver
   Change Request (SCR) nbr:  15
   SCR Title:  Integrate archive tool
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
     Added 'page' for Archive tool adjustments.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.16   15 Sep 1999 17:11:34   sjones
   Project:  SVObserver
   Change Request (SCR) nbr:  9
   SCR Title:  Integr. a common object interface to handle and to identify obj. in-/outputs
   Checked in by:  sJones;  Steve Jones
   Change Description:  
     Revised for SVUserMaskOperatorClass Embedded Objects.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.15   Sep 14 1999 17:11:14   Nick
   Project:  SVObserver
   Change Request (SCR) nbr:  34
   SCR Title:  Add Build Reference Tool
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
     Added Build Reference Page.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.14   14 Sep 1999 17:06:28   sjones
   Project:  SVObserver
   Change Request (SCR) nbr:  16
   SCR Title:  Integrate Math Tool
   Checked in by:  sJones;  Steve Jones
   Change Description:  
     Added equation validation in OnOk method
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.13   14 Sep 1999 13:21:24   sjones
   Project:  SVObserver
   Change Request (SCR) nbr:  9
   SCR Title:  Integr. a common object interface to handle and to identify obj. in-/outputs
   Checked in by:  sJones;  Steve Jones
   Change Description:  
     Revised to set pointer to SVIPDoc in SVChildrenSetupDialog
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.12   Sep 13 1999 15:22:56   Nick
   Project:  SVObserver
   Change Request (SCR) nbr:  34
   SCR Title:  Add Build Reference Tool
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
     Remove unnececessary DDX_Control line for dialogImage.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.11   Sep 13 1999 14:01:10   Nick
   Project:  SVObserver
   Change Request (SCR) nbr:  33
   SCR Title:  Port Profile Tool from v2.42
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
     Ported TwoImagePage
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.10   11 Sep 1999 19:54:04   sjones
   Project:  SVObserver
   Change Request (SCR) nbr:  16
   SCR Title:  Integrate Math Tool
   Checked in by:  sJones;  Steve Jones
   Change Description:  
     Removed old math tool adjust dialogs includes.
   Added include for SVFormulaEditorPage.
   Added Conditional dialog page to all tools.
   Revised Dialog pages for the MathTool.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.9   Sep 10 1999 18:46:16   Nick
   Project:  SVObserver
   Change Request (SCR) nbr:  33
   SCR Title:  Port Profile Tool from v2.42
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
     Added profile pages.
   Modified Analyzer page to support ObjectTypes.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.8   08 Sep 1999 09:20:44   sjones
   Project:  SVObserver
   Change Request (SCR) nbr:  9
   SCR Title:  Integr. a common object interface to handle and to identify obj. in-/outputs
   Checked in by:  sJones;  Steve Jones
   Change Description:  
     Removed include of SVPublishResultDialog.
   Revised OnPublishButton to refresh Published List.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.7   07 Sep 1999 08:21:32   sjones
   Project:  SVObserver
   Change Request (SCR) nbr:  9
   SCR Title:  Integr. a common object interface to handle and to identify obj. in-/outputs
   Checked in by:  sJones;  Steve Jones
   Change Description:  
     Revised all calls to Run() to new Run(RunStatus)
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.6   30 Aug 1999 20:11:58   jim
   Project:  SVObserver
   Change Request (SCR) nbr:  14
   SCR Title:  Integrate luminance analyzer into window tool.
   Checked in by:  Jim;  James A. Brown
   Change Description:  
     Integrating Pixel Analyzer;
   Modified OnPublishButton() to use new result picker.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.5   28 Aug 1999 14:27:56   robert
   Project:  SVObserver
   Change Request (SCR) nbr:  21
   SCR Title:  Port Gage Tool code from program version 2.42.
   Checked in by:  Robert;  Robert Massinger
   Change Description:  
     Replaced DoDetailDialog() call with SetupDialog() call.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.4   25 Aug 1999 12:21:16   robert
   Project:  SVObserver
   Change Request (SCR) nbr:  21
   SCR Title:  Port Gage Tool code from program version 2.42.
   Checked in by:  Robert Massinger
   Change Description:  
     Removed old Multiple Analyzer Page.
   Adapted new object type enumerations into SVToolAdjustmentDialogSheet::addPage().
   
   /////////////////////////////////////////////////////////////////////////////////////
*/