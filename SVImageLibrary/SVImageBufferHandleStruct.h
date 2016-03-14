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

#ifndef SVIMAGEBUFFERHANDLESTRUCT_H
#define SVIMAGEBUFFERHANDLESTRUCT_H

#include "SVImageBufferHandleInterface.h"
#include "SVMatroxLibrary\SVMatroxLibrary.h"

struct SVImageBufferHandleStruct : public SVImageBufferHandleInterface
{
	friend class SVImageBufferHandleImage;

	SVImageBufferHandleStruct();
	SVImageBufferHandleStruct( const SVMatroxBuffer& p_rMilImage );
	virtual ~SVImageBufferHandleStruct();

	virtual bool empty() const;
	virtual void clear();

	virtual SVPOINT GetPositionPoint() const;
	virtual SVBitmapInfo GetBitmapInfo() const;
	virtual unsigned char* GetBufferAddress() const;

	virtual HRESULT GetData( SVImageBufferHandleData& p_rData ) const;

protected:
	SVMatroxBuffer milImage;            // MIL Buffer Handle for images, if available...

private:
	void LocalClear();

};

#endif

