//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVImageArchive
//* .File Name       : $Workfile:   SVImageArchive.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   04 Feb 2014 15:21:20  $
//******************************************************************************

#include "stdafx.h"
#include "SVImageArchive.h"
#include "SVImageLibrary/SVImageBufferHandleImage.h"
#include "SVImageLibrary/SVImageBufferHandleInterface.h"
#include "SVObserver.h"
#include "SVImageClass.h"
#include "SVImageObjectClass.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

SVImageArchiveClass::SVImageArchiveClass()
{
  CString szStartFileName = _T("");
  Init (szStartFileName);
}

SVImageArchiveClass::SVImageArchiveClass(CString &szStartFileName)
{
  Init (szStartFileName);
}

void SVImageArchiveClass::Init (CString &szStartFileName)
{
  SetImageArchiveFileTemplate (szStartFileName);
  mszCurrentFileName.Empty ();
  mszImageArchivePath = CString( TheSVObserverApp.getConfigPathName() ) + "\\" + 
	                      CString( TheSVObserverApp.getConfigFileNameOnly() );
  if ( mszImageArchivePath.IsEmpty () || mszImageArchivePath.Right(1) != _T("\\") )
  {
    mszImageArchivePath += (TCHAR)'\\';
  }
  mszImageArchivePath += _T("Image Archive\\");
  mdwFileNumber = 1L;
}

SVImageArchiveClass::~SVImageArchiveClass()
{

}

BOOL SVImageArchiveClass::SetImageArchiveFileTemplate(CString &szTemplate)
{
  mszArchiveFileTemplate = szTemplate;
  mszArchiveFileTemplate.Replace(_T('.'), _T('_'));
  return TRUE;
}

BOOL SVImageArchiveClass::SetImageArchivePath(CString &szPath)
{
  mszImageArchivePath = szPath;
  if (mszImageArchivePath.IsEmpty () || mszImageArchivePath.Right(1) != _T("\\"))
    mszImageArchivePath += _T("\\");
  return TRUE;
}

DWORD SVImageArchiveClass::ResetFileNumber()
{
  DWORD dwFileNumber;

  dwFileNumber = mdwFileNumber;
  mdwFileNumber = 1L;
  mszCurrentFileName.Empty ();
  return dwFileNumber;
}

DWORD SVImageArchiveClass::NextFileName()
{
  CString szFormatString;

  szFormatString = mszArchiveFileTemplate;
  szFormatString += _T("__%06.6d.bmp");
  mszCurrentFileName.Format(szFormatString, mdwFileNumber);
  return mdwFileNumber++;
}

BOOL SVImageArchiveClass::LoadImageArchiveFile( SVSmartHandlePointer p_HandlePtr )
{
	SVFileNameClass svFileName;
	CString szFileName;
	SVMatroxString sFileName;
	HCURSOR hCursor;
	
	SVMatroxBufferInterface::SVStatusCode l_Code;
	
	NextFileName ();
	
	svFileName.SetPathName( mszImageArchivePath );
	svFileName.SetFileName( mszCurrentFileName );
	
	szFileName = svFileName.GetFullFileName();
	
	if (ImageArchiveFileExists (szFileName) && !( p_HandlePtr.empty() ) )
	{
		hCursor = SetCursor (LoadCursor(NULL, IDC_WAIT));
		sFileName = szFileName;

		SVImageBufferHandleImage l_MilHandle;
		p_HandlePtr->GetData( l_MilHandle );

		l_Code = SVMatroxBufferInterface::Import(l_MilHandle.GetBuffer(), sFileName, SVFileBitmap, false );
		SetCursor (hCursor);
		if( l_Code == SVMEE_STATUS_OK )
		{
			return TRUE;
		}
	}
	ResetFileNumber();
	return FALSE;
}

BOOL SVImageArchiveClass::LoadImageArchiveFile(SVImageClass *pImage)
{
  BOOL bOk = FALSE;

  SVSmartHandlePointer l_ImageHandlePtr;

  if ( pImage->GetImageHandle( l_ImageHandlePtr ) )
  {
    bOk = LoadImageArchiveFile( l_ImageHandlePtr );
  }

  return bOk;
}


