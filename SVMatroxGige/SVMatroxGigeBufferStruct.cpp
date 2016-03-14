//******************************************************************************
//* COPYRIGHT (c) 2009 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVMatroxGigeBufferStruct
//* .File Name       : $Workfile:   SVMatroxGigeBufferStruct.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 14:38:24  $
//******************************************************************************

#include "stdafx.h"
#include "SVMatroxGigeBufferStruct.h"
#include "SVHBitmapUtilitiesLibrary/SVImageFormatEnum.h"

SVMatroxGigeBufferStruct::SVMatroxGigeBufferStruct()
: m_ulWidth(640)
, m_ulHeight(480)
, m_ulFormat(SVImageFormatMono8)
, m_ulAddress(0)
, m_bIsLocked(false)
{
}

SVMatroxGigeBufferStruct::SVMatroxGigeBufferStruct( const SVMatroxGigeBufferStruct& p_rValue )
{
	if (this != &p_rValue)
	{
		*this = p_rValue;
	}
}

SVMatroxGigeBufferStruct::~SVMatroxGigeBufferStruct()
{
}

const SVMatroxGigeBufferStruct& SVMatroxGigeBufferStruct::operator=( const SVMatroxGigeBufferStruct& p_rValue )
{
	m_ulWidth = p_rValue.m_ulWidth;
	m_ulHeight = p_rValue.m_ulHeight;
	m_ulFormat = p_rValue.m_ulFormat;
	m_ulAddress = p_rValue.m_ulAddress;
	m_bIsLocked = p_rValue.m_bIsLocked;

	m_Buffer = p_rValue.m_Buffer;

	return *this;
}

bool SVMatroxGigeBufferStruct::IsValid() const
{
	return (m_Buffer.get() != NULL && !m_Buffer.get()->empty());
}

