//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVDlgBitmapClass
//* .File Name       : $Workfile:   SVDlgBitmap.H  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 10:14:56  $
//******************************************************************************

#pragma once

#include "SVContainerLibrary/SVVector.h"

namespace Seidenader
{
	namespace SVMFCControls
	{
		struct SVDlgBitmapTextStruct
		{
			SVDlgBitmapTextStruct()
			{
				BAbsCoord = TRUE;
				X = 0.0;
				Y = 0.0;
				Color = 0;
				StrText = _T( "" );
			};

			BOOL BAbsCoord;
			double X;
			double Y;
			COLORREF Color;
			CString StrText;
		};


		class SVDlgBitmapTextListClass : public SVVector< SVDlgBitmapTextStruct* >
		{
		public:
			SVDlgBitmapTextListClass() {};

			virtual ~SVDlgBitmapTextListClass() 
			{ 
				for( int i = GetSize() - 1; i >= 0; --i )
					if( GetAt( i ) )
						delete( GetAt( i ) );

				RemoveAll();
			}
		};


		class SVDlgBitmapClass : public CStatic
		{
		protected:
			//{{AFX_MSG(SVDlgBitmapClass)
			afx_msg void OnPaint();
			afx_msg void OnPaletteIsChanging(CWnd* pRealizeWnd);
			//}}AFX_MSG

			DECLARE_MESSAGE_MAP()

		public:
			SVDlgBitmapClass();
			virtual ~SVDlgBitmapClass();

			BOOL Init( LPCTSTR lpszResourceName );
			BOOL Init( UINT nIDResource );

			CSize GetDimensions();

			BOOL SetWindowSize( int cx = -1, int cy = -1 );
			CSize GetWindowSize();

			BOOL TextOut( int X, int Y, COLORREF Color, LPCTSTR StrText );
			BOOL TextOut( double X, double Y, COLORREF Color, LPCTSTR StrText );

		public:
			BOOL OEMMode;

		protected:
			HBITMAP InitFromFile();

			//{{AFX_VIRTUAL(SVDlgBitmapClass)
			//}}AFX_VIRTUAL	

		protected:
			CBitmap		bitmap;
			SVDlgBitmapTextListClass textList;

		};
	} //SVMFCControls
} //Seidenader

namespace SvMc = Seidenader::SVMFCControls;

