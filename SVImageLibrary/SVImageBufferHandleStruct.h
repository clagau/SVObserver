//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVImageBufferHandleStruct
//* .File Name       : $Workfile:   SVImageBufferHandleStruct.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 10:53:24  $
//******************************************************************************

#pragma once

#include "SVImageBufferHandleInterface.h"
#include "SVMatroxLibrary\SVMatroxLibrary.h"

struct SVImageBufferHandleStruct : public SVImageBufferHandleInterface
{
	friend class SVImageBufferHandleImage;

	SVImageBufferHandleStruct();
	SVImageBufferHandleStruct( const SVMatroxBuffer& p_rMilImage );
	virtual ~SVImageBufferHandleStruct();

	virtual bool empty() const override;
	virtual void clear() override;

	virtual SVPOINT GetPositionPoint() const override;
	virtual SVBitmapInfo GetBitmapInfo() const override;
	virtual unsigned char* GetBufferAddress() const override;

	virtual HRESULT GetData( SVImageBufferHandleData& p_rData ) const override;

protected:
	SVMatroxBuffer milImage;            // MIL Buffer Handle for images, if available...

private:
	void LocalClear();

};

