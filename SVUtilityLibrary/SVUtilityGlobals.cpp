//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVUtilityGlobals
//* .File Name       : $Workfile:   SVUtilityGlobals.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   25 Apr 2013 19:39:08  $
//******************************************************************************

#include "stdafx.h"
#include "SVUtilityGlobals.h"
#include "SVString.h"

HRESULT SafeArrayGetElementNoCopy(SAFEARRAY* psa, long* rgIndices, void* pv)
// Does a blind copy of the requested element.
// E.G., if the element is a BSTR (wchar_t*), only the value of the pointer is copied,
//      not the contents of the BSTR.
{
    void* pElement;
    HRESULT hr = SafeArrayGetElementPointer(psa, rgIndices, &pElement);
    if (S_OK == hr)
	{
        memcpy(pv, pElement, psa->cbElements);
	}
    return hr;
}

HRESULT SafeArrayPutElementNoCopy(SAFEARRAY* psa, long* rgIndices, void* pv)
// Does a blind put of the specified element.
// E.G., if the element is a BSTR (wchar_t*), only the value of the pointer is copied,
//      not the contents of the BSTR.
{
    void* pElement;
    HRESULT hr = SafeArrayGetElementPointer(psa, rgIndices, &pElement);
    if (S_OK == hr)
	{
		memcpy(pElement, &pv, psa->cbElements);
	}
    return hr;
}

HRESULT SafeArrayGetElementPointer(SAFEARRAY* psa, long* rgIndices, void** ppv)
// sets the pointer to point to the actual memory of the SAFEARRAY that contains the element.
{
    DWORD dwOffset=0;
    DWORD dwIndexOffset=1;
    for (int i=psa->cDims-1; i >= 0 ; i--)
    {
		long lIndex = *(rgIndices+i);
		int iDimensionIndex = psa->cDims-1 - i;
		if ( lIndex < psa->rgsabound[iDimensionIndex].lLbound || lIndex >= (long)( psa->rgsabound[iDimensionIndex].lLbound + psa->rgsabound[iDimensionIndex].cElements ) )
			return DISP_E_BADINDEX;
		
		long dwIndex = (lIndex - psa->rgsabound[iDimensionIndex].lLbound);   // offset index in dimension used for figuring out memory offset
		
		dwOffset += dwIndex * dwIndexOffset;
		dwIndexOffset *= psa->rgsabound[iDimensionIndex].cElements;   // offset of dimension used in calculating next dim
    }
    dwOffset *= psa->cbElements;    // scale by size of element
	
    BYTE* pElement = ((BYTE*) psa->pvData) + dwOffset;
    *ppv = pElement;
	
    return S_OK;
}

void KeepPrevError( HRESULT& p_rhrPrev, HRESULT p_hrNew )
{
	if( S_OK == p_rhrPrev )
	{
		p_rhrPrev = p_hrNew;
	}
}

HRESULT KeepPrevErrorReturnPrev( HRESULT& p_rhrPrev, HRESULT p_hrNew )
{
	if( S_OK == p_rhrPrev )
	{
		p_rhrPrev = p_hrNew;
	}

	return p_rhrPrev;
}

HRESULT KeepPrevErrorReturnNew( HRESULT& p_rhrPrev, HRESULT p_hrNew )
{
	if( S_OK == p_rhrPrev )
	{
		p_rhrPrev = p_hrNew;
	}

	return p_hrNew;
}

bool SVRemoveEscapedSpecialCharacters( SVString& p_rString, LPCTSTR p_szSpecialCharStr )
{
	bool bRemoved = false;
	TCHAR escapeChar = (TCHAR)'\\';

	SVString s;

	size_t nCount = p_rString.size();

	for( size_t i = 0; i < nCount; i++ )
	{
		TCHAR t = p_rString[ i ];
		if( t == escapeChar )
		{
			if( i < nCount - 1 )
			{
				bool found = false;

				TCHAR t2 = p_rString[i + 1];

				for( size_t n = 0; n < strlen( p_szSpecialCharStr ); n++ )
				{
					TCHAR specialChar = p_szSpecialCharStr[ n ];

					if( t2 == specialChar )
					{
						found = true;
						bRemoved = true;
						break;
					}
				}

				if( found )
				{
					continue;
				}
			}
		}

		s += t;
	}

	if ( bRemoved )
	{
		p_rString = s;
	}

	return bRemoved;
}

