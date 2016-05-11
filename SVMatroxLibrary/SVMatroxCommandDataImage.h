//******************************************************************************
//* COPYRIGHT (c) 2010 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVMatroxCommandDataImage
//* .File Name       : $Workfile:   SVMatroxCommandDataImage.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 15:01:30  $
//******************************************************************************

#pragma once

#include "SVCommandLibrary/SVCommandDataImage.h"
#include "SVMatroxBuffer.h"
#include "SVMatroxBufferCreateStruct.h"

class SVMatroxCommandDataImage : public SVCommandDataImage::SVCommandDataInterface
{
public:
	friend class SVMatroxImageInterface;
	friend class SVMatroxOcrInterface;

	SVMatroxCommandDataImage();
	SVMatroxCommandDataImage( const SVMatroxBufferCreateStruct& p_rCreateData );
	SVMatroxCommandDataImage( const SVMatroxBuffer& p_rBuffer, bool p_ReadOnly = false );
	SVMatroxCommandDataImage( const SVByteVector& p_rData, bool p_ReadOnly = false );

	virtual ~SVMatroxCommandDataImage();

	virtual HRESULT GetData( SVByteVector& p_rData ) const;
	virtual HRESULT SetData( const SVByteVector& p_rData );

protected:
	static HRESULT Convert( HBITMAP hBitmap, SVMatroxBuffer& rImage );
	static HRESULT Convert( const SVMatroxBuffer& rImage, HBITMAP& hBitmap );
	static HRESULT CreateGrayscaleImage( long width, long height, SVMatroxBuffer& rImage );
	static HRESULT ConvertToGrayscale( SVMatroxBuffer& rDstImage, const SVMatroxBuffer& rSrcImage );

	SVMatroxBuffer m_Buffer;

};

