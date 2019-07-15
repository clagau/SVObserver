// ******************************************************************************
// * COPYRIGHT (c) 2007 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVMaskeditor
// * .File Name       : $Workfile:   SVMaskEditor.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.1  $
// * .Check In Date   : $Date:   01 Oct 2013 10:22:32  $
// ******************************************************************************
#pragma region Includes
#include "stdafx.h"
#include "SVMaskEditor.h"
#include "SVMatroxLibrary/SVMatroxBufferInterface.h"
#include "SVMatroxLibrary/SVMatroxBufferCreateStruct.h"
#pragma endregion Includes

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

SVMaskEditorApp theApp;

const GUID CDECL BASED_CODE _tlid =
		{ 0xc87c9b70, 0xe6dc, 0x11d2, { 0xa7, 0x7b, 0, 0x10, 0x6f, 0, 0xc, 0x7d } };
const WORD _wVerMajor = 1;
const WORD _wVerMinor = 0;

////////////////////////////////////////////////////////////////////////////
// SVMaskEditorApp::InitInstance - DLL initialization

BOOL SVMaskEditorApp::InitInstance()
{
	BOOL bInit = COleControlModule::InitInstance();

	if (bInit)
	{
		// TODO: Add your own module initialization code here.
	}

	return bInit;
}

////////////////////////////////////////////////////////////////////////////
// SVMaskEditorApp::ExitInstance - DLL termination

int SVMaskEditorApp::ExitInstance()
{
	// TODO: Add your own module termination code here.

	return COleControlModule::ExitInstance();
}

/////////////////////////////////////////////////////////////////////////////
// DllRegisterServer - Adds entries to the system registry

STDAPI DllRegisterServer(void)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if (!AfxOleRegisterTypeLib(AfxGetInstanceHandle(), _tlid))
		return ResultFromScode(SELFREG_E_TYPELIB);

	if (!COleObjectFactoryEx::UpdateRegistryAll(TRUE))
		return ResultFromScode(SELFREG_E_CLASS);

	return NOERROR;
}

/////////////////////////////////////////////////////////////////////////////
// DllUnregisterServer - Removes entries from the system registry

STDAPI DllUnregisterServer(void)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if (!AfxOleUnregisterTypeLib(_tlid, _wVerMajor, _wVerMinor))
		return ResultFromScode(SELFREG_E_TYPELIB);

	if (!COleObjectFactoryEx::UpdateRegistryAll(FALSE))
		return ResultFromScode(SELFREG_E_CLASS);

	return NOERROR;
}


// Global Functions...

SVMatroxBuffer SVBitmapToMilBuffer( HBITMAP HBM ) 
{
	SVMatroxBuffer l_MilBuffer;

    if( ! HBM )
        return l_MilBuffer;

    // Get Bitmap info...
    CBitmap bm;
    bm.Attach( HBM );
	BITMAP bmInfo;
	bm.GetBitmap( &bmInfo );
    bm.Detach();

    bmInfo.bmBitsPixel = ( bmInfo.bmBitsPixel < 8 ) ? 8 : bmInfo.bmBitsPixel;

    // Try to create mil buffer...
	SVMatroxBufferInterface l_BufIntf;
	SVMatroxBufferCreateStruct l_Create;
	l_Create.m_eAttribute = SVBufAttImageProcDispDib;
	l_Create.m_lSizeX = bmInfo.bmWidth;
	l_Create.m_lSizeY = bmInfo.bmHeight;
	SetImageDepth(l_Create, bmInfo.bmBitsPixel );
	l_Create.m_lSizeBand = bmInfo.bmPlanes;
	/*l_Code =*/ l_BufIntf.Create( l_MilBuffer, l_Create );

    if( l_MilBuffer.empty() )
        return l_MilBuffer;

    // Try to get device context of mil buffer...
	/*l_Code =*/ l_BufIntf.Set( l_MilBuffer, SVBufWindowDCAlloc, (long long)SVValueDefault );

    HDC hDC;

	LONGLONG Handle;
	l_BufIntf.Get( l_MilBuffer, SVWindowDC, Handle );
	hDC = reinterpret_cast<HDC> (Handle);

    HDC hMemDC = nullptr;

    if( nullptr != hDC )
	{
        if( hMemDC = ::CreateCompatibleDC( hDC ) )
        {
            // Select source bitmap...
            HBITMAP hOldBM = ( HBITMAP ) ::SelectObject( hMemDC, HBM );

            // Copy source bitmap into mil buffer...
            ::SetStretchBltMode( hDC, COLORONCOLOR );
            ::StretchBlt( hDC, 0, 0, bmInfo.bmWidth, bmInfo.bmHeight, hMemDC, 0, 0, bmInfo.bmWidth, bmInfo.bmHeight, SRCCOPY );

            // Deselect source bitmap and kill memDC...
            ::SelectObject( hMemDC, hOldBM );
            ::DeleteDC( hMemDC );

            // Release mil buffer dc...
			l_BufIntf.Set( l_MilBuffer, SVBufWindowDCFree, (long long)SVValueDefault );


			l_BufIntf.Set( l_MilBuffer, SVBufModified, (long long)SVValueDefault );

            return l_MilBuffer;
        }

        // Release mil buffer dc...
		l_BufIntf.Set( l_MilBuffer, SVBufWindowDCFree, (long long)SVValueDefault );
	}

	l_MilBuffer.clear();

    return l_MilBuffer;
}


