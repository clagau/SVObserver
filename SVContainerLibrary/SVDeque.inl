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


//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVContainerLibrary\SVDeque.inl_v  $
 * 
 *    Rev 1.0   22 Apr 2013 07:44:10   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   05 Mar 2012 15:41:12   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  756
 * SCR Title:  Fix Variant Array Object Script Issue
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added new functions to act like CStringArray.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   07 Dec 2010 15:35:56   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to include changes in notification functionality using the Observer Design Pattern.
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
 *    Rev 1.0   17 Nov 2008 07:20:42   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  615
 * SCR Title:  Integrate PLC Classes into SVObserver Outputs
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Initial Check-in
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/

