//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVTADlgAcquisitionSourcePage
//* .File Name       : $Workfile:   SVTADlgAcquisitionSourcePage.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.3  $
//* .Check In Date   : $Date:   21 May 2014 12:24:36  $
//******************************************************************************

#pragma region Includes
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
#pragma endregion Includes

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

SVToolAdjustmentDialogAcquisitionSourcePageClass::SVToolAdjustmentDialogAcquisitionSourcePageClass( const SVGUID& rInspectionID, const SVGUID& rTaskObjectID, SVToolAdjustmentDialogSheetClass* PSheet ) 
: CPropertyPage(SVToolAdjustmentDialogAcquisitionSourcePageClass::IDD)
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
		pMainImage = reinterpret_cast<SVCameraImageTemplate*>(::SVSendMessage( pTool, SVM_GETFIRST_OBJECT, NULL, reinterpret_cast<DWORD_PTR>(&info) ));
		if( pMainImage )
		{
			SVInspectionProcess* pInspection = dynamic_cast< SVInspectionProcess* >( pTool->GetAncestor( SVInspectionObjectType ) );

			if( nullptr != pInspection )
			{
				availableDigitizerTreeCtrl.ModifyStyle( 0, TVS_HASBUTTONS | TVS_HASLINES | TVS_LINESATROOT );

				SVVirtualCameraPtrSet l_CameraList;

				pInspection->GetPPQCameras( l_CameraList );

				SVVirtualCameraPtrSet::iterator l_Iter = l_CameraList.begin();

				while( l_Iter != l_CameraList.end() )
				{
					if( ( *l_Iter ) != NULL && ( *l_Iter )->mpsvDevice != NULL )
					{
						availableDigitizerTreeCtrl.AddItem( ( *l_Iter )->GetCompleteObjectName(), reinterpret_cast<DWORD_PTR>(*l_Iter) );
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
	SVVirtualCamera* pCamera = reinterpret_cast<SVVirtualCamera*>(availableDigitizerTreeCtrl.GetSelectedItemValue());
	if( nullptr != pTool && nullptr != pMainImage && nullptr != pCamera )
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

