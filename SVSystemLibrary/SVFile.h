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

#ifndef SVFILE_H
#define SVFILE_H

#include <stdio.h>
#include "SVUtilityLibrary/SVUtilityGlobals.h"
#include "SVUtilityLibrary/SVString.h"

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
		//shareCompat =      (int) 0x00000,
		//shareExclusive =   (int) 0x00010,
		//shareDenyWrite =   (int) 0x00020,
		//shareDenyRead =    (int) 0x00030,
		shareDenyNone =    (int) 0x00040,
		//modeNoInherit =    (int) 0x00080,
		modeCreate =       (int) 0x01000,
		modeNoTruncate =   (int) 0x02000,
		typeText =         (int) 0x04000, // typeText and typeBinary are
		typeBinary =       (int) 0x08000, // used in derived classes only
		//osNoBuffer =       (int) 0x10000,
		//osWriteThrough =   (int) 0x20000,
		//osRandomAccess =   (int) 0x40000,
		//osSequentialScan = (int) 0x80000,
	};

	SVFile();

	virtual ~SVFile();

	virtual bool IsOpen() const;

	virtual HRESULT Open( const char* p_szFileName, UINT p_OpenFlags );
	virtual void Close();

	virtual UINT Read( void* lpBuf, UINT nCount );
	virtual HRESULT ReadContents( SVByteVector& p_rContents );

	virtual HRESULT Write( const void* lpBuf, UINT nCount );
	virtual HRESULT WriteContents( const SVByteVector& p_rContents );

	virtual ULONGLONG GetLength() const;

	ULONGLONG SeekToEnd();

protected:
	HRESULT GetAccessMode( SVString& p_rAccessMode, int& p_rShareMode, UINT p_OpenFlags );

private:
	FILE* m_pFile;

};

#endif

