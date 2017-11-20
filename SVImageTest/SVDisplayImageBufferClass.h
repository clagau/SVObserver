// ******************************************************************************
// * COPYRIGHT (c) 2006 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVDisplayImageBufferClass
// * .File Name       : $Workfile:   SVDisplayImageBufferClass.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   22 Apr 2013 11:08:34  $
// ******************************************************************************

#pragma once

#include "SVImageLibrary/SVImageBufferHandleInterface.h"

class SVDisplayImageBufferClass : public SVImageBufferHandleInterface  
{
public:
	SVDisplayImageBufferClass();
	virtual ~SVDisplayImageBufferClass();

	virtual bool empty() const override;
	virtual void clear() override;

	virtual SVPoint GetPositionPoint() const override;
	virtual SVBitmapInfo GetBitmapInfo() const override;
	virtual unsigned char* GetBufferAddress() const override;

	virtual HRESULT GetData( SVImageBufferHandleData& p_rData ) const override;

	bool UpdateDisplayBufferInfo( long bufWidth, long bufHeight, int iFormat );
	bool DestroyDisplayBuffer();

	SVBitmapInfo m_BitmapInfo;
	unsigned char *m_pucImageData;

};

