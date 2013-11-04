//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVPPQShiftRegister
//* .File Name       : $Workfile:   SVPPQShiftRegister.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   08 May 2013 16:31:14  $
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

	p_rIndex = m_Products.size();

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
				p_rIndex = std::distance( m_Products.begin(), l_Iter );
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

	p_rIndex = m_Products.size();

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
				p_rIndex = std::distance( m_Products.begin(), l_Iter );
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

	p_rIndex = m_Products.size();

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
				p_rIndex = std::distance( m_Products.begin(), l_Iter );
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
				l_TempString.Format( _T( "%d:%s\n" ), i, m_Products[ i ]->m_ProductState.ToString() );
			}
			else
			{
				l_TempString.Format( _T( "%d:NULL\n" ), i );
			}

			p_rProductStates += l_TempString;
		}
	}

	return l_Status;
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVPPQShiftRegister.cpp_v  $
 * 
 *    Rev 1.1   08 May 2013 16:31:14   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 012.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   29 Apr 2013 14:25:06   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  830
 * SCR Title:  Consolidate all Time Stamp and Clock Functionality
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to use the new SVClock::SVTimeStamp type for all time stamp variables.  Update to use new SVClock::GetTimeStamp() function to get the time stamp value in milliseconds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 14:33:28   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   19 Jun 2012 13:49:40   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  744
 * SCR Title:  Add Shared Memory and Socket Functionality for Run Page Web Server
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Optimized the application state functionality.  The application state class was altered to simplify its usage and elements were added to object manager to separate tracking states.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   16 Sep 2011 16:18:06   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  730
 * SCR Title:  Adjust SVObserver to fix issues with Inspection resource handshaking
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Updated objects with new device and resource management functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   14 Jul 2011 16:19:40   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  730
 * SCR Title:  Adjust SVObserver to fix issues with Inspection resource handshaking
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Updated source code to remove un-used product info struct attribute and inspection attribute.  Added logging attribute to product info struct for product state.  Remove extra Inspection Complete notification.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   25 Feb 2010 09:20:16   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  686
 * SCR Title:  Update Laptop version to load all valid configurations
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated PPQ to move the indexing of the PPQ to the PPQ thread.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   24 Feb 2010 10:43:40   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  686
 * SCR Title:  Update Laptop version to load all valid configurations
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added new files to separate PPQ functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
