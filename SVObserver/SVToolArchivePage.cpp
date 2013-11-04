//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVToolAdjustmentArchivePage
//* .File Name       : $Workfile:   SVToolArchivePage.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 15:41:22  $
//******************************************************************************

#include "stdafx.h"
#include <numeric>
#include "SVToolArchivePage.h"

#include "SVObjectLibrary/SVObjectManagerClass.h"

#include "SVArchiveTool.h"
#include "SVGetObjectDequeByTypeVisitor.h"
#include "SVGlobal.h"
#include "SVIPDoc.h"
#include "SVImageClass.h"
#include "SVInspectionProcess.h"
#include "SVMemoryManager.h"
#include "SVToolAdjustmentDialogSheetClass.h"
#include "SVToolSet.h"

BEGIN_MESSAGE_MAP(SVToolAdjustmentArchivePage, CPropertyPage)
	//{{AFX_MSG_MAP(SVToolAdjustmentArchivePage)
	ON_BN_CLICKED(IDC_BROWSE, OnBrowse)
	ON_BN_CLICKED(IDC_BROWSE2, OnBrowse2)
	ON_CBN_SELCHANGE(IDC_MODE_COMBO, OnSelchangeModeCombo)
	ON_EN_CHANGE(IDC_EDIT_MAX_IMAGES, OnChangeEditMaxImages)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
//
//
//
SVToolAdjustmentArchivePage::SVToolAdjustmentArchivePage( 
	SVToolAdjustmentDialogSheetClass* Parent ) 
	: CPropertyPage(SVToolAdjustmentArchivePage::IDD)
{
	//{{AFX_DATA_INIT(SVToolAdjustmentArchivePage)
	m_checkAppendArchive = 0;
	m_checkStopAtMaxImages = FALSE;
	m_iModeIndex = -1;
	m_strAvailableArchiveImageMemory = _T("");
	//}}AFX_DATA_INIT

	m_pParentDialog = Parent;
	m_pTool = NULL;

	if( m_pParentDialog )
	{
		m_pTool = dynamic_cast <SVArchiveTool*> (m_pParentDialog->GetTool());
	}
}

/////////////////////////////////////////////////////////////////////////////
//
//
//
SVToolAdjustmentArchivePage::~SVToolAdjustmentArchivePage()
{
}

/////////////////////////////////////////////////////////////////////////////
//
//
//
void SVToolAdjustmentArchivePage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(SVToolAdjustmentArchivePage)
	DDX_Control(pDX, IDC_AVAILABLE_ARCHIVE_IMAGE_MEMORY, m_wndAvailableArchiveImageMemory);
	DDX_Control(pDX, IDC_TEXT_AVAILABLE_ARCHIVE_IMAGE_MEMORY, m_wndTxtAvailableArchiveImageMemory);
	DDX_Control(pDX, IDC_MODE_COMBO, m_cbMode);
	DDX_Control(pDX, IDC_EDIT_MAX_IMAGES, m_editMaxImages);
	DDX_Control(pDX, IDC_ARCHIVE_IMAGE_FILEROOT, m_editImageFilesRoot);
	DDX_Control(pDX, IDC_TREE_RESULTS, m_treeResultsList);
	DDX_Control(pDX, IDC_TREE_IMAGES, m_treeImagesList);
	DDX_Control(pDX, IDC_ARCHIVE_FILENAME, m_editArchiveFileName);
	DDX_Check(pDX, IDC_CHECK_APPEND, m_checkAppendArchive);
	DDX_Check(pDX, IDC_CHECK_STOP_AT_MAX, m_checkStopAtMaxImages);
	DDX_CBIndex(pDX, IDC_MODE_COMBO, m_iModeIndex);
	DDX_Text(pDX, IDC_AVAILABLE_ARCHIVE_IMAGE_MEMORY, m_strAvailableArchiveImageMemory);
	//}}AFX_DATA_MAP
}


