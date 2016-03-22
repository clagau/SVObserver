//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVEventLogClass
//* .File Name       : $Workfile:   SVEventLogClass.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   25 Apr 2013 17:42:30  $
//******************************************************************************

#pragma once

#include "SVEventRecordStruct.h"
#include "SVUtilityLibrary/SVString.h"

class SVEventLogClass  
{
public:
	SVEventLogClass();
	virtual ~SVEventLogClass();

	HRESULT Open( LPCTSTR p_pszName );
	HRESULT Close();

	HRESULT GetRecordCount( unsigned long& p_rulCount );

	HRESULT ReadLast( SVEventRecordStruct& p_rsvRecord );
	HRESULT ReadPrevious( SVEventRecordStruct& p_rsvRecord );

private:

	HRESULT LocalInitialize();
	HRESULT LocalDestroy();

	SVString m_csName; // Should be one of the following: "Application", "Security", or "System"

	HANDLE m_hHandle;

	unsigned char *m_pucBuffer;
	unsigned long m_ulBufferSize;

	unsigned long m_ulBytesRead;

	unsigned char* m_pucPrevious;
	unsigned char* m_pucCurrent;
};


