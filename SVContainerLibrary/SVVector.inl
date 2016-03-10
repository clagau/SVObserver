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

template< typename SVT_TYPE, typename SVT_ARG_TYPE >
SVVector< SVT_TYPE, SVT_ARG_TYPE >::SVVector()
: m_Vector()
{
}

template< typename SVT_TYPE, typename SVT_ARG_TYPE >
SVVector< SVT_TYPE, SVT_ARG_TYPE >::SVVector( size_type p_InitSize )
: m_Vector( p_InitSize )
{
}

template< typename SVT_TYPE, typename SVT_ARG_TYPE >
SVVector< SVT_TYPE, SVT_ARG_TYPE >::SVVector( const SVVector& p_rObject )
: m_Vector( p_rObject.m_Vector )
{
}

template< typename SVT_TYPE, typename SVT_ARG_TYPE >
SVVector< SVT_TYPE, SVT_ARG_TYPE >::SVVector( const SVVectorBase& p_rObject )
: m_Vector( p_rObject )
{
}

template< typename SVT_TYPE, typename SVT_ARG_TYPE >
SVVector< SVT_TYPE, SVT_ARG_TYPE >::~SVVector()
{
}

template< typename SVT_TYPE, typename SVT_ARG_TYPE >
SVVector< SVT_TYPE, SVT_ARG_TYPE >::operator SVVectorBase const & () const
{
	return m_Vector;
}

template< typename SVT_TYPE, typename SVT_ARG_TYPE >
SVVector< SVT_TYPE, SVT_ARG_TYPE >::operator SVVectorBase & ()
{
	return m_Vector;
}

template< typename SVT_TYPE, typename SVT_ARG_TYPE >
const SVVector< SVT_TYPE, SVT_ARG_TYPE >& SVVector< SVT_TYPE, SVT_ARG_TYPE >::operator=( const SVVector& p_rObject )
{
	if( this != &p_rObject )
	{
		m_Vector = p_rObject.m_Vector;
	}

	return *this;
}

template< typename SVT_TYPE, typename SVT_ARG_TYPE >
const SVVector< SVT_TYPE, SVT_ARG_TYPE >& SVVector< SVT_TYPE, SVT_ARG_TYPE >::operator=( const SVVectorBase& p_rObject )
{
	if( &m_Vector != &p_rObject )
	{
		m_Vector = p_rObject;
	}

	return *this;
}

template< typename SVT_TYPE, typename SVT_ARG_TYPE >
bool SVVector< SVT_TYPE, SVT_ARG_TYPE >::empty() const
{
	return m_Vector.empty();
}

template< typename SVT_TYPE, typename SVT_ARG_TYPE >
size_t SVVector< SVT_TYPE, SVT_ARG_TYPE >::size()const
{
	return m_Vector.size();
}

template< typename SVT_TYPE, typename SVT_ARG_TYPE >
typename SVVector< SVT_TYPE, SVT_ARG_TYPE >::iterator SVVector< SVT_TYPE, SVT_ARG_TYPE >::insert( iterator p_Where, SVArgType p_Value )
{
	return m_Vector.insert( p_Where, p_Value );
}

template< typename SVT_TYPE, typename SVT_ARG_TYPE >
typename SVVector< SVT_TYPE, SVT_ARG_TYPE >::iterator SVVector< SVT_TYPE, SVT_ARG_TYPE >::erase( iterator p_Where )
{
	return m_Vector.erase( p_Where );
}

template< typename SVT_TYPE, typename SVT_ARG_TYPE >
typename SVVector< SVT_TYPE, SVT_ARG_TYPE >::iterator SVVector< SVT_TYPE, SVT_ARG_TYPE >::erase( const_iterator p_Where )
{
	return m_Vector.erase( p_Where );
}

template< typename SVT_TYPE, typename SVT_ARG_TYPE >
void SVVector< SVT_TYPE, SVT_ARG_TYPE >::clear()
{
	if( ! m_Vector.empty() )
	{
		m_Vector.clear();
	}
}

