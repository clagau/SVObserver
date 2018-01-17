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
#include "SVMatroxLibrary\SVMatroxBufferInterface.h"

SVImageBufferHandleImage::SVImageBufferHandleImage()
	: SvOi::SVImageBufferHandleInterface(), m_Buffer()
{
}

SVImageBufferHandleImage::SVImageBufferHandleImage(const SVMatroxBuffer& rMilImage)
	: m_Buffer(rMilImage)
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

SVPoint SVImageBufferHandleImage::GetPositionPoint() const
{
	SVPoint l_Point;

	if (!(m_Buffer.empty()))
	{
		SVMatroxBufferInterface::GetPositionPoint(l_Point, m_Buffer);
	}

	return l_Point;
}

SVBitmapInfo SVImageBufferHandleImage::GetBitmapInfo() const
{
	SVBitmapInfo l_BitmapInfo;

	if (!(m_Buffer.empty()))
	{
		SVMatroxBufferInterface::GetBitmapInfo(l_BitmapInfo, m_Buffer);
	}

	return l_BitmapInfo;
}

unsigned char* SVImageBufferHandleImage::GetBufferAddress() const
{
	unsigned char* l_pHostBuffer = nullptr;

	if (!(m_Buffer.empty()))
	{
		SVMatroxBufferInterface::GetHostAddress(&l_pHostBuffer, m_Buffer);
	}

	return l_pHostBuffer;
}

const SVMatroxBuffer& SVImageBufferHandleImage::GetBuffer() const
{
	return m_Buffer;
}

SVMatroxBuffer& SVImageBufferHandleImage::GetBuffer()
{
	return m_Buffer;
}

HBITMAP SVImageBufferHandleImage::GetHBitmap() const
{
	HBITMAP hBitmap = nullptr;
	HRESULT status = SVMatroxBufferInterface::Create(hBitmap, GetBuffer());

	return hBitmap;
}