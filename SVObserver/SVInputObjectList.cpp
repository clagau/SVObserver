//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVInputObjectList
//* .File Name       : $Workfile:   SVInputObjectList.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   07 Aug 2013 13:27:28  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVInputObjectList.h"
#include "SVInfoStructs.h"
#include "SVValueObject.h"
#include "SVDigitalInputObject.h"
#include "SVRemoteInputObject.h"
#include "SVCameraDataInputObject.h"
#pragma endregion Includes

SVInputObjectList::SVInputObjectList( LPCSTR ObjectName )
: SVObjectClass( ObjectName ), m_bCreated( false )
{
}

SVInputObjectList::SVInputObjectList( SVObjectClass* POwner, int StringResourceID )
: SVObjectClass( POwner, StringResourceID ), m_bCreated( false )
{
}

SVInputObjectList::~SVInputObjectList()
{
	if( m_bCreated )
	{
		Destroy();
	}
}

BOOL SVInputObjectList::Create()
{
	m_outObjectInfo.ObjectTypeInfo.ObjectType = SVInputObjectListType;

	try
	{
		m_bCreated = ::InitializeCriticalSectionAndSpinCount( &m_hCriticalSection, 4000 );
	}
	catch(...)
	{
		m_bCreated = false;
	}

	return m_bCreated;
}// end Create

BOOL SVInputObjectList::Destroy()
{
	try
	{
		if( m_bCreated )
		{
			Lock();

			SVGuidSVInputObjectPtrMap::iterator	l_Iter = m_InputObjects.begin();

			while( l_Iter != m_InputObjects.end() )
			{
				SVInputObject *pInput = l_Iter->second;

				l_Iter = m_InputObjects.erase( l_Iter );

				if( nullptr != pInput )
				{
					delete pInput;
				}
			}

			Unlock();

			::DeleteCriticalSection( &m_hCriticalSection );

		}// end if
	}
	catch(...)
	{
		return FALSE;
	}

	m_bCreated = FALSE;
	return TRUE;
}// end Destroy

BOOL SVInputObjectList::Lock() const
{
	if( m_bCreated )
	{
		::EnterCriticalSection( const_cast< LPCRITICAL_SECTION >( &m_hCriticalSection ) );

		return TRUE;
	}// end if

	return FALSE;
}// end Lock

BOOL SVInputObjectList::Unlock() const
{
	if( m_bCreated )
	{
		::LeaveCriticalSection( const_cast< LPCRITICAL_SECTION >( &m_hCriticalSection ) );

		return TRUE;
	}// end if

	return FALSE;
}// end Unlock

HRESULT SVInputObjectList::AttachInput( SVInputObject* pInput )
{
	HRESULT l_Status = S_OK;

	if( nullptr != pInput )
	{
		if( Lock() )
		{
			SVGuidSVInputObjectPtrMap::iterator	l_Iter = m_InputObjects.find( pInput->GetUniqueObjectID() );

			if( l_Iter == m_InputObjects.end() )
			{
				m_InputObjects[ pInput->GetUniqueObjectID() ] = pInput;
			}

			Unlock();
		}
		else
		{
			l_Status = E_FAIL;
		}
	}
	else
	{
		l_Status = E_INVALIDARG;
	}

	return l_Status;
}

