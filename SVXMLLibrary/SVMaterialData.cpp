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

#pragma region Includes
#include "stdafx.h"
#include "SVMaterialData.h"
#pragma endregion Includes

namespace  SvXml
{
	SVMaterialData::SVMaterialData()
	: m_Variant()
	{
	}

	SVMaterialData::SVMaterialData( const SVMaterialData& rObject )
	{
		m_Variant.Clear();
		::VariantInit( &m_Variant.GetVARIANT() );
		::VariantCopy( &m_Variant.GetVARIANT(), &rObject.m_Variant );
	}

	SVMaterialData::SVMaterialData( const _variant_t& rVariant )
	{
		m_Variant.Clear();
		::VariantInit( &m_Variant.GetVARIANT() );
		::VariantCopy( &m_Variant.GetVARIANT(), &rVariant );
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

	VARIANT SVMaterialData::GetVARIANT()
	{
		return m_Variant.GetVARIANT();
	}

	SVMaterialData::operator const _variant_t & () const
	{
		return m_Variant;
	}

	const SVMaterialData& SVMaterialData::operator=( const SVMaterialData& rObject )
	{
		if( this != &rObject )
		{
			m_Variant.Clear();
			::VariantInit( &m_Variant.GetVARIANT() );
			::VariantCopy( &m_Variant.GetVARIANT(), &rObject.m_Variant );
		}

		return *this;
	}

	const SVMaterialData& SVMaterialData::operator=( const _variant_t& rVariant )
	{
		if( &m_Variant != &rVariant )
		{
			m_Variant.Clear();
			::VariantInit( &m_Variant.GetVARIANT() );
			::VariantCopy( &m_Variant.GetVARIANT(), &rVariant );
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

} //namespace SvXml
