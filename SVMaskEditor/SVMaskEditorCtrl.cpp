// ******************************************************************************
// * COPYRIGHT (c) 2007 by K�rber Pharma Inspection GmbH. All Rights Reserved
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVMaskEditorCtrl
// * .File Name       : $Workfile:   SVMaskEditorCtrl.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.2  $
// * .Check In Date   : $Date:   03 Jun 2014 13:20:58  $
// ******************************************************************************

// SVMaskEditorCtrl.cpp : Implementation of the SVMaskEditorCtrl ActiveX Control class.

#pragma region Includes
#include "stdafx.h"
#include "resource.h"
#include "SVMaskEditorCtrl.h"
#include "SVMaskEditor.h"
#include "SVSystemLibrary\SVImageConvertorGDI.h"
#include "SVMaskEditorPropPage.h"
#include "SVStatusLibrary\MessageManagerHelper.h"
#pragma endregion Includes

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(SVMaskEditorCtrl, COleControl)

/////////////////////////////////////////////////////////////////////////////
// Message map

BEGIN_MESSAGE_MAP(SVMaskEditorCtrl, COleControl)
	//{{AFX_MSG_MAP(SVMaskEditorCtrl)
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
	ON_OLEVERB(AFX_IDS_VERB_PROPERTIES, OnProperties)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// Dispatch map

BEGIN_DISPATCH_MAP(SVMaskEditorCtrl, COleControl)
	//{{AFX_DISPATCH_MAP(SVMaskEditorCtrl)
	DISP_PROPERTY_EX(SVMaskEditorCtrl, "ImageDIBHandle", GetImageDIBHandle, SetImageDIBHandle, VT_UNKNOWN)
	DISP_PROPERTY_EX(SVMaskEditorCtrl, "MaskOperator", GetMaskOperator, SetMaskOperator, VT_I4)
	DISP_PROPERTY_EX(SVMaskEditorCtrl, "MaskData", GetMaskData, SetMaskData, VT_UNKNOWN  )
	DISP_FUNCTION(SVMaskEditorCtrl, "DoEditMaskModal", DoEditMaskModal, VT_I4, VTS_NONE)
	DISP_FUNCTION_ID(SVMaskEditorCtrl, "Refresh", DISPID_REFRESH, Refresh, VT_EMPTY, VTS_NONE)
	//}}AFX_DISPATCH_MAP
	DISP_FUNCTION_ID(SVMaskEditorCtrl, "AboutBox", DISPID_ABOUTBOX, AboutBox, VT_EMPTY, VTS_NONE)
END_DISPATCH_MAP()


/////////////////////////////////////////////////////////////////////////////
// Event map

BEGIN_EVENT_MAP(SVMaskEditorCtrl, COleControl)
	//{{AFX_EVENT_MAP(SVMaskEditorCtrl)
	EVENT_STOCK_CLICK()
	//}}AFX_EVENT_MAP
END_EVENT_MAP()


/////////////////////////////////////////////////////////////////////////////
// Property pages

// TODO: Add more property pages as needed.  Remember to increase the count!
BEGIN_PROPPAGEIDS(SVMaskEditorCtrl, 1)
	PROPPAGEID(SVMaskEditorPropPage::guid)
END_PROPPAGEIDS(SVMaskEditorCtrl)


/////////////////////////////////////////////////////////////////////////////
// Initialize class factory and guid

IMPLEMENT_OLECREATE_EX(SVMaskEditorCtrl, "SVMASKEDITOR.SVMaskEditorCtrl.1",
	0xc87c9b74, 0xe6dc, 0x11d2, 0xa7, 0x7b, 0, 0x10, 0x6f, 0, 0xc, 0x7d)


/////////////////////////////////////////////////////////////////////////////
// Type library ID and version

IMPLEMENT_OLETYPELIB(SVMaskEditorCtrl, _tlid, _wVerMajor, _wVerMinor)


/////////////////////////////////////////////////////////////////////////////
// Interface IDs

const IID BASED_CODE IID_DSVMaskEditor =
		{ 0xc87c9b72, 0xe6dc, 0x11d2, { 0xa7, 0x7b, 0, 0x10, 0x6f, 0, 0xc, 0x7d } };
const IID BASED_CODE IID_DSVMaskEditorEvents =
		{ 0xc87c9b73, 0xe6dc, 0x11d2, { 0xa7, 0x7b, 0, 0x10, 0x6f, 0, 0xc, 0x7d } };


/////////////////////////////////////////////////////////////////////////////
// Control type information

constexpr DWORD BASED_CODE _dwSVMaskEditorOleMisc =
	OLEMISC_INVISIBLEATRUNTIME |
	OLEMISC_ACTIVATEWHENVISIBLE |
	OLEMISC_SETCLIENTSITEFIRST |
	OLEMISC_INSIDEOUT |
	OLEMISC_CANTLINKINSIDE |
	OLEMISC_RECOMPOSEONRESIZE;

