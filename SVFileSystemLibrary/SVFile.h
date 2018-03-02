//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVFile
//* .File Name       : $Workfile:   SVFile.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   25 Apr 2013 17:51:32  $
//******************************************************************************

#pragma once

//Moved to precompiled header: #include <stdio.h>
#include "SVUtilityLibrary/SVUtilityGlobals.h"


/**
@SVObjectName File

@SVObjectOverview

@SVObjectOperations

*/
class SVFile
{
public:
	enum SVOpenFlags
	{
		modeRead =         (int) 0x00000,
		modeWrite =        (int) 0x00001,
		modeReadWrite =    (int) 0x00002,
		shareDenyNone =    (int) 0x00040,
		modeCreate =       (int) 0x01000,
		modeNoTruncate =   (int) 0x02000,
		typeText =         (int) 0x04000, // typeText and typeBinary are
		typeBinary =       (int) 0x08000, // used in derived classes only
	};

	SVFile();

	~SVFile();

	bool IsOpen() const;

	HRESULT Open( const char* p_szFileName, UINT p_OpenFlags );
	void Close();

	UINT Read( void* lpBuf, UINT nCount );
	HRESULT ReadContents( SVByteVector& p_rContents );

	HRESULT Write( const void* lpBuf, UINT nCount );
	HRESULT WriteContents( const SVByteVector& p_rContents );

	ULONGLONG GetLength() const;

	ULONGLONG SeekToEnd();

protected:
	HRESULT GetAccessMode( std::string& p_rAccessMode, int& p_rShareMode, UINT p_OpenFlags );

private:
	FILE* m_pFile;

};
