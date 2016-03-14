//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVDlgGraph
//* .File Name       : $Workfile:   SVDlgGraph.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 10:15:28  $
//******************************************************************************

#pragma once

#include "SVContainerLibrary\SVObjectArrayClassTemplate.h"

namespace Seidenader
{
	namespace SVOMFCLibrary
	{
		// Use this to define a function which is called,
		// while somebody pushes the left mouse button.
		typedef BOOL ( CALLBACK* SVProcMousePointFunc ) ( POINT, LPVOID );

		// Normalize Modes...
		enum SVNormalizeModeEnum
		{
			SVNormalize1D,	// Normalize x and y separately
			SVNormalize2D,	// Normalize x and y together ( square )
			SVNormalizeXYMinMax,// Normalize on specified X and Y max values
		};


		class SVDlgGraphClass : public CStatic
		{
		public:
			SVDlgGraphClass();

			virtual ~SVDlgGraphClass();

			BOOL SetColor(COLORREF rgb, BOOL bRedraw=TRUE);

			virtual BOOL SetPoints( const SvCl::SVObjectByteArrayClass& RByteVec );
			virtual BOOL SetPoints( const SvCl::SVObjectLongArrayClass& RLongVec );
			virtual BOOL SetPoints( const SvCl::SVObjectDoubleArrayClass& RDoubleVec );
			virtual BOOL SetPoints( const SvCl::SVObjectCPointArrayClass& RPointVec );

			BOOL SetMousePointProcFunc( SVProcMousePointFunc PFunc, LPVOID PUserData );	
			void SetNormalizeMode( SVNormalizeModeEnum NormalizeMode );
			void SetXYMinMax(long lXMin, long lYMin, long lXMax, long lYMax);

			BOOL Normalize();
			void Retransform( CPoint& RPoint );


			//{{AFX_VIRTUAL(SVDlgGraphClass)
			//}}AFX_VIRTUAL

		protected:
			//{{AFX_MSG(SVDlgGraphClass)
			afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
			afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
			afx_msg void OnMouseMove(UINT nFlags, CPoint point);
			afx_msg void OnPaint();
			//}}AFX_MSG

			DECLARE_MESSAGE_MAP()

		protected:

			SvCl::SVObjectCPointArrayClass pointVec;
			SVProcMousePointFunc pProcFunc;
			LPVOID pUserData;

			double xMult;
			double yMult;
			long xDisp;
			long yDisp;
			long yClient;
			long m_lXMin;
			long m_lYMin;
			long m_lXMax;
			long m_lYMax;

			COLORREF mrgbGraphColor;

			BOOL bButtonDown;

			SVNormalizeModeEnum normalizeMode;
		};
	} //SVOMFCLibrary
} //Seidenader

namespace SvOml = Seidenader::SVOMFCLibrary;

