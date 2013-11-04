//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVList
//* .File Name       : $Workfile:   SVList.inl  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 07:44:12  $
//******************************************************************************

template
<
	typename SVT_TYPE
>
SVList< SVT_TYPE >::SVList()
: m_List()
{
}

template
<
	typename SVT_TYPE
>
SVList< SVT_TYPE >::SVList( const SVList& p_rObject )
: m_List( p_rObject.m_List )
{
}

template
<
	typename SVT_TYPE
>
SVList< SVT_TYPE >::SVList( const SVListBase& p_rObject )
: m_List( p_rObject )
{
}

template
<
	typename SVT_TYPE
>
SVList< SVT_TYPE >::~SVList()
{
}

template
<
	typename SVT_TYPE
>
SVList< SVT_TYPE >::operator SVListBase const & () const
{
	return m_List;
}

template
<
	typename SVT_TYPE
>
SVList< SVT_TYPE >::operator SVListBase & ()
{
	return m_List;
}

template
<
	typename SVT_TYPE
>
const SVList< SVT_TYPE >& SVList< SVT_TYPE >::operator=( const SVList& p_rObject )
{
	if( this != &p_rObject )
	{
		m_List = p_rObject.m_List;
	}

	return *this;
}

template
<
	typename SVT_TYPE
>
const SVList< SVT_TYPE >& SVList< SVT_TYPE >::operator=( const SVListBase& p_rObject )
{
	if( &m_List != &p_rObject )
	{
		m_List = p_rObject;
	}

	return *this;
}

template
<
	typename SVT_TYPE
>
INT_PTR SVList< SVT_TYPE >::GetCount() const
{
	return static_cast< INT_PTR >( m_List.size() );
}

template
<
	typename SVT_TYPE
>
BOOL SVList< SVT_TYPE >::IsEmpty() const
{
	return static_cast< BOOL >( m_List.empty() );
}

template
<
	typename SVT_TYPE
>
typename SVList< SVT_TYPE >::iterator SVList< SVT_TYPE >::AddHead( SVArgType newElement )
{
	return m_List.insert( m_List.begin(), newElement );
}

template
<
	typename SVT_TYPE
>
typename SVList< SVT_TYPE >::iterator SVList< SVT_TYPE >::AddTail( SVArgType newElement )
{
	return m_List.insert( m_List.end(), newElement );
}

template
<
	typename SVT_TYPE
>
void SVList< SVT_TYPE >::AddTail( SVList* pNewList )
{
	if( pNewList != NULL )
	{
		SVListBase& l_NewList( *pNewList );

		m_List.insert( m_List.end(), l_NewList.begin(), l_NewList.end() );
	}
}

template
<
	typename SVT_TYPE
>
void SVList< SVT_TYPE >::RemoveAll()
{
	m_List.clear();
}

template
<
	typename SVT_TYPE
>
void SVList< SVT_TYPE >::SetAt( iterator pos, SVArgType newElement )
{
	if( pos != m_List.end() )
	{
		*pos = newElement;
	}
}

template
<
	typename SVT_TYPE
>
void SVList< SVT_TYPE >::RemoveAt( iterator& position )
{
	if( position != m_List.end() )
	{
		m_List.erase( position );
	}

	position = m_List.end();
}

template
<
	typename SVT_TYPE
>
typename SVList< SVT_TYPE >::iterator SVList< SVT_TYPE >::NotFound()
{
	return m_List.end();
}

template
<
	typename SVT_TYPE
>
typename SVList< SVT_TYPE >::iterator SVList< SVT_TYPE >::Find( SVArgType searchValue ) const
{
	SVListBase* l_pList( const_cast< SVListBase* >( &m_List ) );

	iterator l_Iter( l_pList->begin() );

	while( l_Iter != l_pList->end() && !( *l_Iter == searchValue ) )
	{
		++l_Iter;
	}

	return l_Iter;
}

template
<
	typename SVT_TYPE
>
typename SVList< SVT_TYPE >::iterator SVList< SVT_TYPE >::FindIndex( int nIndex ) const
{
	SVListBase* l_pList( const_cast< SVListBase* >( &m_List ) );

	iterator l_Iter( l_pList->begin() );

	if( nIndex < static_cast< int >( GetCount() ) )
	{
		std::advance( l_Iter, nIndex );
	}
	else
	{
		l_Iter = l_pList->end();
	}

	return l_Iter;
}

template
<
	typename SVT_TYPE
>
typename SVList< SVT_TYPE >::iterator SVList< SVT_TYPE >::GetHeadPosition() const
{
	SVListBase* l_pList( const_cast< SVListBase* >( &m_List ) );

	return l_pList->begin();
}

template
<
	typename SVT_TYPE
>
typename SVList< SVT_TYPE >::SVType& SVList< SVT_TYPE >::GetNext( iterator& rPosition )
{
	SVType& l_rValue( *rPosition );

	++rPosition;

	return l_rValue;
}

template
<
	typename SVT_TYPE
>
const typename SVList< SVT_TYPE >::SVType& SVList< SVT_TYPE >::GetNext( iterator& rPosition ) const
{
	const SVType& l_rValue( *rPosition );

	++rPosition;

	return l_rValue;
}

template
<
	typename SVT_TYPE
>
typename SVList< SVT_TYPE >::SVType& SVList< SVT_TYPE >::GetAt( iterator position )
{
	return *position;
}

template
<
	typename SVT_TYPE
>
const typename SVList< SVT_TYPE >::SVType& SVList< SVT_TYPE >::GetAt( iterator position ) const
{
	return *position;
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVContainerLibrary\SVList.inl_v  $
 * 
 *    Rev 1.0   22 Apr 2013 07:44:12   bWalter
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

