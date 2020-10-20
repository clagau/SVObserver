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

#pragma region Includes
#include "stdafx.h"
//Moved to precompiled header: #include <boost/config.hpp>
//Moved to precompiled header: #include <boost/bind.hpp>
#include "SVPPQShiftRegister.h"
#include "SVInfoStructs.h"
#include "SVUtilityLibrary/StringHelper.h"
#pragma endregion Includes

size_t SVPPQShiftRegister::size() const
{
	return m_Products.size();
}

void SVPPQShiftRegister::resize( size_t p_Size )
{
	m_Products.resize( p_Size, nullptr );
}

void SVPPQShiftRegister::clear()
{
	m_HeadTriggerCount = 0;
	m_TailTriggerCount = 0;

	m_Products.clear();
}

SVProductInfoStruct* SVPPQShiftRegister::GetProductAt( size_t p_Index ) const
{
	SVProductInfoStruct* l_pProduct = nullptr;

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
			if( nullptr != p_pProduct )
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
	SVProductInfoStruct* l_pProduct = nullptr;

	if( 0 < m_Products.size() )
	{
		long l_HeadTriggerCount = 0;
		long l_TailTriggerCount = 0;

		l_pProduct = m_Products[ m_Products.size() - 1 ];

		for( size_t i = m_Products.size() - 1; i > 0; i-- )
		{
			m_Products[ i ] = m_Products[ i - 1 ];

			if( nullptr != m_Products[ i ] )
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

		m_Products[ 0 ] = nullptr;
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

long SVPPQShiftRegister::GetIndexByTriggerCount(long triggerCount) const
{
	long Result{-1};

	//@TODO[gra][8.20][03.12.2018]: The trigger count check < 0 needs to be removed so that
	//reset output timer works when PPQ not yet full
	if(0 < triggerCount)
	{
		for(auto iter{m_Products.crbegin()}; m_Products.crend() != iter; ++iter)
		{
			if(nullptr != *iter)
			{
				if((*iter)->ProcessCount() == triggerCount)
				{
					Result = static_cast<long>(std::distance(iter, m_Products.crend())-1);
					return Result;
				}
			}
		}
	}

	return Result;
}

SVProductInfoStruct* SVPPQShiftRegister::GetProductByTriggerCount(long triggerCount) const
{
	long index = GetIndexByTriggerCount(triggerCount);

	if (0 <= index && index < static_cast<long> (m_Products.size()))
	{
		return m_Products[index];
	}

	return nullptr;
}

long SVPPQShiftRegister::GetIndexByTriggerTimeStamp(double checkTime, int cameraID) const
{
	long result {-1};

	if (0.0 < checkTime)
	{
		auto startPos = m_Products.crend() - 1;
		for (auto iter = m_Products.crbegin(); m_Products.crend() != iter; ++iter)
		{
			if (nullptr != *iter)
			{
				bool hasCameraImage = (cameraID >= 0 && cameraID < SvDef::cMaximumCameras) ? (*iter)->m_hasCameraImage[cameraID] : false;
				//If product already has camera image or product inactive then skip
				if (!hasCameraImage && (*iter)->m_triggered && (*iter)->IsProductActive() && !(*iter)->m_dataComplete)
				{
					double triggerTime {(*iter)->TimeStamp()};
					double nextTriggerTime = (startPos == iter) ? 0.0 : (*(iter+1))->TimeStamp();
					if(findTimeMatch(checkTime, triggerTime, nextTriggerTime, startPos == iter))
					{
						result = static_cast<long>(std::distance(iter, startPos));
					}
				}
				//When we have reached the start position of the PPQ and have no match is it still possible to obtain a match ?
				//When the newest trigger is newer then the acquisition time stamp then acquisition can no longer be correlated
				if(-1 == result && startPos == iter && (*startPos)->TimeStamp() > checkTime)
				{
					result = static_cast<long>(m_Products.size());
					break;
				}
			}
		}
	}

	return result;
}

HRESULT SVPPQShiftRegister::GetProductStates( std::string& p_rProductStates ) const
{
	HRESULT l_Status = S_OK;

	if( !( m_Products.empty() ) )
	{
		for( size_t i = 0; i < m_Products.size(); ++i )
		{
			std::string l_TempString;

			if( nullptr != m_Products[ i ] )
			{
				l_TempString = SvUl::Format( _T( "%d:%s\n" ), i, m_Products[ i ]->m_ProductState.c_str() );
			}
			else
			{
				l_TempString = SvUl::Format( _T( "%d:NULL\n" ), i );
			}

			p_rProductStates += l_TempString;
		}
	}

	return l_Status;
}

bool SVPPQShiftRegister::findTimeMatch(double checkTime, double triggerTime, double nextTriggerTime, bool isStartPos)  const
{
	//If this is the start PPQ position then use the pre-trigger time!
	double PreTriggerTime = isStartPos ? m_preTriggerTimeWindow : 0.0;

	bool result = 0.0 < triggerTime && (triggerTime - PreTriggerTime) < checkTime;
	if(result && nextTriggerTime > 0.0)
	{
		result = checkTime < nextTriggerTime;
	}

	return result;
}