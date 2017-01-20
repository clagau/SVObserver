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

#pragma once

#pragma region Includes
#include "SVImageLibrary/SVImageBufferHandleInterface.h"
#include "SVLibrary/SVFileNameClass.h"
#include "SVOCore/SVImageObjectClass.h"
#include "SVUtilityLibrary/SVString.h"
#pragma endregion Includes

class SVImageClass;

// JAB 030504 (comment, not the code) - it looks like the SVImageArchiveClass 
// is used with setting up the regression test images.
//
class SVImageArchiveClass
{
public:
	BOOL LoadImageArchiveFile( SVImageObjectClassPtr p_AcquisitionCircleBufferPtr );
	BOOL ImageArchiveFileExists( const SVString& rFileName );
	BOOL LoadImageArchiveFile( SVImageClass *pImage );
	BOOL LoadImageArchiveFile( SVSmartHandlePointer p_HandlePtr );
	DWORD NextFileName ();
	DWORD ResetFileNumber ();
	BOOL SetImageArchivePath( const SVString& rPath );
	BOOL SetImageArchiveFileTemplate(const SVString& rTemplate);
	SVImageArchiveClass();
	SVImageArchiveClass( const SVString& rFileName);
	virtual ~SVImageArchiveClass();

protected:
	DWORD m_FileNumber;
	SVString m_ArchiveFileTemplate;  // Template to use to generate unique filenames
	SVString m_CurrentFileName;      // Most recent "generated" filename
	SVString m_ImageArchivePath;     // Path where image files reside

	SVFileNameClass   msvfnFileName;

private:
  void Init (CONST SVString& rFileName);
};

