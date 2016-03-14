//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVMaterialData
//* .File Name       : $Workfile:   SVMaterialData.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 14:14:24  $
//******************************************************************************

#include "stdafx.h"
#include "SVMaterialData.h"

SVMaterialData::SVMaterialData()
: m_Variant()
{
}

SVMaterialData::SVMaterialData( const SVMaterialData& rObject )
: m_Variant(rObject.m_Variant)
{
}

SVMaterialData::SVMaterialData( const _variant_t& rVariant )
: m_Variant( rVariant )
{
}

SVMaterialData::~SVMaterialData()
{
	m_Variant.Clear();
}

bool SVMaterialData::empty() const
{
	return m_Variant.vt == VT_EMPTY;
}

void SVMaterialData::clear()
{
	m_Variant.Clear();
}

SVMaterialData::operator const _variant_t & () const
{
	return m_Variant;
}

const SVMaterialData& SVMaterialData::operator=( const SVMaterialData& p_rObject )
{
	if( this != &p_rObject )
	{
		m_Variant = p_rObject.m_Variant;
	}

	return *this;
}

const SVMaterialData& SVMaterialData::operator=( const _variant_t& rVariant )
{
	if( &m_Variant != &rVariant )
	{
		m_Variant = rVariant;
	}

	return *this;
}

bool SVMaterialData::operator==( const SVMaterialData& p_rObject ) const
{
	return m_Variant == p_rObject.m_Variant;
}

bool SVMaterialData::operator!=( const SVMaterialData& p_rObject ) const
{
	return m_Variant != p_rObject.m_Variant;
}

