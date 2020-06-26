//*****************************************************************************
/// \copyright (c) 2018,2018 by Jan Kaniewski, Keith J. Jones
/// \file InStreamWrapper.cpp
/// All Rights Reserved 
/// Author           : Jan Kaniewski, Keith J. Jones
/// License          : Public Domain License
/// From             : https://github.com/keithjjones/7zip-cpp
//*****************************************************************************
/// Input stream wrapper
//******************************************************************************

#include "stdafx.h"
#include "InStreamWrapper.h"

constexpr GUID cISequentialInStream =	{0x23170F69, 0x40C1, 0x278A, {0x00, 0x00, 0x00, 0x03, 0x00, 0x01, 0x00, 0x00}};
constexpr GUID cIInStream =				{0x23170F69, 0x40C1, 0x278A, {0x00, 0x00, 0x00, 0x03, 0x00, 0x03, 0x00, 0x00}};
constexpr GUID cIStreamGetSize =		{0x23170F69, 0x40C1, 0x278A, {0x00, 0x00, 0x00, 0x03, 0x00, 0x06, 0x00, 0x00}};

InStreamWrapper::InStreamWrapper( const CComPtr< IStream >& rBaseStream ): 
	m_baseStream{rBaseStream}
{
}

InStreamWrapper::~InStreamWrapper()
{
}

HRESULT STDMETHODCALLTYPE InStreamWrapper::QueryInterface( REFIID iid, void** ppvObject )
{ 
	if ( iid == __uuidof( IUnknown ) )
	{
		*ppvObject = reinterpret_cast< IUnknown* >( this );
		AddRef();
		return S_OK;
	}

	if ( iid == cISequentialInStream )
	{
		*ppvObject = static_cast< ISequentialInStream* >( this );
		AddRef();
		return S_OK;
	}

	if ( iid == cIInStream )
	{
		*ppvObject = static_cast< IInStream* >( this );
		AddRef();
		return S_OK;
	}

	if ( iid == cIStreamGetSize )
	{
		*ppvObject = static_cast< IStreamGetSize* >( this );
		AddRef();
		return S_OK;
	}
	
	return E_NOINTERFACE;
}

ULONG STDMETHODCALLTYPE InStreamWrapper::AddRef()
{
	return static_cast< ULONG >( InterlockedIncrement( &m_refCount ) );
}

ULONG STDMETHODCALLTYPE InStreamWrapper::Release()
{
	ULONG res = static_cast< ULONG >( InterlockedDecrement( &m_refCount ) );
	if ( res == 0 )
	{
		delete this;
	}
	return res;
}

STDMETHODIMP InStreamWrapper::Read( void* data, UInt32 size, UInt32* processedSize )
{
	ULONG read = 0;
	HRESULT hr = m_baseStream->Read( data, size, &read );
	if ( processedSize != NULL )
	{
		*processedSize = read;
	}
	// Transform S_FALSE to S_OK
	return SUCCEEDED( hr ) ? S_OK : hr;
}

STDMETHODIMP InStreamWrapper::Seek( Int64 offset, UInt32 seekOrigin, UInt64* newPosition )
{
	LARGE_INTEGER move;
	ULARGE_INTEGER newPos;

	move.QuadPart = offset;
	HRESULT hr = m_baseStream->Seek( move, seekOrigin, &newPos );
	if ( newPosition != NULL )
	{
		*newPosition =  newPos.QuadPart;
	}
	return hr;
}

STDMETHODIMP InStreamWrapper::GetSize( UInt64* size )
{
	STATSTG statInfo;
	HRESULT hr = m_baseStream->Stat( &statInfo, STATFLAG_NONAME );
	if ( SUCCEEDED( hr ) )
	{
		*size = statInfo.cbSize.QuadPart;
	}
	return hr;
}
