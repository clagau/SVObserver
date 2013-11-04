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

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVCommandLibrary\SVCommandAttributesTemplate.inl_v  $
 * 
 *    Rev 1.0   18 Apr 2013 18:24:14   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   15 Oct 2010 14:31:44   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  701
 * SCR Title:  Add New Font Functionality to the Existing Command Interface
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Adding initial code for Command subsystem.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
