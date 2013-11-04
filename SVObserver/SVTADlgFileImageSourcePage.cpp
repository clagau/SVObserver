//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVToolAdjustmentDialogFileImageSourcePage
//* .File Name       : $Workfile:   SVTADlgFileImageSourcePage.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   24 Apr 2013 11:14:24  $
//******************************************************************************

//******************************************************************************
//* INCLUDE(S):
//******************************************************************************

////////////////////////////////////////////////////////////////////////////////
// General Include File(s)
////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SVTADlgFileImageSourcePage.h"

#include "SVImageClass.h"
#include "SVIPDoc.h"
#include "SVTool.h"
#include "SVToolAdjustmentDialogSheetClass.h"

//******************************************************************************
//* DEFINITIONS OF MODULE-LOCAL VARIABLES:
//******************************************************************************

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


//******************************************************************************
//* CLASS METHOD IMPLEMENTATION(S):
//******************************************************************************



//*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/
//* Class Name : SVToolAdjustmentDialogFileImageSourcePageClass
//* Note(s)    : Property Page
//*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/


////////////////////////////////////////////////////////////////////////////////
// .Title       : SVToolAdjustmentDialogFileImageSourcePageClass
// -----------------------------------------------------------------------------
// .Description : ...
//              :
// -----------------------------------------------------------------------------
// .Input(s)
//	 Type				Name				Description
//	: 
//  :
// .Output(s)
//	:
//  :
// .Return Value
//	: 
// -----------------------------------------------------------------------------
// .Import Function Reference(s)
//	:
// -----------------------------------------------------------------------------
// .Import Variable Reference(s)
//	:
////////////////////////////////////////////////////////////////////////////////
// .History
//	 Date		Author		Comment                                       
//  :01.06.1998 RO			First Implementation
////////////////////////////////////////////////////////////////////////////////
SVToolAdjustmentDialogFileImageSourcePageClass::SVToolAdjustmentDialogFileImageSourcePageClass( SVToolAdjustmentDialogSheetClass* PParent ) : CPropertyPage( SVToolAdjustmentDialogFileImageSourcePageClass::IDD )
{
	//{{AFX_DATA_INIT(SVToolAdjustmentDialogFileImageSourcePageClass)
	StrPathName = _T("");
	BContinuousReload = FALSE;
	//}}AFX_DATA_INIT

	pImage			= NULL;
	pReloadObject	= NULL;
	pPathNameObject = NULL;
	pSheet = PParent;
	pTool  = NULL;
}

SVToolAdjustmentDialogFileImageSourcePageClass::~SVToolAdjustmentDialogFileImageSourcePageClass()
{
}

HRESULT SVToolAdjustmentDialogFileImageSourcePageClass::SetInspectionData()
{
	HRESULT l_hrOk = S_FALSE;

	if( pTool != NULL )
	{
		UpdateData( TRUE ); // get data from dialog

		l_hrOk = AddInputRequest( pPathNameObject, svfncImageSourceFile.GetFullFileName() );

		if( l_hrOk == S_OK )
		{
			l_hrOk = AddInputRequest( pReloadObject, BContinuousReload );
		}

		if( l_hrOk == S_OK )
		{
			l_hrOk = AddInputRequestMarker();
		}

		if( l_hrOk == S_OK )
		{
			l_hrOk = RunOnce( pTool );
		}

		pReloadObject->GetValue( BContinuousReload );
		pPathNameObject->GetValue( StrPathName );

		UpdateData( FALSE );
	}

	return l_hrOk;
}

void SVToolAdjustmentDialogFileImageSourcePageClass::refresh()
{
	SetInspectionData();

	imageCtrl.refresh();
}

void SVToolAdjustmentDialogFileImageSourcePageClass::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(SVToolAdjustmentDialogFileImageSourcePageClass)
	DDX_Control(pDX, IDC_IMAGE, imageCtrl);
	DDX_Text(pDX, IDC_IMAGE_SOURCE_EDIT, StrPathName);
	DDX_Check(pDX, IDC_RELOAD_CHECK, BContinuousReload);
	//}}AFX_DATA_MAP
}

//******************************************************************************
// Message Map Entries
//******************************************************************************

BEGIN_MESSAGE_MAP(SVToolAdjustmentDialogFileImageSourcePageClass, CPropertyPage)
	//{{AFX_MSG_MAP(SVToolAdjustmentDialogFileImageSourcePageClass)
	ON_BN_CLICKED(ID_BROWSE_BUTTON, OnBrowseButton)
	ON_BN_CLICKED(IDC_RELOAD_CHECK, OnReloadCheck)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//******************************************************************************
// Message Handler
//******************************************************************************

