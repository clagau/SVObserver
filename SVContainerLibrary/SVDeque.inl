//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVDeque
//* .File Name       : $Workfile:   SVDeque.inl  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 07:44:10  $
//******************************************************************************

template< typename SVT_TYPE >
SVDeque< SVT_TYPE >::SVDeque()
: m_Deque()
{
}

template< typename SVT_TYPE >
SVDeque< SVT_TYPE >::SVDeque( const SVDeque& p_rObject )
: m_Deque( p_rObject.m_Deque )
{
}

template< typename SVT_TYPE >
SVDeque< SVT_TYPE >::SVDeque( const SVDequeBase& p_rObject )
: m_Deque( p_rObject )
{
}

template< typename SVT_TYPE >
SVDeque< SVT_TYPE >::~SVDeque()
{
}

template< typename SVT_TYPE >
SVDeque< SVT_TYPE >::operator SVDequeBase const & () const
{
	return m_Deque;
}

template< typename SVT_TYPE >
SVDeque< SVT_TYPE >::operator SVDequeBase & ()
{
	return m_Deque;
}

template< typename SVT_TYPE >
const SVDeque< SVT_TYPE >& SVDeque< SVT_TYPE >::operator=( const SVDeque& p_rObject )
{
	if( this != &p_rObject )
	{
		m_Deque = p_rObject.m_Deque;
	}

	return *this;
}

template< typename SVT_TYPE >
const SVDeque< SVT_TYPE >& SVDeque< SVT_TYPE >::operator=( const SVDequeBase& p_rObject )
{
	if( &m_Deque != &p_rObject )
	{
		m_Deque = p_rObject;
	}

	return *this;
}

template< typename SVT_TYPE >
bool SVDeque< SVT_TYPE >::empty() const
{
	return m_Deque.empty();
}

template< typename SVT_TYPE >
void SVDeque< SVT_TYPE >::clear()
{
	m_Deque.clear();
}

template< typename SVT_TYPE >
void SVDeque< SVT_TYPE >::push_back( const SVType& p_Val )
{
	m_Deque.push_back( p_Val );
}

template< typename SVT_TYPE >
typename SVDeque< SVT_TYPE >::iterator SVDeque< SVT_TYPE >::insert( iterator p_Where, const SVType& p_rVal )
{
	return m_Deque.insert( p_Where, p_rVal );
}

template< typename SVT_TYPE >
INT_PTR SVDeque< SVT_TYPE >::GetCount() const
{
	return static_cast< INT_PTR >( m_Deque.size() );
}

template< typename SVT_TYPE >
INT_PTR SVDeque< SVT_TYPE >::GetSize() const
{
	return static_cast< INT_PTR >( m_Deque.size() );
}

template< typename SVT_TYPE >
INT_PTR SVDeque< SVT_TYPE >::GetUpperBound() const
{
	return static_cast< INT_PTR >( m_Deque.size() - 1 );
}

template< typename SVT_TYPE >
INT_PTR SVDeque< SVT_TYPE >::Add(const SVType& newElement)
{
	m_Deque.push_back( newElement );

	return static_cast< INT_PTR >( m_Deque.size() - 1 );
}

template< typename SVT_TYPE >
void SVDeque< SVT_TYPE >::SetAt(INT_PTR nIndex, const SVType& newElement)
{
	m_Deque[ nIndex ] = newElement;
}

template< typename SVT_TYPE >
typename SVDeque< SVT_TYPE >::SVType& SVDeque< SVT_TYPE >::GetAt( iterator position )
{
	return *position;
}

template< typename SVT_TYPE >
const typename SVDeque< SVT_TYPE >::SVType& SVDeque< SVT_TYPE >::GetAt( INT_PTR nIndex ) const
{
	return m_Deque[ nIndex ];
}

template< typename SVT_TYPE >
void SVDeque< SVT_TYPE >::RemoveAll()
{
	if( !empty() )
	{
		clear();
	}
}


