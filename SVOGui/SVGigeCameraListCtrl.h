//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVGigeCameraListCtrl
//* .File Name       : $Workfile:   SVGigeCameraListCtrl.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   12 Jun 2013 15:48:08  $
//******************************************************************************
#pragma once

namespace Seidenader
{ 
	namespace SVOGui
	{
		class SVGigeCameraListCtrl : public CListCtrl
		{
		public:
			SVGigeCameraListCtrl();
			virtual ~SVGigeCameraListCtrl();

			// Overrides
			// ClassWizard generated virtual function overrides
			//{{AFX_VIRTUAL(SVGigeCameraListCtrl)
		public:
			virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
			//}}AFX_VIRTUAL


			// Generated message map functions
		protected:
			//{{AFX_MSG(SVGigeCameraListCtrl)
			// NOTE - the ClassWizard will add and remove member functions here.
			//}}AFX_MSG

			DECLARE_MESSAGE_MAP()
		};
	}
}

namespace SvOg = Seidenader::SVOGui;
