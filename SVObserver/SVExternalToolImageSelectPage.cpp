//******************************************************************************
//* COPYRIGHT (c) 2004 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVExternalToolImageSelectPage
//* .File Name       : $Workfile:   SVExternalToolImageSelectPage.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 10:32:12  $
//******************************************************************************

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

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BEGIN_MESSAGE_MAP(SVExternalToolImageSelectPage, CPropertyPage)
	//{{AFX_MSG_MAP(SVExternalToolImageSelectPage)
	ON_CBN_SELCHANGE(IDC_SELECT_IMAGE, OnSelchangeImage)
	ON_LBN_SELCHANGE(IDC_IMAGE_LIST, OnSelchangeImageList)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

SVExternalToolImageSelectPage::SVExternalToolImageSelectPage(SVExternalToolDetailsSheet* pParent, int id ) 
: CPropertyPage(id)
{
	ASSERT( m_pParentDialog );

	m_pParentDialog		  = pParent;
	m_pCurrentSourceImage = NULL;

	m_pTool = m_pParentDialog->m_pTool;
	m_pTask = m_pParentDialog->m_pTask;

	for ( int i = 0 ; i < SVExternalToolTaskData::NUM_INPUT_IMAGES ; i++ )
	{
		m_pInputImageInfo[i] = NULL;
	}

	//{{AFX_DATA_INIT(SVExternalToolImageSelectPage)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

}

SVExternalToolImageSelectPage::~SVExternalToolImageSelectPage()
{
}

void SVExternalToolImageSelectPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(SVExternalToolImageSelectPage)
	DDX_Control(pDX, IDC_IMAGE_LIST, m_lbImageList);
	DDX_Control(pDX, IDC_IMAGE_DISPLAY, m_ImageDisplay);
	DDX_Control(pDX, IDC_SELECT_IMAGE, m_cbAvailableImages);
	//}}AFX_DATA_MAP
}

BOOL SVExternalToolImageSelectPage::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	SVIPDoc* l_pIPDoc = m_pParentDialog->GetIPDoc();

	if( l_pIPDoc != NULL )
	{
		SVToolSetClass* pToolSet = NULL;

		for( int i = 0 ; i < m_pTask->m_Data.m_lNumInputImages; i++)
		{
			CString strTmp;
			strTmp.Format("Image_%02d",i+1);
			m_lbImageList.AddString(strTmp);
		}



		pToolSet = dynamic_cast <SVToolSetClass*> ( m_pTool->GetAncestor( SVToolSetObjectType ) );

		if( pToolSet &&	pToolSet->getCurrentImage() && m_pTask->m_Data.m_lNumInputImages > 0 )
		{
			int i;

			GetDlgItem(IDC_NO_IMAGES_TXT)->ShowWindow( SW_HIDE );
			GetDlgItem(IDC_IMAGE_NAME_TXT)->ShowWindow( SW_SHOW );
			GetDlgItem(IDC_SOURCE_IMAGE_INPUT_TXT)->ShowWindow( SW_SHOW );

			// copy pointers to input image info
			for ( i = 0; i < m_pTask->m_Data.m_lNumInputImages; i++ )
			{
				m_pInputImageInfo[i] = &m_pTask->m_Data.m_aInputImageInfo[ i ];
			}// end for ( int i = 0; i < m_pTask->m_Data.m_iNumInputImages; i++ )

			// init currently selected image
			int iInitialSelect = 0;
		
			if( m_pInputImageInfo[iInitialSelect]->GetInputObjectInfo().PObject != NULL && m_pInputImageInfo[iInitialSelect]->IsConnected() )
			{

				m_pCurrentSourceImage = dynamic_cast <SVImageClass*> ( m_pInputImageInfo[iInitialSelect]->GetInputObjectInfo().PObject );
			}

			int iIndex = 0;

			SVObjectTypeInfoStruct imageObjectInfo;
			imageObjectInfo.ObjectType = SVImageObjectType;

			SVGetObjectDequeByTypeVisitor l_Visitor( imageObjectInfo );

			SVObjectManagerClass::Instance().VisitElements( l_Visitor, pToolSet->GetUniqueObjectID() );

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
						++iIndex;
					}
				}
			}
			/*
			// Find all available Images...
			SVObjectTypeInfoStruct imageObjectInfo;
			imageObjectInfo.ObjectType = SVImageObjectType;
			
			int iIndex = 0;

			// Search for first...
			SVImageClass* pImage = dynamic_cast <SVImageClass*> ((SVObjectClass*) ::SVSendMessage( pToolSet, SVM_GETFIRST_OBJECT, NULL, ( DWORD ) &imageObjectInfo ) );
			while( pImage )
			{
				SVImageInfoClass l_ImageInfo = pImage->GetImageInfo();

				bool bUseImage = true;
				SVToolClass* pImageOwnerTool = dynamic_cast<SVToolClass *>(l_ImageInfo.GetOwner());

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
					++iIndex;
				}// end if ( bUseImage )

				// Search for next image...
				pImage = dynamic_cast <SVImageClass*> ( (SVObjectClass*) ::SVSendMessage( pToolSet, SVM_GETNEXT_OBJECT, ( DWORD ) pImage, ( DWORD ) &imageObjectInfo ) );

				// Ensure only image sources which are produced by tools above the current tool....
				if( pImage )
				{
					SVToolClass* pImageOwnerTool = dynamic_cast <SVToolClass*> ( pImage->GetAncestor( SVToolObjectType ) );

					if( pImageOwnerTool != NULL )
					{
						if( !( l_pIPDoc->IsToolPreviousToSelected( pImageOwnerTool->GetUniqueObjectID() ) ) )
						{
							break;
						}
					}
				}// end if( pImage )

			}// end while( pImage )
			*/
			// Init source image list box...
			// the item data is the index (same as position)
			m_iOldIndex = m_cbAvailableImages.init( &m_AvailableImages, m_pCurrentSourceImage );

			// set the indexes
			// if any input images use this image, set the index
			for ( int iImageIndex = 0; iImageIndex < m_pTask->m_Data.m_lNumInputImages; iImageIndex++ )
			{
				for (int iComboIndex=0; iComboIndex < m_cbAvailableImages.GetCount(); iComboIndex++)
				{
					int iAvailableIndex = m_cbAvailableImages.GetItemData(iComboIndex);
					SVImageClass* pImage = m_AvailableImages.GetAt(iAvailableIndex);
					if ( pImage == m_pInputImageInfo[iImageIndex]->GetInputObjectInfo().PObject )
					{
						m_lbImageList.SetItemData(iImageIndex, iComboIndex);
					}
				}

			}

			m_ImageDisplay.UpdateImageInfo( m_pCurrentSourceImage );
			m_ImageDisplay.refresh();

			UpdateData( FALSE ); // set data to dialog

		}// end if( pToolSet &&	pToolSet->getCurrentImage() )
		else
		{
			m_ImageDisplay.ShowWindow( SW_HIDE );
			m_lbImageList.ShowWindow( SW_HIDE );
			m_cbAvailableImages.ShowWindow( SW_HIDE );
			GetDlgItem(IDC_NO_IMAGES_TXT)->ShowWindow( SW_SHOW );
			GetDlgItem(IDC_IMAGE_NAME_TXT)->ShowWindow( SW_HIDE );
			GetDlgItem(IDC_SOURCE_IMAGE_INPUT_TXT)->ShowWindow( SW_HIDE );
		}

		// ******** Update List Box **********

		m_lbImageList.SetCurSel(0);
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void SVExternalToolImageSelectPage::Refresh()
{
	SVToolClass* pTool;
	if( m_pParentDialog && ( pTool = m_pParentDialog->m_pTool ) )
	{
		SVInspectionProcess* l_pInspection = pTool->GetInspection();

		if( l_pInspection != NULL )
		{
			SVCommandInspectionRunOncePtr l_CommandPtr = new SVCommandInspectionRunOnce( l_pInspection->GetUniqueObjectID(), pTool->GetUniqueObjectID() );
			SVObjectSynchronousCommandTemplate< SVCommandInspectionRunOncePtr > l_Command( l_pInspection->GetUniqueObjectID(), l_CommandPtr );

			l_Command.Execute( 120000 );

			m_ImageDisplay.refresh();
			
			SVIPDoc* l_pIPDoc = SVObjectManagerClass::Instance().GetIPDoc( l_pInspection->GetUniqueObjectID() );

			if( l_pIPDoc != NULL )
			{
				l_pIPDoc->UpdateAllViews( NULL );
			}

			UpdateData( FALSE ); // set data to dialog
		}
	}
}

