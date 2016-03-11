//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVArchiveTool
//* .File Name       : $Workfile:   SVArchiveTool.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.5  $
//* .Check In Date   : $Date:   15 May 2014 10:09:10  $
//******************************************************************************

#pragma once

#pragma region Includes
#include <deque>
#include <vector>

#include "SVMatroxLibrary\SVMatroxBuffer.h"
#include "SVTool.h"
#include "SVImageObjectClass.h"

#include "ArchiveMethodEnum.h"
#include "ArchiveToolHelper.h"
#include "SVArchiveRecord.h"
#include "SVArchiveRecordsArray.h"

#pragma endregion Includes

class SVImageListClass;
class SVArchiveRecord;

class SVArchiveTool : public SVToolClass
{
public:
	SVArchiveTool( 
		BOOL BCreateDefaultTaskList = FALSE, 
		SVObjectClass* POwner = NULL, 
		int StringResourceID = IDS_CLASSNAME_SVARCHIVETOOL
	    );
	virtual ~SVArchiveTool();

	virtual BOOL CreateObject( SVObjectLevelCreateStruct* PCreateStructure );
	virtual HRESULT ResetObject();
	void UpdateTaskObjectOutputList();
	void RebuildResultsArchiveList();
	void AddImageToArray(SVImageClass* pImage);
	void RebuildImageArchiveList();
	void SetImageAttributesFromArchiveList(SVImageListClass* pImageList);

	BOOL CheckForUniqueArchiveFilePath(LPCTSTR pszArchiveFilePathToTest);

	BOOL GetFileArchive( CString& rcsName );
	BOOL GetImageArchivePath( CString& rcsName );
	BOOL SetFileArchive( LPCTSTR lpszName );
	BOOL SetImageArchivePath( LPCTSTR lpszName );

	virtual BOOL Validate();
	virtual BOOL OnValidate();
	virtual BOOL SetObjectDepth( int NewObjectDepth );
	virtual BOOL SetObjectDepthWithIndex( int NewObjectDepth, int NewLastSetIndex );

	virtual DWORD_PTR processMessage( DWORD DwMessageID, DWORD_PTR DwMessageValue, DWORD_PTR DwMessageContext );

	long TotalImageMemoryUsage();

	static long CalculateImageMemory( SVImageClass* p_pImage );
	static long CalculateImageMemory( std::vector<SVImageClass*> p_apImages );

	//--ValidateArvhiveTool - called from the Tool Adjustment Dialog
	//-- This will cause the Archive Tool to run through a full validation cycle
	//--Previously the OnValidate would only run through the initial time or every 10th time
	//--This method guarantees that the OnValidate will run. 
	HRESULT ValidateArchiveTool();
	
	//--IsImagePathUsingKeywords - Called from SVArchiveRecord::BuildArchiveImageFilePath
	//--when archiving images it needs to know if the images has keywords in it
	bool isImagePathUsingKeywords();

	//--GetTranslatedImagePath -Called from SVArchiveRecord::BuildArchiveImageFilePath
	//--to get the already translated path.
	void getTranslatedImagePath(CString &ImagePath);

	//
	// The arrays for results and images to archive.
	//
	SVArchiveRecordsArray  m_arrayResultsInfoObjectsToArchive;
	SVArchiveRecordsArray  m_arrayImagesInfoObjectsToArchive;

	//
	// A string value to represent all the Guids for the
	// images so they can
	// be stored and retrieved from storage.
	//
	SVStringValueObjectClass    m_stringArchiveImageGuids_OBSOLETE;
	SVStringValueObjectClass    m_stringArchiveResultGuids_OBSOLETE;
	SVStringValueObjectClass    m_svoArchiveImageNames;
	/// This variable holds an array of result names with an Zero based Index (In the svx  files these names have also a zero based index). 
	SVStringValueObjectClass    m_svoArchiveResultNames;

	SVDWordValueObjectClass     m_dwAppendArchiveFile;
	SVDWordValueObjectClass     m_dwArchiveStopAtMaxImages;
	SVDWordValueObjectClass     m_dwArchiveMaxImagesCount;
	SVEnumerateValueObjectClass m_evoArchiveMethod;

	SVArchiveMethodEnum         m_eArchiveMethod;
	SVBoolValueObjectClass      m_bvoUseHeaders;
	SVStaticStringValueObjectClass m_HeaderLabelNames;
	SVStaticStringValueObjectClass m_HeaderObjectGUIDs;

protected:
	SV_DECLARE_CLASS( SVArchiveTool )

	void PutSelectedObjectGuidsIntoObjectScript(
		CString& RStrScript, CString& RStrAliasTable, int Indent );