BOOL SVToolAdjustmentDialogFileImageSourcePageClass::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();

	if( pSheet && ( pTool = pSheet->GetTool() ) )
	{
		SetTaskObject( pTool );

		// Find Image...
		SVObjectTypeInfoStruct imageObjectInfo;
		imageObjectInfo.ObjectType = SVImageObjectType;
		pImage = ( SVImageClass* ) ::SVSendMessage( pTool, SVM_GETFIRST_OBJECT, NULL, ( DWORD ) &imageObjectInfo );

		// Find Reload...
		SVObjectTypeInfoStruct reloadObjectInfo;
		reloadObjectInfo.ObjectType = SVBoolValueObjectType;
		reloadObjectInfo.EmbeddedID = SVContinuousReloadObjectGuid;
		pReloadObject = ( SVBoolValueObjectClass* ) ::SVSendMessage( pTool, SVM_GETFIRST_OBJECT, NULL, ( DWORD ) &reloadObjectInfo );

		// Find PathName...
		SVObjectTypeInfoStruct pathNameObjectInfo;
		pathNameObjectInfo.ObjectType = SVStringValueObjectType;
		pathNameObjectInfo.EmbeddedID = SVPathNameObjectGuid;
		pPathNameObject = ( SVFileNameValueObjectClass* ) ::SVSendMessage( pTool, SVM_GETFIRST_OBJECT, NULL, ( DWORD ) &pathNameObjectInfo );

		if( pImage && pReloadObject && pPathNameObject )
		{
			imageCtrl.UpdateImageInfo( pImage );

			pReloadObject->GetValue( BContinuousReload );
			pPathNameObject->GetValue( StrPathName );

			UpdateData( FALSE ); // set data to dialog

			refresh();

			return TRUE;
		}
	}

	// Not valid call...try to close!
	if( GetParent() )
		GetParent()->SendMessage( WM_CLOSE );
	else
		SendMessage( WM_CLOSE );

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX-Eigenschaftenseiten sollten FALSE zurückgeben
}


void SVToolAdjustmentDialogFileImageSourcePageClass::OnBrowseButton() 
{

	UpdateData( TRUE );

	if( pTool && pReloadObject && pPathNameObject )
	{
		//
		// Try to read the current image file path name from registry...
		//
		CString csPath = AfxGetApp()->GetProfileString(	_T("Settings"),
																										_T( "ImagesFilePath" ), 
																										_T("C:\\Images"));
		
		svfncImageSourceFile.SetPathName(csPath);
		svfncImageSourceFile.SetFileType(SV_IMAGE_SOURCE_FILE_TYPE);
		if (svfncImageSourceFile.SelectFile())
		{
			//
			// Save the directory selected from..
			//
			csPath = svfncImageSourceFile.GetPathName();
			
			//
			// Write this path name back to registry...to initialize registry.
			//
			AfxGetApp()->WriteProfileString(_T("Settings"),
																			_T( "ImagesFilePath" ), 
																			csPath);

			refresh();
		}
	}
}

void SVToolAdjustmentDialogFileImageSourcePageClass::OnReloadCheck() 
{
	refresh();
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVTADlgFileImageSourcePage.cpp_v  $
 * 
 *    Rev 1.0   24 Apr 2013 11:14:24   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.14   02 Jul 2012 17:40:18   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to promote new display functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.13   07 Apr 2011 16:39:16   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated SVDlgImageClass to change the method init to UpdateImageInfo to make it trackable throughout the system.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.12   16 Aug 2005 09:45:48   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Removed un-necessary casting from AddInputRequest method.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.11   09 Aug 2005 08:18:18   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated objects and dialogs to use the ActiveX Interface to update inspection data.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.10   21 Jun 2005 08:32:10   ebeyeler
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
 *    Rev 3.9   22 Feb 2005 10:49:36   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  456
 * SCR Title:  Update Image and Tool Objects to use the new Extent Classes
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fixed load image tool problems.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.8   17 Feb 2005 15:49:18   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  456
 * SCR Title:  Update Image and Tool Objects to use the new Extent Classes
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Modifications to support new extents.  Also use reset objects rather than recreating them.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.7   22 Apr 2003 16:11:56   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.6   21 Mar 2003 12:32:30   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  330
 * SCR Title:  Add SetImageList to ActiveX Interface
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   setting a flag so that the image will get updated upon exit.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.5   04 Mar 2003 11:46:58   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  327
 * SCR Title:  Load Image Tool - unless the reload is checked you can not preview the image
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   added refresh() to OnInitDialog()
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.4   20 Nov 2002 13:39:14   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  226
 * SCR Title:  Monochrome SVIM configuration compatibility between ViperQUAD and ViperDUAL
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Add ResultDataIndex and ResultImageIndex
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.3   13 Jun 2001 10:14:18   ed
 * Project:  SVObserver
 * Change Request (SCR) nbr:  200
 * SCR Title:  Update code to implement SVIM COM Server
 * Checked in by:  Ed;  Ed Chobanoff
 * Change Description:  
 *   Modified the following functions: OnInitDialog.
 * Changed pPathNameObject from SVStringValueObjectClass* to SVFileNameValueObjectClass*.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.2   06 Feb 2001 13:44:30   cSchmittinger
 * Project:  SVObserver
 * Change Request (SCR) nbr:  191
 * SCR Title:  Restructure File Management
 * Checked in by:  cSchmittinger;  Carl Schmittinger
 * Change Description:  
 *   Modified source code to use new file management classes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.1.1.0   16 May 2001 14:11:48   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  202
 * SCR Title:  Operator, delete and cut should not be available in File Open/Save Dialogs
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Changed all occurences of CFileDialog to CSVFileDialog to enable the new security features and privileges
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.1   Nov 30 1999 16:21:34   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  69
 * SCR Title:  Load Image tool fails to load image file
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Repaired loading saved image tool.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.0   23 Nov 1999 13:40:34   mike
 * Project:  SVObserver
 * Change Request (SCR) nbr:  61
 * SCR Title:  Update PVCS versioning to version 3.0.
 * Checked in by:  Mike;  Mike McCarl
 * Change Description:  
 *   
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   Sep 24 1999 08:59:18   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  4
 * SCR Title:  Add "Load image" option
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Provides for tool for loading an image from a file.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
