//******************************************************************************
//* COPYRIGHT (c) 2004 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVImageChildStruct
//* .File Name       : $Workfile:   SVImageChildStruct.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 10:49:56  $
//******************************************************************************

#include "stdafx.h"
#include "SVImageChildStruct.h"

SVImageChildStruct::SVImageChildStruct()
:	m_ImageInfo(),
	m_pImageHandles()
{
}

SVImageChildStruct::SVImageChildStruct( const SVImageChildStruct &p_rsvValue )
:	m_ImageInfo( p_rsvValue.m_ImageInfo ),
	m_pImageHandles( p_rsvValue.m_pImageHandles )
{
}

SVImageChildStruct::~SVImageChildStruct()
{
	m_ImageInfo.Initialize();
	m_pImageHandles.reset();
}

const SVImageChildStruct &SVImageChildStruct::operator=( const SVImageChildStruct &p_rsvValue )
{
	if( this != &p_rsvValue )
	{
		m_ImageInfo = p_rsvValue.m_ImageInfo;
		m_pImageHandles = p_rsvValue.m_pImageHandles;
	}

	return *this;
}