template< typename SVT_TYPE, typename SVT_ARG_TYPE >
bool SVVector< SVT_TYPE, SVT_ARG_TYPE >::operator==(const SVVector< SVT_TYPE, SVT_ARG_TYPE >& rVector) const
{
	return (m_Vector == rVector.m_Vector);
}

template< typename SVT_TYPE, typename SVT_ARG_TYPE >
bool SVVector< SVT_TYPE, SVT_ARG_TYPE >::operator!=(const SVVector< SVT_TYPE, SVT_ARG_TYPE >& rVector) const
{
	return (m_Vector != rVector.m_Vector);
}

template< typename SVT_TYPE, typename SVT_ARG_TYPE >
void SVVector< SVT_TYPE, SVT_ARG_TYPE >::swap(SVVector< SVT_TYPE, SVT_ARG_TYPE >& rVector)
{
	m_Vector.swap(rVector.m_Vector);
}

template< typename SVT_TYPE, typename SVT_ARG_TYPE >
int SVVector< SVT_TYPE, SVT_ARG_TYPE >::GetSize() const
{
	return static_cast< int >( m_Vector.size() );
}

template< typename SVT_TYPE, typename SVT_ARG_TYPE >
INT_PTR SVVector< SVT_TYPE, SVT_ARG_TYPE >::GetUpperBound() const
{
	return GetSize() - 1;
}

template< typename SVT_TYPE, typename SVT_ARG_TYPE >
void SVVector< SVT_TYPE, SVT_ARG_TYPE >::SetSize( int nNewSize )
{
	m_Vector.resize( nNewSize );
}

// Add SFINAE to this
template< typename SVT_TYPE, typename SVT_ARG_TYPE >
const typename SVVector< SVT_TYPE, SVT_ARG_TYPE >::SVType* SVVector< SVT_TYPE, SVT_ARG_TYPE >::GetData() const
{
	const SVType* l_pData( NULL );

	if( 0 < m_Vector.size() )
	{
		l_pData = &( m_Vector[ 0 ] );
	}

	return l_pData;
}

// Add SFINAE to this
template< typename SVT_TYPE, typename SVT_ARG_TYPE >
typename SVVector< SVT_TYPE, SVT_ARG_TYPE >::SVType* SVVector< SVT_TYPE, SVT_ARG_TYPE >::GetData()
{
	SVType* l_pData( NULL );

	if( 0 < m_Vector.size() )
	{
		l_pData = &( m_Vector[ 0 ] );
	}

	return l_pData;
}

template< typename SVT_TYPE, typename SVT_ARG_TYPE >
typename SVVector< SVT_TYPE, SVT_ARG_TYPE >::SVType SVVector< SVT_TYPE, SVT_ARG_TYPE >::GetAt( int nIndex ) const
{
	return m_Vector[ nIndex ];
}

template< typename SVT_TYPE, typename SVT_ARG_TYPE >
typename SVVector< SVT_TYPE, SVT_ARG_TYPE >::SVType& SVVector< SVT_TYPE, SVT_ARG_TYPE >::ElementAt( int nIndex )
{
	return m_Vector[ nIndex ];
}

template< typename SVT_TYPE, typename SVT_ARG_TYPE >
const typename SVVector< SVT_TYPE, SVT_ARG_TYPE >::SVType& SVVector< SVT_TYPE, SVT_ARG_TYPE >::ElementAt( int nIndex ) const
{
	return m_Vector[ nIndex ];
}

template< typename SVT_TYPE, typename SVT_ARG_TYPE >
const typename SVVector< SVT_TYPE, SVT_ARG_TYPE >::SVType& SVVector< SVT_TYPE, SVT_ARG_TYPE >::operator[]( size_type nIndex ) const
{
	return m_Vector[ nIndex ];
}

// Add SFINAE to this
template< typename SVT_TYPE, typename SVT_ARG_TYPE >
typename SVVector< SVT_TYPE, SVT_ARG_TYPE >::SVType& SVVector< SVT_TYPE, SVT_ARG_TYPE >::operator[]( size_type nIndex )
{
	return m_Vector[ nIndex ];
}

