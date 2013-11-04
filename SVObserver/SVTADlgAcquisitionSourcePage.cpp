//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVTADlgAcquisitionSourcePage
//* .File Name       : $Workfile:   SVTADlgAcquisitionSourcePage.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 15:16:14  $
//******************************************************************************

#include "stdafx.h"
#include "SVTADlgAcquisitionSourcePage.h"
#include "SVObjectLibrary/SVObjectClass.h"
#include "SVToolSet.h"
#include "SVTool.h"
#include "SVVirtualCamera.h"
#include "SVToolAcquisition.h"
#include "SVAcquisitionClass.h"
#include "SVToolAdjustmentDialogSheetClass.h"
#include "SVIPDoc.h"
#include "SVInspectionProcess.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

SVToolAdjustmentDialogAcquisitionSourcePageClass::SVToolAdjustmentDialogAcquisitionSourcePageClass( SVToolAdjustmentDialogSheetClass* PSheet ) : CPropertyPage(SVToolAdjustmentDialogAcquisitionSourcePageClass::IDD)
{
	//{{AFX_DATA_INIT(SVToolAdjustmentDialogAcquisitionSourcePageClass)
	StrSelectedDigitizer = _T("");
	bChannel0 = FALSE;
	bChannel1 = FALSE;
	bChannel2 = FALSE;
	bChannel3 = FALSE;
	//}}AFX_DATA_INIT

	pSheet		= PSheet;
	pTool		= NULL;
	pMainImage	= NULL;
	availableBandNum  = 0;
	availableBandLink = -1;
}

SVToolAdjustmentDialogAcquisitionSourcePageClass::~SVToolAdjustmentDialogAcquisitionSourcePageClass()
{
}

void SVToolAdjustmentDialogAcquisitionSourcePageClass::refresh()
{
	CWnd* pWnd = NULL;
	BOOL bColor = GetCheckedRadioButton( IDC_SINGLEBAND_RADIO, IDC_MULTIBAND_RADIO ) == IDC_MULTIBAND_RADIO;

	if ( !bColor )
	{
		CheckRadioButton( IDC_SINGLEBAND_RADIO, IDC_MULTIBAND_RADIO, IDC_SINGLEBAND_RADIO );
	}

	// Get current selected object in tree...
	SVObjectClass* pTreeSelectionObject = ( SVObjectClass* ) availableDigitizerTreeCtrl.GetSelectedItemValue();

	// Check enabled States of all Controls...
	// ...Select Button
	if( pWnd = GetDlgItem( IDC_SELECT_BUTTON ) )
	{
		if( pTreeSelectionObject )
		{
			pWnd->EnableWindow( TRUE );		
		}
		else
		{
			pWnd->EnableWindow( FALSE );
		}
	}

	// Set to defaults...
	StrSelectedDigitizer.Empty();
	availableBandNum  = 0;
	availableBandLink = -1;

	if( pMainImage )
	{
		// Get available Band resources...
		SVImageInfoClass ImageInfo;
		SVImageInfoClass oImageInfo;

		if ( pMainImage->GetCamera() != NULL )
		{
			// Get current selected Digitizer Name...
			StrSelectedDigitizer = pMainImage->GetCamera()->GetCompleteObjectName();

			pMainImage->GetCamera()->mpsvDevice->GetImageInfo( &oImageInfo );

			oImageInfo.GetImageProperty( SVImagePropertyBandNumber, availableBandNum );
			oImageInfo.GetImageProperty( SVImagePropertyBandLink, availableBandLink );
		}

		// Get current settings...
		int bandNum  = 1;
		int bandLink = 0;

		ImageInfo = pMainImage->GetImageInfo();

		// Get current settings...
		ImageInfo.GetImageProperty( SVImagePropertyBandNumber, bandNum );
		ImageInfo.GetImageProperty( SVImagePropertyBandLink, bandLink );

		// Set up channel states...
		bChannel0 = ( bandLink == 0 || ( bColor && bandNum > 1 ) );
		bChannel1 = ( bandLink == 1 || ( bColor && bandNum > 1 ) );
		bChannel2 = ( bandLink == 2 || ( bColor && bandNum > 2 ) );
		bChannel3 = ( bandLink == 3 || ( bColor && bandNum > 3 ) );
	}

	// Check Channel Checkboxes enabled state...
	if( pWnd = GetDlgItem( IDC_CHANNEL0_CHECK ) )
		pWnd->EnableWindow( FALSE );		
	if( pWnd = GetDlgItem( IDC_CHANNEL1_CHECK ) )
		pWnd->EnableWindow( FALSE );		
	if( pWnd = GetDlgItem( IDC_CHANNEL2_CHECK ) )
		pWnd->EnableWindow( FALSE );		
	if( pWnd = GetDlgItem( IDC_CHANNEL3_CHECK ) )
		pWnd->EnableWindow( FALSE );

	// Check Band Radio Buttons enabled state...
	if( pWnd = GetDlgItem( IDC_MULTIBAND_RADIO ) )
		pWnd->EnableWindow( FALSE );		
	if( pWnd = GetDlgItem( IDC_SINGLEBAND_RADIO ) )
		pWnd->EnableWindow( FALSE );		

	UpdateData( FALSE ); // Send Data to Dialog...
}


