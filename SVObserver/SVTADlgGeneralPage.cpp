//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVTADlgGeneralPage
//* .File Name       : $Workfile:   SVTADlgGeneralPage.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   15 May 2014 12:50:40  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVTADlgGeneralPage.h"
#include "GuiCommands\GetDependencies.h"
#include "SVObjectLibrary\SVObjectSynchronousCommandTemplate.h"
#include "SVTool.h"
#include "SVToolAdjustmentDialogSheetClass.h"
#include "SVIPDoc.h"
#include "SVInspectionProcess.h"

#include "SVGageTool.h"
#include "SVShowDependentsDialog.h"
#pragma endregion

//******************************************************************************
//* DEFINITIONS OF MODULE-LOCAL VARIABLES:
//******************************************************************************

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/
//* Class Name : SVToolAdjustmentDialogGeneralPageClass
//* Note(s)    : property page
//*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/


SVToolAdjustmentDialogGeneralPageClass::SVToolAdjustmentDialogGeneralPageClass( SVToolAdjustmentDialogSheetClass* PSheet ) : CPropertyPage(SVToolAdjustmentDialogGeneralPageClass::IDD)
{
	//{{AFX_DATA_INIT(SVToolAdjustmentDialogGeneralPageClass)
	m_bUpdateAuxilliaryExtents = FALSE;
	//}}AFX_DATA_INIT

	pSheet		= PSheet;
	pTool		= NULL;
	m_bIsImageTool = false;
	m_pUpdateAuxilliary = NULL;
	pToolDrawFlag = NULL;
}

SVToolAdjustmentDialogGeneralPageClass::~SVToolAdjustmentDialogGeneralPageClass()
{
}

HRESULT SVToolAdjustmentDialogGeneralPageClass::SetInspectionData()
{
	HRESULT l_hrOk = S_FALSE;

	if( pToolDrawFlag )
	{
		UpdateData( TRUE ); // get data from dialog

		int sel = drawToolCombo.GetCurSel();
		if( sel >= 0 )
		{
			long lValue = ( long ) drawToolCombo.GetItemData( sel );
	
			l_hrOk = AddInputRequest( pToolDrawFlag, lValue );

			l_hrOk = AddInputRequest( m_pUpdateAuxilliary, m_bUpdateAuxilliaryExtents );

			if( l_hrOk == S_OK )
			{
				l_hrOk = AddInputRequestMarker();
			}

			if( l_hrOk == S_OK )
			{
				l_hrOk = RunOnce( pTool );
			}
		}

		UpdateData( FALSE );
	}

	return l_hrOk;
}



void SVToolAdjustmentDialogGeneralPageClass::refresh()
{

	CWnd* pWnd = NULL;

	// Update dialog with freeze tool attributes...
	BOOL bOk = FALSE;
	if( pToolDrawFlag )
	{
		CString strEnum;
		if( pToolDrawFlag->GetValue( strEnum ) == S_OK )
		{
			// Set cur sel in combo box...
			int index = drawToolCombo.SelectString( -1, strEnum );
			bOk = TRUE;
		}
	}
	// Check, if drawToolCombo must be disabled/enabled...
	if( pWnd = GetDlgItem( IDC_DRAW_TOOL_COMBO ) )
		pWnd->EnableWindow( bOk );

	if( m_pUpdateAuxilliary )
	{
		if( m_bIsImageTool )
		{
			bOk = m_pUpdateAuxilliary->GetValue( m_bUpdateAuxilliaryExtents ) == S_OK ;

			GetDlgItem( IDC_SOURCE_IMAGE_COMBO )->EnableWindow( m_bUpdateAuxilliaryExtents );
		}
	}
	else
	{
		bOk = FALSE;
	}

	// ... do next control
	// bOk = FALSE;


	UpdateData( FALSE ); // Send Data to Dialog...
}




void SVToolAdjustmentDialogGeneralPageClass::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(SVToolAdjustmentDialogGeneralPageClass)
	DDX_Control(pDX, IDC_SOURCE_IMAGE_COMBO, m_AvailableSourceImageCombo);
	DDX_Control(pDX, IDC_DRAW_TOOL_COMBO, drawToolCombo);
	DDX_Check(pDX, IDC_ENABLE_AUXILIARY_EXTENTS, m_bUpdateAuxilliaryExtents);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(SVToolAdjustmentDialogGeneralPageClass, CPropertyPage)
	//{{AFX_MSG_MAP(SVToolAdjustmentDialogGeneralPageClass)
	ON_CBN_SELCHANGE(IDC_DRAW_TOOL_COMBO, OnSelchangeDrawToolCombo)
	ON_BN_CLICKED(IDC_ENABLE_AUXILIARY_EXTENTS, OnUpdateAuxilliaryExtents)
	ON_CBN_SELCHANGE(IDC_SOURCE_IMAGE_COMBO, OnSelchangeSourceImageCombo)
	ON_BN_CLICKED(ID_SHOW_RELATIONS, OnShowRelations)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// SVToolAdjustmentDialogGeneralPageClass message handlers

