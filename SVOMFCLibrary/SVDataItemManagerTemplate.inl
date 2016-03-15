// ******************************************************************************
// * COPYRIGHT (c) 2013 by Seidenader, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVDataItemManagerTemplate
// * .File Name       : $Workfile:   SVDataItemManagerTemplate.inl  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.2  $
// * .Check In Date   : $Date:   02 Oct 2013 08:55:16  $
// ******************************************************************************

#ifndef SVDATAITEMMANAGERTEMPLATE_INL
#define SVDATAITEMMANAGERTEMPLATE_INL

#include "SVDataItemManagerTemplate.h"

template< typename SVItemData, typename SVIndexType >
SVDataItemManagerTemplate< SVItemData, SVIndexType >::SVDataItemManagerTemplate()
: m_GetFunctor(), m_SetFunctor(), m_LastUsedIdentifier( 0 ), m_Items()
{
}

template< typename SVItemData, typename SVIndexType  >
SVDataItemManagerTemplate< SVItemData, SVIndexType >::SVDataItemManagerTemplate( SVGetItemDataFunctor p_GetFunctor, SVSetItemDataFunctor p_SetFunctor )
: m_GetFunctor( p_GetFunctor ), m_SetFunctor( p_SetFunctor ), m_LastUsedIdentifier( 0 ), m_Items()
{
}

template< typename SVItemData, typename SVIndexType  >
SVDataItemManagerTemplate< SVItemData, SVIndexType >::SVDataItemManagerTemplate( const SVDataItemManagerTemplate& p_rObject )
: m_LastUsedIdentifier( p_rObject.m_LastUsedIdentifier ), m_Items( p_rObject.m_Items )
{
}

template< typename SVItemData, typename SVIndexType  >
SVDataItemManagerTemplate< SVItemData, SVIndexType >::~SVDataItemManagerTemplate()
{
	clear();
}

template< typename SVItemData, typename SVIndexType >
bool SVDataItemManagerTemplate< SVItemData, SVIndexType >::empty() const
{
	return m_Items.empty();
}

template< typename SVItemData, typename SVIndexType >
void SVDataItemManagerTemplate< SVItemData, SVIndexType >::clear()
{
	if( !( empty() ) )
	{
		m_Items.clear();
	}
}

template< typename SVItemData, typename SVIndexType >
typename SVDataItemManagerTemplate< SVItemData, SVIndexType >::iterator SVDataItemManagerTemplate< SVItemData, SVIndexType >::end()
{
	return m_Items.end();
}

template< typename SVItemData, typename SVIndexType >
typename SVDataItemManagerTemplate< SVItemData, SVIndexType >::const_iterator SVDataItemManagerTemplate< SVItemData, SVIndexType >::end() const
{
	return m_Items.end();
}

template< typename SVItemData, typename SVIndexType >
typename SVDataItemManagerTemplate< SVItemData, SVIndexType >::iterator SVDataItemManagerTemplate< SVItemData, SVIndexType >::find( const key_type& p_Keyval )
{
	return m_Items.find( p_Keyval );
}

template< typename SVItemData, typename SVIndexType >
typename SVDataItemManagerTemplate< SVItemData, SVIndexType >::const_iterator SVDataItemManagerTemplate< SVItemData, SVIndexType >::find( const key_type& p_Keyval ) const
{
	return m_Items.find( p_Keyval );
}

template< typename SVItemData, typename SVIndexType >
void SVDataItemManagerTemplate< SVItemData, SVIndexType >::erase( iterator p_Where )
{
	m_Items.erase( p_Where );
}

template< typename SVItemData, typename SVIndexType >
typename SVDataItemManagerTemplate< SVItemData, SVIndexType >::size_type SVDataItemManagerTemplate< SVItemData, SVIndexType >::erase( const key_type& p_Keyval )
{
	return m_Items.erase( p_Keyval );
}

template< typename SVItemData, typename SVIndexType >
typename SVDataItemManagerTemplate< SVItemData, SVIndexType >::iterator SVDataItemManagerTemplate< SVItemData, SVIndexType >::GetItemData( const SVIndexType& p_IndexVal )
{
	iterator l_Iter = end();
	unsigned long l_Identifer = static_cast< unsigned long >( m_GetFunctor( p_IndexVal ) );

	if( l_Identifer != 0 )
	{
		l_Iter = find( l_Identifer );
	}

	return l_Iter;
}

template< typename SVItemData, typename SVIndexType >
typename SVDataItemManagerTemplate< SVItemData, SVIndexType >::const_iterator SVDataItemManagerTemplate< SVItemData, SVIndexType >::GetItemData( const SVIndexType& p_IndexVal ) const
{
	const_iterator l_Iter = end();

	unsigned long l_Identifer = static_cast< unsigned long >( m_GetFunctor( p_IndexVal ) );

	if( l_Identifer != 0 )
	{
		l_Iter = find( l_Identifer );
	}

	return l_Iter;
}

template< typename SVItemData, typename SVIndexType >
bool SVDataItemManagerTemplate< SVItemData, SVIndexType >::SetItemData( const SVIndexType& p_IndexVal, const SVItemData& p_rItem )
{
	ClearItemData( p_IndexVal );

	unsigned long l_NewIdentifier = ++m_LastUsedIdentifier;

	if( l_NewIdentifier == 0 )
	{
		l_NewIdentifier = ++m_LastUsedIdentifier;
	}

	m_Items[ l_NewIdentifier ] = p_rItem;

	return ( m_SetFunctor( p_IndexVal, static_cast< DWORD_PTR >( l_NewIdentifier ) ) != FALSE );
}

template< typename SVItemData, typename SVIndexType >
void SVDataItemManagerTemplate< SVItemData, SVIndexType >::ClearItemData( const SVIndexType& p_IndexVal )
{
	unsigned long l_Identifer = static_cast< unsigned long >( m_GetFunctor( p_IndexVal ) );

	if( l_Identifer != 0 )
	{
		m_SetFunctor( p_IndexVal, 0 );

		iterator l_Iter = find( l_Identifer );

		if( l_Iter != end() )
		{
			erase( l_Iter );
		}
	}
}

#endif

