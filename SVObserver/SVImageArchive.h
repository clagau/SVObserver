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
#include "SVFileSystemLibrary/SVFileNameClass.h"
#include "InspectionEngine/SVImageObjectClass.h"
#pragma endregion Includes

class SVImageClass;

// JAB 030504 (comment, not the code) - it looks like the SVImageArchiveClass 
// is used with setting up the regression test images.
//
class SVImageArchiveClass
{
public:
	bool LoadImageArchiveFile( SVImageObjectClassPtr p_AcquisitionCircleBufferPtr );
	bool ImageArchiveFileExists( const std::string& rFileName );
	bool LoadImageArchiveFile( SVImageClass *pImage );
	bool LoadImageArchiveFile( SVSmartHandlePointer p_HandlePtr );
	DWORD NextFileName ();
	DWORD ResetFileNumber ();
	void SetImageArchivePath( const std::string& rPath );
	void SetImageArchiveFileTemplate(const std::string& rTemplate);
	SVImageArchiveClass();
	SVImageArchiveClass( const std::string& rFileName);
	virtual ~SVImageArchiveClass();

protected:
	DWORD m_FileNumber;
	std::string m_ArchiveFileTemplate;  // Template to use to generate unique filenames
	std::string m_CurrentFileName;      // Most recent "generated" filename
	std::string m_ImageArchivePath;     // Path where image files reside

	SVFileNameClass   msvfnFileName;

private:
  void Init (CONST std::string& rFileName);
};

