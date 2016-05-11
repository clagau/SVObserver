//******************************************************************************
//* COPYRIGHT (c) 2010 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVInputObjectList
//* .File Name       : $Workfile:   SVInputObjectList.inl  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   11 Jun 2013 15:26:16  $
//******************************************************************************
#pragma once

#pragma region Includes
#include "SVInputObjectList.h"
#pragma endregion Includes

template< typename SVInputType >
HRESULT SVInputObjectList::GetInput( const SVGUID& p_rInputID, SVInputType*& p_rpInput ) const
{
	HRESULT l_Status = S_OK;

	p_rpInput = nullptr;

	if( Lock() )
	{
		SVGuidSVInputObjectPtrMap::const_iterator l_Iter = m_InputObjects.find( p_rInputID );

		if( l_Iter != m_InputObjects.end() )
		{
			p_rpInput = dynamic_cast< SVInputType* >( l_Iter->second );

			if( nullptr == p_rpInput )
			{
				l_Status = E_FAIL;
			}
		}
		else
		{
			l_Status = E_INVALIDARG;
		}

		Unlock();
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

template< typename SVInputType >
HRESULT SVInputObjectList::GetInput( const SVString& p_rInputName, SVInputType*& p_rpInput ) const
{
	HRESULT l_Status = S_OK;

	p_rpInput = nullptr;

	if( Lock() )
	{
		SVGuidSVInputObjectPtrMap::const_iterator	l_Iter;
		
		for( l_Iter = m_InputObjects.begin(); nullptr == p_rpInput && l_Iter != m_InputObjects.end(); ++l_Iter )
		{
			SVInputObject* pInput = l_Iter->second;

			if ( nullptr != pInput )
			{
				if ( p_rInputName == pInput->GetName() )
				{
					p_rpInput = dynamic_cast< SVInputType* >( pInput );
				}
			}
		}

		if( nullptr == p_rpInput )
		{
			l_Status = E_INVALIDARG;
		}

		Unlock();
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

template< typename SVInputType >
HRESULT SVInputObjectList::GetInputFlyweight( const SVString& p_rInputName, SVInputType*& p_rpInput )
{
	HRESULT l_Status = S_OK;

	p_rpInput = nullptr;

	if( Lock() )
	{
		SVGuidSVInputObjectPtrMap::const_iterator	l_Iter;
		
		for( l_Iter = m_InputObjects.begin(); nullptr == p_rpInput && l_Iter != m_InputObjects.end(); ++l_Iter )
		{
			SVInputObject* pInput = l_Iter->second;

			if ( nullptr != pInput )
			{
				if ( p_rInputName == pInput->GetName() )
				{
					p_rpInput = dynamic_cast< SVInputType* >( pInput );
				}
			}
		}

		if( nullptr == p_rpInput )
		{
			p_rpInput = new SVInputType;

			if( nullptr != p_rpInput )
			{
				p_rpInput->SetName( p_rInputName.c_str() );

				if( S_OK != AttachInput( p_rpInput ) )
				{
					delete p_rpInput;
					p_rpInput = nullptr;
				}
			}
		}

		if( nullptr == p_rpInput )
		{
			l_Status = E_INVALIDARG;
		}

		Unlock();
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}