HRESULT SVInputObjectList::DetachInput( const SVGUID& p_rOutputID )
{
	HRESULT l_Status = S_OK;

	if( Lock() )
	{
		SVGuidSVInputObjectPtrMap::iterator	l_Iter = m_InputObjects.find( p_rOutputID );

		if( l_Iter != m_InputObjects.end() )
		{
			m_InputObjects.erase( l_Iter );
		}

		Unlock();
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}// end RemoveInput

BOOL SVInputObjectList::ReadInputs( const SVIOEntryHostStructPtrList& p_rInputs, SVVariantBoolVector& p_rInputValues )
{
	size_t l_IOSize( p_rInputs.size() );
	size_t i( 0 );

	if( Lock() )
	{
		p_rInputValues.resize( l_IOSize );

		for( i = 0; i < l_IOSize; i++ )
		{
			SVIOEntryHostStructPtr pIOEntry = p_rInputs[i];

			// Check if output is enabled for this call
			if( !( pIOEntry.empty() ) && pIOEntry->m_Enabled )
			{
				SVInputObject *pInput = nullptr;

				SVGuidSVInputObjectPtrMap::iterator	l_Iter = m_InputObjects.find( pIOEntry->m_IOId );

				if( l_Iter != m_InputObjects.end() )
				{
					pInput = l_Iter->second;
				}

				if( nullptr != pInput )
				{
					p_rInputValues[ i ].second = ( S_OK == pInput->Read( p_rInputValues[ i ].first ) );
				}
				else
				{
					p_rInputValues[ i ].first.Clear();
					p_rInputValues[ i ].second = false;
				}
			}// end if
		}// end for

		Unlock();
		return TRUE;
	}// end if

	return FALSE;
}// end ReadInputs

BOOL SVInputObjectList::ReadInput( SVIOEntryStruct pIOEntry, _variant_t& p_rVariant )
{
	p_rVariant.Clear();

	BOOL l_Status = Lock();

	if( l_Status )
	{
		// Check if output is enabled for this call
		l_Status = ( !( pIOEntry.empty() ) && pIOEntry.m_IOEntryPtr->m_Enabled );

		if( l_Status )
		{
			SVGuidSVInputObjectPtrMap::iterator	l_Iter = m_InputObjects.find( pIOEntry.m_IOEntryPtr->m_IOId );

			l_Status = ( l_Iter != m_InputObjects.end() );

			if( l_Status )
			{
				SVInputObject* l_pInput = l_Iter->second;

				l_Status = ( nullptr != l_pInput ) && ( S_OK == l_pInput->Read( p_rVariant ) ); 
			}
		}

		Unlock();
	}

	return l_Status;
}// end ReadInput

BOOL SVInputObjectList::FillInputs( SVIOEntryHostStructPtrList& p_IOEntries )
{
	p_IOEntries.clear();

	if( Lock() )
	{
		SVGuidSVInputObjectPtrMap::iterator	l_Iter = m_InputObjects.begin();

		while( l_Iter != m_InputObjects.end() )
		{
			SVInputObject *pInput = l_Iter->second;

			SVIOEntryHostStructPtr pIOEntry;

			pIOEntry = new SVIOEntryHostStruct;

			pIOEntry->m_IOId = pInput->GetUniqueObjectID();

			if( SV_IS_KIND_OF( pInput, SVDigitalInputObject ) )
				pIOEntry->m_ObjectType = IO_DIGITAL_INPUT;
			else if( SV_IS_KIND_OF( pInput, SVRemoteInputObject ) )
				pIOEntry->m_ObjectType = IO_REMOTE_INPUT;
			else if( SV_IS_KIND_OF( pInput, SVCameraDataInputObject ) )
				pIOEntry->m_ObjectType = IO_CAMERA_DATA_INPUT;

			p_IOEntries.push_back( pIOEntry );

			++l_Iter;
		}

		Unlock();
		return TRUE;
	}

	return FALSE;
}

BOOL SVInputObjectList::GetRemoteInputCount( long &lCount )
{
	long lTempCount;

	// Set to default of 0
	lCount = 0;
	lTempCount = 0;

	if( Lock() )
	{
		SVGuidSVInputObjectPtrMap::iterator	l_Iter = m_InputObjects.begin();

		while( l_Iter != m_InputObjects.end() )
		{
			SVInputObject *pInput = l_Iter->second;

			if( SV_IS_KIND_OF( pInput, SVRemoteInputObject ) )
				lTempCount++;

			++l_Iter;
		}

		lCount = lTempCount;

		Unlock();
		return TRUE;
	}

	return FALSE;
}

BOOL SVInputObjectList::SetRemoteInput( long lIndex, VARIANT vtValue )
{
	if( Lock() )
	{
		BOOL bFound = FALSE;

		SVGuidSVInputObjectPtrMap::iterator	l_Iter = m_InputObjects.begin();

		while( l_Iter != m_InputObjects.end() )
		{
			SVRemoteInputObject* pInput = dynamic_cast< SVRemoteInputObject* >( l_Iter->second );

			if( nullptr != pInput )
			{
				bFound = ( lIndex == pInput->m_lIndex );

				if( bFound )
				{
					pInput->WriteCache( vtValue );

					break;
				}
			}

			++l_Iter;
		}

		Unlock();

		return bFound;
	}

	return FALSE;
}

