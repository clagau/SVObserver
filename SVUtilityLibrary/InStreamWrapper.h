//*****************************************************************************
/// \copyright (c) 2018,2018 by Jan Kaniewski, Keith J. Jones
/// \file InStreamWrapper.h
/// All Rights Reserved 
/// Author           : Jan Kaniewski, Keith J. Jones
/// License          : Public Domain License
/// From             : https://github.com/keithjjones/7zip-cpp
//*****************************************************************************
/// Input stream wrapper
//******************************************************************************

#pragma once


#include "IStream.h"

class InStreamWrapper : public IInStream, public IStreamGetSize
{
public:

	explicit InStreamWrapper( const CComPtr< IStream >& baseStream );
	virtual ~InStreamWrapper();

	STDMETHOD(QueryInterface)( REFIID iid, void** ppvObject );
	STDMETHOD_(ULONG, AddRef)();
	STDMETHOD_(ULONG, Release)();

	// ISequentialInStream
	STDMETHOD(Read)( void* data, UInt32 size, UInt32* processedSize );

	// IInStream
	STDMETHOD(Seek)( Int64 offset, UInt32 seekOrigin, UInt64* newPosition );

	// IStreamGetSize
	STDMETHOD(GetSize)( UInt64* size );

private:
	long				m_refCount{0};
	CComPtr<IStream>	m_baseStream;
};
