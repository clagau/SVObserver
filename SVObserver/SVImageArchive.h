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
#include "SVOMFCLibrary/SVFileNameClass.h"
#include "SVImageObjectClass.h"
#pragma endregion Includes

class SVImageClass;

// JAB 030504 (comment, not the code) - it looks like the SVImageArchiveClass 
// is used with setting up the regression test images.
//
class SVImageArchiveClass
{
public:
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

