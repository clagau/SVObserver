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


//#ifdef _DEBUG
//#define new DEBUG_NEW
//#undef THIS_FILE
//static char THIS_FILE[] = __FILE__;
//#endif

#include "stdafx.h"
#include "SVMaskEditor.h"
#include "SVMatroxLibrary\SVMatroxLibrary.h"

SVMaskEditorApp NEAR theApp;

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
/*
CString strText;
strText.Format( _T( "Planes: %d" ), bmInfo.bmPlanes );
AfxMessageBox( strText );
strText.Format( _T( "BitsPerPixel: %d" ), bmInfo.bmBitsPixel );
AfxMessageBox( strText );
*/
    // Try to create mil buffer...
	SVMatroxBufferInterface l_BufIntf;
	SVMatroxBufferInterface::SVStatusCode l_Code;
	SVMatroxBufferCreateStruct l_Create;

	l_Create.m_eAttribute = SVBufAttImageProcDispDib;
	l_Create.m_lSizeX = bmInfo.bmWidth;
	l_Create.m_lSizeY = bmInfo.bmHeight;
	l_Create.SetImageDepth( bmInfo.bmBitsPixel );
	l_Create.m_lSizeBand = bmInfo.bmPlanes;
	l_Code = l_BufIntf.Create( l_MilBuffer, l_Create );

    if( l_MilBuffer.empty() )
        return l_MilBuffer;

    // Try to get device context of mil buffer...
	l_Code = l_BufIntf.Set( l_MilBuffer, SVBufWindowDCAlloc, (SVMatroxInt)SVValueDefault );

    HDC hDC;

	long l_lTmp;
	l_BufIntf.Get( l_MilBuffer, SVWindowDC, l_lTmp );
	hDC = reinterpret_cast<HDC>(l_lTmp);

    HDC hMemDC = NULL;

    if( hDC != NULL )
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
			l_BufIntf.Set( l_MilBuffer, SVBufWindowDCFree, (SVMatroxInt)SVValueDefault );


			l_BufIntf.Set( l_MilBuffer, SVBufModified, (SVMatroxInt)SVValueDefault );

            return l_MilBuffer;
        }

        // Release mil buffer dc...
		l_BufIntf.Set( l_MilBuffer, SVBufWindowDCFree, (SVMatroxInt)SVValueDefault );
	}

	l_MilBuffer.clear();

    return l_MilBuffer;
}


// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVMaskEditor\SVMaskEditor.cpp_v  $
 * 
 *    Rev 1.1   01 Oct 2013 10:22:32   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Add x64 platform.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   26 Apr 2013 14:06:34   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.3   27 Jan 2011 10:14:04   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  712
   SCR Title:  Fix issues with black images when using command interface (SIAC)
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated source code to fix issues with Matrox Buffer Management.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.2   24 Jul 2007 10:18:48   tbair
   Project:  SVObserver
   Change Request (SCR) nbr:  597
   SCR Title:  Upgrade Matrox Imaging Library to version 8.0
   Checked in by:  tBair;  Tom Bair
   Change Description:  
     Upgrade to Matrox Imaging Library 8.0
   
   /////////////////////////////////////////////////////////////////////////////////////
*/
