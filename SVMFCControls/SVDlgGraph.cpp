//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVDlgGraph
//* .File Name       : $Workfile:   SVDlgGraph.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 10:15:20  $
//******************************************************************************
#pragma region Includes
#include "stdafx.h"
#include "Definitions/SVUserMessage.h"
#include "SVDlgGraph.h"
#pragma endregion Includes

//******************************************************************************
//* DEFINITIONS OF MODULE-LOCAL VARIABLES:
//******************************************************************************

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace SvMc
{
	SVDlgGraph::SVDlgGraph()
	{
		pProcFunc = nullptr;
		pUserData = nullptr;
		xMult = 0.0;
		yMult = 0.0;
		xDisp = 0;
		yDisp = 0;
		yClient = 0;
		bButtonDown = FALSE;
		normalizeMode = SVNormalize1D;
		mrgbGraphColor = RGB(0,0,0);	// default black
		m_lXMax = 0;
		m_lYMax = 0;
		m_lXMin = 0;
		m_lYMin = 0;
	}

	SVDlgGraph::~SVDlgGraph()
	{
	}


	BEGIN_MESSAGE_MAP(SVDlgGraph, CStatic)
		//{{AFX_MSG_MAP(SVDlgGraph)
		ON_WM_LBUTTONDOWN()
		ON_WM_LBUTTONUP()
		ON_WM_MOUSEMOVE()
		ON_WM_PAINT()
		//}}AFX_MSG_MAP
	END_MESSAGE_MAP()

	BOOL SVDlgGraph::SetPoints( const SvCl::SVObjectByteArrayClass& rByteVec ) 
	{
		int size = static_cast<int> (rByteVec.size());
		m_PointVec.resize( size );
		for( int i = 0; i < size; ++ i )
		{
			POINT& rPoint = m_PointVec[i];
			rPoint.x = i;
			rPoint.y = rByteVec[i];
		}

		return( Normalize() && RedrawWindow() );
	}

	BOOL SVDlgGraph::SetPoints( const SvCl::SVObjectLongArrayClass& rLongVec ) 
	{
		int size = static_cast<int> (rLongVec.size());
		m_PointVec.resize( size );
		for( int i = 0; i < size; ++ i )
		{
			POINT& rPoint = m_PointVec[i];
			rPoint.x = i;
			rPoint.y = rLongVec[i];
		}

		return( Normalize() && RedrawWindow() );
	}

	BOOL SVDlgGraph::SetPoints( const SvCl::SVObjectDoubleArrayClass& rDoubleVec ) 
	{
		int size = static_cast<int> (rDoubleVec.size());
		m_PointVec.resize( size );
		for( int i = 0; i < size; ++ i )
		{
			POINT& rPoint = m_PointVec[i];
			rPoint.x = i;
			rPoint.y = static_cast<long> (rDoubleVec[i]);
		}

		return( Normalize() && RedrawWindow() );
	}

	BOOL SVDlgGraph::SetPoints( const SvCl::SVObjectPointArrayClass& rPointVec ) 
	{
		m_PointVec = rPointVec;

		return( Normalize() && RedrawWindow() );
	}

	BOOL SVDlgGraph::SetMousePointProcFunc( SVProcMousePointFunc PFunc, LPVOID PUserData )
	{
		pProcFunc = PFunc;
		pUserData = PUserData;

		return TRUE;
	}

	void SVDlgGraph::SetNormalizeMode( SVNormalizeModeEnum NormalizeMode ) 
	{
		normalizeMode = NormalizeMode;
	}

	BOOL SVDlgGraph::Normalize() 
	{
		if( ::IsWindow( m_hWnd ) )
		{
			CRect clientRect;
			GetClientRect( &clientRect );
			long xMax = 0;
			long xMin = 0;
			long yMax = 0;
			long yMin = 0;
			int size = static_cast<int> (m_PointVec.size());

			int i;

			if ( normalizeMode == SVNormalizeXYMinMax )
			{
				xMax = m_lXMax;
				xMin = m_lXMin;
				yMax = m_lYMax;
				yMin = m_lYMin;
			}
			else
			{
				// Get extremes...
				for(i = 0; i < size; ++ i )
				{
					POINT& rPoint = m_PointVec[ i ];
					xMax = __max( xMax, rPoint.x );
					xMin = __min( xMin, rPoint.x );
					yMax = __max( yMax, rPoint.y );
					yMin = __min( yMin, rPoint.y );
				}
			}

			// Get Displacement...
			xDisp = - xMin;
			yDisp = - yMin;

			if( normalizeMode == SVNormalize2D )
			{
				// Make a square of extremes...
				xMax = yMax = __max( xMax, yMax );
				xMin = yMin = __min( xMin, yMin );
			}

			// Calc x multiplier...
			xMult = clientRect.Width();
			if( xMax - xMin != 0 )
				xMult /= xMax - xMin;
			else
				xMult = 0;

			// Calc y multiplier...
			yMult = clientRect.Height();
			if( yMax - yMin != 0 )
				yMult /= yMax - yMin;
			else
				yMult = 0;


			yClient = clientRect.Height();

			// Normalize...
			for( i = 0; i < size; ++ i )
			{
				POINT& rPoint = m_PointVec[ i ];
				rPoint.x = ( long ) ( ( double ) ( rPoint.x + xDisp ) * xMult );
				rPoint.y = yClient - ( long ) ( ( double ) ( rPoint.y + yDisp ) * yMult );
			}

			return TRUE;
		}
		return FALSE;
	}

	void SVDlgGraph::Retransform( CPoint& RPoint ) 
	{
		if( xMult != 0.0 )
			RPoint.x = ( long ) ( ( double ) RPoint.x / xMult ) - xDisp;

		if( yMult != 0.0 )
			RPoint.y = ( long ) ( ( double ) ( yClient - RPoint.y ) / yMult ) - yDisp;
	}

	/////////////////////////////////////////////////////////////////////////////
	// SVDlgGraph message handlers

	void SVDlgGraph::OnLButtonDown(UINT , CPoint point) 
	{
		bButtonDown = TRUE;
		if( pProcFunc )
		{
			// Capture mouse...
			SetCapture();

			// Retransform mouse point...
			Retransform( point );

			// Call mouse point proc...
			if( pProcFunc( point, pUserData ) )
			{
				if( GetParent() )
					GetParent()->PostMessage( SV_REFRESH_DIALOG );
			}
		}
	}

	void SVDlgGraph::OnLButtonUp(UINT , CPoint ) 
	{
		bButtonDown = FALSE;

		// Release mouse...
		if( GetCapture() == this )
			ReleaseCapture();
	}

	void SVDlgGraph::OnMouseMove(UINT , CPoint point) 
	{
		if( bButtonDown && pProcFunc )
		{
			// Retransform mouse point...
			Retransform( point );

			// Call mouse point proc...
			if( pProcFunc( point, pUserData ) )
				GetParent()->PostMessage( SV_REFRESH_DIALOG );
		}
	}

	void SVDlgGraph::OnPaint() 
	{
		CPaintDC dc(this); // device context for painting

		CRect clientRect;
		GetClientRect( &clientRect );

		// White background...
		dc.BitBlt( 
			0, 
			0,
			clientRect.Width(),
			clientRect.Height(),
			nullptr,
			0,
			0,
			WHITENESS
			);

		// Draw black graph...
		if( 0 < m_PointVec.size() )
		{
			CPen pen;
			pen.CreatePen( PS_SOLID, 1, mrgbGraphColor );
			CPen* pOldPen = dc.SelectObject( &pen );

			dc.Polyline( m_PointVec.data(), static_cast<int> (m_PointVec.size()) );

			if( dc.SelectObject( pOldPen ) )
			{
				pen.DeleteObject();
			}
		}

		// Do not call CStatic::OnPaint() for painting messages
	}

	BOOL SVDlgGraph::SetColor(COLORREF rgb, BOOL bRedraw)
	{
		mrgbGraphColor = rgb;
		if ( bRedraw )
			RedrawWindow();
		return TRUE;
	}

	void SVDlgGraph::SetXYMinMax(long lXMin, long lYMin, long lXMax, long lYMax)
	{
		m_lXMin = lXMin;
		m_lYMin = lYMin;
		m_lXMax = lXMax;
		m_lYMax = lYMax;
	}
} //namespace SvMc
