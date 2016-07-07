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

#pragma region Includes
#include "stdafx.h"
#include "SVImageArchive.h"
#include "SVImageLibrary/SVImageBufferHandleImage.h"
#include "SVImageLibrary/SVImageBufferHandleInterface.h"
#include "SVObserver.h"
#include "SVImageClass.h"
#include "SVImageObjectClass.h"
#include "SVOMFCLibrary/SVDeviceParams.h"
#pragma endregion Includes

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
	SVString sFileName;
	HCURSOR hCursor;
	
	SVMatroxBufferInterface::SVStatusCode l_Code;
	
	NextFileName ();
	
	svFileName.SetPathName( mszImageArchivePath );
	svFileName.SetFileName( mszCurrentFileName );
	
	szFileName = svFileName.GetFullFileName();
	
	if (ImageArchiveFileExists (szFileName) && !( p_HandlePtr.empty() ) )
	{
		hCursor = SetCursor (LoadCursor(nullptr, IDC_WAIT));
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

BOOL SVImageArchiveClass::ImageArchiveFileExists(CString &szFileName)
{
  CFile fFile;

  if (!fFile.Open ((LPCTSTR) szFileName,
                  CFile::modeRead | CFile::typeBinary,
                  (CFileException *) nullptr))
  {
    return FALSE;
  }
  fFile.Close();
  return TRUE;
}

