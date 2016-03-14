//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVImageBufferHandleStruct
//* .File Name       : $Workfile:   SVImageBufferHandleStruct.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 10:44:08  $
//******************************************************************************

#include "stdafx.h"
#include "SVImageBufferHandleStruct.h"
#include "SVMatroxLibrary/SVMatroxLibrary.h"
#include "SVImageBufferHandleData.h"

SVImageBufferHandleStruct::SVImageBufferHandleStruct()
: milImage()
{
}

SVImageBufferHandleStruct::SVImageBufferHandleStruct( const SVMatroxBuffer& p_rMilImage )
: milImage( p_rMilImage )
{
}

SVImageBufferHandleStruct::~SVImageBufferHandleStruct()
{
	LocalClear();
}

bool SVImageBufferHandleStruct::empty() const
{
	return milImage.empty();
}

void SVImageBufferHandleStruct::clear()
{
	LocalClear();
}

SVPOINT SVImageBufferHandleStruct::GetPositionPoint() const
{
	SVPOINT l_Point;

	if( !( milImage.empty() ) )
	{
		SVMatroxBufferInterface::GetPositionPoint( l_Point, milImage );
	}

	return l_Point;
}

SVBitmapInfo SVImageBufferHandleStruct::GetBitmapInfo() const
{
	SVBitmapInfo l_BitmapInfo;

	if( !( milImage.empty() ) )
	{
		SVMatroxBufferInterface::GetBitmapInfo( l_BitmapInfo, milImage );
	}

	return l_BitmapInfo;
}

unsigned char* SVImageBufferHandleStruct::GetBufferAddress() const
{
	unsigned char* l_pHostBuffer = NULL;

	if( !( milImage.empty() ) )
	{
		SVMatroxBufferInterface::GetHostAddress( &l_pHostBuffer, milImage );
	}

	return l_pHostBuffer;
}

HRESULT SVImageBufferHandleStruct::GetData( SVImageBufferHandleData& p_rData ) const
{
	return p_rData.SetHandleData( *this );
}

void SVImageBufferHandleStruct::LocalClear()
{
	milImage.clear();
}

