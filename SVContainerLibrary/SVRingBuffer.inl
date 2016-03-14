//******************************************************************************
//* COPYRIGHT (c) 2011 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVRingBuffer
//* .File Name       : $Workfile:   SVRingBuffer.inl  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 07:44:18  $
//******************************************************************************

#ifndef SVRINGBUFFER_INL
#define SVRINGBUFFER_INL

#include "SVRingBuffer.h"

template< typename SVElement, typename SVClearElement >
SVRingBuffer< SVElement, SVClearElement >::SVRingBuffer( size_t p_Capacity )
: m_Active( 1 ), m_Head( 0 ), m_PopHead( 0 ), m_SizeHead( 0 ), m_Tail( 0 ), m_PushTail( 0 ), m_Buffer( ( 0 < p_Capacity ) ? p_Capacity : 1 ), m_Clear()
{
}

template< typename SVElement, typename SVClearElement >
SVRingBuffer< SVElement, SVClearElement >::~SVRingBuffer()
{
	::InterlockedExchange( &m_Active, 0 );

	while( !( empty() ) )
	{
		RemoveHead();
	}
}

template< typename SVElement, typename SVClearElement >
bool SVRingBuffer< SVElement, SVClearElement >::empty() const
{
	return ( m_SizeHead == m_Tail );
}

template< typename SVElement, typename SVClearElement >
void SVRingBuffer< SVElement, SVClearElement >::clear()
{
	if( m_Active == 1 )
	{
		long l_Tail = m_Tail;

		while( ( m_Active == 1 ) && ( 0 < distance( m_SizeHead, l_Tail ) ) )
		{
			RemoveHead();
		}
	}
}

template< typename SVElement, typename SVClearElement >
size_t SVRingBuffer< SVElement, SVClearElement >::size() const
{
	return distance( m_SizeHead, m_Tail );
}

template< typename SVElement, typename SVClearElement >
size_t SVRingBuffer< SVElement, SVClearElement >::capacity() const
{
	return m_Buffer.size();
}

template< typename SVElement, typename SVClearElement >
HRESULT SVRingBuffer< SVElement, SVClearElement >::GetHead( SVElement& p_rElement ) const
{
	HRESULT l_Status = S_OK;

	if( m_Active == 1 )
	{
		SVRingElement l_RingElement;
		long l_Head = l_RingElement.m_Index + 1;

		while( ( m_Active == 1 ) && !( empty() ) && ( l_Head != l_RingElement.m_Index ) )
		{
			l_Head = m_Head;
			l_RingElement = m_Buffer[ l_Head % m_Buffer.size() ];

			if( l_Head != l_RingElement.m_Index )
			{
				Sleep( 0 );
			}
		}

		if( l_Head == l_RingElement.m_Index )
		{
			p_rElement = l_RingElement.m_Element;
		}
		else
		{
			m_Clear( p_rElement );

			l_Status = ERROR_OUTOFMEMORY;
		}
	}
	else
	{
		l_Status = ERROR_OUTOFMEMORY;
	}


	return l_Status;
}

template< typename SVElement, typename SVClearElement >
HRESULT SVRingBuffer< SVElement, SVClearElement >::PopHead()
{
	HRESULT l_Status = S_OK;

	if( m_Active == 1 )
	{
		l_Status = RemoveHead();
	}
	else
	{
		l_Status = ERROR_OUTOFMEMORY;
	}

	return l_Status;
}