BOOL SVToolAdjustmentDialogGeneralPageClass::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();

	SVToolClass* pTool = NULL;
	if( pSheet && ( pTool = pSheet->GetTool() ) )
	{
		SetTaskObject( pTool );

		// Get tool's draw flag object...
		SVObjectTypeInfoStruct drawFlagObjectInfo;
		drawFlagObjectInfo.EmbeddedID = SVConditionalToolDrawFlagObjectGuid;
		pToolDrawFlag = reinterpret_cast<SVEnumerateValueObjectClass*>(::SVSendMessage( pTool, SVM_GETFIRST_OBJECT, NULL, reinterpret_cast<DWORD_PTR>(&drawFlagObjectInfo) ));
		if( pToolDrawFlag )
		{
			// Populate Draw Flag combo...
			CString strEnumList;
			pToolDrawFlag->GetEnumTypes( strEnumList );
			drawToolCombo.SetEnumTypes( strEnumList );
		}

		// Get tool's Update Auxilliary Extents...
		if( pTool->GetToolImage() == NULL  || 
			dynamic_cast<SVGageToolClass*>( pTool ) != NULL ||
			pTool->GetInspection()->GetEnableAuxiliaryExtent() == 0 )
		{
			// Disable Auxilliary Extents
			GetDlgItem( IDC_ENABLE_AUXILIARY_EXTENTS )->ShowWindow( SW_HIDE );
			GetDlgItem( IDC_AUXILIARY_GROUP )->ShowWindow( SW_HIDE );
			GetDlgItem( IDC_SOURCE_IMAGE_TEXT )->ShowWindow( SW_HIDE );
			GetDlgItem( IDC_SOURCE_IMAGE_COMBO )->ShowWindow( SW_HIDE );
			m_bIsImageTool = false;
		}
		else
		{
			// Enable Auxilliary Extents
			GetDlgItem( IDC_ENABLE_AUXILIARY_EXTENTS )->ShowWindow( SW_SHOW );
			GetDlgItem( IDC_AUXILIARY_GROUP )->ShowWindow( SW_SHOW );
			GetDlgItem( IDC_SOURCE_IMAGE_TEXT )->ShowWindow( SW_SHOW );
			GetDlgItem( IDC_SOURCE_IMAGE_COMBO )->ShowWindow( SW_SHOW );
			m_bIsImageTool = true;

			SVObjectTypeInfoStruct l_svObjectInfo;
			l_svObjectInfo.EmbeddedID = SVUpdateAuxilliaryExtentsObjectGuid;
			m_pUpdateAuxilliary = reinterpret_cast<SVBoolValueObjectClass*>(::SVSendMessage( pTool, SVM_GETFIRST_OBJECT, NULL, reinterpret_cast<DWORD_PTR>(&l_svObjectInfo) ));
			if( m_pUpdateAuxilliary )
			{
				m_pUpdateAuxilliary->GetValue( m_bUpdateAuxilliaryExtents );
			}

			GetDlgItem( IDC_SOURCE_IMAGE_COMBO )->EnableWindow( m_bUpdateAuxilliaryExtents );

			// Auxiliary Extent Source Images.
			pTool->GetSourceImages( &m_aImageList );

			pTool->SetAuxSourceImage( pTool->GetAuxSourceImage() );

			m_AvailableSourceImageCombo.init( &m_aImageList, pTool->GetAuxSourceImage() );
		}
		

		UpdateData( FALSE );

		refresh();

		// Success...
		return TRUE;  // return TRUE unless you set the focus to a control
					  // EXCEPTION: OCX Property Pages should return FALSE
	}
	
	
	// Not valid call...try to close!
	if( GetParent() )
		GetParent()->SendMessage( WM_CLOSE );
	else
		SendMessage( WM_CLOSE );
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void SVToolAdjustmentDialogGeneralPageClass::OnSelchangeDrawToolCombo() 
{
	SetInspectionData();

	refresh();
}

void SVToolAdjustmentDialogGeneralPageClass::OnUpdateAuxilliaryExtents() 
{
	SetInspectionData();

	SVToolClass* pTool = NULL;
	if( pSheet && ( pTool = pSheet->GetTool() ) )
	{
		pTool->SetAuxSourceImage( pTool->GetAuxSourceImage() );
	}

	GetDlgItem( IDC_SOURCE_IMAGE_COMBO )->EnableWindow( m_bUpdateAuxilliaryExtents );

	refresh();
	
}

