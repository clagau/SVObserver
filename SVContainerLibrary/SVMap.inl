//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVMap
//* .File Name       : $Workfile:   SVMap.inl  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 07:44:16  $
//******************************************************************************

template
<
	typename SVT_KEY,
	typename SVT_VALUE
>
SVMap< SVT_KEY, SVT_VALUE>::SVMap()
:m_Map()
{
}

template
<
	typename SVT_KEY,
	typename SVT_VALUE
>
SVMap< SVT_KEY, SVT_VALUE>::SVMap( const SVMap& p_rObject )
:m_Map( p_rObject.m_Map )
{
}

template
<
	typename SVT_KEY,
	typename SVT_VALUE
>
SVMap< SVT_KEY, SVT_VALUE>::SVMap( const SVMapBase& p_rObject )
:m_Map( p_rObject )
{
}

template
<
	typename SVT_KEY,
	typename SVT_VALUE
>
SVMap< SVT_KEY, SVT_VALUE>::~SVMap()
{
}

template
<
	typename SVT_KEY,
	typename SVT_VALUE
>
SVMap< SVT_KEY, SVT_VALUE>::operator SVMapBase const & () const
{
	return m_Map;
}

template
<
	typename SVT_KEY,
	typename SVT_VALUE
>
SVMap< SVT_KEY, SVT_VALUE>::operator SVMapBase & ()
{
	return m_Map;
}

template
<
	typename SVT_KEY,
	typename SVT_VALUE
>
const SVMap< SVT_KEY, SVT_VALUE>& SVMap< SVT_KEY, SVT_VALUE>::operator=( const SVMap& p_rObject )
{
	if( this != &p_rObject )
	{
		m_Map = p_rObject.m_Map;
	}

	return *this;
}

template
<
	typename SVT_KEY,
	typename SVT_VALUE
>
const SVMap< SVT_KEY, SVT_VALUE>& SVMap< SVT_KEY, SVT_VALUE>::operator=( const SVMapBase& p_rObject )
{
	if( &m_Map != &p_rObject )
	{
		m_Map = p_rObject;
	}

	return *this;
}

template
<
	typename SVT_KEY,
	typename SVT_VALUE
>
bool SVMap< SVT_KEY, SVT_VALUE>::empty() const
{
	return m_Map.empty();
}

template
<
	typename SVT_KEY,
	typename SVT_VALUE
>
void SVMap< SVT_KEY, SVT_VALUE>::clear()
{
	return m_Map.clear();
}

template
<
	typename SVT_KEY,
	typename SVT_VALUE
>
typename SVMap< SVT_KEY, SVT_VALUE>::size_type SVMap< SVT_KEY, SVT_VALUE>::size() const
{
	return m_Map.size();
}

template
<
	typename SVT_KEY,
	typename SVT_VALUE
>
typename SVMap< SVT_KEY, SVT_VALUE>::iterator SVMap< SVT_KEY, SVT_VALUE>::begin()
{
	return m_Map.begin();
}

template
<
	typename SVT_KEY,
	typename SVT_VALUE
>
typename SVMap< SVT_KEY, SVT_VALUE>::const_iterator SVMap< SVT_KEY, SVT_VALUE>::begin() const
{
	return m_Map.begin();
}

template
<
	typename SVT_KEY,
	typename SVT_VALUE
>
typename SVMap< SVT_KEY, SVT_VALUE>::iterator SVMap< SVT_KEY, SVT_VALUE>::end()
{
	return m_Map.end();
}

template
<
	typename SVT_KEY,
	typename SVT_VALUE
>
typename SVMap< SVT_KEY, SVT_VALUE>::const_iterator SVMap< SVT_KEY, SVT_VALUE>::end() const
{
	return m_Map.end();
}

template
<
	typename SVT_KEY,
	typename SVT_VALUE
>
typename SVMap< SVT_KEY, SVT_VALUE>::iterator SVMap< SVT_KEY, SVT_VALUE>::find( const key_type& p_rKey )
{
	return m_Map.find( p_rKey );
}

template
<
	typename SVT_KEY,
	typename SVT_VALUE
>
typename SVMap< SVT_KEY, SVT_VALUE>::const_iterator SVMap< SVT_KEY, SVT_VALUE>::find( const key_type& p_rKey ) const
{
	return m_Map.find( p_rKey );
}

template
<
	typename SVT_KEY,
	typename SVT_VALUE
>
typename SVMap< SVT_KEY, SVT_VALUE>::SVIteratorBoolPair SVMap< SVT_KEY, SVT_VALUE>::insert( const value_type& p_rKeyValuePair )
{
	return m_Map.insert( p_rKeyValuePair );
}