template< typename SVT_TYPE, typename SVT_ARG_TYPE >
void SVVector< SVT_TYPE, SVT_ARG_TYPE >::Fill( SVArgType fillValue )
{
}

template< typename SVT_TYPE, typename SVT_ARG_TYPE >
int SVVector< SVT_TYPE, SVT_ARG_TYPE >::Add( SVArgType newElement )
{
	m_Vector.push_back( newElement );

	return static_cast< int >( GetSize() - 1 );
}

template< typename SVT_TYPE, typename SVT_ARG_TYPE >
INT_PTR SVVector< SVT_TYPE, SVT_ARG_TYPE >::Append( const SVVector& src )
{
	const SVVectorBase& l_rSource( src );

	INT_PTR l_Index( GetSize() );

	m_Vector.insert( m_Vector.end(), l_rSource.begin(), l_rSource.end() );

	return l_Index;
}

template< typename SVT_TYPE, typename SVT_ARG_TYPE >
void SVVector< SVT_TYPE, SVT_ARG_TYPE >::InsertAt( int nIndex, SVArgType newElement, int nCount = 1 )
{
	SVVectorBase::iterator l_Iter( m_Vector.begin() );

	if( 0 < nIndex )
	{
		std::advance( l_Iter, nIndex );
	}

	m_Vector.insert( l_Iter, nCount, newElement );
}

template< typename SVT_TYPE, typename SVT_ARG_TYPE >
void SVVector< SVT_TYPE, SVT_ARG_TYPE >::InsertAt( int nStartIndex, SVVector* pNewArray )
{
	SVVectorBase& l_rSource( *pNewArray );

	SVVectorBase::iterator l_Iter( l_rSource.begin() );

	std::advance( l_Iter, nStartIndex );

	m_Vector.insert( l_Iter, l_rSource.begin(), l_rSource.end() );
}

template< typename SVT_TYPE, typename SVT_ARG_TYPE >
void SVVector< SVT_TYPE, SVT_ARG_TYPE >::SetAt( int nIndex, SVArgType newElement )
{
	m_Vector[ nIndex ] = newElement;
}

template< typename SVT_TYPE, typename SVT_ARG_TYPE >
void SVVector< SVT_TYPE, SVT_ARG_TYPE >::SetAtGrow( int nIndex, SVArgType newElement )
{
	if( GetSize() <= nIndex )
	{
		SetSize( nIndex + 1 );
	}

	m_Vector[ nIndex ] = newElement;
}

template< typename SVT_TYPE, typename SVT_ARG_TYPE >
void SVVector< SVT_TYPE, SVT_ARG_TYPE >::Copy( const SVVector& src )
{
	*this = src;
}

template< typename SVT_TYPE, typename SVT_ARG_TYPE >
void SVVector< SVT_TYPE, SVT_ARG_TYPE >::RemoveAt( int nIndex, int nCount = 1 )
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

template< typename SVT_TYPE, typename SVT_ARG_TYPE >
void SVVector< SVT_TYPE, SVT_ARG_TYPE >::RemoveAll()
{
	if( ! m_Vector.empty() )
	{
		m_Vector.clear();
	}
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVContainerLibrary\SVVector.inl_v  $
 * 
 *    Rev 1.1   30 Sep 2013 14:41:50   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Add x64 platforms.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   22 Apr 2013 07:44:24   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   07 Dec 2010 15:35:54   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to include changes in notification functionality using the Observer Design Pattern.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   09 Nov 2010 11:49:02   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  704
 * SCR Title:  Upgrade SVObserver Version for 5.33 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source to add missing type identifiers.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   22 Jun 2010 13:37:56   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  693
 * SCR Title:  Fix Performance Issue with Inspection Process
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fixed issues with containers.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   17 Nov 2008 07:21:22   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  615
 * SCR Title:  Integrate PLC Classes into SVObserver Outputs
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Initial Check-in
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/

