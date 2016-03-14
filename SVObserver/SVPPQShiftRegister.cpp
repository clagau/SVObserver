//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVPPQShiftRegister
//* .File Name       : $Workfile:   SVPPQShiftRegister.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   02 Oct 2013 07:12:20  $
//******************************************************************************

#include "stdafx.h"
#include <boost/config.hpp>
#include <boost/bind.hpp>
#include "SVPPQShiftRegister.h"
#include "SVInfoStructs.h"

SVPPQShiftRegister::SVPPQShiftRegister()
: m_Products(), m_HeadTriggerCount( 0 ), m_TailTriggerCount( 0 )
{
}

SVPPQShiftRegister::~SVPPQShiftRegister()
{
}

size_t SVPPQShiftRegister::size() const
{
	return m_Products.size();
}

void SVPPQShiftRegister::resize( size_t p_Size )
{
	m_Products.resize( p_Size, NULL );
}

void SVPPQShiftRegister::clear()
{
	m_HeadTriggerCount = 0;
	m_TailTriggerCount = 0;

	m_Products.clear();
}

SVProductInfoStruct* SVPPQShiftRegister::GetProductAt( size_t p_Index ) const
{
	SVProductInfoStruct* l_pProduct = NULL;

	l_pProduct = m_Products.at( p_Index );

	return l_pProduct;
}