template
<
	typename SVT_KEY,
	typename SVT_VALUE
>
typename SVMap< SVT_KEY, SVT_VALUE>::SVIteratorBoolPair SVMap< SVT_KEY, SVT_VALUE>::insert( const key_type& p_rKey, const mapped_type& p_rValue )
{
	return m_Map.insert( value_type( p_rKey, p_rValue ) );
}

template
<
	typename SVT_KEY,
	typename SVT_VALUE
>
template< class InputIterator >
void SVMap< SVT_KEY, SVT_VALUE >::insert( InputIterator p_First, InputIterator p_Last )
{
	m_Map.insert( p_First, p_Last );
}

template
<
	typename SVT_KEY,
	typename SVT_VALUE
>
typename SVMap< SVT_KEY, SVT_VALUE>::size_type SVMap< SVT_KEY, SVT_VALUE>::erase( const key_type& p_rKey )
{
	return m_Map.erase( p_rKey );
}

template
<
	typename SVT_KEY,
	typename SVT_VALUE
>
typename SVMap< SVT_KEY, SVT_VALUE>::iterator SVMap< SVT_KEY, SVT_VALUE>::erase( iterator p_Where )
{
	return m_Map.erase( p_Where );
}

template
<
	typename SVT_KEY,
	typename SVT_VALUE
>
bool SVMap< SVT_KEY, SVT_VALUE>::IsEmpty() const
{
	return m_Map.empty();
}

template
<
	typename SVT_KEY,
	typename SVT_VALUE
>
int SVMap< SVT_KEY, SVT_VALUE>::GetCount() const
{
	return static_cast< int >( m_Map.size() );
}

template
<
	typename SVT_KEY,
	typename SVT_VALUE
>
int SVMap< SVT_KEY, SVT_VALUE>::GetSize() const
{
	return static_cast< int >( m_Map.size() );
}

template
<
	typename SVT_KEY,
	typename SVT_VALUE
>
typename SVMap< SVT_KEY, SVT_VALUE>::iterator SVMap< SVT_KEY, SVT_VALUE>::GetStartPosition() const
{
	SVMapBase* l_pMap( const_cast< SVMapBase* >( &m_Map ) );

	return l_pMap->begin();
}

template
<
	typename SVT_KEY,
	typename SVT_VALUE
>
void SVMap< SVT_KEY, SVT_VALUE>::GetNextAssoc( iterator& rNextPosition, key_type& rKey, mapped_type& rValue ) const
{
	if( rNextPosition != m_Map.end() )
	{
		rKey = rNextPosition->first;
		rValue = rNextPosition->second;

		++rNextPosition;
	}
}

template
<
	typename SVT_KEY,
	typename SVT_VALUE
>
bool SVMap< SVT_KEY, SVT_VALUE>::Lookup( const key_type& key, mapped_type& rValue ) const
{
	bool l_Status( false );

	SVMapBase::const_iterator l_Iter( m_Map.find( key ) );

	l_Status = l_Iter != m_Map.end();

	if( l_Status )
	{
		rValue = l_Iter->second;
	}

	return l_Status;
}

template
<
	typename SVT_KEY,
	typename SVT_VALUE
>
void SVMap< SVT_KEY, SVT_VALUE>::SetAt( const key_type& key, mapped_type& newElement )
{
	m_Map[ key ] = newElement;
}

template
<
	typename SVT_KEY,
	typename SVT_VALUE
>
typename SVMap< SVT_KEY, SVT_VALUE>::mapped_type& SVMap< SVT_KEY, SVT_VALUE>::operator[]( const key_type& key )
{
	return m_Map[ key ];
}

template
<
	typename SVT_KEY,
	typename SVT_VALUE
>
void SVMap< SVT_KEY, SVT_VALUE>::RemoveKey( const key_type& key )
{
	m_Map.erase( key );
}

template
<
	typename SVT_KEY,
	typename SVT_VALUE
>
void SVMap< SVT_KEY, SVT_VALUE>::RemoveAll()
{
	m_Map.clear();
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVContainerLibrary\SVMap.inl_v  $
 * 
 *    Rev 1.0   22 Apr 2013 07:44:16   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   09 Nov 2010 11:49:04   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  704
 * SCR Title:  Upgrade SVObserver Version for 5.33 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source to add missing type identifiers.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   22 Jun 2010 13:37:58   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  693
 * SCR Title:  Fix Performance Issue with Inspection Process
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fixed issues with containers.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   17 Nov 2008 07:21:24   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  615
 * SCR Title:  Integrate PLC Classes into SVObserver Outputs
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Initial Check-in
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/

