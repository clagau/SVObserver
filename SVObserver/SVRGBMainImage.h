//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVRGBMainImage
//* .File Name       : $Workfile:   SVRGBMainImage.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 15:00:42  $
//******************************************************************************
#pragma once

#pragma region Includes
#include "SVCameraImageTemplate.h"
#pragma endregion Includes

class SVRGBMainImageClass : public SVCameraImageTemplate
{
public:
	SVRGBMainImageClass( LPCSTR ObjectName );
	SVRGBMainImageClass( SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVRGBMAINIMAGE );

	virtual ~SVRGBMainImageClass();

	virtual HRESULT ResetObject();

	virtual BOOL SetImageHandleIndex( SVImageIndexStruct svIndex );

	virtual BOOL CopyImageTo( SVImageIndexStruct svIndex );

protected:
	virtual BOOL CreateBuffers( const SVImageInfoClass& p_rImageInfo, SVImageObjectClassPtr p_ImageArrayPtr );

	virtual BOOL GetCameraImageHandle( SVSmartHandlePointer& p_rHandlePtr );
	virtual BOOL GetCameraImageHandle( SVImageIndexStruct svIndex, SVSmartHandlePointer& rHandle );

	virtual BOOL UpdateBuffer();
};