void SVExternalToolImageSelectPage::OnSelchangeImage() 
{
	UpdateData( TRUE ); // get data from dialog

	int iCurrentImage = m_lbImageList.GetCurSel();
	int iCurSel = m_cbAvailableImages.GetCurSel();
	int index = ( int ) m_cbAvailableImages.GetItemData( iCurSel );

	m_lbImageList.SetItemData(iCurrentImage, iCurSel);

	if( m_pInputImageInfo[iCurrentImage] && m_pInputImageInfo[iCurrentImage]->PObject && index != LB_ERR && index >= 0 && index < m_AvailableImages.GetSize() )
	{
		// disconnect old selected image, connect new one

		SVToolSetClass* pToolSet = dynamic_cast <SVToolSetClass*> (m_pInputImageInfo[iCurrentImage]->PObject->GetAncestor( SVToolSetObjectType ));

		// Get new input...
		m_pCurrentSourceImage = m_AvailableImages.GetAt( index );

		SVInObjectInfoStruct& rInfo = *(m_pInputImageInfo[iCurrentImage]);
		rInfo.SetInputObject( m_pCurrentSourceImage );
		BOOL bSuccess = ::SVSendMessage( rInfo.GetInputObjectInfo().PObject, SVM_CONNECT_OBJECT_INPUT, ( DWORD )&rInfo, NULL );
		ASSERT( bSuccess );

		// Tell Tool image source has changed
		::SVSendMessage( m_pParentDialog->m_pTool, SVM_IMAGE_SOURCE_CHANGED, NULL, NULL );

		// Reset all objects again...
		::SVSendMessage( m_pParentDialog->m_pTool, SVM_RESET_ALL_OBJECTS, NULL, NULL );

		m_ImageDisplay.UpdateImageInfo( m_pCurrentSourceImage );
	
		Refresh();
	}
}

void SVExternalToolImageSelectPage::OnSelchangeImageList() 
{
	// update image list selection
	int iIndex = m_lbImageList.GetItemData(m_lbImageList.GetCurSel());
	m_cbAvailableImages.SetCurSel(iIndex);
	OnSelchangeImage();
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVExternalToolImageSelectPage.cpp_v  $
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
