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

#pragma region Includes
#include "SVTimerLibrary/SVClock.h"
#include "SVCameraImageTemplate.h"
#pragma endregion Includes

class SVMainImageClass : public SVCameraImageTemplate 
{
// Wasn't in old implementation, take back out for now. may be needed in future.
// 	SV_DECLARE_CLASS( SVMainImageClass );

public:
	SVMainImageClass( LPCSTR ObjectName );
	SVMainImageClass( SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVMAINIMAGE );

	virtual ~SVMainImageClass();

	virtual BOOL SetImageHandleIndex( SVImageIndexStruct lIndex ) override;

	virtual BOOL CopyImageTo( SVImageIndexStruct lIndex ) override;

	virtual BOOL GetImageHandle( SVSmartHandlePointer& p_rHandlePtr ) override;
	virtual BOOL GetImageHandle( SVImageIndexStruct svIndex, SVSmartHandlePointer& rHandle ) override;

	virtual BOOL OnValidate() override;

	virtual const SVClock::SVTimeStamp& GetLastResetTimeStamp() const override;
	
protected:
	virtual BOOL CreateBuffers( const SVImageInfoClass& p_rImageInfo, SVImageObjectClassPtr p_ImageArrayPtr ) override;

	virtual SVImageObjectClassPtr GetBufferArrayPtr() const override;

private:
	void init();
};

