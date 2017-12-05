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

	virtual bool SetImageHandleIndex( SVImageIndexStruct lIndex ) override;

	virtual bool CopyImageTo( SVImageIndexStruct lIndex ) override;

	virtual bool GetImageHandle( SVSmartHandlePointer& p_rHandlePtr ) override;
	virtual bool GetImageHandle( SVImageIndexStruct svIndex, SVSmartHandlePointer& rHandle ) override;

	virtual const SvTl::SVTimeStamp& GetLastResetTimeStamp() const override;
	
protected:
	virtual bool CreateBuffers( const SVImageInfoClass& p_rImageInfo, SVImageObjectClassPtr p_ImageArrayPtr ) override;

	virtual SVImageObjectClassPtr GetBufferArrayPtr() const override;

private:
	void init();
};

