//******************************************************************************
//* COPYRIGHT (c) 2010 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVCommandAttributesTemplate
//* .File Name       : $Workfile:   SVCommandAttributesTemplate.inl  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   18 Apr 2013 18:24:14  $
//******************************************************************************

#ifndef SVCOMMANDATTRIBUTESTEMPLATE_INL
#define SVCOMMANDATTRIBUTESTEMPLATE_INL

#include "SVCommandAttributesTemplate.h"

template< unsigned long p_CommandType, typename SVAttributeCollection, typename SVDefaultCollection >
SVCommandAttributesTemplate< p_CommandType, SVAttributeCollection, SVDefaultCollection >::SVCommandAttributesTemplate()
: m_Attributes( m_DefaultAttributes )
{
}

template< unsigned long p_CommandType, typename SVAttributeCollection, typename SVDefaultCollection >
SVCommandAttributesTemplate< p_CommandType, SVAttributeCollection, SVDefaultCollection >::SVCommandAttributesTemplate( const SVCommandAttributesTemplate& p_rObject )
: m_Attributes( p_rObject.m_Attributes )
{
}

template< unsigned long p_CommandType, typename SVAttributeCollection, typename SVDefaultCollection >
SVCommandAttributesTemplate< p_CommandType, SVAttributeCollection, SVDefaultCollection >::~SVCommandAttributesTemplate()
{
}

template< unsigned long p_CommandType, typename SVAttributeCollection, typename SVDefaultCollection >
const SVCommandAttributesTemplate< p_CommandType, SVAttributeCollection, SVDefaultCollection >& SVCommandAttributesTemplate< p_CommandType, SVAttributeCollection, SVDefaultCollection >::operator=( const SVCommandAttributesTemplate& p_rObject )
{
	if( this != &p_rObject )
	{
		m_Attributes = p_rObject.m_Attributes;
	}

	return *this;
}

template< unsigned long p_CommandType, typename SVAttributeCollection, typename SVDefaultCollection >
HRESULT SVCommandAttributesTemplate< p_CommandType, SVAttributeCollection, SVDefaultCollection >::GetAttributes( SVAttributeCollection& p_rAttributes ) const
{
	p_rAttributes = m_Attributes;

	return S_OK;
}

template< unsigned long p_CommandType, typename SVAttributeCollection, typename SVDefaultCollection >
HRESULT SVCommandAttributesTemplate< p_CommandType, SVAttributeCollection, SVDefaultCollection >::SetAttributes( const SVAttributeCollection& p_rAttributes )
{
	m_Attributes = p_rAttributes;

	return S_OK;
}

#endif

