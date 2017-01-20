// ******************************************************************************
// * COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVEventRecordClass
// * .File Name       : $Workfile:   SVEventRecordStruct.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   25 Apr 2013 17:43:00  $
// ******************************************************************************

#pragma once

#pragma region Includes
#include "SVUtilityLibrary\SVString.h"
#pragma endregion Includes

static const unsigned long SV_EVENT_RECORD_STRUCT_RECORD_SIZE = 0x0008000;

struct SVEventRecordStruct
{
	SVEventRecordStruct();
	virtual ~SVEventRecordStruct();

	void SetBuffer( unsigned char* p_pucBuffer, unsigned long p_ulSize );

	EVENTLOGRECORD* GetEventLogRecord();

	LPCTSTR GetSourceName();
	LPCTSTR GetComputerName();

	LPCTSTR GetFirstString();
	LPCTSTR GetNextString();
	LPCTSTR GetType();
	SVString GetDateTime();

private:
	HRESULT LocalInitialize(unsigned long p_ulSize);
	HRESULT LocalDestroy();

	unsigned char *m_pucBuffer;
	unsigned long m_ulBufferSize;

	EVENTLOGRECORD* m_poRecord;

	LPCTSTR m_szSourceName;
	LPCTSTR m_szComputerName;

	LPCTSTR m_szCurrentString;
	unsigned long m_ulStringIndex;
};
