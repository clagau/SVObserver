//******************************************************************************
//* COPYRIGHT (c) 2003 by Körber Pharma Inspection GmbH. All Rights Reserved
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

namespace SvMc
{
	// Use this to define a function which is called,
	// while somebody pushes the left mouse button.
	typedef BOOL(CALLBACK* SVProcMousePointFunc) (POINT, LPVOID);

	// Normalize Modes...
	enum SVNormalizeModeEnum
	{
		SVNormalize1D,	// Normalize x and y separately
		SVNormalize2D,	// Normalize x and y together ( square )
		SVNormalizeXYMinMax,// Normalize on specified X and Y max values
	};


	class SVDlgGraph : public CStatic
	{
	public:
		SVDlgGraph() = default;

		virtual ~SVDlgGraph() = default;

		BOOL SetColor(COLORREF rgb, BOOL bRedraw = TRUE);

		BOOL SetPoints(const SvCl::SVObjectByteArrayClass& rByteVec);
		BOOL SetPoints(const SvCl::SVObjectLongArrayClass& rLongVec);
		BOOL SetPoints(const SvCl::SVObjectDoubleArrayClass& rDoubleVec);
		BOOL SetPoints(const SvCl::SVObjectPointArrayClass& rPointVec);

		BOOL SetMousePointProcFunc(SVProcMousePointFunc PFunc, LPVOID PUserData);
		void SetNormalizeMode(SVNormalizeModeEnum NormalizeMode);
		void SetXYMinMax(long lXMin, long lYMin, long lXMax, long lYMax);

		BOOL Normalize();
		void Retransform(CPoint& RPoint);


		//{{AFX_VIRTUAL(SVDlgGraph)
		//}}AFX_VIRTUAL

	protected:
		//{{AFX_MSG(SVDlgGraph)
		afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
		afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
		afx_msg void OnMouseMove(UINT nFlags, CPoint point);
		afx_msg void OnPaint();
		//}}AFX_MSG

		DECLARE_MESSAGE_MAP()

	protected:

		SvCl::SVObjectPointArrayClass m_PointVec {};
		SVProcMousePointFunc pProcFunc {nullptr};
		LPVOID pUserData {nullptr};

		double xMult {0.0};
		double yMult {0.0};
		long xDisp {0L};
		long yDisp {0L};
		long yClient {0L};
		long m_lXMin {0L};
		long m_lYMin {0L};
		long m_lXMax {0L};
		long m_lYMax {0L};

		COLORREF mrgbGraphColor {RGB(0, 0, 0)};

		BOOL bButtonDown {false};

		SVNormalizeModeEnum normalizeMode {SVNormalizeModeEnum::SVNormalize1D};
	};
} //namespace SvMc
