//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVMainImageClass
//* .File Name       : $Workfile:   SVMainImageClass.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   08 May 2013 16:16:48  $
//******************************************************************************

#pragma once

#include "SVTimerLibrary/SVClock.h"
#include "SVCameraImageTemplate.h"

class SVMainImageClass : public SVCameraImageTemplate 
{
// Wasn't in old implementation, take back out for now. may be needed in future.
// 	SV_DECLARE_CLASS( SVMainImageClass );

public:
	SVMainImageClass( LPCSTR ObjectName );
	SVMainImageClass( SVObjectClass* POwner = NULL, int StringResourceID = IDS_CLASSNAME_SVMAINIMAGE );

	virtual ~SVMainImageClass();

	virtual BOOL SetImageHandleIndex( SVImageIndexStruct lIndex );

	virtual BOOL CopyImageTo( SVImageIndexStruct lIndex );

	virtual BOOL GetImageHandle( SVSmartHandlePointer& p_rHandlePtr );
	virtual BOOL GetImageHandle( SVImageIndexStruct svIndex, SVSmartHandlePointer& rHandle );

	virtual BOOL OnValidate();

	virtual const SVClock::SVTimeStamp& GetLastResetTimeStamp() const;
	
protected:
	virtual BOOL CreateBuffers( const SVImageInfoClass& p_rImageInfo, SVImageObjectClassPtr p_ImageArrayPtr );

	virtual SVImageObjectClassPtr GetBufferArrayPtr() const;

private:
	void init();

};

