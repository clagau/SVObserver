//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVImageBufferHandleImage
//* .File Name       : $Workfile:   SVImageBufferHandleImage.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 10:44:36  $
//******************************************************************************

#pragma once

#include "SVImageBufferHandleData.h"
#include "SVImageBufferHandleStruct.h"

class SVImageBufferHandleImage : public SVImageBufferHandleData
{
public:
	SVImageBufferHandleImage();
	virtual ~SVImageBufferHandleImage();

	bool empty() const;
	void clear();

	const SVMatroxBuffer& GetBuffer() const;
	SVMatroxBuffer& GetBuffer();

protected:
	virtual HRESULT SetHandleData( const SVImageBufferHandleStruct& p_rObject );

	SVMatroxBuffer m_Buffer;

};

