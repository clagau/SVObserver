//*****************************************************************************
/// \copyright (c) 2018,2018 by Jan Kaniewski, Keith J. Jones
/// \file OutStreamWrapper.h
/// All Rights Reserved 
/// Author           : Jan Kaniewski, Keith J. Jones
/// License          : Public Domain License
/// From             : https://github.com/keithjjones/7zip-cpp
//*****************************************************************************
/// Output stream wrapper
//******************************************************************************

#pragma once

#include "IStream.h"


class OutStreamWrapper : public IOutStream
{
public:

	explicit OutStreamWrapper( const CComPtr<IStream>& rBaseStream );
	virtual ~OutStreamWrapper();

	STDMETHOD(QueryInterface)( REFIID iid, void** ppvObject );
	STDMETHOD_(ULONG, AddRef)();
	STDMETHOD_(ULONG, Release)();

	// ISequentialOutStream
	STDMETHOD(Write)( const void* data, UInt32 size, UInt32* processedSize );

	// IOutStream
	STDMETHOD(Seek)( Int64 offset, UInt32 seekOrigin, UInt64* newPosition );
	STDMETHOD(SetSize)( UInt64 newSize );

private:

	long				m_refCount{0};
	CComPtr<IStream>	m_BaseStream;
};