IMPLEMENT_OLECTLTYPE(SVMaskEditorCtrl, IDS_SVMASKEDITOR, _dwSVMaskEditorOleMisc)


/////////////////////////////////////////////////////////////////////////////
// SVMaskEditorCtrl::SVMaskEditorCtrlFactory::UpdateRegistry -
// Adds or removes system registry entries for SVMaskEditorCtrl

BOOL SVMaskEditorCtrl::SVMaskEditorCtrlFactory::UpdateRegistry(BOOL bRegister)
{
	// TODO: Verify that your control follows apartment-model threading rules.
	// Refer to MFC TechNote 64 for more information.
	// If your control does not conform to the apartment-model rules, then
	// you must modify the code below, changing the 6th parameter from
	// afxRegApartmentThreading to 0.

	if (bRegister)
		return AfxOleRegisterControlClass(
			AfxGetInstanceHandle(),
			m_clsid,
			m_lpszProgID,
			IDS_SVMASKEDITOR,
			IDB_SVMASKEDITOR,
			afxRegApartmentThreading,
			_dwSVMaskEditorOleMisc,
			_tlid,
			_wVerMajor,
			_wVerMinor);
	else
		return AfxOleUnregisterClass(m_clsid, m_lpszProgID);
}


/////////////////////////////////////////////////////////////////////////////
// SVMaskEditorCtrl::SVMaskEditorCtrl - Constructor

SVMaskEditorCtrl::SVMaskEditorCtrl()
{
	InitializeIIDs(&IID_DSVMaskEditor, &IID_DSVMaskEditorEvents);

	// TODO: Initialize your control's instance data here.
}


/////////////////////////////////////////////////////////////////////////////
// SVMaskEditorCtrl::~SVMaskEditorCtrl - Destructor

SVMaskEditorCtrl::~SVMaskEditorCtrl()
{
	// TODO: Cleanup your control's instance data here.
}


/////////////////////////////////////////////////////////////////////////////
// SVMaskEditorCtrl::OnDraw - Drawing function

void SVMaskEditorCtrl::OnDraw(
			CDC* pdc, const CRect& rcBounds, const CRect&)
{
	if( pdc )
	{
        BOOL bLazyBitmap = FALSE;

        if( ! HBITMAP( bitmap ) )
        {
        	HBITMAP bm = ( HBITMAP ) ::LoadImage( AfxGetInstanceHandle(), 
		        								  MAKEINTRESOURCE( IDB_BITMAP1 ), //nIDResource ),
				        						  IMAGE_BITMAP,
						        				  0,
								        		  0,
										          LR_DEFAULTCOLOR | LR_CREATEDIBSECTION );
            bitmap.Attach( bm );
            bLazyBitmap = TRUE;
        }

		CDC memDC;
		memDC.CreateCompatibleDC( pdc );
		memDC.SelectObject( &bitmap );

		BITMAP bmInfo;
		bitmap.GetBitmap( &bmInfo );
		WORD colNum = static_cast<WORD> (1 << bmInfo.bmBitsPixel);

		RGBQUAD* pColors = new RGBQUAD[colNum];
		::GetDIBColorTable( memDC.GetSafeHdc(), 0, colNum, pColors );
		for( UINT i = 0; i < colNum; ++i )
		{	// BGR -> RGB
			BYTE swap = pColors[i].rgbBlue;
			pColors[i].rgbBlue = pColors[i].rgbRed;
			pColors[i].rgbRed = swap;
			pColors[i].rgbReserved = 0;//PC_EXPLICIT; //PC_NOCOLLAPSE;//PC_RESERVED; //; // //
		}

		CPalette* pPal = new CPalette;
		LOGPALETTE* pLP = ( LOGPALETTE* ) new BYTE[ sizeof( LOGPALETTE ) + colNum * sizeof( PALETTEENTRY ) ];
		pLP->palVersion = 0x300;
		pLP->palNumEntries = colNum;
		memcpy( pLP->palPalEntry, pColors,  colNum * sizeof( PALETTEENTRY ) );
                delete [] pColors;           // 23 Mar 1999 - frb.
		pPal->CreatePalette( pLP );
                delete [] pLP;               // 22 Mar 1999 - frb

		memDC.SelectPalette( pPal, FALSE );
		memDC.RealizePalette();

		pdc->SelectPalette( pPal, FALSE );
		pdc->RealizePalette();


		pdc->SetStretchBltMode( COLORONCOLOR );
		pdc->StretchBlt( 0, 0, rcBounds.Width(), rcBounds.Height(), 
					   &memDC, 0, 0, bmInfo.bmWidth, bmInfo.bmHeight, SRCCOPY );

		memDC.DeleteDC();

        if( bLazyBitmap )
            bitmap.Detach();
	}
	else
	{
		MessageBox( _T( "No device context available!" ) );
	}

}


