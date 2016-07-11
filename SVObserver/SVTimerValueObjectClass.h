//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVTimerValueObjectClass
//* .File Name       : $Workfile:   SVTimerValueObjectClass.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   08 May 2013 16:43:58  $
//******************************************************************************
#pragma once

#pragma region Includes
#include "SVTimerLibrary/SVClock.h"
#include "SVInt64ValueObjectClass.h"
#pragma endregion Includes

class SVTimerValueObjectClass : public SVInt64ValueObjectClass  
{
	SV_DECLARE_CLASS( SVTimerValueObjectClass );

public:
	SVTimerValueObjectClass( LPCTSTR ObjectName);
	SVTimerValueObjectClass( SVObjectClass *pOwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVINT64VALUEOBJECT );
	virtual ~SVTimerValueObjectClass();

	BOOL Start();
	BOOL Stop(long lIndex);

	virtual HRESULT GetValueAt(int nBucket, int iIndex, CString& rstrValue) const;
	virtual HRESULT GetValueAt( int iBucket, int iIndex, VARIANT& rValue ) const;

private:
	void LocalInitialize();

	SVClock::SVTimeStamp m_Start;  // Time Stamp in Milliseconds
};

