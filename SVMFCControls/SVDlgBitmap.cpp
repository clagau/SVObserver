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
#include "SVStatusLibrary/ErrorNumbers.h"
#include "SVStatusLibrary\MessageManager.h"
#include "SVMessage\SVMessage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace SvMc
{
	BEGIN_MESSAGE_MAP(SVDlgBitmapClass, CStatic)
		//{{AFX_MSG_MAP(SVDlgBitmapClass)
		ON_WM_PAINT()
		ON_WM_PALETTEISCHANGING()
		//}}AFX_MSG_MAP
	END_MESSAGE_MAP()

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
		SVDlgBitmapTextStruct BitmapText;
		BitmapText.AbsoluteCoord = true;
		BitmapText.X = static_cast<double> (X);
		BitmapText.Y = static_cast<double> (Y);
		BitmapText.m_Color = Color;
		BitmapText.m_Text = StrText;
		m_TextVector.push_back(BitmapText);

		return true;
	}

	BOOL SVDlgBitmapClass::TextOut( double X, double Y, COLORREF Color, LPCTSTR StrText )
	{
		if( X >= 0 && X <= 1 && Y >= 0 && Y <= 1 )
		{
			SVDlgBitmapTextStruct BitmapText;
			BitmapText.AbsoluteCoord = false;
			// If BAbsCoord is false, the X and Y values will be interpreted 
			// as weights of the client area to generate the origin of the text !!!
			BitmapText.X = X;
			BitmapText.Y = Y;
			BitmapText.m_Color = Color;
			BitmapText.m_Text = StrText;
			m_TextVector.push_back(BitmapText);
			
			return true;
		}
		return false;
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
			WORD colNum = static_cast<WORD> (1 << bmInfo.bmBitsPixel);

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

			for (auto const& rText : m_TextVector)
			{
				if (!rText.m_Text.empty())
				{
					dc.SetBkMode(TRANSPARENT);
					dc.SetTextColor(rText.m_Color);
					if (rText.AbsoluteCoord)
					{
						dc.TextOut((int)rText.X, (int)rText.Y, rText.m_Text.c_str());
					}
					else
					{
						dc.TextOut((int)(rText.X * rect.Width()), (int)(rText.Y * rect.Height()), rText.m_Text.c_str());
					}
				}
			}
			ReleaseDC( &dc );
			delete pPal;
		}
		else
		{
			SvStl::MessageMgrStd Msg(SvStl::MsgType::Log | SvStl::MsgType::Display );
			Msg.setMessage( SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_Error_NoDeviceContext, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_10233 );
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
} //namespace SvMc
