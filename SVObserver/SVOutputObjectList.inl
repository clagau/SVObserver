//******************************************************************************
//* COPYRIGHT (c) 2010 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVOutputObjectList
//* .File Name       : $Workfile:   SVOutputObjectList.inl  $
//* ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   23 Apr 2013 13:16:24  $
//******************************************************************************

#pragma once

#include "SVOutputObjectList.h"

template< typename SVOutputType >
HRESULT SVOutputObjectList::GetOutput( const SVGUID& p_rOutputID, SVOutputType*& p_rpOutput ) const
{
	HRESULT l_Status = S_OK;

	p_rpOutput = nullptr;

	if( Lock() )
	{
		SVGuidSVOutputObjectPtrMap::const_iterator l_Iter = m_OutputObjects.find( p_rOutputID );

		if( l_Iter != m_OutputObjects.end() )
		{
			p_rpOutput = dynamic_cast< SVOutputType* >( l_Iter->second );

			if( nullptr == p_rpOutput )
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

template< typename SVOutputType >
HRESULT SVOutputObjectList::GetOutput( LPCTSTR p_szName, SVOutputType*& p_rpOutput ) const
{
	HRESULT l_Status = S_OK;

	SVString l_Name = p_szName;

	p_rpOutput = nullptr;

	if( Lock() )
	{
		SVGuidSVOutputObjectPtrMap::const_iterator l_Iter = m_OutputObjects.begin();

		while( nullptr == p_rpOutput && l_Iter != m_OutputObjects.end() )
		{
			SVOutputObject* l_pOutput = l_Iter->second;

			if( nullptr != l_pOutput && l_Name == l_pOutput->GetName() )
			{
				p_rpOutput = dynamic_cast< SVOutputType* >( l_pOutput );
			}

			++l_Iter;
		}

		if( nullptr == p_rpOutput )
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

template< typename SVOutputType >
HRESULT SVOutputObjectList::GetOutputFlyweight( LPCTSTR p_szName, SVOutputType*& p_rpOutput )
{
	HRESULT l_Status = S_OK;

	p_rpOutput = nullptr;

	if( Lock() )
	{
		SVString l_Name = p_szName;

		l_Status = GetOutput( p_szName, p_rpOutput );

		if( nullptr == p_rpOutput )
		{
			p_rpOutput = new SVOutputType;

			if( nullptr != p_rpOutput )
			{
				p_rpOutput->SetName( l_Name.c_str() );

				l_Status = AttachOutput( p_rpOutput );

				if( l_Status != S_OK )
				{
					delete p_rpOutput;

					p_rpOutput = nullptr;
				}
			}
		}

		if( nullptr == p_rpOutput )
		{
			if( S_OK == l_Status )
			{
				l_Status = E_INVALIDARG;
			}
		}

		Unlock();
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

