//*****************************************************************************
/// \copyright (c) 2018,2018 by Jan Kaniewski, Keith J. Jones
/// \file OutStreamWrapper.cpp
/// All Rights Reserved 
/// Author           : Jan Kaniewski, Keith J. Jones
/// License          : Public Domain License
/// From             : https://github.com/keithjjones/7zip-cpp
//*****************************************************************************
/// Output stream wrapper
//******************************************************************************

#include "stdafx.h"
#include "OutStreamWrapper.h"

constexpr GUID cISequentialOutStream = {0x23170F69, 0x40C1, 0x278A, {0x00, 0x00, 0x00, 0x03, 0x00, 0x02, 0x00, 0x00}};
constexpr GUID cIOutStream = {0x23170F69, 0x40C1, 0x278A, {0x00, 0x00, 0x00, 0x03, 0x00, 0x04, 0x00, 0x00}};

OutStreamWrapper::OutStreamWrapper(const CComPtr<IStream>& rBaseStream) :
	m_BaseStream{rBaseStream}
{
}

OutStreamWrapper::~OutStreamWrapper()
{
}

HRESULT STDMETHODCALLTYPE OutStreamWrapper::QueryInterface( REFIID iid, void** ppvObject )
{ 
	if ( iid == __uuidof( IUnknown ) )
	{
		*ppvObject = static_cast< IUnknown* >( this );
		AddRef();
		return S_OK;
	}

	if ( iid == cISequentialOutStream )
	{
		*ppvObject = static_cast< ISequentialOutStream* >( this );
		AddRef();
		return S_OK;
	}

	if ( iid == cIOutStream )
	{
		*ppvObject = static_cast< IOutStream* >( this );
		AddRef();
		return S_OK;
	}
	
	return E_NOINTERFACE;
}

ULONG STDMETHODCALLTYPE OutStreamWrapper::AddRef()
{
	return static_cast< ULONG >( InterlockedIncrement( &m_refCount ) );
}

ULONG STDMETHODCALLTYPE OutStreamWrapper::Release()
{
	ULONG res = static_cast< ULONG >( InterlockedDecrement( &m_refCount ) );
	if ( res == 0 )
	{
		delete this;
	}
	return res;
}

STDMETHODIMP OutStreamWrapper::Write( const void* data, UInt32 size, UInt32* processedSize )
{
	ULONG written = 0;
	HRESULT hr = m_BaseStream->Write( data, size, &written );
	if ( processedSize != NULL )
	{
		*processedSize = written;
	}
	return hr;
}

STDMETHODIMP OutStreamWrapper::Seek( Int64 offset, UInt32 seekOrigin, UInt64* newPosition )
{
	LARGE_INTEGER move;
	ULARGE_INTEGER newPos;

	move.QuadPart = offset;
	HRESULT hr = m_BaseStream->Seek( move, seekOrigin, &newPos );
	if ( newPosition != NULL )
	{
		*newPosition =  newPos.QuadPart;
	}
	return hr;
}

STDMETHODIMP OutStreamWrapper::SetSize( UInt64 newSize )
{
	ULARGE_INTEGER size;
	size.QuadPart = newSize;
	return m_BaseStream->SetSize( size );
}
