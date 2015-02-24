//******************************************************************************
//* COPYRIGHT (c) 2004 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVExternalToolImageSelectPage
//* .File Name       : $Workfile:   SVExternalToolImageSelectPage.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.4  $
//* .Check In Date   : $Date:   26 Jun 2014 17:30:38  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVExternalToolImageSelectPage.h"
#include "SVObjectLibrary/SVObjectManagerClass.h"
#include "SVObjectLibrary/SVObjectSynchronousCommandTemplate.h"
#include "SVExternalTool.h"
#include "SVExternalToolDetailsSheet.h"
#include "SVExternalToolTask.h"
#include "SVGageTool.h"
#include "SVGetObjectDequeByTypeVisitor.h"
#include "SVGlobal.h"
#include "SVInspectionProcess.h"
#include "SVIPDoc.h"
#include "svobserver.h"
#include "SVToolSet.h"
#include "SVTool.h"
#include "SVCommandInspectionRunOnce.h"
#pragma endregion Includes

#pragma region Declarations
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BEGIN_MESSAGE_MAP(SVExternalToolImageSelectPage, CPropertyPage)
	//{{AFX_MSG_MAP(SVExternalToolImageSelectPage)
	ON_NOTIFY(PTN_ITEMCHANGED, IDC_IMAGE_LIST, OnItemChanged)
	ON_NOTIFY(PTN_PROPCLICK, IDC_IMAGE_LIST, OnPropClick)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
#pragma endregion Declarations

#pragma region Constructor
SVExternalToolImageSelectPage::SVExternalToolImageSelectPage( SVExternalToolDetailsSheet* pParent, int id )
: CPropertyPage(id)
, m_pParentDialog( pParent )
{
	ASSERT( m_pParentDialog );

	m_pTool = m_pParentDialog->m_pTool;
	m_pTask = m_pParentDialog->m_pTask;

	for ( int i = 0; i < SVExternalToolTaskData::NUM_INPUT_IMAGES; i++ )
	{
		m_pInputImageInfo[i] = nullptr;
	}
}

SVExternalToolImageSelectPage::~SVExternalToolImageSelectPage()
{
}
#pragma endregion Constructor

#pragma region Protected Methods
void SVExternalToolImageSelectPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(SVExternalToolImageSelectPage)
	DDX_Control(pDX, IDC_IMAGE_DISPLAY, m_ImageDisplay);
	//}}AFX_DATA_MAP
}