/////////////////////////////////////////////////////////////////////////////
// SVMaskEditorCtrl::DoPropExchange - Persistence support

void SVMaskEditorCtrl::DoPropExchange(CPropExchange* pPX)
{
	ExchangeVersion(pPX, MAKELONG(_wVerMinor, _wVerMajor));
	COleControl::DoPropExchange(pPX);

	// TODO: Call PX_ functions for each persistent custom property.

}


/////////////////////////////////////////////////////////////////////////////
// SVMaskEditorCtrl::GetControlFlags -
// Flags to customize MFC's implementation of ActiveX controls.
//
// For information on using these flags, please see MFC technical note
// #nnn, "Optimizing an ActiveX Control".
DWORD SVMaskEditorCtrl::GetControlFlags()
{
	DWORD dwFlags = COleControl::GetControlFlags();


	// The control will not be redrawn when making the transition
	// between the active and inactivate state.
	dwFlags |= noFlickerActivate;
	return dwFlags;
}


/////////////////////////////////////////////////////////////////////////////
// SVMaskEditorCtrl::OnResetState - Reset control to default state

void SVMaskEditorCtrl::OnResetState()
{
	COleControl::OnResetState();  // Resets defaults found in DoPropExchange

	// TODO: Reset any other control state here.
}


/////////////////////////////////////////////////////////////////////////////
// SVMaskEditorCtrl::AboutBox - Display an "About" box to the user

void SVMaskEditorCtrl::AboutBox()
{
	CDialog dlgAbout(IDD_ABOUTBOX_SVMASKEDITOR);
	dlgAbout.DoModal();
}


/////////////////////////////////////////////////////////////////////////////
// SVMaskEditorCtrl message handlers

IPictureDisp* SVMaskEditorCtrl::GetImageDIBHandle() 
{
	CComPtr<IPictureDisp> pPic;
	/*HRESULT hr =*/ SVImageConvertorGDI::GetIPictureDispFromBitmap(bitmap, &pPic);
	return pPic.Detach();
}

void SVMaskEditorCtrl::SetImageDIBHandle(IPictureDisp* pPic) 
{
	bitmap.Detach(); // in case one is already attached
	HBITMAP bm;
	HRESULT hr = SVImageConvertorGDI::GetBitmapFromIPictureDisp( pPic, bm );
	if( S_OK == hr )
	{
		bitmap.Attach( bm );
		SetModifiedFlag();
	}
}


void SVMaskEditorCtrl::OnPaint() 
{
	CPaintDC dc(this); // device context for painting

	CRect rect;
	GetClientRect( &rect );
    OnDraw(	&dc, rect, rect );

	// TODO: Add your message handler code here
	
	// Do not call COleControl::OnPaint() for painting messages
}

void SVMaskEditorCtrl::Refresh() 
{
	// TODO: Add your dispatch handler code here

}

long SVMaskEditorCtrl::GetMaskOperator() 
{
	return maskEditorDlg.m_lMilArithmeticOperation;
}

void SVMaskEditorCtrl::SetMaskOperator(long nNewValue) 
{
	maskEditorDlg.m_lMilArithmeticOperation = nNewValue;

	SetModifiedFlag();
}

IStream* SVMaskEditorCtrl::GetMaskData() 
{
	CComPtr<IStream> stream;
	HRESULT hr = CreateStreamOnHGlobal(maskEditorDlg.GraphixObject.GetGraphixData(), true, &stream );
	Log_Assert( S_OK == hr );
	return stream.Detach();
}

void SVMaskEditorCtrl::SetMaskData(IStream* nNewValue) 
{
	HGLOBAL	hg = 0 ;
	HRESULT hr = GetHGlobalFromStream(nNewValue, &hg);
	if( S_OK == hr )
	{
		if(maskEditorDlg.GraphixObject.SetGraphixData( hg ))
		{
			SetModifiedFlag();
		}
	}
	else
	{
		Log_Assert(FALSE);
	}
}

INT_PTR SVMaskEditorCtrl::DoEditMaskModal() 
{
	// Start Mask Editor Dialog...
    maskEditorDlg.HImageBitmap = HBITMAP( bitmap );
    if( ! HBITMAP( bitmap ) )
    {
        maskEditorDlg.HImageBitmap =  ( HBITMAP ) ::LoadImage( AfxGetInstanceHandle(), 
		        											   MAKEINTRESOURCE( IDB_BITMAP1 ), //nIDResource ),
				        									   IMAGE_BITMAP,
						       								   0,
							        						   0,
															   LR_DEFAULTCOLOR | LR_CREATEDIBSECTION );
    }

    return maskEditorDlg.DoModal();
}