    virtual BOOL   onRun( SVRunStatusClass& RRunStatus );

	HRESULT QueueArchiveString( CString strArchiveString );

	virtual BOOL renameToolSetSymbol( const SVObjectClass* pObject, LPCTSTR originalName);

	//
	// Data elements.
	//
	HRESULT WriteBuffers();
    CFile m_fileArchive;       // The file for archived results.
	UINT m_uiValidateCount;

    //
    // A flag used to indicate a first call to onRun() after a
    // going online message.
    //
    BOOL m_bInitializedForRun;

	std::vector<CString> m_aArchiveStringBuffer;

private:
	void initializeArchiveTool();
    HRESULT initializeOnRun();        // 24 Jan 2000 - frb.
	HRESULT AllocateImageBuffers();
	BOOL CreateTextArchiveFile();
	void UpdateImagePointerInImageArray( SVArchiveRecord* pImageRecord	);

	SVStringValueObjectClass	m_stringImageFileRootPath;
	SVStringValueObjectClass	m_stringFileArchivePath;

	//--m_bDriveError will will hold the error state of the drive.
	//--will be true if the hard drive has less than 100 MG of free space
	//--if images are selected for archive.
	bool m_bDriveError;
	CString m_ImageTranslatedPath;
	bool m_ArchiveImagePathUsingKW;
};


