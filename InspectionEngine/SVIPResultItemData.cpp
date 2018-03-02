//******************************************************************************
//* COPYRIGHT (c) 2012 by Seidenader Vision, Inc.
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVIPResultItemData
//* .File Name       : $Workfile:   SVIPResultItemData.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 11:18:04  $
//******************************************************************************

#include "stdafx.h"
#include "SVIPResultItemData.h"

SVIPResultItemData::SVIPResultItemData()
: m_Value(), m_Color( 0 ), m_IOTypePresent( false ), m_IOType( 0 )
{
}

SVIPResultItemData::SVIPResultItemData( const std::string& p_rValue, unsigned long p_Color )
: m_Value( p_rValue ), m_Color( p_Color ), m_IOTypePresent( false ), m_IOType( 0 )
{
}

SVIPResultItemData::SVIPResultItemData( const std::string& p_rValue, unsigned long p_Color, unsigned long p_IOType )
: m_Value( p_rValue ), m_Color( p_Color ), m_IOTypePresent( true ), m_IOType( p_IOType )
{
}

SVIPResultItemData::SVIPResultItemData( const SVIPResultItemData& p_rObject )
: m_Value( p_rObject.m_Value ), m_Color( p_rObject.m_Color ), m_IOTypePresent( p_rObject.m_IOTypePresent ), m_IOType( p_rObject.m_IOType )
{
}

const std::string& SVIPResultItemData::GetValue() const
{
	return m_Value;
}

unsigned long SVIPResultItemData::GetColor() const
{
	return m_Color;
}

bool SVIPResultItemData::IsIOTypePresent() const
{
	return m_IOTypePresent;
}

unsigned long SVIPResultItemData::GetIOType() const
{
	return m_IOType;
}
