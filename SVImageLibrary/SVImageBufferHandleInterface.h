//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVImageBufferHandleInterface
//* .File Name       : $Workfile:   SVImageBufferHandleInterface.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 10:44:42  $
//******************************************************************************

#pragma once

#include "SVUtilityLibrary/SVBitmapInfo.h"
#include "SVUtilityLibrary/SVPOINT.h"
#include "SVUtilityLibrary/SVSharedPtr.h"

class SVImageBufferHandleData;

class SVImageBufferHandleInterface  
{
public:
	virtual ~SVImageBufferHandleInterface();

	virtual bool empty() const = 0;
	virtual void clear() = 0;

	virtual SVPOINT GetPositionPoint() const = 0;
	virtual SVBitmapInfo GetBitmapInfo() const = 0;
	virtual unsigned char* GetBufferAddress() const = 0;

	virtual HRESULT GetData( SVImageBufferHandleData& p_rData ) const = 0;

protected:
	SVImageBufferHandleInterface();

private:
	// Do not implement
	SVImageBufferHandleInterface( const SVImageBufferHandleInterface& p_rObject );

	// Do not implement
	const SVImageBufferHandleInterface& operator=( const SVImageBufferHandleInterface& p_rObject );

};

typedef SVSharedPtr< SVImageBufferHandleInterface > SVSmartHandlePointer;

