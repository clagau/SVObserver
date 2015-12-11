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

SVMaterialData::SVMaterialData( const SVMaterialData& p_rObject )
: m_Variant()
{
	*this = p_rObject;
}

SVMaterialData::SVMaterialData( const VARIANT& p_rVariant )
: m_Variant()
{
	*this = p_rVariant;
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

const SVMaterialData& SVMaterialData::operator=( const VARIANT& p_rVariant )
{
	if( &m_Variant != &p_rVariant )
	{
		m_Variant = p_rVariant;
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

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVMaterialsLibrary\SVMaterialData.cpp_v  $
 * 
 *    Rev 1.0   22 Apr 2013 14:14:24   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   18 Sep 2012 18:00:00   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Removed redundent casting operator.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   17 Nov 2008 07:23:12   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  615
 * SCR Title:  Integrate PLC Classes into SVObserver Outputs
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Initial Check-in
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/