BOOL SVImageArchiveClass::LoadImageArchiveFile( SVImageObjectClassPtr p_AcquisitionCircleBufferPtr )
{
	SVSmartHandlePointer l_ImageHandlePtr;
	
	BOOL bRetValue = false;
	
	int i;
	
	if( !( p_AcquisitionCircleBufferPtr.empty() ) )
	{
		p_AcquisitionCircleBufferPtr->GetImageHandle( 0, l_ImageHandlePtr );
	}
	
	if ( !( l_ImageHandlePtr.empty() ) )
	{
		
		SVMatroxBufferInterface::SVStatusCode l_Code;

		SVImageBufferHandleImage l_MilHandle;
		l_ImageHandlePtr->GetData( l_MilHandle );

		SVMatroxBuffer MilId = l_MilHandle.GetBuffer();
		bRetValue = LoadImageArchiveFile( l_ImageHandlePtr );
		
		for (i = 1; bRetValue && (i < static_cast< int >( p_AcquisitionCircleBufferPtr->size() ) ); i++)
		{
			bRetValue = p_AcquisitionCircleBufferPtr->GetImageHandle( i, l_ImageHandlePtr );
			if ( bRetValue )
			{
				l_ImageHandlePtr->GetData( l_MilHandle );

				l_Code = SVMatroxBufferInterface::CopyBuffer( l_MilHandle.GetBuffer(), MilId );
			}
		}
	}
	
	return bRetValue;
}

BOOL SVImageArchiveClass::SaveImageArchiveFile(SVImageClass *pImage, BOOL bOverWrite)
{
	CString szFileName;
	HCURSOR hCursor;
	
	SVSmartHandlePointer ImageHandle;
	
	if ( ! pImage->GetImageHandle( ImageHandle ) || ImageHandle.empty() )
	{
		return FALSE;
	}
	
	SVImageBufferHandleImage l_MilHandle;
	ImageHandle->GetData( l_MilHandle );

	if ( l_MilHandle.empty() )
	{
		return FALSE;
	}
	
	NextFileName ();
	szFileName = mszImageArchivePath;
	szFileName += mszCurrentFileName;
	
	if (!bOverWrite && ImageArchiveFileExists(szFileName))
		return FALSE;
	
	hCursor = SetCursor (LoadCursor(NULL, IDC_WAIT));

	
	SVMatroxBufferInterface::SVStatusCode l_Code;
	SVMatroxString sFileName = szFileName;	
	l_Code = SVMatroxBufferInterface::Export( l_MilHandle.GetBuffer(), sFileName, SVFileBitmap); 
	SetCursor (hCursor);
	
	if( l_Code == SVMEE_STATUS_OK )
		return TRUE;
	else
		return FALSE;
}

BOOL SVImageArchiveClass::ImageArchiveFileExists(CString &szFileName)
{
  CFile fFile;

  if (!fFile.Open ((LPCTSTR) szFileName,
                  CFile::modeRead | CFile::typeBinary,
                  (CFileException *) NULL))
  {
    return FALSE;
  }
  fFile.Close();
  return TRUE;
}