/////////////////////////////////////////////////////////////////////////////
//
//
//
BOOL SVToolAdjustmentArchivePage::OnInitDialog() 
{
	CWaitCursor wait;                 // 23 Nov 1999 - frb.

	CPropertyPage::OnInitDialog();

	CDWordArray dwaIndex;
	int iIndex=0;
	
	SVEnumerateVector vec;
	m_pTool->m_evoArchiveMethod.GetEnumTypes(vec);
	for ( size_t i=0; i < vec.size(); i++ )
	{
		m_cbMode.SetItemData( iIndex = m_cbMode.AddString(vec[i].first), vec[i].second );
		dwaIndex.SetAtGrow( vec[i].second, iIndex );
	}

	unsigned long lMode;
	m_pTool->m_evoArchiveMethod.GetValue( lMode );
	m_eSelectedArchiveMethod = static_cast<SVArchiveMethodEnum>( lMode );
	m_iModeIndex = dwaIndex.GetAt( lMode );

	//
	// Get a pointer to the toolset
	//
	SVToolSetClass* pToolSet = m_pTool->GetInspection()->GetToolSet();
	SVTaskObjectListClass* pTaskObjectList = static_cast <SVTaskObjectListClass*> ( pToolSet );

	//
	// Get the list of already existing selected results to archive
	// from the archive tool.
	//
	m_pTool->UpdateTaskObjectOutputList();

	DWORD dwTemp=0;
    m_pTool->m_dwArchiveMaxImagesCount.GetValue( dwTemp );
	m_lImagesToArchive = dwTemp;
	CString s;
	s.Format(_T("%ld"),dwTemp);
	m_editMaxImages.SetWindowText((LPCTSTR)s);

	long lMemUsed = TheSVMemoryManager().ReservedBytes( ARCHIVE_TOOL_MEMORY_POOL_GO_OFFLINE_NAME );
	m_lToolImageMemoryUsage = 0;
	m_lTotalArchiveImageMemoryAvailable = TheSVMemoryManager().SizeOfPoolBytes( ARCHIVE_TOOL_MEMORY_POOL_GO_OFFLINE_NAME );
	m_lInitialArchiveImageMemoryUsage = lMemUsed;

	m_wndAvailableArchiveImageMemory.ShowWindow( lMode == SVArchiveGoOffline );
	m_wndTxtAvailableArchiveImageMemory.ShowWindow( lMode == SVArchiveGoOffline );

	//
	// Initialize and build the 'tree' in the tree control.
	//
	m_treeResultsList.InitOutputListTreeCtrl();
	m_treeImagesList.InitOutputListTreeCtrl();

	m_treeImagesList.SetCanSelectObjectCallback( SVObjectTreeCanSelectObjectCallbackFn(this, &SVToolAdjustmentArchivePage::CanSelectObjectCallback) );

	//
	// Build the output list and set the object 'attributes' to use.
	//
	m_treeResultsList.BuildOutputList(pTaskObjectList, SV_ARCHIVABLE);

	//
	// Get the current path to the archive file if any.
	//
	CString		csArchiveFileName; 
	
	m_pTool->GetFileArchive( csArchiveFileName );
	m_editArchiveFileName.SetWindowText(csArchiveFileName);

	
	CString		csImageFolder; 
	
	m_pTool->GetImageArchivePath( csImageFolder );
	m_editImageFilesRoot.SetWindowText(csImageFolder);

	//
	// Set other dialog controls.
	//
	m_pTool->m_dwAppendArchiveFile.GetValue( dwTemp );
	m_checkAppendArchive = (int)dwTemp;

	m_pTool->m_dwArchiveStopAtMaxImages.GetValue( dwTemp );
	m_checkStopAtMaxImages = (int)dwTemp;

    //
	// Build the image list to select archivable images from.
	//
	BuildImageList();

	UpdateData(FALSE);   // To Controls.

	CalculateFreeMem();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX-Eigenschaftenseiten sollten FALSE zurückgeben
}

