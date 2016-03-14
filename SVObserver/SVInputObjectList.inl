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

#ifndef SVINPUTOBJECTLIST_INL
#define SVINPUTOBJECTLIST_INL

#include "SVInputObjectList.h"

template< typename SVInputType >
HRESULT SVInputObjectList::GetInput( const SVGUID& p_rInputID, SVInputType*& p_rpInput ) const
{
	HRESULT l_Status = S_OK;

	p_rpInput = NULL;

	if( Lock() )
	{
		SVGuidSVInputObjectPtrMap::const_iterator l_Iter = m_InputObjects.find( p_rInputID );

		if( l_Iter != m_InputObjects.end() )
		{
			p_rpInput = dynamic_cast< SVInputType* >( l_Iter->second );

			if( p_rpInput == NULL )
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

	p_rpInput = NULL;

	if( Lock() )
	{
		SVGuidSVInputObjectPtrMap::const_iterator	l_Iter;
		
		for( l_Iter = m_InputObjects.begin(); p_rpInput == NULL && l_Iter != m_InputObjects.end(); ++l_Iter )
		{
			SVInputObject* pInput = l_Iter->second;

			if ( pInput != NULL )
			{
				if ( p_rInputName == pInput->GetName() )
				{
					p_rpInput = dynamic_cast< SVInputType* >( pInput );
				}
			}
		}

		if( p_rpInput == NULL )
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

	p_rpInput = NULL;

	if( Lock() )
	{
		SVGuidSVInputObjectPtrMap::const_iterator	l_Iter;
		
		for( l_Iter = m_InputObjects.begin(); p_rpInput == NULL && l_Iter != m_InputObjects.end(); ++l_Iter )
		{
			SVInputObject* pInput = l_Iter->second;

			if ( pInput != NULL )
			{
				if ( p_rInputName == pInput->GetName() )
				{
					p_rpInput = dynamic_cast< SVInputType* >( pInput );
				}
			}
		}

		if( p_rpInput == NULL )
		{
			p_rpInput = new SVInputType;

			if( p_rpInput != NULL )
			{
				p_rpInput->SetName( p_rInputName.c_str() );

				if( AttachInput( p_rpInput ) != S_OK )
				{
					delete p_rpInput;
					p_rpInput = NULL;
				}
			}
		}

		if( p_rpInput == NULL )
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

#endif

