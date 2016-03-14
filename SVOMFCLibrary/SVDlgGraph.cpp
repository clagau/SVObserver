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

//******************************************************************************
//* INCLUDE(S):
//******************************************************************************

////////////////////////////////////////////////////////////////////////////////
// General Include File(s)
////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ObjectInterfaces/SVUserMessage.h"
#include "SVDlgGraph.h"

//******************************************************************************
//* DEFINITIONS OF MODULE-LOCAL VARIABLES:
//******************************************************************************

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace Seidenader
{
	namespace SVOMFCLibrary
	{
		SVDlgGraphClass::SVDlgGraphClass()
		{
			pProcFunc = NULL;
			pUserData = NULL;
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
		}

		SVDlgGraphClass::~SVDlgGraphClass()
		{
		}


		BEGIN_MESSAGE_MAP(SVDlgGraphClass, CStatic)
			//{{AFX_MSG_MAP(SVDlgGraphClass)
			ON_WM_LBUTTONDOWN()
			ON_WM_LBUTTONUP()
			ON_WM_MOUSEMOVE()
			ON_WM_PAINT()
			//}}AFX_MSG_MAP
		END_MESSAGE_MAP()

		BOOL SVDlgGraphClass::SetPoints( const SvCl::SVObjectByteArrayClass& RByteVec ) 
		{
			int size = RByteVec.GetSize();
			pointVec.SetSize( size );
			for( int i = 0; i < size; ++ i )
			{
				CPoint& rPoint = pointVec[ i ];
				rPoint.y = RByteVec[ i ];
				rPoint.x = i;
			}

			return( Normalize() && RedrawWindow() );
		}

		BOOL SVDlgGraphClass::SetPoints( const SvCl::SVObjectLongArrayClass& RLongVec ) 
		{
			int size = RLongVec.GetSize();
			pointVec.SetSize( size );
			for( int i = 0; i < size; ++ i )
			{
				CPoint& rPoint = pointVec[ i ];
				rPoint.x = i;
				rPoint.y = RLongVec[ i ];
			}

			return( Normalize() && RedrawWindow() );
		}

		BOOL SVDlgGraphClass::SetPoints( const SvCl::SVObjectDoubleArrayClass& RDoubleVec ) 
		{
			int size = RDoubleVec.GetSize();
			pointVec.SetSize( size );
			for( int i = 0; i < size; ++ i )
			{
				CPoint& rPoint = pointVec[ i ];
				rPoint.x = i;
				rPoint.y = (long)(RDoubleVec[ i ]);
			}

			return( Normalize() && RedrawWindow() );
		}

		BOOL SVDlgGraphClass::SetPoints( const SvCl::SVObjectCPointArrayClass& RPointVec ) 
		{
			pointVec.Copy( RPointVec );

			return( Normalize() && RedrawWindow() );
		}

		BOOL SVDlgGraphClass::SetMousePointProcFunc( SVProcMousePointFunc PFunc, LPVOID PUserData )
		{
			pProcFunc = PFunc;
			pUserData = PUserData;

			return TRUE;
		}

		void SVDlgGraphClass::SetNormalizeMode( SVNormalizeModeEnum NormalizeMode ) 
		{
			normalizeMode = NormalizeMode;
		}

		BOOL SVDlgGraphClass::Normalize() 
		{
			if( ::IsWindow( m_hWnd ) )
			{
				CRect clientRect;
				GetClientRect( &clientRect );
				long xMax = 0;
				long xMin = 0;
				long yMax = 0;
				long yMin = 0;
				int size = pointVec.GetSize();

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
						CPoint& rPoint = pointVec[ i ];
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
					CPoint& rPoint = pointVec[ i ];
					rPoint.x = ( long ) ( ( double ) ( rPoint.x + xDisp ) * xMult );
					rPoint.y = yClient - ( long ) ( ( double ) ( rPoint.y + yDisp ) * yMult );
				}

				return TRUE;
			}
			return FALSE;
		}

		void SVDlgGraphClass::Retransform( CPoint& RPoint ) 
		{
			if( xMult != 0.0 )
				RPoint.x = ( long ) ( ( double ) RPoint.x / xMult ) - xDisp;

			if( yMult != 0.0 )
				RPoint.y = ( long ) ( ( double ) ( yClient - RPoint.y ) / yMult ) - yDisp;
		}

		/////////////////////////////////////////////////////////////////////////////
		// SVDlgGraphClass message handlers

		void SVDlgGraphClass::OnLButtonDown(UINT nFlags, CPoint point) 
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

		void SVDlgGraphClass::OnLButtonUp(UINT nFlags, CPoint point) 
		{
			bButtonDown = FALSE;

			// Release mouse...
			if( GetCapture() == this )
				ReleaseCapture();
		}

		void SVDlgGraphClass::OnMouseMove(UINT nFlags, CPoint point) 
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

		void SVDlgGraphClass::OnPaint() 
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
				NULL,
				0,
				0,
				WHITENESS
				);

			// Draw black graph...
			if( pointVec.GetSize() > 0 )
			{
				CPen pen;
				pen.CreatePen( PS_SOLID, 1, mrgbGraphColor );
				CPen* pOldPen = dc.SelectObject( &pen );

				dc.Polyline( pointVec.GetData(), pointVec.GetSize() );

				if( dc.SelectObject( pOldPen ) )
				{
					pen.DeleteObject();
				}
			}

			// Do not call CStatic::OnPaint() for painting messages
		}

		BOOL SVDlgGraphClass::SetColor(COLORREF rgb, BOOL bRedraw)
		{
			mrgbGraphColor = rgb;
			if ( bRedraw )
				RedrawWindow();
			return TRUE;
		}

		void SVDlgGraphClass::SetXYMinMax(long lXMin, long lYMin, long lXMax, long lYMax)
		{
			m_lXMin = lXMin;
			m_lYMin = lYMin;
			m_lXMax = lXMax;
			m_lYMax = lYMax;
		}
	} //SVOMFCLibrary
} //Seidenader