/////////////////////////////////////////////////////////////////////////////
//
// Build a list of all archivable images.  
// Set selected attributes if already selected, i.e. in images archive list.
// Built the 'tree' for the tree control.
//
void SVToolAdjustmentArchivePage::BuildImageList()
{
    SVObjectTypeInfoStruct  info;

	//
	// Get a pointer to the toolset
	//
	SVToolSetClass* pToolSet = m_pTool->GetInspection()->GetToolSet();

	SVErrorClass msvError;
	msvError.ClearLastErrorCd ();

	BOOL lDone = FALSE;

	info.ObjectType = SVImageObjectType;
	// SVMainImageObjectType  possible sub-types??
	// SVNotSetSubObjectType
	// SVImageROIObjectType
	info.SubType = SVNotSetSubObjectType;

	SVGetObjectDequeByTypeVisitor l_Visitor( info );

	SVObjectManagerClass::Instance().VisitElements( l_Visitor, pToolSet->GetUniqueObjectID() );

	SVGetObjectDequeByTypeVisitor::SVObjectPtrDeque::const_iterator l_Iter;

	for( l_Iter = l_Visitor.GetObjects().begin(); l_Iter != l_Visitor.GetObjects().end(); ++l_Iter )
	{
		SVImageClass* pImage = dynamic_cast< SVImageClass* >( const_cast< SVObjectClass* >( *l_Iter ) );

		if( pImage != NULL )
		{
			m_imageListAll.Add(pImage);
		}
	}
	/*
	pImage = static_cast <SVImageClass*>
	                     ( reinterpret_cast <SVObjectClass*>
	                        ( SVSendMessage (  static_cast <SVObjectClass*> (pToolSet), 
	                                           SVM_GETFIRST_OBJECT,
	                                           NULL,
	                                           reinterpret_cast <DWORD> (&info)
	                                         )
	                        )
	                      );

	if(pImage)
	{
		m_imageListAll.Add(pImage);
	}

	while (pImage && !lDone)
	{
		pImage = static_cast <SVImageClass*>
		                     ( reinterpret_cast <SVObjectClass*> 
		                        ( SVSendMessage (  static_cast <SVObjectClass*> (pToolSet), 
		                                           SVM_GETNEXT_OBJECT,
		                                           reinterpret_cast <DWORD> (pImage),
		                                           reinterpret_cast <DWORD> (&info)
		                                         )
		                         )
		                     );
		if(pImage)
		{
			m_imageListAll.Add(pImage);
		}
	}
	*/

	//
	// Set the archivable attributes in images based on.
	//
	m_pTool->SetImageAttributesFromArchiveList(&m_imageListAll);
	
	//
	// Now build the 'tree' in the tree list control.
	//
	m_treeImagesList.BuildImageListTree(
		&m_imageListAll, // SVImageListClass
		SV_ARCHIVABLE_IMAGE, // UINT Attributes we desire
		SVNotSetObjectType   // SVObjectTypeEnum
	);

	SVObjectListClass l_ObjectList;
	m_treeImagesList.GetCheckedObjects( &l_ObjectList );

	m_mapSelectedImageMemUsage.clear();

	for ( int i=0; i < l_ObjectList.GetSize(); i++ )
	{
		SVObjectReference refObject = l_ObjectList.GetAt(i);
		SVImageClass* pImage = dynamic_cast <SVImageClass*> ( refObject.Object() );
		if ( pImage )
		{
			m_mapSelectedImageMemUsage[ pImage ] = SVArchiveTool::CalculateImageMemory( pImage );
		}
	}

	m_mapInitialSelectedImageMemUsage = m_mapSelectedImageMemUsage;

	m_lToolImageMemoryUsage = CalculateToolMemoryUsage();

	m_lInitialToolImageMemoryUsage = m_lToolImageMemoryUsage;

	m_lInitialArchiveImageMemoryUsageExcludingThisTool = m_lInitialArchiveImageMemoryUsage;

	if ( m_eSelectedArchiveMethod == SVArchiveGoOffline )
	{
		m_lInitialArchiveImageMemoryUsageExcludingThisTool -= m_lToolImageMemoryUsage;
	}

	return;
}


