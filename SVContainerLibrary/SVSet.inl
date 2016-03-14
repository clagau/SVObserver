//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVSet
//* .File Name       : $Workfile:   SVSet.inl  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   30 Sep 2013 14:41:50  $
//******************************************************************************

template
<
	typename SVT_KEY
>
SVSet<SVT_KEY> SVSet< SVT_KEY >::Difference( const SVSet& p_SourceSet, const SVSet& p_FilterSet )
{
	SVSet l_Set;

	if( !( p_SourceSet.m_Set.empty() ) )
	{
		if( p_FilterSet.m_Set.empty() )
		{
			l_Set = p_SourceSet;
		}
		else
		{
			std::set_difference( p_SourceSet.m_Set.begin(), p_SourceSet.m_Set.end(), 
				p_FilterSet.m_Set.begin(), p_FilterSet.m_Set.end(), std::inserter( l_Set, l_Set.m_Set.begin() ) );
		}
	}

	return l_Set;
}

template
<
	typename SVT_KEY
>
SVSet< SVT_KEY >::SVSet()
:m_Set()
{
}

template
<
	typename SVT_KEY
>
SVSet< SVT_KEY >::SVSet( const SVSet& p_rObject )
:m_Set( p_rObject.m_Set )
{
}

template
<
	typename SVT_KEY
>
SVSet< SVT_KEY >::SVSet( const SVSetBase& p_rObject )
:m_Set( p_rObject )
{
}

template
<
	typename SVT_KEY
>
SVSet< SVT_KEY >::~SVSet()
{
}

template
<
	typename SVT_KEY
>
SVSet< SVT_KEY >::operator SVSetBase const & () const
{
	return m_Set;
}

template
<
	typename SVT_KEY
>
SVSet< SVT_KEY >::operator SVSetBase & ()
{
	return m_Set;
}

template
<
	typename SVT_KEY
>
const SVSet< SVT_KEY >& SVSet< SVT_KEY >::operator=( const SVSet& p_rObject )
{
	if( this != &p_rObject )
	{
		m_Set = p_rObject.m_Set;
	}

	return *this;
}

template
<
	typename SVT_KEY
>
const SVSet< SVT_KEY >& SVSet< SVT_KEY >::operator=( const SVSetBase& p_rObject )
{
	if( &m_Set != &p_rObject )
	{
		m_Set = p_rObject;
	}

	return *this;
}

template
<
	typename SVT_KEY
>
INT_PTR SVSet< SVT_KEY >::GetSize() const
{
	return static_cast< INT_PTR >( m_Set.size() );
}

template
<
	typename SVT_KEY
>
typename SVSet< SVT_KEY >::key_type& SVSet< SVT_KEY >::GetAt( INT_PTR nIndex )
{
	iterator l_Iter( m_Set.begin() );

	std::advance( l_Iter, nIndex );

	return const_cast< SVSet< SVT_KEY >::key_type& >( *l_Iter );
}

template
<
	typename SVT_KEY
>
INT_PTR SVSet< SVT_KEY >::Add( key_type newElement )
{
	INT_PTR l_Index( -1 );

	std::pair< iterator, bool > l_Status;

	l_Status = m_Set.insert( newElement );

	if( l_Status.second )
	{
		l_Index = static_cast< INT_PTR >( std::distance( m_Set.begin(), l_Status.first ) );
	}

	return l_Index;
}

template
<
	typename SVT_KEY
>
void SVSet< SVT_KEY >::Remove( key_type newElement )
{
	m_Set.erase( newElement );
}

template
<
	typename SVT_KEY
>
INT_PTR SVSet< SVT_KEY >::Find( key_type searchValue ) const
{
	INT_PTR l_Index( -1 );

	const_iterator l_Iter( m_Set.find( searchValue ) );

	if( l_Iter != m_Set.end() )
	{
		l_Index = static_cast< INT_PTR >( std::distance( m_Set.begin(), l_Iter ) );
	}

	return l_Index;
}

