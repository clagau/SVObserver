// ******************************************************************************
// * COPYRIGHT (c) 2009 by Seidenader Vision, Inc.
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : modulename
// * .File Name       : $Workfile:   SVPackedFile.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.1  $
// * .Check In Date   : $Date:   03 Feb 2014 16:37:30  $
// ******************************************************************************
#pragma once

#define SV_PACKEDFILE_VERSION 1

class SVPackedFile 
{
public:
	const CString& getConfigFilePath() const;
	//Adds the file(s) specified by szFile to the packed file 
	//specified by szPackedFile. The szFile parameter may 
	//specify one file or may contain wildcard characters to 
	//specify multiple files.
	BOOL PackFiles( const CString& szFile, const CString& szPackedFile );

	//Extracts the files contained in szPackedFile.
	BOOL UnPackFiles( const CString& szPackedFile, const CString& szUnPackDir = _T("") );

	SVPackedFile();

	virtual ~SVPackedFile();

private:
	CString m_configFilePath;
};