HRESULT SVPPQShiftRegister::SetProductAt( size_t p_Index, SVProductInfoStruct* p_pProduct )
{
	HRESULT l_Status = S_OK;

	if( p_Index < m_Products.size() )
	{
		m_Products[ p_Index ] = p_pProduct;

		if( p_Index == 0 )
		{
			if( p_pProduct != NULL )
			{
				m_HeadTriggerCount = p_pProduct->ProcessCount();
			}
			else
			{
				m_HeadTriggerCount = 0;
			}

			if( m_TailTriggerCount == 0 )
			{
				m_TailTriggerCount = m_HeadTriggerCount;
			}
		}
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

SVProductInfoStruct* SVPPQShiftRegister::IndexRegister()
{
	SVProductInfoStruct* l_pProduct = NULL;

	if( 0 < m_Products.size() )
	{
		long l_HeadTriggerCount = 0;
		long l_TailTriggerCount = 0;

		l_pProduct = m_Products[ m_Products.size() - 1 ];

		for( size_t i = m_Products.size() - 1; i > 0; i-- )
		{
			m_Products[ i ] = m_Products[ i - 1 ];

			if( m_Products[ i ] != NULL )
			{
				l_HeadTriggerCount = m_Products[ i ]->ProcessCount();

				if( l_TailTriggerCount == 0 )
				{
					l_TailTriggerCount = l_HeadTriggerCount;
				}
			}
		}

		m_HeadTriggerCount = l_HeadTriggerCount;
		m_TailTriggerCount = l_TailTriggerCount;

		m_Products[ 0 ] = NULL;
	}

	return l_pProduct;
}

HRESULT SVPPQShiftRegister::IsProductAlive( long p_TriggerCount ) const
{
	HRESULT l_Status = S_OK;

	if( m_HeadTriggerCount != 0 && m_TailTriggerCount != 0 )
	{
		if( p_TriggerCount > m_HeadTriggerCount || m_TailTriggerCount > p_TriggerCount )
		{
			l_Status = S_FALSE;
		}
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

bool SVTriggerCountLess( const SVProductInfoStruct* l_pLeft, const SVProductInfoStruct* l_pRight )
{
	bool l_Status = false;

	if( l_pLeft != NULL && l_pRight != NULL )
	{
		l_Status = ( l_pRight->ProcessCount() < l_pLeft->ProcessCount() );
	}
	else
	{
		::OutputDebugString( _T( "SVTriggerCountLess - Invalid SVProductInfoStruct\n" ) );
	}

	return l_Status;
}

HRESULT SVPPQShiftRegister::GetIndexByTriggerCount( long& p_rIndex, long p_TriggerCount ) const
{
	HRESULT l_Status = S_OK;

	p_rIndex = static_cast<long>(m_Products.size());

	if( 0 < p_TriggerCount )
	{
		SVProductVector::const_iterator l_Iter;
		
		SVProductInfoStruct l_Product;

		l_Product.oTriggerInfo.lTriggerCount = p_TriggerCount;

		l_Iter = std::lower_bound( m_Products.begin(), m_Products.end(), &l_Product, SVTriggerCountLess );

		if( l_Iter != m_Products.end() && ( *l_Iter ) != NULL )
		{
			if( p_TriggerCount == ( *l_Iter )->ProcessCount() )
			{
				p_rIndex = static_cast<long>(std::distance( m_Products.begin(), l_Iter ));
			}
			else if( l_Iter == m_Products.begin() && p_TriggerCount > ( *l_Iter )->ProcessCount() )
			{
				p_rIndex = -1;

				l_Status = E_FAIL;
			}
			else
			{
				l_Status = E_FAIL;
			}
		}
		else
		{
			l_Status = E_FAIL;
		}
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

HRESULT SVPPQShiftRegister::GetProductByTriggerCount( SVProductInfoStruct*& p_rpProduct, long p_TriggerCount ) const
{
	HRESULT l_Status = S_OK;

	p_rpProduct = NULL;

	if( 0 < p_TriggerCount )
	{
		SVProductVector::const_iterator l_Iter;
		
		SVProductInfoStruct l_Product;

		l_Product.oTriggerInfo.lTriggerCount = p_TriggerCount;

		l_Iter = std::lower_bound( m_Products.begin(), m_Products.end(), &l_Product, SVTriggerCountLess );

		if( l_Iter != m_Products.end() && ( *l_Iter ) != NULL )
		{
			if( p_TriggerCount == ( *l_Iter )->ProcessCount() )
			{
				p_rpProduct = ( *l_Iter );
			}
			else
			{
				l_Status = E_FAIL;
			}
		}
		else
		{
			l_Status = E_FAIL;
		}
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

struct SVTriggerTimeStampCompare
{
	SVTriggerTimeStampCompare() : m_LowerThresholdInMilliseconds( 0 ) {}
	SVTriggerTimeStampCompare( SVClock::SVTimeStamp p_LowerThresholdInMilliseconds ) : m_LowerThresholdInMilliseconds( p_LowerThresholdInMilliseconds ) {}
	SVTriggerTimeStampCompare( const SVTriggerTimeStampCompare& p_rObject ) : m_LowerThresholdInMilliseconds( p_rObject.m_LowerThresholdInMilliseconds ) {}

	bool operator()( const SVProductInfoStruct* l_pLeft, const SVProductInfoStruct* l_pRight )
	{
		bool l_Status = false;

		if( l_pLeft != NULL && l_pRight != NULL )
		{
			SVClock::SVTimeStamp l_RightTimeStamp = l_pRight->TimeStamp();
			SVClock::SVTimeStamp l_LeftTimeStamp = l_pLeft->TimeStamp() + m_LowerThresholdInMilliseconds;
			//l_Status = (l_RightTimeStamp < l_LeftTimeStamp);
			l_Status = (l_RightTimeStamp - l_LeftTimeStamp) < 0;
		}
		else
		{
			::OutputDebugString( _T( "SVTriggerTimeStampCompare - Invalid SVProductInfoStruct\n" ) );
		}

		return l_Status;
	}

	SVClock::SVTimeStamp m_LowerThresholdInMilliseconds;
};

HRESULT SVPPQShiftRegister::GetIndexByTriggerTimeStamp( long& p_rIndex, SVClock::SVTimeStamp p_TimeStamp, SVClock::SVTimeStamp p_LowerThresholdInMilliseconds ) const
{
	HRESULT l_Status = S_OK;

	p_rIndex = static_cast<long>(m_Products.size());

	if( 0 < p_TimeStamp )
	{
		SVProductVector::const_iterator l_Iter;
		
		SVProductInfoStruct l_Product;

		l_Product.oTriggerInfo.m_BeginProcess = p_TimeStamp;

		l_Iter = std::lower_bound( m_Products.begin(), m_Products.end(), &l_Product, SVTriggerTimeStampCompare( p_LowerThresholdInMilliseconds ) );

		if( l_Iter != m_Products.end() && ( *l_Iter ) != NULL )
		{
			SVClock::SVTimeStamp l_ProductTS = ( *l_Iter )->TimeStamp();

			if( p_TimeStamp >= ( l_ProductTS + p_LowerThresholdInMilliseconds ) )
			{
				p_rIndex = static_cast<long>(std::distance( m_Products.begin(), l_Iter ));
			}
			else
			{
				l_Status = E_FAIL;
			}
		}
		else
		{
			l_Status = E_FAIL;
		}
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

HRESULT SVPPQShiftRegister::GetIndexByTriggerTimeStamp( long& p_rIndex, SVClock::SVTimeStamp p_TimeStamp, SVClock::SVTimeStamp p_LowerThresholdInMilliseconds, SVClock::SVTimeStamp p_UpperThresholdInMilliseconds ) const
{
	HRESULT l_Status = S_OK;

	p_rIndex = static_cast<long>(m_Products.size());

	if( 0 < p_TimeStamp )
	{
		SVProductVector::const_iterator l_Iter;
		SVProductInfoStruct l_Product;

		SVClock::SVTimeStamp l_LowerThreshold = p_LowerThresholdInMilliseconds;
		SVClock::SVTimeStamp l_UpperThreshold = p_UpperThresholdInMilliseconds;

		l_Product.oTriggerInfo.m_BeginProcess = p_TimeStamp;

		l_Iter = std::lower_bound( m_Products.begin(), m_Products.end(), &l_Product, SVTriggerTimeStampCompare( l_LowerThreshold ) );

		if( l_Iter != m_Products.end() && ( *l_Iter ) != NULL )
		{
			SVClock::SVTimeStamp l_ProductTS = ( *l_Iter )->TimeStamp();
			SVClock::SVTimeStamp l_Upper = l_ProductTS + l_UpperThreshold;
			SVClock::SVTimeStamp l_Lower = l_ProductTS + l_LowerThreshold;
			SVClock::SVTimeStamp l_Distance = p_TimeStamp - l_ProductTS;

			if( l_Iter == m_Products.begin() && p_TimeStamp > l_Upper )
			{
				p_rIndex = -1;

				l_Status = E_FAIL;
			}
			else if( p_TimeStamp >= l_Lower )
			{
				p_rIndex = static_cast<long>(std::distance( m_Products.begin(), l_Iter ));
			}
			else
			{
				l_Status = E_FAIL;
			}
		}
		else
		{
			l_Status = E_FAIL;
		}
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

HRESULT SVPPQShiftRegister::GetProductStates( SVString& p_rProductStates ) const
{
	HRESULT l_Status = S_OK;

	if( !( m_Products.empty() ) )
	{
		for( size_t i = 0; i < m_Products.size(); ++i )
		{
			SVString l_TempString;

			if( m_Products[ i ] != NULL )
			{
				l_TempString = SvUl_SF::Format( _T( "%d:%s\n" ), i, m_Products[ i ]->m_ProductState.c_str() );
			}
			else
			{
				l_TempString = SvUl_SF::Format( _T( "%d:NULL\n" ), i );
			}

			p_rProductStates += l_TempString;
		}
	}

	return l_Status;
}

