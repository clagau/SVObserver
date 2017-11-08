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
#include "SVUtilityLibrary/StringHelper.h"
#include "SVOCore/SVImageClass.h"
#include "SVOCore/SVImageObjectClass.h"
#pragma endregion Includes

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

SVImageArchiveClass::SVImageArchiveClass()
{
	Init( std::string(_T("")) );
}

SVImageArchiveClass::SVImageArchiveClass(const std::string& rFileName)
{
	Init( rFileName );
}

void SVImageArchiveClass::Init( const std::string& rFileName )
{
	SetImageArchiveFileTemplate(rFileName);
	m_CurrentFileName.clear();
	m_ImageArchivePath = std::string( TheSVObserverApp.getConfigPathName() ) + "\\";
	m_ImageArchivePath += TheSVObserverApp.getConfigFileNameOnly();
	if ( m_ImageArchivePath.empty() || SvUl::Right( m_ImageArchivePath, 1) != _T("\\") )
	{
		m_ImageArchivePath += _T('\\');
	}
	m_ImageArchivePath += _T("Image Archive\\");
	m_FileNumber = 1L;
}

SVImageArchiveClass::~SVImageArchiveClass()
{
}

void SVImageArchiveClass::SetImageArchiveFileTemplate( const std::string& rTemplate )
{
	m_ArchiveFileTemplate = rTemplate;
	SvUl::searchAndReplace( m_ArchiveFileTemplate, _T("."), _T("_") );
}

void SVImageArchiveClass::SetImageArchivePath( const std::string& rPath )
{
	m_ImageArchivePath = rPath;
	if ( m_ImageArchivePath.empty() || SvUl::Right( m_ImageArchivePath, 1) != _T("\\") )
	{
		m_ImageArchivePath += _T('\\');
	}
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
	std::string FormatString;

	FormatString = m_ArchiveFileTemplate;
	FormatString += _T("__%06.6d.bmp");
	m_CurrentFileName = SvUl::Format(FormatString.c_str(), m_FileNumber);
	return m_FileNumber++;
}

bool SVImageArchiveClass::LoadImageArchiveFile( SVSmartHandlePointer p_HandlePtr )
{
	SVFileNameClass svFileName;
	std::string FileName;
	HCURSOR hCursor;
	
	HRESULT MatroxCode(S_OK);
	
	NextFileName ();
	
	svFileName.SetPathName( m_ImageArchivePath.c_str() );
	svFileName.SetFileName( m_CurrentFileName.c_str() );
	
	FileName = svFileName.GetFullFileName();
	
	if( ImageArchiveFileExists(FileName) && !p_HandlePtr.empty() )
	{
		hCursor = SetCursor (LoadCursor(nullptr, IDC_WAIT));

		SVImageBufferHandleImage l_MilHandle;
		p_HandlePtr->GetData( l_MilHandle );

		MatroxCode = SVMatroxBufferInterface::Import(l_MilHandle.GetBuffer(), FileName, SVFileBitmap, false );
		SetCursor (hCursor);
		if (S_OK == MatroxCode)
		{
			return true;
		}
	}
	ResetFileNumber();
	return false;
}

bool SVImageArchiveClass::LoadImageArchiveFile(SVImageClass *pImage)
{
	bool bOk = false;

	SVSmartHandlePointer l_ImageHandlePtr;

	if( pImage->GetImageHandle( l_ImageHandlePtr ) )
	{
		bOk = LoadImageArchiveFile( l_ImageHandlePtr );
	}

	return bOk;
}


bool SVImageArchiveClass::LoadImageArchiveFile( SVImageObjectClassPtr p_AcquisitionCircleBufferPtr )
{
	SVSmartHandlePointer l_ImageHandlePtr;
	
	bool bRetValue = false;
	
	int i;
	
	if( !( p_AcquisitionCircleBufferPtr.empty() ) )
	{
		p_AcquisitionCircleBufferPtr->GetImageHandle( 0, l_ImageHandlePtr );
	}
	
	if ( !( l_ImageHandlePtr.empty() ) )
	{
		
		HRESULT l_Code;

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

bool SVImageArchiveClass::ImageArchiveFileExists( const std::string &rFileName)
{
	return (0 == ::_access( rFileName.c_str(), 0 ) );
}

