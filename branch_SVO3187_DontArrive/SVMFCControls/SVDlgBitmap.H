//******************************************************************************
//* COPYRIGHT (c) 2003 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVDlgBitmapClass
//* .File Name       : $Workfile:   SVDlgBitmap.H  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 10:14:56  $
//******************************************************************************

#pragma once

//Moved to precompiled header: #include <vector>

namespace SvMc
{
	struct SVDlgBitmapTextStruct
	{
		SVDlgBitmapTextStruct()
		{
			AbsoluteCoord = true;
			X = 0.0;
			Y = 0.0;
			m_Color = 0;
			m_Text = _T( "" );
		};

		bool AbsoluteCoord;
		double X;
		double Y;
		COLORREF m_Color;
		std::string m_Text;
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

		CSize GetDimensions() const;

		BOOL TextOut( int X, int Y, COLORREF Color, LPCTSTR StrText );
		BOOL TextOut( double X, double Y, COLORREF Color, LPCTSTR StrText );

	public:
		BOOL OEMMode;

	protected:
		HBITMAP InitFromFile();

		//{{AFX_VIRTUAL(SVDlgBitmapClass)
		//}}AFX_VIRTUAL	

	protected:
		mutable CBitmap	bitmap;
		std::vector<SVDlgBitmapTextStruct> m_TextVector;

	};
} //namespace SvMc
