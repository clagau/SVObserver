//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVVector
//* .File Name       : $Workfile:   SVVector.inl  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   30 Sep 2013 14:41:50  $
//******************************************************************************

template<typename SVT_TYPE>
SVVector<SVT_TYPE>::SVVector()
: m_Vector()
{
}

template<typename SVT_TYPE>
SVVector<SVT_TYPE>::SVVector( size_type p_InitSize )
: m_Vector( p_InitSize )
{
}

template<typename SVT_TYPE>
SVVector<SVT_TYPE>::SVVector( const SVVector& p_rObject )
: m_Vector( p_rObject.m_Vector )
{
}

template<typename SVT_TYPE>
SVVector<SVT_TYPE>::SVVector( const SVVectorBase& p_rObject )
: m_Vector( p_rObject )
{
}

template<typename SVT_TYPE>
SVVector<SVT_TYPE>::~SVVector()
{
}

template<typename SVT_TYPE>
SVVector<SVT_TYPE>::operator SVVectorBase const & () const
{
	return m_Vector;
}

template<typename SVT_TYPE>
SVVector<SVT_TYPE>::operator SVVectorBase & ()
{
	return m_Vector;
}

template<typename SVT_TYPE>
const SVVector<SVT_TYPE>& SVVector<SVT_TYPE>::operator=( const SVVector& p_rObject )
{
	if( this != &p_rObject )
	{
		m_Vector = p_rObject.m_Vector;
	}

	return *this;
}

template<typename SVT_TYPE>
const SVVector<SVT_TYPE>& SVVector<SVT_TYPE>::operator=( const SVVectorBase& p_rObject )
{
	if( &m_Vector != &p_rObject )
	{
		m_Vector = p_rObject;
	}

	return *this;
}

template<typename SVT_TYPE>
bool SVVector<SVT_TYPE>::empty() const
{
	return m_Vector.empty();
}

template<typename SVT_TYPE>
size_t SVVector<SVT_TYPE>::size()const
{
	return m_Vector.size();
}

template<typename SVT_TYPE>
typename SVVector<SVT_TYPE>::iterator SVVector<SVT_TYPE>::insert( iterator p_Where, SVArgType p_Value )
{
	return m_Vector.insert( p_Where, p_Value );
}

template<typename SVT_TYPE>
typename SVVector<SVT_TYPE>::iterator SVVector<SVT_TYPE>::erase( iterator p_Where )
{
	return m_Vector.erase( p_Where );
}

template<typename SVT_TYPE>
typename SVVector<SVT_TYPE>::iterator SVVector<SVT_TYPE>::erase( const_iterator p_Where )
{
	return m_Vector.erase( p_Where );
}

template<typename SVT_TYPE>
void SVVector<SVT_TYPE>::clear()
{
	if( ! m_Vector.empty() )
	{
		m_Vector.clear();
	}
}

template<typename SVT_TYPE>
bool SVVector<SVT_TYPE>::operator==(const SVVector<SVT_TYPE>& rVector) const
{
	return (m_Vector == rVector.m_Vector);
}

template<typename SVT_TYPE>
bool SVVector<SVT_TYPE>::operator!=(const SVVector<SVT_TYPE>& rVector) const
{
	return (m_Vector != rVector.m_Vector);
}

template<typename SVT_TYPE>
void SVVector<SVT_TYPE>::swap(SVVector<SVT_TYPE>& rVector)
{
	m_Vector.swap(rVector.m_Vector);
}

template<typename SVT_TYPE>
int SVVector<SVT_TYPE>::GetSize() const
{
	return static_cast< int >( m_Vector.size() );
}

template<typename SVT_TYPE>
INT_PTR SVVector<SVT_TYPE>::GetUpperBound() const
{
	return GetSize() - 1;
}

template<typename SVT_TYPE>
void SVVector<SVT_TYPE>::SetSize( int nNewSize )
{
	m_Vector.resize( nNewSize );
}

// Add SFINAE to this
template<typename SVT_TYPE>
const typename SVT_TYPE* SVVector<SVT_TYPE>::GetData() const
{
	const SVT_TYPE* pData( nullptr );

	if( 0 < m_Vector.size() )
	{
		pData = &m_Vector[ 0 ];
	}

	return pData;
}