/////////////////////////////////////////////////////////////////////////////
//
//
// Called by Property Sheet (parent of this page) when OK button pressed.
//
bool SVToolAdjustmentArchivePage::QueryAllowExit() 
{
	UpdateData(TRUE);        

	//
	// Update the file path to the archive file for associated archive tool.
	//
	CString csArchiveFileName;
	m_editArchiveFileName.GetWindowText( csArchiveFileName );

	//check for valid drive for text archive
	
	CString szDrive;

	SVCheckPathDir( csArchiveFileName, TRUE );
	/*
	bOk = access( csImagePath, 0 ) == 0;

	if ( !bOk )
	{
		SVFileNameManagerClass svFileManager;

		bOk = svFileManager.CreatePath( csImagePath );
	}
	*/

	if(!m_pTool->ValidateDrive(csArchiveFileName,szDrive) || csArchiveFileName.IsEmpty())
	{
		CString temp;
		temp.Format ("Invalid drive:  %s", szDrive);
		AfxMessageBox (temp);
		return false; 
	}


	//update the image path
	CString csImageFolder;
	m_editImageFilesRoot.GetWindowText( csImageFolder );

	SVCheckPathDir( csImageFolder, TRUE );

	//check for valid drive for image archive
	if(!m_pTool->ValidateDrive(csImageFolder,szDrive ) || csImageFolder.IsEmpty())
	{
		CString temp;
		temp.Format ("Invalid drive:  %s", szDrive);
		AfxMessageBox (temp);
		return false; 
	}


	//
	// Check for possible duplicate path to results archive in any other
	// archive tool in any other toolset, i.e. it must be unique on the 
	// NT file system.
	//
	BOOL bResult = m_pTool->CheckForUniqueArchiveFilePath(csArchiveFileName);
	if(!bResult)
	{
		CString s;
		s.Format(	_T("ERROR: Archive File is not unique in system:\nChange archive file name:\n%s"),
							csArchiveFileName);
		AfxMessageBox(s);
		return false;   // Property is ready to exit.
	}

	m_pTool->SetFileArchive( csArchiveFileName );

	m_pTool->SetImageArchivePath( csImageFolder );

	m_pTool->m_dwAppendArchiveFile.SetValue( 1, m_checkAppendArchive );
	m_pTool->m_dwArchiveStopAtMaxImages.SetValue( 1, m_checkStopAtMaxImages );


	int iCurSel = m_cbMode.GetCurSel();
	m_eSelectedArchiveMethod = static_cast <SVArchiveMethodEnum> (m_cbMode.GetItemData(iCurSel));
	m_pTool->m_evoArchiveMethod.SetValue( 1, static_cast <DWORD> (m_eSelectedArchiveMethod) );

	CString s;
	m_editMaxImages.GetWindowText(s);
	DWORD dwTemp = atol((LPCTSTR)s);
	m_lImagesToArchive = dwTemp;
	if(dwTemp > 100L)
	{
		//dwTemp = 100L;
		CString s;
		s.Format(	_T("WARNING: You have selected %ld for the Max Images count"),
							dwTemp);
		AfxMessageBox((LPCTSTR)s);
	}
	if(dwTemp < 1L)
	{
		dwTemp = 1L;
	}

	m_pTool->m_dwArchiveMaxImagesCount.SetValue( 1, dwTemp );

	//
	// Make sure the tree 'states' are converted into 
	// selected 'attributes'.
	//
	m_treeResultsList.UpdateAttributesInOutputList();
	m_treeImagesList.UpdateAttributesInOutputList();

	//
	// Now make a list of archivable objects from the SVOutputInfoListClass.
	//
	m_pTool->RebuildResultsArchiveList();
	m_pTool->RebuildImageArchiveList();

	//
	// Mark the document as 'dirty' so user will be promped to save
	// this configuration on program exit.
	//
	SVIPDoc* l_pIPDoc = NULL;

	l_pIPDoc = SVObjectManagerClass::Instance().GetIPDoc( m_pTool->GetInspection()->GetUniqueObjectID() );

	if( l_pIPDoc != NULL )
	{
		l_pIPDoc->SetModifiedFlag();
	}

	SVSendMessage( m_pTool, SVM_RESET_ALL_OBJECTS, NULL, NULL );

	return true;   // Everything is OK
}


/////////////////////////////////////////////////////////////////////////////
//
//
//
//
void SVToolAdjustmentArchivePage::OnBrowse() 
{
	SVFileNameClass	svfncArchiveFileName;

	svfncArchiveFileName.SetFileType(SV_DEFAULT_FILE_TYPE);
	if (svfncArchiveFileName.SelectFile())
	{
		m_editArchiveFileName.SetWindowText(svfncArchiveFileName.GetFullFileName());
	}
}

/////////////////////////////////////////////////////////////////////////////
//
//
//
void SVToolAdjustmentArchivePage::OnBrowse2() 
{
	SVFileNameClass	svfncImageFolder;

	//
	// Set the default folder to start with.
	//
	CString csInitialPath(_T("C:\\Temp\\"));

	m_editImageFilesRoot.GetWindowText(csInitialPath);
	if (!csInitialPath.IsEmpty())
		svfncImageFolder.SetPathName(csInitialPath);

    //
    // Select the folder to copy to..
    //
	svfncImageFolder.SetFileType(SV_DEFAULT_FILE_TYPE);
	if (svfncImageFolder.SelectPath())
    {
		csInitialPath = svfncImageFolder.GetPathName();
		if (csInitialPath.GetAt(csInitialPath.GetLength() - 1) != (TCHAR)'\\')
			csInitialPath += (TCHAR)'\\';
    }

	m_editImageFilesRoot.SetWindowText (csInitialPath);
}

