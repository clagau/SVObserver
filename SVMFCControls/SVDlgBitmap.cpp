//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVDlgBitmapClass
//* .File Name       : $Workfile:   SVDlgBitmap.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 10:14:46  $
//******************************************************************************

#include "stdafx.h"
//Moved to precompiled header: #include <io.h>
#include "SVDlgBitmap.h"
#include "SVMessage/ErrorNumbers.h"
#include "SVStatusLibrary\MessageManager.h"
#include "SVMessage\SVMessage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace Seidenader
{
	namespace SVMFCControls
	{
		SVDlgBitmapClass::SVDlgBitmapClass()
		{
			OEMMode = FALSE;
		}

		SVDlgBitmapClass::~SVDlgBitmapClass()
		{
			bitmap.DeleteObject();
		}

		BOOL SVDlgBitmapClass::Init( LPCTSTR lpszResourceName )
		{
			HBITMAP bm;
			bm = InitFromFile();

			if( !bm )
			{
				bm = ( HBITMAP ) ::LoadImage( AfxGetResourceHandle(), 
					lpszResourceName,
					IMAGE_BITMAP,
					0,
					0,
					LR_DEFAULTCOLOR | LR_CREATEDIBSECTION );
			}
			return bitmap.Attach( bm );
		}

		BOOL SVDlgBitmapClass::Init( UINT nIDResource )
		{
			HBITMAP bm;
			bm = InitFromFile();

			if( !bm )
			{
				bm = (HBITMAP) ::LoadImage(  AfxGetResourceHandle(), 
					MAKEINTRESOURCE( nIDResource ),
					IMAGE_BITMAP,
					0,
					0,
					LR_DEFAULTCOLOR | LR_CREATEDIBSECTION );
			}// end if

			return bitmap.Attach( bm );
		}

		HBITMAP SVDlgBitmapClass::InitFromFile()
		{
			HBITMAP bm = nullptr;

			if( 0 == ::_access( _T("C:\\SVObserver\\Bin\\OEM\\splash.bmp"), 0 ) )
			{
				bm = (HBITMAP) ::LoadImage( nullptr, 
					_T("C:\\SVObserver\\Bin\\OEM\\splash.bmp"),
					IMAGE_BITMAP,
					0,
					0,
					LR_DEFAULTCOLOR | LR_CREATEDIBSECTION | LR_LOADFROMFILE );
				if( bm )
				{
					OEMMode = TRUE;
				}

			}// end if

			return bm;
		}

		BOOL SVDlgBitmapClass::TextOut( int X, int Y, COLORREF Color, LPCTSTR StrText )
		{
			SVDlgBitmapTextStruct* pTxt = new SVDlgBitmapTextStruct();
			if( pTxt )
			{
				pTxt->BAbsCoord = TRUE;
				pTxt->X			= ( double ) X;
				pTxt->Y			= ( double ) Y;
				pTxt->Color		= Color;
				pTxt->StrText	= StrText;
				textList.Add( pTxt );

				return TRUE;
			}
			return FALSE;
		}

		BOOL SVDlgBitmapClass::TextOut( double X, double Y, COLORREF Color, LPCTSTR StrText )
		{
			if( X >= 0 && X <= 1 && Y >= 0 && Y <= 1 )
			{
				SVDlgBitmapTextStruct* pTxt = new SVDlgBitmapTextStruct();
				if( pTxt )
				{
					pTxt->BAbsCoord = FALSE;

					// If BAbsCoord is FALSE, the X and Y values will be interpreted 
					// as weights of the client area to generate the origin of the text !!!

					pTxt->X			= X;
					pTxt->Y			= Y;
					pTxt->Color		= Color;
					pTxt->StrText	= StrText;
					textList.Add( pTxt );

					return TRUE;
				}
			}
			return FALSE;
		}

		CSize SVDlgBitmapClass::GetDimensions() const
		{
			CSize size;

			BITMAP bmp;
			bitmap.GetBitmap( &bmp );

			size.cx = bmp.bmWidth;
			size.cy = bmp.bmHeight;

			return size;
		}

		BEGIN_MESSAGE_MAP(SVDlgBitmapClass, CStatic)
			//{{AFX_MSG_MAP(SVDlgBitmapClass)
			ON_WM_PAINT()
			ON_WM_PALETTEISCHANGING()
			//}}AFX_MSG_MAP
		END_MESSAGE_MAP()

		/////////////////////////////////////////////////////////////////////////////
		// Behandlungsroutinen für Nachrichten SVDlgBitmapClass 

		static RGBQUAD rgbStd256[] =
		{
			// Start Sky Colors: Begin
			{ 123,173,214, 0 },	{ 132,181,222, 0 },	{ 115,173,214, 0 },	{ 123,173,222, 0 },
			{ 148,189,222, 0 },	{ 123,181,222, 0 },	{ 140,181,222, 0 },	{ 140,189,222, 0 },
			{ 156,198,222, 0 },	{ 206,222,239, 0 },	{ 222,231,239, 0 },	{ 165,198,231, 0 },
			{ 189,214,231, 0 },	{ 173,206,231, 0 },	{ 165,206,231, 0 },	{ 231,239,247, 0 },
			{ 231,231,239, 0 },	{ 156,189,222, 0 },	{ 123,181,214, 0 },	{ 115,173,222, 0 },
			{ 198,222,239, 0 },	{ 247,247,247, 0 },	{ 239,239,247, 0 },	{ 132,189,222, 0 },
			{ 239,247,247, 0 },	{ 181,214,231, 0 },	{ 214,231,239, 0 },	{ 198,214,231, 0 },
			{ 156,198,231, 0 },	{ 206,231,239, 0 },	{ 189,214,239, 0 },	{ 222,239,239, 0 },
			{ 222,231,247, 0 },	{ 222,239,247, 0 },	{ 231,247,247, 0 },	{ 115,181,222, 0 },
			{ 181,214,239, 0 },	{ 156,189,231, 0 },	{ 214,231,247, 0 },	{ 214,222,239, 0 },
			{ 189,222,239, 0 },	{ 198,214,239, 0 },	{ 247,247,255, 0 },	{ 148,189,231, 0 },
			{ 148,198,222, 0 },	{ 181,206,239, 0 },	{ 173,198,231, 0 },	{ 247,255,255, 0 },
			{ 255,247,247, 0 },	{ 247,255,247, 0 },	{ 206,222,247, 0 },	{ 206,231,247, 0 },
			{ 255,255,255, 0 },	{ 173,214,231, 0 },	{ 148,198,231, 0 },	{ 255,247,255, 0 },
			{ 173,206,239, 0 },	{ 156,206,231, 0 },	{ 140,189,231, 0 },	{ 255,255,247, 0 },
			{ 173,214,239, 0 },	{ 140,181,231, 0 },	{ 132,181,231, 0 },	{ 165,206,239, 0 },
			// Start Sky Colors: End
			{ 102,204,204, 0 },	{  51,204,204, 0 },	{   0,204,204, 0 },	{ 255,153,204, 0 },
			{ 204,153,204, 0 },	{ 153,153,204, 0 },	{ 102,153,204, 0 },	{  51,153,204, 0 },
			{   0,153,204, 0 },	{ 255,102,204, 0 },	{ 204,102,204, 0 },	{ 153,102,204, 0 },
			{ 102,102,204, 0 },	{  51,102,204, 0 },	{   0,102,204, 0 },	{ 255, 51,204, 0 },
			{ 204, 51,204, 0 },	{ 153, 51,204, 0 },	{ 102, 51,204, 0 },	{  51, 51,204, 0 },
			{   0, 51,204, 0 },	{ 255,  0,204, 0 },	{ 204,  0,204, 0 },	{ 153,  0,204, 0 },
			{ 102,  0,204, 0 },	{  51,  0,204, 0 },	{ 255,255,153, 0 },	{ 204,255,153, 0 },
			{ 153,255,153, 0 },	{ 102,255,153, 0 },	{  51,255,153, 0 },	{   0,255,153, 0 },
			{ 255,204,153, 0 },	{ 204,204,153, 0 },	{ 153,204,153, 0 },	{ 102,204,153, 0 },
			{  51,204,153, 0 },	{   0,204,153, 0 },	{ 255,153,153, 0 },	{ 204,153,153, 0 },
			{ 102,153,153, 0 },	{  51,153,153, 0 },	{   0,153,153, 0 },	{ 255,102,153, 0 },
			{ 204,102,153, 0 },	{ 153,102,153, 0 },	{ 102,102,153, 0 },	{  51,102,153, 0 },
			{   0,102,153, 0 },	{ 255, 51,153, 0 },	{ 204, 51,153, 0 },	{ 153, 51,153, 0 },
			{ 102, 51,153, 0 },	{  51, 51,153, 0 },	{   0, 51,153, 0 },	{ 255,  0,153, 0 },
			{ 204,  0,153, 0 },	{ 153,  0,153, 0 },	{ 102,  0,153, 0 },	{  51,  0,153, 0 },
			{ 255,255,102, 0 },	{ 204,255,102, 0 },	{ 153,255,102, 0 },	{ 102,255,102, 0 },
			{  51,255,102, 0 },	{   0,255,102, 0 },	{ 255,204,102, 0 },	{ 204,204,102, 0 },
			{ 153,204,102, 0 },	{ 102,204,102, 0 },	{  51,204,102, 0 },	{   0,204,102, 0 },
			{ 255,153,102, 0 },	{ 204,153,102, 0 },	{ 153,153,102, 0 },	{ 102,153,102, 0 },
			{  51,153,102, 0 },	{   0,153,102, 0 },	{ 255,102,102, 0 },	{ 204,102,102, 0 },
			{ 153,102,102, 0 },	{  51,102,102, 0 },	{   0,102,102, 0 },	{ 255, 51,102, 0 },
			{ 204, 51,102, 0 },	{ 153, 51,102, 0 },	{ 102, 51,102, 0 },	{  51, 51,102, 0 },
			{   0, 51,102, 0 },	{ 255,  0,102, 0 },	{ 204,  0,102, 0 },	{ 153,  0,102, 0 },
			{ 102,  0,102, 0 },	{  51,  0,102, 0 },	{ 255,255, 51, 0 },	{ 204,255, 51, 0 },
			{ 153,255, 51, 0 },	{ 102,255, 51, 0 },	{  51,255, 51, 0 },	{   0,255, 51, 0 },
			{ 255,204, 51, 0 },	{ 204,204, 51, 0 },	{ 153,204, 51, 0 },	{ 102,204, 51, 0 },
			{  51,204, 51, 0 },	{   0,204, 51, 0 },	{ 255,153, 51, 0 },	{ 204,153, 51, 0 },
			{ 153,153, 51, 0 },	{ 102,153, 51, 0 },	{  51,153, 51, 0 },	{   0,153, 51, 0 },
			{ 255,102, 51, 0 },	{ 204,102, 51, 0 },	{ 153,102, 51, 0 },	{ 102,102, 51, 0 },
			{  51,102, 51, 0 },	{   0,102, 51, 0 },	{ 255, 51, 51, 0 },	{ 204, 51, 51, 0 },
			{ 153, 51, 51, 0 },	{ 102, 51, 51, 0 },	{   0, 51, 51, 0 },	{ 255,  0, 51, 0 },
			{ 204,  0, 51, 0 },	{ 153,  0, 51, 0 },	{ 102,  0, 51, 0 },	{  51,  0, 51, 0 },
			{ 204,255,  0, 0 },	{ 153,255,  0, 0 },	{ 102,255,  0, 0 },	{  51,255,  0, 0 },
			{ 255,204,  0, 0 },	{ 204,204,  0, 0 },	{ 153,204,  0, 0 },	{ 102,204,  0, 0 },
			{  51,204,  0, 0 },	{ 255,153,  0, 0 },	{ 204,153,  0, 0 },	{ 153,153,  0, 0 },
			{ 102,153,  0, 0 },	{   0,  0,238, 0 },	{   0,  0,221, 0 },	{   0,  0,204, 0 },
			{   0,  0,187, 0 },	{   0,  0,170, 0 },	{   0,  0,153, 0 },	{   0,  0,136, 0 },
			{   0,  0,119, 0 },	{   0,  0,102, 0 },	{   0,  0, 85, 0 },	{   0,  0, 68, 0 },
			{   0,  0, 51, 0 },	{   0,  0, 34, 0 },	{   0,  0, 17, 0 },	{   0,238,  0, 0 },
			{   0,221,  0, 0 },	{   0,204,  0, 0 },	{   0,187,  0, 0 },	{   0,170,  0, 0 },
			{   0,153,  0, 0 },	{   0,136,  0, 0 },	{   0,119,  0, 0 },	{   0,102,  0, 0 },
			{   0, 85,  0, 0 },	{   0, 68,  0, 0 },	{   0, 51,  0, 0 },	{   0, 34,  0, 0 },
			{   0, 17,  0, 0 },	{ 238,  0,  0, 0 },	{ 221,  0,  0, 0 },	{ 204,  0,  0, 0 },
			{ 187,  0,  0, 0 },	{ 170,  0,  0, 0 },	{ 153,  0,  0, 0 },	{ 136,  0,  0, 0 },
			{ 119,  0,  0, 0 },	{ 102,  0,  0, 0 },	{  85,  0,  0, 0 },	{  68,  0,  0, 0 },
			{  51,  0,  0, 0 },	{  34,  0,  0, 0 },	{ 240,251,255, 0 },	{ 164,160,160, 0 },
			{ 128,128,128, 0 },	{   0,  0,255, 0 },	{   0,255,  0, 0 },	{   0,255,255, 0 },
			{ 255,  0,  0, 0 },	{ 255,  0,255, 0 },	{ 255,255,  0, 0 },	{ 255,255,255, 0 }
		};					   


		void SVDlgBitmapClass::OnPaint() 
		{
			CPaintDC dc(this); // device context for painting

			if( nullptr != dc ) // @WTF
			{
				CDC memDC;
				memDC.CreateCompatibleDC( &dc );
				memDC.SelectObject( &bitmap );

				BITMAP bmInfo;
				bitmap.GetBitmap( &bmInfo );
				UINT colNum = ( ( UINT ) 1 ) << bmInfo.bmBitsPixel; 

				RGBQUAD* pColors = new RGBQUAD[colNum];
				::GetDIBColorTable( memDC.GetSafeHdc(), 0, colNum, pColors );
				BYTE swap;
				for( UINT i = 0; i < colNum; ++i )
				{	// BGR -> RGB
					swap = pColors[i].rgbBlue;
					pColors[i].rgbBlue = pColors[i].rgbRed;
					pColors[i].rgbRed = swap;
					pColors[i].rgbReserved = 0;//PC_EXPLICIT; //PC_NOCOLLAPSE;//PC_RESERVED; //; // //
				};

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

				dc.SelectPalette( pPal, FALSE );
				dc.RealizePalette();

				CRect rect;
				GetClientRect( &rect );

				dc.SetStretchBltMode( COLORONCOLOR );
				dc.StretchBlt( 0, 0, rect.Width(), rect.Height(), 
					&memDC, 0, 0, bmInfo.bmWidth, bmInfo.bmHeight, SRCCOPY );

				memDC.DeleteDC();

				for( int u = textList.GetSize() - 1; u >= 0; --u )
					if( ! textList.GetAt( u )->StrText.IsEmpty() )
					{
						dc.SetBkMode( TRANSPARENT );
						dc.SetTextColor( textList.GetAt( u )->Color );
						if( textList.GetAt( u )->BAbsCoord )
							dc.TextOut( ( int ) textList.GetAt( u )->X, ( int ) textList.GetAt( u )->Y, textList.GetAt( u )->StrText );
						else
							dc.TextOut( ( int ) ( textList.GetAt( u )->X * rect.Width() ), ( int ) ( textList.GetAt( u )->Y * rect.Height() ), textList.GetAt( u )->StrText );
					}

					ReleaseDC( &dc );
					delete pPal;
			}
			else
			{
				SvStl::MessageMgrStd Msg( SvStl::LogAndDisplay );
				Msg.setMessage( SVMSG_SVO_93_GENERAL_WARNING, SvOi::Tid_Error_NoDeviceContext, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_10233 );
			}


			// Kein Aufruf von CStatic::OnPaint() für Zeichnungsnachrichten
		}

		void SVDlgBitmapClass::OnPaletteIsChanging(CWnd* pRealizeWnd) 
		{
			CStatic::OnPaletteIsChanging(pRealizeWnd);

			if( nullptr != pRealizeWnd )
			{
				HDC hdc = ::GetDC( pRealizeWnd->m_hWnd );

				::RealizePalette( hdc );

				::ReleaseDC( pRealizeWnd->m_hWnd, hdc );
			}
		}
	} //SVMFCControls
} //Seidenader