BOOL SVExternalToolImageSelectPage::OnInitDialog()
{
	CPropertyPage::OnInitDialog();
	SVIPDoc* l_pIPDoc = m_pParentDialog->GetIPDoc();

	if( nullptr != l_pIPDoc )
	{
		SVToolSetClass* pToolSet = dynamic_cast <SVToolSetClass*> ( m_pTool->GetAncestor( SVToolSetObjectType ) );

		if( nullptr != pToolSet && pToolSet->getCurrentImage() && m_pTask->m_Data.m_lNumInputImages > 0 )
		{
			GetDlgItem(IDC_NO_IMAGES_TXT)->ShowWindow( SW_HIDE );
			GetDlgItem(IDC_IMAGE_LIST)->ShowWindow( SW_SHOW );

			// copy pointers to input image info
			for ( int i = 0; i < m_pTask->m_Data.m_lNumInputImages; i++ )
			{
				m_pInputImageInfo[i] = &m_pTask->m_Data.m_aInputImageInfo[ i ];
			}

			SVGUID toolsetGUID = pToolSet->GetUniqueObjectID();
			createAvailableImageList(toolsetGUID, l_pIPDoc);
			buildPropertyTree();
			setImages();
			UpdateData( FALSE ); // set data to dialog
		}// end if( pToolSet &&	pToolSet->getCurrentImage() )
		else
		{
			m_ImageDisplay.ShowWindow( SW_HIDE );
			GetDlgItem(IDC_NO_IMAGES_TXT)->ShowWindow( SW_SHOW );
			GetDlgItem(IDC_IMAGE_LIST)->ShowWindow( SW_HIDE );
		}
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void SVExternalToolImageSelectPage::OnItemChanged(NMHDR* pNotifyStruct, LRESULT* plResult)
{
	UpdateData( TRUE ); // get data from dialog
	LPNMPROPTREE pNMPropTree = reinterpret_cast< LPNMPROPTREE >( pNotifyStruct );
	*plResult = S_FALSE;

	if ( nullptr != pNMPropTree->pItem )
	{
		SVRPropertyItemCombo* pItem = dynamic_cast<SVRPropertyItemCombo*>(pNMPropTree->pItem);
		if ( nullptr != pItem && pItem->GetCtrlID() >= 0 && static_cast<int>(pItem->GetCtrlID()) < m_pTask->m_Data.m_lNumInputImages )
		{
			long comboData = -1;
			pItem->GetItemValue(comboData);
			if (comboData >= 0 && comboData< static_cast<int>(m_AvailableImages.size()) && nullptr != m_AvailableImages[comboData])
			{
				SVImageClass* image = m_AvailableImages[comboData];
				SVInObjectInfoStruct& rInfo = *(m_pInputImageInfo[pItem->GetCtrlID()]);
				// disconnect old selected image
				if ( rInfo.GetInputObjectInfo().PObject )
				{	// disconnect existing connection
					BOOL bSuccess = (::SVSendMessage( rInfo.GetInputObjectInfo().PObject, SVM_DISCONNECT_OBJECT_INPUT, reinterpret_cast<DWORD_PTR>(&rInfo), NULL ) == SVMR_SUCCESS);
					ASSERT( bSuccess );
					rInfo.SetInputObject( nullptr );
				}
				//set new image to the Input image info
				rInfo.SetInputObject(image);

				BOOL bSuccess = ( ::SVSendMessage( rInfo.GetInputObjectInfo().PObject, SVM_CONNECT_OBJECT_INPUT, reinterpret_cast< DWORD_PTR >( &rInfo ), NULL ) == SVMR_SUCCESS );
				ASSERT( bSuccess );
				*plResult = bSuccess ? S_OK : S_FALSE;

				// Tell Tool image source has changed
				bSuccess = ( ::SVSendMessage( m_pParentDialog->m_pTool, SVM_IMAGE_SOURCE_CHANGED, NULL, NULL ) == SVMR_SUCCESS );
				ASSERT( bSuccess );
				*plResult = bSuccess ? S_OK : S_FALSE;

				// Reset all objects again...
				bSuccess = ( ::SVSendMessage( m_pParentDialog->m_pTool, SVM_RESET_ALL_OBJECTS, NULL, NULL ) == SVMR_SUCCESS );
				ASSERT( bSuccess );
				*plResult = bSuccess ? S_OK : S_FALSE;
			}

			setImages();
			Refresh();
		}
	}
}

void SVExternalToolImageSelectPage::OnPropClick( NMHDR* pNotifyStruct, LRESULT* plResult )
{
	LPNMPROPTREE pNMPropTree = reinterpret_cast< LPNMPROPTREE >( pNotifyStruct );
	*plResult = S_FALSE;

	if ( nullptr != pNMPropTree->pItem )
	{
		SVRPropertyItemCombo* pItem = dynamic_cast<SVRPropertyItemCombo*>(pNMPropTree->pItem);
		if ( nullptr != pItem && pItem->GetCtrlID() >= 0 && static_cast<int>(pItem->GetCtrlID()) < m_pTask->m_Data.m_lNumInputImages )
		{
			//set displayed tab to the current selected entry in the propTree
			m_ImageDisplay.SelectTab(pItem->GetCtrlID());
			*plResult = S_OK;
		}
	}
}
#pragma endregion Protected Methods

#pragma region Private Methods
void SVExternalToolImageSelectPage::Refresh()
{
	SVToolClass* pTool = m_pParentDialog->m_pTool;
	if( nullptr != m_pParentDialog && nullptr != pTool )
	{
		SVInspectionProcess* l_pInspection = pTool->GetInspection();

		if( l_pInspection != nullptr )
		{
			SVCommandInspectionRunOncePtr l_CommandPtr = new SVCommandInspectionRunOnce( l_pInspection->GetUniqueObjectID(), pTool->GetUniqueObjectID() );
			SVObjectSynchronousCommandTemplate< SVCommandInspectionRunOncePtr > l_Command( l_pInspection->GetUniqueObjectID(), l_CommandPtr );

			l_Command.Execute( 120000 ); // Allow the command 2 minutes to execute.
			m_ImageDisplay.Refresh();
			SVIPDoc* l_pIPDoc = SVObjectManagerClass::Instance().GetIPDoc( l_pInspection->GetUniqueObjectID() );

			if( l_pIPDoc != nullptr )
			{
				l_pIPDoc->UpdateAllViews( nullptr );
			}

			UpdateData( FALSE ); // set data to dialog
		}
	}
}

void SVExternalToolImageSelectPage::buildPropertyTree()
{
	// PTS_NOTIFY - SVRPropTree will send notification messages to the parent window
	DWORD dwStyle = WS_CHILD|WS_VISIBLE|PTS_NOTIFY;
	CRect rc;
	// Init the control's size to cover the entire client area
	GetDlgItem(IDC_IMAGE_LIST)->GetWindowRect(rc);
	ScreenToClient(rc);
	// Create SVRPropTree control
	m_Tree.Create(dwStyle, rc, this, IDC_IMAGE_LIST);
	m_Tree.SetColumn(static_cast<long>(rc.Width() * 0.15));
	m_Tree.ShowInfoText( false );

	SVRPropertyItem* pRoot = m_Tree.InsertItem(new SVRPropertyItem());
	ASSERT( pRoot );
	pRoot->SetCanShrink(false);
	pRoot->SetLabelText(_T("Input Source Images"));
	pRoot->SetInfoText(_T(""));

	// set the indexes
	// if any input images use this image, set the index
	for ( int iImageIndex = 0; iImageIndex < m_pTask->m_Data.m_lNumInputImages; iImageIndex++ )
	{
		CString strTmp;
		strTmp.Format( "Image %02d", iImageIndex + 1 );
		SVRPropertyItemCombo* pCombo = static_cast< SVRPropertyItemCombo* >( m_Tree.InsertItem( new SVRPropertyItemCombo(), pRoot ) );

		if ( nullptr != pCombo )
		{
			pCombo->SetCtrlID( iImageIndex );
			pCombo->SetLabelText( strTmp );
			pCombo->CreateComboBox();
			int curSel = 0;
			for (unsigned int iComboIndex=0; iComboIndex < m_AvailableImages.size(); iComboIndex++)
			{
				int iInsIndex = pCombo->AddString( m_AvailableImages[iComboIndex]->getDisplayedName() );
				pCombo->SetItemData( iInsIndex, iComboIndex );
				if ( m_AvailableImages[iComboIndex] == m_pInputImageInfo[iImageIndex]->GetInputObjectInfo().PObject)
				{
					curSel = iComboIndex;
				}
			}
			pCombo->SetItemValue(curSel);
		}

		m_ImageDisplay.AddTab(strTmp);
	}
	pRoot->Expand();
}

void SVExternalToolImageSelectPage::setImages()
{
	for ( int iImageIndex = 0; iImageIndex < m_pTask->m_Data.m_lNumInputImages; iImageIndex++ )
	{
		SVImageClass* image = dynamic_cast <SVImageClass*> ( m_pInputImageInfo[iImageIndex]->GetInputObjectInfo().PObject );
		m_ImageDisplay.setImage( image, iImageIndex );
	}
	m_ImageDisplay.Refresh();
}

void SVExternalToolImageSelectPage::createAvailableImageList( const SVGUID toolSetGUID, const SVIPDoc* l_pIPDoc )
{
	SVObjectTypeInfoStruct imageObjectInfo;
	imageObjectInfo.ObjectType = SVImageObjectType;
	SVGetObjectDequeByTypeVisitor l_Visitor( imageObjectInfo );
	SVObjectManagerClass::Instance().VisitElements( l_Visitor, toolSetGUID );
	SVGetObjectDequeByTypeVisitor::SVObjectPtrDeque::const_iterator l_Iter;

	for( l_Iter = l_Visitor.GetObjects().begin(); l_Iter != l_Visitor.GetObjects().end(); ++l_Iter )
	{
		SVImageClass* pImage = dynamic_cast< SVImageClass* >( const_cast< SVObjectClass* >( *l_Iter ) );

		// Ensure only image sources which are produced by tools above the current tool....
		if( pImage )
		{
			SVToolClass* pImageOwnerTool = dynamic_cast <SVToolClass*> ( pImage->GetAncestor( SVToolObjectType ) );

			if( pImageOwnerTool != NULL )
			{
				if( !( l_pIPDoc->IsToolPreviousToSelected( pImageOwnerTool->GetUniqueObjectID() )) || (pImageOwnerTool == m_pTool) )
				{
					break;
				}
			}

			SVImageInfoClass l_ImageInfo = pImage->GetImageInfo();
			bool bUseImage = true;
			pImageOwnerTool = dynamic_cast<SVToolClass *>(l_ImageInfo.GetOwner());

			if( pImageOwnerTool )
			{
				bUseImage = bUseImage && !SV_IS_KIND_OF(pImageOwnerTool, SVGageToolClass);
			}

			long l_lBandNumber = 1;
			l_ImageInfo.GetImageProperty( SVImagePropertyBandNumber, l_lBandNumber );
			bUseImage = bUseImage && (l_lBandNumber == 1);
			bUseImage = bUseImage && !(pImage->ObjectAttributesAllowed() & SV_HIDDEN);
			if ( bUseImage )
			{
				m_AvailableImages.Add( pImage );
			}
		}
	}
}
#pragma endregion Private Methods

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVExternalToolImageSelectPage.cpp_v  $
 * 
 *    Rev 1.4   26 Jun 2014 17:30:38   mziegler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  885
 * SCR Title:  Replace image display in TA-dialogs with activeX SVPictureDisplay
 * Checked in by:  mZiegler;  Marc Ziegler
 * Change Description:  
 *   rearrange the dialog
 * use SVPictureDisplay-control
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   15 May 2014 12:13:16   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Revised SVSendMessage to use DWORD_PTR
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   15 May 2014 11:24:08   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Revised SVSendMessage to use DWORD_PTR
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   01 Feb 2014 10:42:24   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changed sendmessage to use LONG_PTR instead of DWORD.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   01 Oct 2013 14:12:24   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Add x64 platform.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 10:32:12   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.22   01 Aug 2012 12:22:02   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added new command functionality to allow for synchronous command execution through the inspection thread.  This will fix problems with race conditions with the inspection process.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.21   30 Jul 2012 12:57:08   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated the parameter change for the visitor fucntionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.20   27 Jul 2012 08:14:58   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fixed include information and updated overlay collection functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.19   02 Jul 2012 16:52:06   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to promote new display functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.18   19 Jun 2012 12:43:30   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  744
 * SCR Title:  Add Shared Memory and Socket Functionality for Run Page Web Server
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Protected Object Information structure attributes.  This change will add accessor methods to get and set attributes.
 * Converted GUID to SVGUID.   This allows the use of standard operators for comparison and methods for conversion.
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.17   14 Jul 2011 08:47:56   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  730
 * SCR Title:  Adjust SVObserver to fix issues with Inspection resource handshaking
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Updated source code to better track resource management, and clear un-used resources.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.16   05 Jul 2011 14:45:44   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  724
 * SCR Title:  Add Resource Logging to SVObserver
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to for Last Inspected to defined reference catagories.  Added new logging based on low availiable resources.  Added offline Data Manager dumps of reference counts.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.15   07 Apr 2011 16:36:38   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated SVDlgImageClass to change the method init to UpdateImageInfo to make it trackable throughout the system.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.14   16 Mar 2011 13:32:38   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated extent functionality and renamed extent methods to correct extent issues and add clarity to the logical flow.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.13   25 Feb 2011 12:08:06   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated classes needed to consolidate SVImageClass and SVImageObjectClass updating calls to one method name.  Fixed updating problems with child buffers.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.12   08 Dec 2010 08:05:44   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to include changes in notification functionality using the Observer Design Pattern.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.11   16 Dec 2009 10:18:24   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  677
 * SCR Title:  Fix problem in camera notify thread
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fix issues with includes and comments.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.10   14 Jul 2009 13:13:24   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to use the new data manager objects.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   04 Aug 2006 16:28:22   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  556
 * SCR Title:  Fix external tool problems
 * Checked in by:  JimAdmin;  James A. Brown
 * Change Description:  
 *   Only display normal controls if there items to be displayed, otherwise show a message stating there are no items.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   20 Jan 2006 08:15:26   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  541
 * SCR Title:  Upgrade SVObserver for Pan Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Removed invalid commented out code.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   02 Nov 2005 07:33:12   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  512
 * SCR Title:  Add Published Result Images functionality
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   changed RunStatus.m_lResultImageIndex to .Images
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   20 Oct 2005 15:31:56   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  517
 * SCR Title:  Fix External Tool dependency issues
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   fixed dependency problems
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   22 Jun 2005 13:31:00   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  464
 * SCR Title:  Add array indexing for value objects
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   object attributes use accessor functions
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   08 Apr 2005 14:05:50   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  480
 * SCR Title:  Fix Bug in External Tool for Image selections
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added logic to OnInitDialog fo check the uObjectAttributesAllowed to have the SV_HIDDEN bit clear before adding the image to the select list.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   17 Feb 2005 14:07:26   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  456
 * SCR Title:  Update Image and Tool Objects to use the new Extent Classes
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Update class with new methodologies based on new base object, extents and reset structure.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   16 Feb 2005 14:17:22   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  440
 * SCR Title:  Create Internal Tool Object to Managing Tool Extents and Result Extents
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   updated header files
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   08 Jul 2004 12:32:32   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  406
 * SCR Title:  Implement External Tool
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   first iteration
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/