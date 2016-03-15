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

#ifndef SVDISPLAYIMAGEBUFFERCLASS_H
#define SVDISPLAYIMAGEBUFFERCLASS_H

#include "SVImageLibrary/SVImageBufferHandleInterface.h"

class SVDisplayImageBufferClass : public SVImageBufferHandleInterface  
{
public:
	SVDisplayImageBufferClass();
	virtual ~SVDisplayImageBufferClass();

	virtual bool empty() const;
	virtual void clear();

	virtual SVPOINT GetPositionPoint() const;
	virtual SVBitmapInfo GetBitmapInfo() const;
	virtual unsigned char* GetBufferAddress() const;

	virtual HRESULT GetData( SVImageBufferHandleData& p_rData ) const;

	bool UpdateDisplayBufferInfo( long bufWidth, long bufHeight, int iFormat );
	bool DestroyDisplayBuffer();

	SVBitmapInfo m_BitmapInfo;
	unsigned char *m_pucImageData;

};

#endif