void SVToolAdjustmentDialogAcquisitionSourcePageClass::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(SVToolAdjustmentDialogAcquisitionSourcePageClass)
	DDX_Control(pDX, IDC_AVAILABLE_DIGITIZERS_TREE, availableDigitizerTreeCtrl);
	DDX_Text(pDX, IDC_SELECTED_EDIT, StrSelectedDigitizer);
	DDX_Check(pDX, IDC_CHANNEL0_CHECK, bChannel0);
	DDX_Check(pDX, IDC_CHANNEL1_CHECK, bChannel1);
	DDX_Check(pDX, IDC_CHANNEL2_CHECK, bChannel2);
	DDX_Check(pDX, IDC_CHANNEL3_CHECK, bChannel3);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(SVToolAdjustmentDialogAcquisitionSourcePageClass, CPropertyPage)
	//{{AFX_MSG_MAP(SVToolAdjustmentDialogAcquisitionSourcePageClass)
	ON_BN_CLICKED(IDC_SELECT_BUTTON, OnSelectButton)
	ON_NOTIFY(TVN_SELCHANGED, IDC_AVAILABLE_DIGITIZERS_TREE, OnSelchangedAvailableDigitizersTree)
	ON_BN_CLICKED(IDC_CHANNEL0_CHECK, OnChannel0Check)
	ON_BN_CLICKED(IDC_CHANNEL1_CHECK, OnChannel1Check)
	ON_BN_CLICKED(IDC_CHANNEL2_CHECK, OnChannel2Check)
	ON_BN_CLICKED(IDC_CHANNEL3_CHECK, OnChannel3Check)
	ON_BN_CLICKED(IDC_MULTIBAND_RADIO, OnMultibandRadio)
	ON_BN_CLICKED(IDC_SINGLEBAND_RADIO, OnSinglebandRadio)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// SVToolAdjustmentDialogAcquisitionSourcePageClass message handlers

BOOL SVToolAdjustmentDialogAcquisitionSourcePageClass::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();

	CheckRadioButton( IDC_SINGLEBAND_RADIO, IDC_MULTIBAND_RADIO, IDC_SINGLEBAND_RADIO );
	UpdateData( FALSE );

	if( pSheet && ( pTool = pSheet->GetTool() ) && SV_IS_KIND_OF( pTool, SVAcquisitionToolClass ) )
	{
		// Try to get main image of the current acquisition tool...
		SVObjectTypeInfoStruct info;
		info.ObjectType = SVImageObjectType;
		info.SubType    = SVMainImageObjectType;
		pMainImage = ( SVCameraImageTemplate* ) ::SVSendMessage( pTool, SVM_GETFIRST_OBJECT, NULL, ( DWORD )&info );
		if( pMainImage )
		{
			SVInspectionProcess* pIP = dynamic_cast< SVInspectionProcess* >( pTool->GetAncestor( SVInspectionObjectType ) );

			if( pIP != NULL )
			{
				availableDigitizerTreeCtrl.ModifyStyle( 0, TVS_HASBUTTONS | TVS_HASLINES | TVS_LINESATROOT );

				SVVirtualCameraPtrSet l_CameraList;

				pIP->GetPPQCameras( l_CameraList );

				SVVirtualCameraPtrSet::iterator l_Iter = l_CameraList.begin();

				while( l_Iter != l_CameraList.end() )
				{
					if( ( *l_Iter ) != NULL && ( *l_Iter )->mpsvDevice != NULL )
					{
						availableDigitizerTreeCtrl.AddItem( ( *l_Iter )->GetCompleteObjectName(), ( DWORD ) ( *l_Iter ) );
					}

					++l_Iter;
				}

				refresh();
			}

			// Success...
			return TRUE;  // return TRUE unless you set the focus to a control
						  // EXCEPTION: OCX Property Pages should return FALSE
		}
	}

	// Not valid call...try to close!
	if( GetParent() )
		GetParent()->SendMessage( WM_CLOSE );
	else
		SendMessage( WM_CLOSE );
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void SVToolAdjustmentDialogAcquisitionSourcePageClass::OnSelectButton() 
{
	SVToolSetClass* pToolSet = NULL;

	// &&&
	// Get current selected object in tree...
	SVVirtualCamera* pCamera = ( SVVirtualCamera* ) availableDigitizerTreeCtrl.GetSelectedItemValue();
	if( pTool && pMainImage )
	{
		// Set new digitizer of main image...
		pMainImage->UpdateCameraImage( pCamera->GetUniqueObjectID() );

		// Reset band number and link to defaults...
		SVImageInfoClass ImageInfo = pMainImage->GetImageInfo();

		ImageInfo.SetImageProperty( SVImagePropertyBandNumber, 1 );
		ImageInfo.SetImageProperty( SVImagePropertyBandLink, 0 );

		if ( pMainImage->UpdateImage( ImageInfo ) == S_OK )
		{
			// Reset all objects...
			::SVSendMessage( pTool, SVM_RESET_ALL_OBJECTS, NULL, NULL );

			refresh();
		}
	}
}

