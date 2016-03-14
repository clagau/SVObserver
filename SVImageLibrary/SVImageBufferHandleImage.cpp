//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVImageBufferHandleImage
//* .File Name       : $Workfile:   SVImageBufferHandleImage.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 10:44:32  $
//******************************************************************************

#include "stdafx.h"
#include "SVImageBufferHandleImage.h"

SVImageBufferHandleImage::SVImageBufferHandleImage()
: SVImageBufferHandleData(), m_Buffer()
{
}

SVImageBufferHandleImage::~SVImageBufferHandleImage()
{
	clear();
}

bool SVImageBufferHandleImage::empty() const
{
	return m_Buffer.empty();
}

void SVImageBufferHandleImage::clear()
{
	m_Buffer.clear();
}

const SVMatroxBuffer& SVImageBufferHandleImage::GetBuffer() const
{
	return m_Buffer;
}

SVMatroxBuffer& SVImageBufferHandleImage::GetBuffer()
{
	return m_Buffer;
}

HRESULT SVImageBufferHandleImage::SetHandleData( const SVImageBufferHandleStruct& p_rObject )
{
	m_Buffer = p_rObject.milImage;

	return S_OK;
}