bool SVToolAdjustmentArchivePage::CanSelectObjectCallback( SVObjectReference refObject, bool bCurrentState, int )
{
	UpdateData();
	bool bOk = true;

	long l_lFreeMem = CalculateFreeMem();

	//if ( m_eSelectedArchiveMethod == SVArchiveGoOffline )	// do the calculations regardless
	{
		SVImageClass* pImage = dynamic_cast <SVImageClass*> ( refObject.Object() );
		ASSERT( pImage );

		long lMemory = SVArchiveTool::CalculateImageMemory( pImage );
		lMemory *= m_lImagesToArchive;

		if ( bCurrentState == false )// want to select
		{
			long lDelta = lMemory - m_lInitialToolImageMemoryUsage + m_lToolImageMemoryUsage;
			bool bCanReserve = ( l_lFreeMem - lDelta >= 0 );
			if ( bCanReserve && (lDelta > 0) )
				bCanReserve = TheSVMemoryManager().CanReservePoolMemory( ARCHIVE_TOOL_MEMORY_POOL_GO_OFFLINE_NAME, lDelta );
			if ( bCanReserve )
			{
				m_mapSelectedImageMemUsage[ pImage ] = lMemory / m_lImagesToArchive;
				long lFreeMem = CalculateFreeMem();
			}
			else
			{
				if ( m_eSelectedArchiveMethod == SVArchiveGoOffline )
				{
					bOk = false;
					ASSERT( bOk );
					CString strMessage;
					strMessage.Format(_T("Not enough Archive Tool image memory to select %s"), pImage->GetCompleteObjectName());
					AfxMessageBox( strMessage );
				}
			}
		}
		else	// want to deselect
		{
			m_mapSelectedImageMemUsage.erase( pImage );
			long lFreeMem = CalculateFreeMem();

			if ( lFreeMem < 0 )
			{
				AfxMessageBox(_T("Not enough Archive Tool image memory for the images selected. Please deselect some images."));
			}
		}
	}

	return bOk;
}

void SVToolAdjustmentArchivePage::OnSelchangeModeCombo() 
{
	UpdateData();
	int iSel = m_cbMode.GetCurSel();
	if ( iSel != CB_ERR )
	{
		m_eSelectedArchiveMethod = static_cast <SVArchiveMethodEnum> (m_cbMode.GetItemData( iSel ));
		m_wndAvailableArchiveImageMemory.ShowWindow( m_eSelectedArchiveMethod == SVArchiveGoOffline );
		m_wndTxtAvailableArchiveImageMemory.ShowWindow( m_eSelectedArchiveMethod == SVArchiveGoOffline );
		CalculateFreeMem();
	}
}

void SVToolAdjustmentArchivePage::OnChangeEditMaxImages() 
{
	CString strNumImages;
	m_editMaxImages.GetWindowText(strNumImages);
	m_lImagesToArchive = atol(strNumImages);
	CalculateFreeMem();
}

long SVToolAdjustmentArchivePage::CalculateToolMemoryUsage()
{
	long l_lToolImageMemoryUsage = 0;
	MapSelectedImageType::iterator iter;
	for ( iter = m_mapSelectedImageMemUsage.begin(); iter != m_mapSelectedImageMemUsage.end(); ++iter )
		l_lToolImageMemoryUsage += iter->second;
	l_lToolImageMemoryUsage *= m_lImagesToArchive;
	long lToolImageMemoryUsage = l_lToolImageMemoryUsage;
	return lToolImageMemoryUsage;
}

