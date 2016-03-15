// ******************************************************************************
// * COPYRIGHT (c) 2002 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVCrc32
// * .File Name       : $Workfile:   SVCrc32.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   18 Apr 2013 18:39:38  $
// ******************************************************************************

// SVCrc32.h: interface for the SVCrc32 class.
//
//////////////////////////////////////////////////////////////////////

#ifndef SVCRC32_H
#define SVCRC32_H

class  SVCrc32
{
public:
// Construction/Destruction
	SVCrc32();
	virtual ~SVCrc32();

// Methods	
	static DWORD ComputeCRC( BYTE *pBuffer, int iLength );
	static void ComputeCRC( BYTE *pBuffer, int iLength, DWORD &dwCRC );

private:
// Members
	static unsigned long m_tblCRC[256];

// Methods
};

#endif