template< typename SVElement, typename SVClearElement >
HRESULT SVRingBuffer< SVElement, SVClearElement >::PopHead( SVElement& p_rElement )
{
	HRESULT l_Status = S_OK;

	if( m_Active == 1 )
	{
		long l_LastHead = ::InterlockedExchangeAdd( &m_PopHead, 1 );
			
		if( 0 < distance( l_LastHead, m_Tail ) )
		{
			while( ( m_Active == 1 ) && ( 0 < distance( m_SizeHead, l_LastHead ) ) )
			{
				Sleep( 0 );
			}

			if( m_Active == 1 )
			{
				size_t l_Index = l_LastHead % m_Buffer.size();

				::InterlockedIncrement( &( m_Buffer[ l_Index ].m_Index ) );

				::InterlockedIncrement( &m_Head );

				p_rElement = m_Buffer[ l_Index ].m_Element;

				m_Clear( m_Buffer[ l_Index ].m_Element );

				::InterlockedIncrement( &m_SizeHead );
			}
			else
			{
				l_Status = ERROR_OUTOFMEMORY;
			}
		}
		else
		{
			m_Clear( p_rElement );

			::InterlockedDecrement( &m_PopHead );

			l_Status = ERROR_OUTOFMEMORY;
		}
	}
	else
	{
		l_Status = ERROR_OUTOFMEMORY;
	}

	return l_Status;
}

template< typename SVElement, typename SVClearElement >
HRESULT SVRingBuffer< SVElement, SVClearElement >::PushTail( const SVElement& p_rElement )
{
	HRESULT l_Status = S_OK;

	if( m_Active == 1 )
	{
		long l_Tail = ::InterlockedExchangeAdd( &m_PushTail, 1 );

		if( distance( m_SizeHead, l_Tail ) < static_cast< long >( capacity() ) )
		{
			while( ( m_Active == 1 ) && 0 < distance( m_Tail, l_Tail ) )
			{
				Sleep( 0 );
			}

			if( m_Active == 1 )
			{
				size_t l_Index = l_Tail % m_Buffer.size();

				m_Buffer[ l_Index ].m_Element = p_rElement;

				::InterlockedExchange( &( m_Buffer[ l_Index ].m_Index ), l_Tail );

				::InterlockedIncrement( &m_Tail );
			}
			else
			{
				l_Status = ERROR_OUTOFMEMORY;
			}
		}
		else
		{
			::InterlockedDecrement( &m_PushTail );

			l_Status = ERROR_OUTOFMEMORY;
		}
	}
	else
	{
		l_Status = ERROR_OUTOFMEMORY;
	}

	return l_Status;
}

template< typename SVElement, typename SVClearElement >
SVRingBuffer< SVElement, SVClearElement >::SVRingElement::SVRingElement()
: m_Index( 0 ), m_Element()
{
}

template< typename SVElement, typename SVClearElement >
SVRingBuffer< SVElement, SVClearElement >::SVRingElement::SVRingElement( const SVRingElement& p_rObject )
: m_Index( p_rObject.m_Index ), m_Element( p_rObject.m_Element )
{
}

template< typename SVElement, typename SVClearElement >
HRESULT SVRingBuffer< SVElement, SVClearElement >::RemoveHead()
{
	HRESULT l_Status = S_OK;

	long l_LastHead = ::InterlockedExchangeAdd( &m_PopHead, 1 );
		
	if( 0 < distance( l_LastHead, m_Tail ) )
	{
		while( 0 < distance( m_SizeHead, l_LastHead ) )
		{
			Sleep( 0 );
		}

		size_t l_Index = l_LastHead % m_Buffer.size();

		::InterlockedIncrement( &( m_Buffer[ l_Index ].m_Index ) );

		::InterlockedIncrement( &m_Head );

		m_Clear( m_Buffer[ l_Index ].m_Element );

		::InterlockedIncrement( &m_SizeHead );
	}
	else
	{
		::InterlockedDecrement( &m_PopHead );

		l_Status = ERROR_OUTOFMEMORY;
	}

	return l_Status;
}

template< typename SVElement, typename SVClearElement >
long SVRingBuffer< SVElement, SVClearElement >::distance( long p_rStart, long p_rEnd ) const
{
	long l_Size = 0;

	l_Size = ( p_rEnd - p_rStart );

	return l_Size;
}

#endif