long SVToolAdjustmentArchivePage::CalculateFreeMem()
{
	m_lToolImageMemoryUsage = CalculateToolMemoryUsage();
	long lFreeMem = m_lTotalArchiveImageMemoryAvailable - (m_lToolImageMemoryUsage + m_lInitialArchiveImageMemoryUsageExcludingThisTool);
	m_strAvailableArchiveImageMemory.Format( _T("%8.1f MB"), (double) lFreeMem / (double) (1024 * 1024) );
	UpdateData(FALSE);
	return lFreeMem;
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVToolArchivePage.cpp_v  $
 * 
 *    Rev 1.0   23 Apr 2013 15:41:22   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.32   04 Oct 2012 11:00:18   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Changed from post incrementor to pre incrementor.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.31   30 Jul 2012 13:08:32   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated the parameter change for the visitor fucntionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.30   27 Jul 2012 09:05:14   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fixed include information and updated overlay collection functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.29   02 Jul 2012 17:50:38   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to promote new display functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.28   27 Jan 2011 12:03:00   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix issues with Matrox Buffer Management.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.27   08 Dec 2010 13:45:14   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to include changes in notification functionality using the Observer Design Pattern.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.26   21 Jun 2007 15:03:04   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  598
 * SCR Title:  Upgrade SVObserver to compile using vc++ in VS2005
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   These changes include modification based on fixing compiler-based and project-based differences between VC6 and VC8.  These changes mainly include casting issues, but some include type conversion and assignment of new compiler controlling defines.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.25   11 Apr 2006 15:14:54   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  559
 * SCR Title:  Fix Archive Tool dialog bug
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Fixed image selection bug - memory calculation
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.24   25 Jan 2006 12:40:20   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  529
 * SCR Title:  Add Conditional Product History
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   modified to match change in SVOutputInfoListTreeCtrl
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.23   13 Oct 2005 14:04:06   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Removed index 1 from GetValue calls.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.22   06 Oct 2005 07:47:02   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  450
 * SCR Title:  Add asynchronous functionality to the archive tool
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   fixed problem of incorrect Change Mode memory usage reported in Archive page
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.21   08 Sep 2005 13:09:42   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  450
 * SCR Title:  Add asynchronous functionality to the archive tool
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   changed ArchiveMethod from a DWord value object to an Enumerate value object
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.20   06 Sep 2005 08:42:44   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  450
 * SCR Title:  Add asynchronous functionality to the archive tool
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   fixed initialization bug
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.19   31 Aug 2005 10:14:44   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  450
 * SCR Title:  Add asynchronous functionality to the archive tool
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Modified text descriptions on dialog and in "when to archive" drop list.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.18   25 Aug 2005 15:00:10   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  450
 * SCR Title:  Add asynchronous functionality to the archive tool
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   fixed uninitialized variable problem
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.17   18 Aug 2005 08:33:24   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  455
 * SCR Title:  New Security for SVObserver
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   change "Going offline" to "Leave Run Mode"
 * change "Online asynchronous" to "Run Mode Asynchronous"
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.16   09 Aug 2005 08:49:28   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated objects and dialogs to use the ActiveX Interface to update inspection data.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.15   08 Aug 2005 15:20:38   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  450
 * SCR Title:  Add asynchronous functionality to the archive tool
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   fixed display of free mem on change max images
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.14   08 Aug 2005 14:00:34   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  450
 * SCR Title:  Add asynchronous functionality to the archive tool
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   renamed SVToolArchive to SVArchiveTool
 * implemented usage tracking
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.13   03 Aug 2005 14:56:50   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  450
 * SCR Title:  Add asynchronous functionality to the archive tool
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   change ArchivePageOK to QueryAllowExit
 * create paths on exit
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.12   20 Jun 2005 10:11:58   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  450
 * SCR Title:  Add asynchronous functionality to the archive tool
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   updated how the archive enums are referenced
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.11   20 May 2005 12:29:34   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  450
 * SCR Title:  Add asynchronous functionality to the archive tool
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added archive mode combo
 * archive member variable name changed
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.10   18 Feb 2005 07:26:12   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  456
 * SCR Title:  Update Image and Tool Objects to use the new Extent Classes
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Changes to support new Reset methodology.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.9   22 Apr 2003 16:46:52   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.8   20 Nov 2002 14:00:38   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  226
 * SCR Title:  Monochrome SVIM configuration compatibility between ViperQUAD and ViperDUAL
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   theSVObjectManager
 * ValueObject Data Indexes
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.7   14 Aug 2001 10:43:14   ed
 * Project:  SVObserver
 * Change Request (SCR) nbr:  220
 * SCR Title:  Fix invalid path message when adding a Load Image tool
 * Checked in by:  Ed;  Ed Chobanoff
 * Change Description:  
 *   Added code in the SVToolAdjustmentArchivePage::ArchivePageOK() function to
 * validate the drive of the archive text file as well as the archive
 * image file. 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.6   13 Jun 2001 10:26:30   ed
 * Project:  SVObserver
 * Change Request (SCR) nbr:  200
 * SCR Title:  Update code to implement SVIM COM Server
 * Checked in by:  Ed;  Ed Chobanoff
 * Change Description:  
 *   Modified the following functions: OnInitDialog, ArchivePageOK.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.5   19 Apr 2001 20:00:36   Steve
 * Project:  SVObserver
 * Change Request (SCR) nbr:  196
 * SCR Title:  Restructure Scripted Load/Save Procedures Functional Requirement
 * Checked in by:  Steve;  Stephen E. Steffan
 * Change Description:  
 *   Header file changes due to include file restructuring.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.4   11 Apr 2001 16:36:38   cSchmittinger
 * Project:  SVObserver
 * Change Request (SCR) nbr:  191
 * SCR Title:  Restructure File Management
 * Checked in by:  cSchmittinger;  Carl Schmittinger
 * Change Description:  
 *   Changes were made in the way stringFileArchivePath and stringImageFileRootPath variables are handled
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.3   06 Feb 2001 13:47:48   cSchmittinger
 * Project:  SVObserver
 * Change Request (SCR) nbr:  191
 * SCR Title:  Restructure File Management
 * Checked in by:  cSchmittinger;  Carl Schmittinger
 * Change Description:  
 *   Modified source code to use new file management classes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.2.2.0   16 May 2001 14:12:20   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  202
 * SCR Title:  Operator, delete and cut should not be available in File Open/Save Dialogs
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Changed all occurences of CFileDialog to CSVFileDialog to enable the new security features and privileges
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.2   Jan 19 2000 15:12:08   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  87
 * SCR Title:  Archive Images - All Max Count greater than 100
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Allow more than 100 images to be archived.  If user selects number of images greater than 100, a dialog is displayed with a warning.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.1   Nov 30 1999 16:33:32   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  70
 * SCR Title:  Archive Tool fails to save selected toolset image in configuration.
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Changed software to ignore 'attributes allowed' as saved in configuration files for image objects.  This repairs the problem noted by this SCR.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.0   23 Nov 1999 13:51:46   mike
 * Project:  SVObserver
 * Change Request (SCR) nbr:  61
 * SCR Title:  Update PVCS versioning to version 3.0.
 * Checked in by:  Mike;  Mike McCarl
 * Change Description:  
 *   
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.12   Nov 17 1999 07:25:14   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  15
 * SCR Title:  Integrate archive tool
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Added check for unique results archive file paths.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.11   Nov 10 1999 12:38:42   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  57
 * SCR Title:  Version 3.00 Beta 18 Versioning
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Documents marked as changed when Dialog OK buttons pressed.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.10   Oct 26 1999 15:04:44   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  15
 * SCR Title:  Integrate archive tool
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Limit Archived images to a user selectable number.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   Oct 22 1999 14:24:00   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  15
 * SCR Title:  Integrate archive tool
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Implement the capability to append to results archive, and include a time stamp for first record archived on each 'run'.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   Oct 21 1999 17:11:20   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  15
 * SCR Title:  Integrate archive tool
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Repair object connections so objects deleted will report their dependencies if selected to be archived.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   Oct 20 1999 12:26:40   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  15
 * SCR Title:  Integrate archive tool
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Fix bug that causes crash if object with image to be archive  is removed from toolset.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   Oct 18 1999 12:55:26   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  15
 * SCR Title:  Integrate archive tool
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Image file arhive folder selection dialog.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   Oct 14 1999 11:29:12   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  15
 * SCR Title:  Integrate archive tool
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Image archive changes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   Oct 13 1999 14:37:48   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  15
 * SCR Title:  Integrate archive tool
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Changes required to archive images into .bmp files.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   Oct 06 1999 15:22:22   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  15
 * SCR Title:  Integrate archive tool
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   More archive work.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   Oct 05 1999 16:46:52   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  15
 * SCR Title:  Integrate archive tool
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Scripting work.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   Sep 16 1999 13:40:48   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  34
 * SCR Title:  Add Build Reference Tool
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Class name changed from SVArchiveToolClass to SVToolArchive.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   Sep 15 1999 15:58:54   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  15
 * SCR Title:  Integrate archive tool
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Added a Archive configuration 'page' to the tool adjust property 'sheet'.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
