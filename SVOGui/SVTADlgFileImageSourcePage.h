//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVToolAdjustmentDialogFileImageSourcePage
//* .File Name       : $Workfile:   SVTADlgFileImageSourcePage.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   26 Jun 2014 18:21:18  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVOResource/resource.h"
#include "SVOMFCLibrary/SVFileNameClass.h"
//TODO: MZA(10.Nov 2014): Move this files to SVOGui project and then remove folder from include and Namespace add-on add PictureDisplay declaration.
#include "SVOGui/PictureDisplay.h"
#include "SVOGui/ImageController.h"
#include "SVOGui/GuiController.h"
#include "SVOGui/ValuesAccessor.h"
#include "SVOGui/BoundValue.h"
#pragma endregion Includes

namespace Seidenader
{
	namespace SVOGui
	{
		class SVToolAdjustmentDialogFileImageSourcePageClass : public CPropertyPage
		{
			typedef SvOg::ValuesAccessor<SvOg::BoundValues> Values;
			typedef SvOg::GuiController<Values, Values::value_type> ValueController;

		#pragma region Constructor
		public:
			SVToolAdjustmentDialogFileImageSourcePageClass(const SVGUID& rInspectionID, const SVGUID& rTaskObjectID);
			virtual ~SVToolAdjustmentDialogFileImageSourcePageClass();
		#pragma endregion Constructor

		#pragma region Protected Methods
		protected:
			//{{AFX_MSG(SVToolAdjustmentDialogFileImageSourcePageClass)
			afx_msg void OnBrowseButton();
			afx_msg void OnReloadCheck();
			//}}AFX_MSG
			DECLARE_MESSAGE_MAP()

			//{{AFX_VIRTUAL(SVToolAdjustmentDialogFileImageSourcePageClass)
			protected:
			virtual BOOL OnInitDialog() override;
			virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV-Unterstützung
			//}}AFX_VIRTUAL

		protected:
			void GetData();
			void SetData();
			HRESULT SetInspectionData();
			void refresh();
			void setImages();
		#pragma endregion Protected Methods

		#pragma region Member variables
		private:
			SVFileNameClass m_svfncImageSourceFile;
			SvOg::ImageController m_ImageController;
			ValueController m_Values;
			//{{AFX_DATA(SVToolAdjustmentDialogFileImageSourcePageClass)
			enum { IDD = IDD_TA_FILE_IMAGE_DIALOG };
			SvOg::PictureDisplay m_imageCtrl;
			CString	m_StrPathName;
			BOOL m_BContinuousReload;
			//}}AFX_DATA
		#pragma endregion Member variables
		};
	}
}

namespace SvOg = Seidenader::SVOGui;

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

