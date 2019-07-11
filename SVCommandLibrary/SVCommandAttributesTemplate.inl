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

template< unsigned long CommandType, typename SVAttributeCollection, typename SVDefaultCollection >
SVCommandAttributesTemplate< CommandType, SVAttributeCollection, SVDefaultCollection >::SVCommandAttributesTemplate()
: m_Attributes( m_DefaultAttributes )
{
}

template< unsigned long CommandType, typename SVAttributeCollection, typename SVDefaultCollection >
SVCommandAttributesTemplate< CommandType, SVAttributeCollection, SVDefaultCollection >::SVCommandAttributesTemplate( const SVCommandAttributesTemplate& p_rObject )
: m_Attributes( p_rObject.m_Attributes )
{
}

template< unsigned long CommandType, typename SVAttributeCollection, typename SVDefaultCollection >
SVCommandAttributesTemplate< CommandType, SVAttributeCollection, SVDefaultCollection >::~SVCommandAttributesTemplate()
{
}

template< unsigned long CommandType, typename SVAttributeCollection, typename SVDefaultCollection >
const SVCommandAttributesTemplate< CommandType, SVAttributeCollection, SVDefaultCollection >& SVCommandAttributesTemplate< CommandType, SVAttributeCollection, SVDefaultCollection >::operator=( const SVCommandAttributesTemplate& p_rObject )
{
	if( this != &p_rObject )
	{
		m_Attributes = p_rObject.m_Attributes;
	}

	return *this;
}

template< unsigned long CommandType, typename SVAttributeCollection, typename SVDefaultCollection >
HRESULT SVCommandAttributesTemplate< CommandType, SVAttributeCollection, SVDefaultCollection >::GetAttributes( SVAttributeCollection& p_rAttributes ) const
{
	p_rAttributes = m_Attributes;

	return S_OK;
}

template< unsigned long CommandType, typename SVAttributeCollection, typename SVDefaultCollection >
HRESULT SVCommandAttributesTemplate< CommandType, SVAttributeCollection, SVDefaultCollection >::SetAttributes( const SVAttributeCollection& p_rAttributes )
{
	m_Attributes = p_rAttributes;

	return S_OK;
}

#endif

