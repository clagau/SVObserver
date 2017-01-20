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
#include "SVOCore/SVImageClass.h"
#include "SVOCore/SVImageObjectClass.h"
#include "CameraLibrary/SVDeviceParams.h" //Arvid added to avoid VS2015 compile Error

#pragma endregion Includes

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

SVImageArchiveClass::SVImageArchiveClass()
{
	Init( SVString(_T("")) );
}

SVImageArchiveClass::SVImageArchiveClass(const SVString& rFileName)
{
	Init( rFileName );
}

void SVImageArchiveClass::Init( const SVString& rFileName )
{
	SetImageArchiveFileTemplate(rFileName);
	m_CurrentFileName.clear();
	m_ImageArchivePath = SVString( TheSVObserverApp.getConfigPathName() ) + "\\";
	m_ImageArchivePath += TheSVObserverApp.getConfigFileNameOnly();
	if ( m_ImageArchivePath.empty() || SvUl_SF::Right( m_ImageArchivePath, 1) != _T("\\") )
	{
		m_ImageArchivePath += _T('\\');
	}
	m_ImageArchivePath += _T("Image Archive\\");
	m_FileNumber = 1L;
}

SVImageArchiveClass::~SVImageArchiveClass()
{
}

BOOL SVImageArchiveClass::SetImageArchiveFileTemplate( const SVString& rTemplate )
{
	m_ArchiveFileTemplate = rTemplate;
	SvUl_SF::searchAndReplace( m_ArchiveFileTemplate, _T("."), _T("_") );
	return true;
}

BOOL SVImageArchiveClass::SetImageArchivePath( const SVString& rPath )
{
	m_ImageArchivePath = rPath;
	if ( m_ImageArchivePath.empty() || SvUl_SF::Right( m_ImageArchivePath, 1) != _T("\\") )
	{
		m_ImageArchivePath += _T('\\');
	}
	return true;
}

DWORD SVImageArchiveClass::ResetFileNumber()
{
	DWORD dwFileNumber;

	dwFileNumber = m_FileNumber;
	m_FileNumber = 1L;
	m_CurrentFileName.clear();
	return dwFileNumber;
}

DWORD SVImageArchiveClass::NextFileName()
{
	SVString FormatString;

	FormatString = m_ArchiveFileTemplate;
	FormatString += _T("__%06.6d.bmp");
	m_CurrentFileName = SvUl_SF::Format(FormatString.c_str(), m_FileNumber);
	return m_FileNumber++;
}

BOOL SVImageArchiveClass::LoadImageArchiveFile( SVSmartHandlePointer p_HandlePtr )
{
	SVFileNameClass svFileName;
	SVString FileName;
	HCURSOR hCursor;
	
	SVMatroxBufferInterface::SVStatusCode l_Code;
	
	NextFileName ();
	
	svFileName.SetPathName( m_ImageArchivePath.c_str() );
	svFileName.SetFileName( m_CurrentFileName.c_str() );
	
	FileName = svFileName.GetFullFileName();
	
	if( ImageArchiveFileExists(FileName) && !p_HandlePtr.empty() )
	{
		hCursor = SetCursor (LoadCursor(nullptr, IDC_WAIT));

		SVImageBufferHandleImage l_MilHandle;
		p_HandlePtr->GetData( l_MilHandle );

		l_Code = SVMatroxBufferInterface::Import(l_MilHandle.GetBuffer(), FileName, SVFileBitmap, false );
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

	if( pImage->GetImageHandle( l_ImageHandlePtr ) )
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

BOOL SVImageArchiveClass::ImageArchiveFileExists( const SVString &rFileName)
{
	return (0 == ::_access( rFileName.c_str(), 0 ) );
}

