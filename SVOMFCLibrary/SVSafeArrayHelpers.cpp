// ******************************************************************************
// * COPYRIGHT (c) 2003 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVSafeArrayHelpers
// * .File Name       : $Workfile:   SVSafeArrayHelpers.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   25 Apr 2013 13:13:44  $
// ******************************************************************************

#include "stdafx.h"
#include "SVSafeArrayHelpers.h"

HRESULT SafeArrayGetElementNoCopy(SAFEARRAY* psa, long* rgIndices, void* pv)
// Does a blind copy of the requested element.
// E.G., if the element is a BSTR (wchar_t*), only the value of the pointer is copied,
//      not the contents of the BSTR.
{
    void* pElement;
    HRESULT hr = SafeArrayGetElementPointer(psa, rgIndices, &pElement);
    if (hr == S_OK)
        memcpy(pv, pElement, psa->cbElements);
	
    return hr;
}

HRESULT SafeArrayPutElementNoCopy(SAFEARRAY* psa, long* rgIndices, void* pv)
// Does a blind put of the specified element.
// E.G., if the element is a BSTR (wchar_t*), only the value of the pointer is copied,
//      not the contents of the BSTR.
{
    void* pElement;
    HRESULT hr = SafeArrayGetElementPointer(psa, rgIndices, &pElement);
    if (hr == S_OK)
        memcpy(pElement, pv, psa->cbElements);
	
    return hr;
}

HRESULT SafeArrayGetElementPointer(SAFEARRAY* psa, long* rgIndices, void** ppv)
// sets the pointer to point to the actual memory of the SAFEARRAY that contains the element.
{
    DWORD dwOffset=0;
    DWORD dwIndexOffset=1;
    for (int i=psa->cDims-1; i >= 0 ; i--)
    {
        long dwIndex = (*(rgIndices+i) - psa->rgsabound[i].lLbound);   // index in dimension
        if (dwIndex < 0)
            return S_FALSE;
        dwOffset += dwIndex * dwIndexOffset;
        dwIndexOffset *= psa->rgsabound[i].cElements;   // offset of dimension used in calculating next dim
    }
    dwOffset *= psa->cbElements;    // scale by size of element
	
    BYTE* pElement = ((BYTE*) psa->pvData) + dwOffset;
    *ppv = pElement;
	
    return S_OK;
}

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVOMFCLibrary\SVSafeArrayHelpers.cpp_v  $
 * 
 *    Rev 1.0   25 Apr 2013 13:13:44   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   07 Jul 2003 10:19:34   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   transfered from SVObserver project
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/