// Add SFINAE to this
template<typename SVT_TYPE>
typename SVT_TYPE* SVVector<SVT_TYPE>::GetData()
{
	SVT_TYPE* pData( nullptr );

	if( 0 < m_Vector.size() )
	{
		pData = &m_Vector[0];
	}

	return pData;
}

template<typename SVT_TYPE>
typename SVT_TYPE SVVector<SVT_TYPE>::GetAt( int nIndex ) const
{
	return m_Vector[ nIndex ];
}

template<typename SVT_TYPE>
typename SVT_TYPE& SVVector<SVT_TYPE>::ElementAt( int nIndex )
{
	return m_Vector[ nIndex ];
}

template<typename SVT_TYPE>
const typename SVT_TYPE& SVVector<SVT_TYPE>::ElementAt( int nIndex ) const
{
	return m_Vector[ nIndex ];
}

template<typename SVT_TYPE>
const typename SVT_TYPE& SVVector<SVT_TYPE>::operator[]( size_type nIndex ) const
{
	return m_Vector[ nIndex ];
}

// Add SFINAE to this
template<typename SVT_TYPE>
typename SVT_TYPE& SVVector<SVT_TYPE>::operator[]( size_type nIndex )
{
	return m_Vector[ nIndex ];
}

template<typename SVT_TYPE>
void SVVector<SVT_TYPE>::Fill( SVArgType fillValue )
{
}

template<typename SVT_TYPE>
int SVVector<SVT_TYPE>::Add( SVArgType newElement )
{
	m_Vector.push_back( newElement );

	return static_cast< int >( GetSize() - 1 );
}

template<typename SVT_TYPE>
INT_PTR SVVector<SVT_TYPE>::Append( const SVVector& src )
{
	const SVVectorBase& l_rSource( src );

	INT_PTR l_Index( GetSize() );

	m_Vector.insert( m_Vector.end(), l_rSource.begin(), l_rSource.end() );

	return l_Index;
}

template<typename SVT_TYPE>
void SVVector<SVT_TYPE>::InsertAt( int nIndex, SVArgType newElement, int nCount = 1 )
{
	SVVectorBase::iterator l_Iter( m_Vector.begin() );

	if( 0 < nIndex )
	{
		std::advance( l_Iter, nIndex );
	}

	m_Vector.insert( l_Iter, nCount, newElement );
}

template<typename SVT_TYPE>
void SVVector<SVT_TYPE>::InsertAt( int nStartIndex, SVVector* pNewArray )
{
	SVVectorBase& l_rSource( *pNewArray );

	SVVectorBase::iterator l_Iter( l_rSource.begin() );

	std::advance( l_Iter, nStartIndex );

	m_Vector.insert( l_Iter, l_rSource.begin(), l_rSource.end() );
}

template<typename SVT_TYPE>
void SVVector<SVT_TYPE>::SetAt( int nIndex, SVArgType newElement )
{
	m_Vector[ nIndex ] = newElement;
}

template<typename SVT_TYPE>
void SVVector<SVT_TYPE>::SetAtGrow( int nIndex, SVArgType newElement )
{
	if( GetSize() <= nIndex )
	{
		SetSize( nIndex + 1 );
	}

	m_Vector[ nIndex ] = newElement;
}

template<typename SVT_TYPE>
void SVVector<SVT_TYPE>::Copy( const SVVector& src )
{
	*this = src;
}

template<typename SVT_TYPE>
void SVVector<SVT_TYPE>::RemoveAt( int nIndex, int nCount = 1 )
{
	int l_Count( 0 );

	SVVectorBase::iterator l_Iter( m_Vector.begin() );

	std::advance( l_Iter, nIndex );

	while( l_Count < nCount && l_Iter != m_Vector.end() )
	{
		l_Iter = m_Vector.erase( l_Iter );

		l_Count++;
	}
}

template<typename SVT_TYPE>
void SVVector<SVT_TYPE>::RemoveAll()
{
	if( ! m_Vector.empty() )
	{
		m_Vector.clear();
	}
}