BOOL SVImageArchiveClass::SelectImageArchivePath()
{
	CString  szFilter, szFileName;

	DWORD dwOldFileNumber = ResetFileNumber ();
   
   NextFileName ();

   szFilter = _T ( "Image Archive Files|" );
   szFilter += mszArchiveFileTemplate;
   szFilter += _T ( "*.bmp|Bitmap Files|*.bmp||" );

   msvfnFileName.SetFileExtensionFilterList ( szFilter );
   msvfnFileName.SetDefaultFileName ( mszCurrentFileName );
	msvfnFileName.SetFileSelectFlags ( OFN_PATHMUSTEXIST | OFN_NOTESTFILECREATE | 
	                                   OFN_NONETWORKBUTTON | OFN_NOCHANGEDIR );

	//
   // Try to read the current image file path name from registry...
	//
	CString csPath = AfxGetApp ()->GetProfileString ( _T ( "Settings" ), 
											                              _T ( "ArchivedImagesFilePath" ), 
											                              (LPCTSTR) mszImageArchivePath );

   msvfnFileName.SetPathName ( csPath );

	msvfnFileName.SetFileSelectDialogTitle ( _T( "Image Archive Directory Selection" ) );

   if ( msvfnFileName.SelectFile () == IDOK )
   {
		mszImageArchivePath = msvfnFileName.GetPathName ();
	   
      //
	   // Write this path name back to registry...to initialize registry.
	   //
	   AfxGetApp ()->WriteProfileString (  _T( "Settings" ), 
		                                    _T( "ArchivedImagesFilePath" ), 
                                       (LPCTSTR) mszImageArchivePath );
    
      szFileName = msvfnFileName.GetFileNameOnly ();

      if ( szFileName.GetLength () > 8 )
      {
         szFileName.Delete ( szFileName.GetLength () - 8, 8 );
      }
      else
      {
         szFileName = mszArchiveFileTemplate;
      }
      
      SetImageArchiveFileTemplate ( szFileName );
      
      ResetFileNumber ();
      
      mdwFileNumber = dwOldFileNumber;
      
      return TRUE;
   }

   ResetFileNumber ();
   mdwFileNumber = dwOldFileNumber;
   return FALSE;
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVImageArchive.cpp_v  $
 * 
 *    Rev 1.1   04 Feb 2014 15:21:20   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  880
 * SCR Title:  Remove .SEC
 * Checked in by:  mZiegler;  Marc Ziegler
 * Change Description:  
 *   Changed the Init method to use the method getConfigPathName and getConfigFileNameOnly instead of GetSECPathName and GetSECFileNameOnly from TheSVObserverApp.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 10:48:44   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.17   25 Feb 2011 12:10:56   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated classes needed to consolidate SVImageClass and SVImageObjectClass updating calls to one method name.  Fixed updating problems with child buffers.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.16   10 Feb 2011 14:45:38   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to now use shared pointer instead of untracked structure.  This fixes destruction, tracking and rebuilding issues.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.15   01 Feb 2011 11:21:40   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix a casting issues with the buffer management.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.14   27 Jan 2011 11:20:46   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix issues with Matrox Buffer Management.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.13   30 Aug 2010 13:48:30   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  700
 * SCR Title:  Remove String Buffer Problems
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Update code to remove or fix uses of CString GetBuffer and ReleaseBuffer methods.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.12   16 Dec 2009 11:00:38   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  677
 * SCR Title:  Fix problem in camera notify thread
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fix issues with includes and comments.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.11   30 Jul 2009 11:56:18   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Merged branched changes into current code branch with appropriate updates.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.10   24 Jul 2007 13:06:28   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  597
 * SCR Title:  Upgrade Matrox Imaging Library to version 8.0
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Upgrade Matrox Imaging Library to 8.0
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.9.1.0   14 Jul 2009 13:40:48   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to use the new data manager objects.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.9   22 Apr 2003 10:07:54   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.8   14 Mar 2003 11:02:08   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  301
 * SCR Title:  Implement Result Image Circle Buffer
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   fixed problem for Regression mode.  Changed from GetObjectDepth() to GetImageDepth().
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.7   29 Jan 2003 16:19:44   joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  301
 * SCR Title:  Implement Result Image Circle Buffer
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated LoadImageArchiveFile and SaveImageArchiveFile method to use the new image circle buffer methodology.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.6   19 Nov 2002 13:20:16   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  226
 * SCR Title:  Monochrome SVIM configuration compatibility between ViperQUAD and ViperDUAL
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   LoadImageArchiveFile  
 * 
 * Changed MIL_ID type to long
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.5   04 Oct 2001 14:58:00   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  229
 * SCR Title:  Upgrade SVObserver to Version 3.34 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Modified source code to remove compiler warning message.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.4   12 Jun 2001 14:47:00   ed
 * Project:  SVObserver
 * Change Request (SCR) nbr:  200
 * SCR Title:  Update code to implement SVIM COM Server
 * Checked in by:  Ed;  Ed Chobanoff
 * Change Description:  
 *   Modified the following functions: LoadImageArchiveFile, SelectImageArchivePath.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.3   01 Feb 2001 11:55:12   Steve
 * Project:  SVObserver
 * Change Request (SCR) nbr:  191
 * SCR Title:  Restructure File Management
 * Checked in by:  Steve;  Stephen S. Steffan
 * Change Description:  
 *   Modified files to use the SVFileNameClass and the SVFileNameManagerClass if applicable, if the object refers to a filename.  The SVFileNameManagerClass is used if the Object is a persistant file object, that is, if the file object is saved as part of the configuration data.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.2   25 Jan 2001 16:17:48   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  191
 * SCR Title:  Restructure File Management
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   These changes fixed the SVObserverApp file handling and associated systems.  These systems include the .SEC, .IOD, and .IPD files as well as the camera files.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.1.1.0   16 May 2001 14:08:18   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  202
 * SCR Title:  Operator, delete and cut should not be available in File Open/Save Dialogs
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Changed all occurences of CFileDialog to CSVFileDialog to enable the new security features and privileges
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.1   Feb 01 2000 09:09:04   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  91
 * SCR Title:  Archive Errors During Regression Test
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Changed code to save the directory where regression images are stored, so the user will 'have' this same directory the next time an image needs to be selected.  Added SV copyright header.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