void SVToolAdjustmentDialogAcquisitionSourcePageClass::OnSelchangedAvailableDigitizersTree(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;

	refresh();
	
	*pResult = 0;
}


void SVToolAdjustmentDialogAcquisitionSourcePageClass::OnChannel0Check() 
{
	UpdateData( TRUE );

	if( pMainImage && availableBandNum > 0 )
	{
		if( bChannel0 )
		{
			SVImageInfoClass ImageInfo = pMainImage->GetImageInfo();

			ImageInfo.SetImageProperty( SVImagePropertyBandNumber, 1 );
			ImageInfo.SetImageProperty( SVImagePropertyBandLink, 0 );

			if( pMainImage->UpdateImage( ImageInfo ) == S_OK )
			{
				// Reset all objects...
				::SVSendMessage( pTool, SVM_RESET_ALL_OBJECTS, NULL, NULL );

				refresh();
			}
		}
	}
}

void SVToolAdjustmentDialogAcquisitionSourcePageClass::OnChannel1Check() 
{
	UpdateData( TRUE );

	if( pMainImage && availableBandNum > 1 )
	{
		if( bChannel1 )
		{
			SVImageInfoClass ImageInfo = pMainImage->GetImageInfo();

			ImageInfo.SetImageProperty( SVImagePropertyBandNumber, 1 );
			ImageInfo.SetImageProperty( SVImagePropertyBandLink, 1 );

			if ( pMainImage->UpdateImage( ImageInfo ) == S_OK )
			{
				// Reset all objects...
				::SVSendMessage( pTool, SVM_RESET_ALL_OBJECTS, NULL, NULL );

				refresh();
			}
		}
	}
}

void SVToolAdjustmentDialogAcquisitionSourcePageClass::OnChannel2Check() 
{
	UpdateData( TRUE );

	if( pMainImage && availableBandNum > 2 )
	{
		if( bChannel2 )
		{
			SVImageInfoClass ImageInfo = pMainImage->GetImageInfo();

			ImageInfo.SetImageProperty( SVImagePropertyBandNumber, 1 );
			ImageInfo.SetImageProperty( SVImagePropertyBandLink, 2 );

			if( pMainImage->UpdateImage( ImageInfo ) == S_OK )
			{
				// Reset all objects...
				::SVSendMessage( pTool, SVM_RESET_ALL_OBJECTS, NULL, NULL );

				refresh();
			}
		}
	}
}

void SVToolAdjustmentDialogAcquisitionSourcePageClass::OnChannel3Check() 
{
	UpdateData( TRUE );

	if( pMainImage && availableBandNum > 3 )
	{
		if( bChannel3 )
		{
			SVImageInfoClass ImageInfo = pMainImage->GetImageInfo();

			ImageInfo.SetImageProperty( SVImagePropertyBandNumber, 1 );
			ImageInfo.SetImageProperty( SVImagePropertyBandLink, 3 );

			if ( pMainImage->UpdateImage( ImageInfo ) == S_OK )
			{
				// Reset all objects...
				::SVSendMessage( pTool, SVM_RESET_ALL_OBJECTS, NULL, NULL );

				refresh();
			}
		}
	}
}

void SVToolAdjustmentDialogAcquisitionSourcePageClass::OnMultibandRadio() 
{
	UpdateData( TRUE );

	if( pMainImage )
	{
		SVImageInfoClass ImageInfo = pMainImage->GetImageInfo();

		ImageInfo.SetImageProperty( SVImagePropertyBandNumber, availableBandNum );
		ImageInfo.SetImageProperty( SVImagePropertyBandLink, 0 );

		if( pMainImage->UpdateImage( ImageInfo ) == S_OK )
		{
			// Reset all objects...
			::SVSendMessage( pTool, SVM_RESET_ALL_OBJECTS, NULL, NULL );

			CheckRadioButton( IDC_SINGLEBAND_RADIO, IDC_MULTIBAND_RADIO, IDC_MULTIBAND_RADIO );

			refresh();
		}
	}
}

