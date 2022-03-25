//*****************************************************************************
/// \copyright (c) 2018,2018 by Jan Kaniewski, Keith J. Jones
/// \file PropVariant.h
/// All Rights Reserved 
/// Author           : Jan Kaniewski, Keith J. Jones
/// License          : Public Domain License
/// From             : https://github.com/keithjjones/7zip-cpp
//*****************************************************************************
/// Property variant class
//******************************************************************************

// This file is based on the following file from the LZMA SDK (http://www.7-zip.org/sdk.html):
//   ./CPP/Windows/PropVariant.h

#pragma once
#include "7zTypes.h"

template <class T> inline int MyCompare(T a, T b)
{
	return a == b ? 0 : (a < b ? -1 : 1);
}

class PropVariant : public PROPVARIANT
{
public:

	PropVariant() { vt = VT_EMPTY; wReserved1 = 0; }
	~PropVariant() { Clear(); }
	explicit PropVariant(const PROPVARIANT &varSrc);
	PropVariant(const PropVariant &varSrc);
	explicit PropVariant(BSTR bstrSrc);
	explicit PropVariant(LPCOLESTR lpszSrc);
	explicit PropVariant(bool bSrc) { vt = VT_BOOL; wReserved1 = 0; boolVal = (bSrc ? VARIANT_TRUE : VARIANT_FALSE); };
	explicit PropVariant(Byte value) { vt = VT_UI1; wReserved1 = 0; bVal = value; }
	explicit PropVariant(Int16 value) { vt = VT_I2; wReserved1 = 0; iVal = value; }
	explicit PropVariant(Int32 value) { vt = VT_I4; wReserved1 = 0; lVal = value; }
	explicit PropVariant(UInt32 value) { vt = VT_UI4; wReserved1 = 0; ulVal = value; }
	explicit PropVariant(UInt64 value) { vt = VT_UI8; wReserved1 = 0; uhVal.QuadPart = value; }
	explicit PropVariant(const FILETIME &value) { vt = VT_FILETIME; wReserved1 = 0; filetime = value; }

	PropVariant& operator=(const PropVariant &varSrc);
	PropVariant& operator=(const PROPVARIANT &varSrc);
	PropVariant& operator=(BSTR bstrSrc);
	PropVariant& operator=(LPCOLESTR lpszSrc);
	PropVariant& operator=(const char *s);
	PropVariant& operator=(bool bSrc);
	PropVariant& operator=(Byte value);
	PropVariant& operator=(Int16 value);
	PropVariant& operator=(Int32 value);
	PropVariant& operator=(UInt32 value);
	PropVariant& operator=(Int64 value);
	PropVariant& operator=(UInt64 value);
	PropVariant& operator=(const FILETIME &value);

	HRESULT Clear();
	HRESULT Copy(const PROPVARIANT *pSrc);
	HRESULT Attach(PROPVARIANT *pSrc);
	HRESULT Detach(PROPVARIANT *pDest);

	HRESULT InternalClear();
	void InternalCopy(const PROPVARIANT *pSrc);

	int Compare(const PropVariant &a1);
};