//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVArchiveTool.h_v  $
 * 
 *    Rev 1.5   15 May 2014 10:09:10   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Revised processMessage to use DWORD_PTR
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   23 Apr 2014 11:17:08   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  903
 * SCR Title:  Fix Archive Tool Bug where inputs get lost when inspection name changes.
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Removed const from the SVStringValueObjectClass& parameter of InitialezeObjects because this function will modify the class when an inspection is renamed.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   01 Feb 2014 10:16:32   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changed sendmessage to use LONG_PTR instead of DWORD.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   11 Nov 2013 07:10:00   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  872
 * SCR Title:  Add Archive Tool Headers to Archive File
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Add functionality for headers in text archive.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   08 May 2013 16:01:36   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 012.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.34   29 Apr 2013 14:04:40   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  830
 * SCR Title:  Consolidate all Time Stamp and Clock Functionality
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to use the new SVClock::SVTimeStamp type for all time stamp variables.  Update to use new SVClock::GetTimeStamp() function to get the time stamp value in milliseconds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 09:33:12   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.33   08 Jan 2013 11:50:40   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  805
 * SCR Title:  Fix Value Object Indexing Problem when Shortening PPQ
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added new virtual method to classes to fix adjusting Object Depth of Value Object and Last Set Index.  Also, updated code with new methodology.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.32   25 Jul 2012 13:59:00   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Removing dead code.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.31   18 Jun 2012 17:47:30   jspila
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
 *    Rev 3.30   10 Feb 2011 14:24:28   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to now use shared pointer instead of untracked structure.  This fixes destruction, tracking and rebuilding issues.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.29   28 Oct 2010 13:39:22   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  704
 * SCR Title:  Upgrade SVObserver Version for 5.33 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to remove duplicate definition of HRESULT.  The SVHRESULT definition will be removed in place of the Microsoft defined HRESULT.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.28   06 Nov 2009 16:22:00   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  633
 * SCR Title:  Upgrade Intek FireStack Driver and API to version 2.32
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   Updated issues with archiving and image tracking.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.27   30 Jul 2009 11:12:18   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Merged branced changes into current code branch with appropriate updates.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.26   24 Jul 2007 10:49:32   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  597
 * SCR Title:  Upgrade Matrox Imaging Library to version 8.0
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Update to Matrox Imaging Library 8.0
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.25.1.0   14 Jul 2009 11:56:18   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated to use the new data manager objects.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.25   02 Nov 2005 09:07:42   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  512
 * SCR Title:  Add Published Result Images functionality
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   implemented published result images
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.24   19 Oct 2005 11:22:22   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  450
 * SCR Title:  Add asynchronous functionality to the archive tool
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   fixed problem with inspection name being renamed. also will remove invalid objects during the reset.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.23   12 Oct 2005 10:48:46   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  450
 * SCR Title:  Add asynchronous functionality to the archive tool
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Update InitArchiveRecord method to remove un-necessary parameter and updated connect methodology to handle the images and value objects.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.22   06 Oct 2005 12:14:26   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  450
 * SCR Title:  Add asynchronous functionality to the archive tool
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Added new method to SVArchiveRecordClass: GetImage.  This method will verify that the archive record has a valid object before trying to get the GUID from that object.  It will then request the object, by GUID, from the object manager.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.21   06 Oct 2005 07:47:02   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  450
 * SCR Title:  Add asynchronous functionality to the archive tool
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   fixed problem of incorrect Change Mode memory usage reported in Archive page
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.20   27 Sep 2005 10:15:48   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  450
 * SCR Title:  Add asynchronous functionality to the archive tool
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   modified the Asynchronous mode behavior to match the functional requirements - allocate image buffers on the fly.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.19   08 Sep 2005 13:09:42   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  450
 * SCR Title:  Add asynchronous functionality to the archive tool
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   changed ArchiveMethod from a DWord value object to an Enumerate value object
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.18   11 Aug 2005 10:11:44   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  450
 * SCR Title:  Add asynchronous functionality to the archive tool
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   fixed input connection / disconnection problems
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.17   08 Aug 2005 14:00:34   ebeyeler
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
 *    Rev 3.16   26 Jul 2005 13:38:30   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  450
 * SCR Title:  Add asynchronous functionality to the archive tool
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   fixed file create bugs
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.15   14 Jul 2005 12:33:44   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  464
 * SCR Title:  Add array indexing for value objects
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   updated to work with array value objects
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.14   20 Jun 2005 10:06:16   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  450
 * SCR Title:  Add asynchronous functionality to the archive tool
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   implemented correct functionality for async and offline
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.13   20 May 2005 12:27:24   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  450
 * SCR Title:  Add asynchronous functionality to the archive tool
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   implemented Asynchronous archive tool methods
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.12   17 Feb 2005 13:05:44   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  456
 * SCR Title:  Update Image and Tool Objects to use the new Extent Classes
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated Create, Reset, and Object functionality based on SVObjectAppClass and new extents.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.11   13 May 2004 13:53:32   jBrown
 * Project:  SVObserver
 * Change Request (SCR) nbr:  422
 * SCR Title:  Image archiving across directories
 * Checked in by:  JimAdmin;  James A. Brown
 * Change Description:  
 *   SVArchiveRecord class was changed such that 
 * 
 * the BuildArchiveImageFilePath function return type was changed from BOOL to HRESULT.
 * 
 * the csPathRoot member variable was removed.
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.11   13 May 2004 13:53:18   jBrown
 * Project:  SVObserver
 * Change Request (SCR) nbr:  422
 * SCR Title:  Image archiving across directories
 * Checked in by:  JimAdmin;  James A. Brown
 * Change Description:  
 *   SVArchiveRecord class was changed such that 
 * 
 * the BuildArchiveImageFilePath function return type was changed from BOOL to HRESULT.
 * 
 * the csPathRoot member variable was removed.
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.10   17 Apr 2003 16:45:08   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.9   29 Jan 2003 10:45:18   joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  301
 * SCR Title:  Implement Result Image Circle Buffer
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated header information and updated WriteArchiveImageFile method to use the new image circle buffer mthodolodgy.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.8   14 Aug 2001 10:44:20   ed
 * Project:  SVObserver
 * Change Request (SCR) nbr:  220
 * SCR Title:  Fix invalid path message when adding a Load Image tool
 * Checked in by:  Ed;  Ed Chobanoff
 * Change Description:  
 *   Modified the code in SVArchiveTool::OnValidate() so that the last system error code, via GetLastError(), is only used when the return value of the call to the API function GetDiskFreeSpaceEx indicates a failure.   Additionally, this function was modified to
 * enhance system performance by performing image path 
 * validation and free drive space determination only on every tenth 
 * call of this function.  Previous to this change, image path validation 
 * and free drive space determination was performed on every call 
 * of this function.
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.7   12 Jun 2001 12:28:14   ed
 * Project:  SVObserver
 * Change Request (SCR) nbr:  200
 * SCR Title:  Update code to implement SVIM COM Server
 * Checked in by:  Ed;  Ed Chobanoff
 * Change Description:  
 *   Modified the following functions: 
 * class SVArchiveRecord   --  SVArchiveRecord, BuildFileName, BuildArchiveImageFilePath, ConnectInputObject, DisconnectInputObject.
 * class SVArchiveRecordsArray -- ConvertStringToGuids, RemoveDisconnectedObject, WriteArchiveImageFiles, BuildResultsArchiveStrin, DisconnectAllResultObjects.
 * class SVArchiveTool  -- initializeArchiveTool, CreateObject, ~SVArchiveTool, CreateTextArchiveFile, OnValidate, processMessage, initializeOnFirstOnRun, onRun, CheckForUniqueArchiveFilePath, GetFileArchivePath, GetImageFileRootPath, SetFileArchivePath, SetImageFileRootPath.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.6   19 Apr 2001 20:30:48   Steve
 * Project:  SVObserver
 * Change Request (SCR) nbr:  196
 * SCR Title:  Restructure Scripted Load/Save Procedures Functional Requirement
 * Checked in by:  Steve;  Stephen E. Steffan
 * Change Description:  
 *   Header file changes due to include file restructuring.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.4   11 Apr 2001 14:56:02   cSchmittinger
 * Project:  SVObserver
 * Change Request (SCR) nbr:  191
 * SCR Title:  Restructure File Management
 * Checked in by:  cSchmittinger;  Carl Schmittinger
 * Change Description:  
 *   stringImageFileRootPath and stringFileArchivePath do not require
 * the SVFileNameClass to maintain their value, so the references were removed.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.3   06 Feb 2001 13:24:28   cSchmittinger
 * Project:  SVObserver
 * Change Request (SCR) nbr:  191
 * SCR Title:  Restructure File Management
 * Checked in by:  cSchmittinger;  Carl Schmittinger
 * Change Description:  
 *   Modified source code to use new file management classes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.2   Feb 01 2000 08:18:24   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  91
 * SCR Title:  Archive Errors During Regression Test
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Correct errors in code that process 'going online' message.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.1   Jan 14 2000 15:21:44   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  86
 * SCR Title:  Archive Tool - Images - Does not start numbering with 00001.bmp
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Added code to 'reset' the archived image file sequence number back to 00001 when SVObserver goes 'online'.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.0   23 Nov 1999 11:00:36   mike
 * Project:  SVObserver
 * Change Request (SCR) nbr:  61
 * SCR Title:  Update PVCS versioning to version 3.0.
 * Checked in by:  Mike;  Mike McCarl
 * Change Description:  
 *   
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.20   Nov 18 1999 08:48:34   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  15
 * SCR Title:  Integrate archive tool
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Added code to check for disabled tool and no results to archive before opening archive file.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.19   Nov 17 1999 07:25:12   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  15
 * SCR Title:  Integrate archive tool
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Added check for unique results archive file paths.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.18   Oct 26 1999 15:04:38   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  15
 * SCR Title:  Integrate archive tool
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Limit Archived images to a user selectable number.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.17   Oct 22 1999 14:24:06   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  15
 * SCR Title:  Integrate archive tool
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Implement the capability to append to results archive, and include a time stamp for first record archived on each 'run'.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.16   Oct 22 1999 10:10:34   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  15
 * SCR Title:  Integrate archive tool
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Fix bugs in archive based on Beta 11 tests.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.15   Oct 21 1999 17:11:20   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  15
 * SCR Title:  Integrate archive tool
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Repair object connections so objects deleted will report their dependencies if selected to be archived.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.14   Oct 20 1999 12:26:40   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  15
 * SCR Title:  Integrate archive tool
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Fix bug that causes crash if object with image to be archive  is removed from toolset.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.13   Oct 18 1999 12:55:24   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  15
 * SCR Title:  Integrate archive tool
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Image file arhive folder selection dialog.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.12   Oct 13 1999 14:37:46   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  15
 * SCR Title:  Integrate archive tool
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Changes required to archive images into .bmp files.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.11   Oct 07 1999 15:43:14   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  15
 * SCR Title:  Integrate archive tool
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Archive Tool bug repairs.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.10   Oct 06 1999 15:22:24   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  15
 * SCR Title:  Integrate archive tool
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   More archive work.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   Oct 05 1999 16:46:52   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  15
 * SCR Title:  Integrate archive tool
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Scripting work.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   Sep 16 1999 13:40:46   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  34
 * SCR Title:  Add Build Reference Tool
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Class name changed from SVArchiveToolClass to SVToolArchive.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   Sep 15 1999 17:39:42   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  15
 * SCR Title:  Integrate archive tool
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Removed unneeded reference to SVArchiveResult.h.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   Sep 15 1999 16:14:02   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  15
 * SCR Title:  Integrate archive tool
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Begin port to SVObserver 3.00 architecture from v2.42.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   Sep 10 1999 18:07:22   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  33
 * SCR Title:  Port Profile Tool from v2.42
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Removed obsolete constructors and Serialize().
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   02 Sep 1999 10:10:50   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  9
 * SCR Title:  Integr. a common object interface to handle and to identify obj. in-/outputs
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised Run Logic
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   Aug 31 1999 19:32:42   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  15
 * SCR Title:  Integrate archive tool
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   SVObserver 3.0 architectural changes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   25 Aug 1999 10:28:50   robert
 * Project:  SVObserver
 * Change Request (SCR) nbr:  21
 * SCR Title:  Port Gage Tool code from program version 2.42.
 * Checked in by:  Robert Massinger
 * Change Description:  
 *   Changed to support onDraw().
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
