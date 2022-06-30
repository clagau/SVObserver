//******************************************************************************
//* COPYRIGHT (c) 2003 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVImageBufferHandleImage
//* .File Name       : $Workfile:   SVImageBufferHandleImage.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 10:44:36  $
//******************************************************************************

#pragma once

#include "ObjectInterfaces\SVImageBufferHandleInterface.h"
#include "SVMatroxLibrary\SVMatroxBuffer.h"

class SVImageBufferHandleImage : public SvOi::SVImageBufferHandleInterface
{
public:
	SVImageBufferHandleImage();
	explicit SVImageBufferHandleImage(const SVMatroxBuffer& rMilImage);
	virtual ~SVImageBufferHandleImage();

	bool empty() const override;
	void clear() override;

	virtual SVPoint<long> GetPositionPoint() const override;
	virtual SVBitmapInfo GetBitmapInfo() const override;
	virtual unsigned char* GetBufferAddress() const override;

	virtual const SVMatroxBuffer& GetBuffer() const override;
	virtual SVMatroxBuffer& GetBuffer() override;

protected:

	SVMatroxBuffer m_Buffer;

};