void SVToolAdjustmentDialogAcquisitionSourcePageClass::OnSinglebandRadio() 
{
	UpdateData( TRUE );

	if( pMainImage && availableBandNum > 0 )
	{
		SVImageInfoClass ImageInfo = pMainImage->GetImageInfo();

		long l_lBandLink = 0;

		ImageInfo.GetImageProperty( SVImagePropertyBandLink, l_lBandLink );

		ImageInfo.SetImageProperty( SVImagePropertyBandNumber, 1 );

		if ( l_lBandLink < 0 )
		{
			ImageInfo.SetImageProperty( SVImagePropertyBandLink, 0 );
		}

		if( pMainImage->UpdateImage( ImageInfo ) == S_OK )
		{
			// Reset all objects...
			::SVSendMessage( pTool, SVM_RESET_ALL_OBJECTS, NULL, NULL );

			CheckRadioButton( IDC_SINGLEBAND_RADIO, IDC_MULTIBAND_RADIO, IDC_SINGLEBAND_RADIO );

			refresh();
		}
	}
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVTADlgAcquisitionSourcePage.cpp_v  $
 * 
 *    Rev 1.0   23 Apr 2013 15:16:14   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.19   02 Jul 2012 17:40:16   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to promote new display functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.18   28 Sep 2011 13:16:08   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  730
 * SCR Title:  Adjust SVObserver to fix issues with Inspection resource handshaking
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Updated source code to fix issues with camera image interface and methodology.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.17   16 Sep 2011 16:23:12   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  730
 * SCR Title:  Adjust SVObserver to fix issues with Inspection resource handshaking
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Updated objects with new device and resource management functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.16   03 May 2011 09:50:04   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   changed the return check to look for an HRESULT for UpdateImage
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.15   13 Apr 2011 15:35:12   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated system to fix color processing issues with color configurations.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.14   16 Mar 2011 13:59:08   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated extent functionality and renamed extent methods to correct extent issues and add clarity to the logical flow.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.13   25 Feb 2011 12:25:40   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated classes needed to consolidate SVImageClass and SVImageObjectClass updating calls to one method name.  Fixed updating problems with child buffers.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.12   08 Dec 2010 13:39:10   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to include changes in notification functionality using the Observer Design Pattern.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.11   09 Nov 2010 16:30:30   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  704
 * SCR Title:  Upgrade SVObserver Version for 5.33 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to remove duplicate container objects.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.10   16 Dec 2009 13:12:40   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  677
 * SCR Title:  Fix problem in camera notify thread
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fix issues with includes and comments.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   06 Oct 2005 09:45:24   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to handle the issues when a camera gets removed from the system.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   09 Aug 2005 08:15:24   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated objects and dialogs to use the ActiveX Interface to update inspection data.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   17 Feb 2005 15:49:20   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  456
 * SCR Title:  Update Image and Tool Objects to use the new Extent Classes
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Modifications to support new extents.  Also use reset objects rather than recreating them.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   16 Feb 2005 15:19:06   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  440
 * SCR Title:  Create Internal Tool Object to Managing Tool Extents and Result Extents
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   updated header files
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   22 Apr 2003 15:43:24   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   10 Apr 2003 13:29:24   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  339
 * SCR Title:  Deleting a camera used by an Acquisition Tool causes problems
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   added additional check to make sure the device on the camera is not NULL before adding the device to the AvaiableDigitizerTree
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   30 Jan 2003 16:41:42   joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  301
 * SCR Title:  Implement Result Image Circle Buffer
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated OnSinglebandRadio, OnMultibandRadio, OnChannel3Check, OnChannel2Check, OnCannel1Check, OnChannel0Check, OnselectButton and refresh methods to use the new image circle buffer methodology.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   20 Nov 2002 13:32:08   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  226
 * SCR Title:  Monochrome SVIM configuration compatibility between ViperQUAD and ViperDUAL
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   removed  "SVSystem.h"
 * removed OnFinalRelease
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   17 Oct 2000 08:31:24   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  185
 * SCR Title:  Color SVIM Light Reference Problem
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fixes channel selection problem in light reference dialog and acquisition tool dialog.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   07 Feb 2000 23:28:14   robert
 * Project:  SVObserver
 * Change Request (SCR) nbr:  93
 * SCR Title:  Integrate Multiple Camera Image Input per IPD
 * Checked in by:  Robert;  Robert Massinger
 * Change Description:  
 *   Initial Check In.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
