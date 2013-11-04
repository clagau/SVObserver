//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVImageArchiveClass
//* .File Name       : $Workfile:   SVImageArchive.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 10:48:52  $
//******************************************************************************

#ifndef SVIMAGEARCHIVE_H
#define SVIMAGEARCHIVE_H

#include "SVImageLibrary/SVImageBufferHandleInterface.h"
#include "SVFileNameClass.h"
#include "SVImageObjectClass.h"

class SVImageClass;

// JAB 030504 (comment, not the code) - it looks like the SVImageArchiveClass 
// is used with setting up the regression test images.
//
class SVImageArchiveClass
{
public:
	BOOL SelectImageArchivePath ();
	BOOL SaveImageArchiveFile (SVImageClass *pImage, BOOL bOverWrite = TRUE);
	BOOL LoadImageArchiveFile( SVImageObjectClassPtr p_AcquisitionCircleBufferPtr );
	BOOL ImageArchiveFileExists (CString &szFileName);
	BOOL LoadImageArchiveFile (SVImageClass *pImage);
	BOOL LoadImageArchiveFile( SVSmartHandlePointer p_HandlePtr );
	DWORD NextFileName ();
	DWORD ResetFileNumber ();
	BOOL SetImageArchivePath (CString &szPath);
	BOOL SetImageArchiveFileTemplate (CString &szTemplate);
	SVImageArchiveClass();
	SVImageArchiveClass(CString &szStartFileName);
	virtual ~SVImageArchiveClass();

protected:
	DWORD mdwFileNumber;
	CString mszArchiveFileTemplate;  // Template to use to generate unique filenames
	CString mszCurrentFileName;      // Most recent "generated" filename
	CString mszImageArchivePath;     // Path where image files reside

	SVFileNameClass   msvfnFileName;

private:
  void Init (CString &szStartFileName);

};

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVImageArchive.h_v  $
 * 
 *    Rev 1.0   23 Apr 2013 10:48:52   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.9   04 Oct 2012 11:06:36   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Remove scope declaration on header methods.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.8   10 Feb 2011 14:45:38   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to now use shared pointer instead of untracked structure.  This fixes destruction, tracking and rebuilding issues.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.7   01 Feb 2011 11:21:40   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix a casting issues with the buffer management.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.6   14 Jul 2009 13:40:50   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to use the new data manager objects.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.5   13 May 2004 13:57:46   jBrown
 * Project:  SVObserver
 * Change Request (SCR) nbr:  422
 * SCR Title:  Image archiving across directories
 * Checked in by:  JimAdmin;  James A. Brown
 * Change Description:  
 *   Comments were added to clarify the role of SVImageArchiveClass with respect to the image archive portion of the Archive Tool (there is none).
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.4   22 Apr 2003 10:07:14   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.3   29 Jan 2003 16:19:42   joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  301
 * SCR Title:  Implement Result Image Circle Buffer
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated LoadImageArchiveFile and SaveImageArchiveFile method to use the new image circle buffer methodology.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