void SVToolAdjustmentDialogGeneralPageClass::OnSelchangeSourceImageCombo() 
{
	UpdateData( TRUE ); // get data from dialog

	int index = ( int ) m_AvailableSourceImageCombo.GetItemData( m_AvailableSourceImageCombo.GetCurSel() );

	if( index != LB_ERR && index >= 0 && index < m_aImageList.GetSize() )
	{
		SVToolClass* pTool = NULL;
		if( pSheet && ( pTool = pSheet->GetTool() ) )
		{
			pTool->SetAuxSourceImage( m_aImageList.GetAt( index ) );
		}
	}

	refresh();
}

void SVToolAdjustmentDialogGeneralPageClass::OnShowRelations() 
{
	SVToolClass* pTool = nullptr;

	if (pSheet && (pTool = pSheet->GetTool()))
	{
		SVGUID toolId = pTool->GetUniqueObjectID();
		SVGUID inspectionID = pTool->GetInspection()->GetUniqueObjectID();
		
		SVShowDependentsDialog Dlg(inspectionID, toolId, false, SVToolObjectType, nullptr, SVShowDependentsDialog::DialogType::Show);
		Dlg.DoModal();
	}
}

BOOL SVToolAdjustmentDialogGeneralPageClass::OnSetActive() 
{
	SVToolClass* pTool = NULL;

	if( pSheet && ( pTool = pSheet->GetTool() ) )
	{
		m_aImageList.RemoveAll();

		// Auxiliary Extent Source Images.
		pTool->GetSourceImages( &m_aImageList );

		pTool->SetAuxSourceImage( pTool->GetAuxSourceImage() );

		m_AvailableSourceImageCombo.init( &m_aImageList, pTool->GetAuxSourceImage() );
	}

	refresh();

	return CPropertyPage::OnSetActive();
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVTADlgGeneralPage.cpp_v  $
 * 
 *    Rev 1.2   15 May 2014 12:50:40   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changed SendMessage to use proper type cast of DWORD_PTR.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   01 Feb 2014 12:08:54   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changed SVSendmessage and processmessage to use LONG_PTR instead of DWORD.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   24 Apr 2013 11:14:52   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.15   02 Jul 2012 17:40:18   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to promote new display functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.14   08 Dec 2010 13:39:12   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to include changes in notification functionality using the Observer Design Pattern.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.13   16 Jan 2006 17:13:16   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  534
 * SCR Title:  Add Image Select for Auxiliary Extent Source
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Moved removed embedded object to create object method for load of configuration to happen properly.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.12   24 Oct 2005 09:46:14   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  510
 * SCR Title:  Add source image extents to all image using tools
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changes to support adding a flag to the Inspection that will enable Auxiliary Extents.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.11   10 Oct 2005 13:08:00   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  510
 * SCR Title:  Add source image extents to all image using tools
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Fixed Spelling error for Auxiliary Extent IDs
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.10   09 Sep 2005 15:11:52   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  510
 * SCR Title:  Add source image extents to all image using tools
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Removed Auxilliary Extent option from gage tool class and dlg.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   01 Sep 2005 14:50:32   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  510
 * SCR Title:  Add source image extents to all image using tools
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added Auxilliary Group Box.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   01 Sep 2005 11:50:16   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  510
 * SCR Title:  Add source image extents to all image using tools
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   OnInitDialog - Changes for GUI
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   11 Aug 2005 13:28:22   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added UpdateData( FALSE ) to OnInitDialog method to make sure the inspection data was pushed to the dialog.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   09 Aug 2005 08:18:14   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated objects and dialogs to use the ActiveX Interface to update inspection data.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   21 Jun 2005 08:32:10   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  464
 * SCR Title:  Add array indexing for value objects
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   object attributes now use accessor methods
 * value object functions now use HRESULT
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   22 Apr 2003 16:12:10   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   20 Nov 2002 13:41:56   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  226
 * SCR Title:  Monochrome SVIM configuration compatibility between ViperQUAD and ViperDUAL
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   - Removed Call to EnableAutomation()
 * - Add ResultDataIndex to SetValue
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   19 Apr 2001 19:25:48   Steve
 * Project:  SVObserver
 * Change Request (SCR) nbr:  196
 * SCR Title:  Restructure Scripted Load/Save Procedures Functional Requirement
 * Checked in by:  Steve;  Stephen E. Steffan
 * Change Description:  
 *   Header file changes due to include file restructuring.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   19 Apr 2001 15:39:44   Steve
 * Project:  SVObserver
 * Change Request (SCR) nbr:  191
 * SCR Title:  Restructure File Management
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Header file changes due to include file restructuring.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   Feb 24 2000 12:04:18   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  111
 * SCR Title:  Integrate Conditional Tool Set/Tool Drawing
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Provides a page for general tool settings.
 * ( First General Setting: Implements access to tool draw flag).
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
