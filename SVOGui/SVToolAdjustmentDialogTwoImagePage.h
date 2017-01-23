//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVToolAdjustmentDialogTwoImagePage
//* .File Name       : $Workfile:   SVToolAdjustmentDialogTwoImagePage.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   26 Jun 2014 18:29:24  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVOResource/resource.h"
#include "SVMFCControls\AvailableObjectListComboBox.h"
//TODO: MZA(10.Nov 2014): Move this files to SVOGui project and then remove folder from include and Namespace add-on add PictureDisplay declaration.
#include "PictureDisplay.h"
#include "ImageController.h"
#include "BoundValue.h"
#include "ValuesAccessor.h"
#include "GuiController.h"
#pragma endregion Includes

namespace Seidenader
{
	namespace SVOGui
	{
		class SVToolAdjustmentDialogTwoImagePageClass : public CPropertyPage, protected SvOg::ImageController
		{
			DECLARE_MESSAGE_MAP()

			typedef SvOg::ValuesAccessor<SvOg::BoundValues> Values;
			typedef SvOg::GuiController<Values, Values::value_type> ValueController;

		#pragma region Constructor
		public:
			SVToolAdjustmentDialogTwoImagePageClass(const SVGUID& rInspectionID, const SVGUID& rTaskObjectID);
			virtual ~SVToolAdjustmentDialogTwoImagePageClass();
		#pragma endregion Constructor

		#pragma region Protected Methods
		//******************************************************************************
		// Class Wizard Generated Virtual Function(s):
		//******************************************************************************
			//{{AFX_VIRTUAL(SVToolAdjustmentDialogTwoImagePageClass)
			protected:
			virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV
			//}}AFX_VIRTUAL

		//******************************************************************************
		// Class Wizard Generated Message Map Entries:
		//******************************************************************************
			// Generated message map functions
		protected:
			//{{AFX_MSG(SVToolAdjustmentDialogTwoImagePageClass)
			virtual BOOL OnInitDialog() override;
			afx_msg void OnSelchangeCombo1();
			afx_msg void OnSelChangeCombo2();
			afx_msg void OnSelchangeOperatorCombo();
			//}}AFX_MSG
		#pragma endregion Protected Methods

		#pragma region Private Methods
		private:
			HRESULT SetInspectionData();
			void refresh();
			void setImages();
			void RetreiveCurrentlySelectedImageNames();
			void RetreiveResultImageNames();
		#pragma endregion Private Methods

		#pragma region Member variables
		protected:
			//{{AFX_DATA(SVToolAdjustmentDialogTwoImagePageClass)
			enum { IDD = IDD_TA_TWO_IMAGE_DIALOG };
			CComboBox m_operatorCtrl;
			SvOg::PictureDisplay m_secondImageCtrl;
			SvOg::PictureDisplay m_firstImageCtrl;
			SvMc::AvailableObjectListComboBox m_firstAvailableSourceImageListBoxCtl;
			SvMc::AvailableObjectListComboBox m_secondAvailableSourceImageListBoxCtl;
			//}}AFX_DATA

		private:
			SVString m_firstInputName;
			SVString m_firstImageName;
			SVString m_secondInputName;
			SVString m_secondImageName;
			SVString m_resultImageName;
			SVGUID m_resultImageID;

			SVGUID m_InspectionID;
			SVGUID m_TaskObjectID;

			ValueController m_Values;
		#pragma endregion Member variables
		};
	}
}

namespace SvOg = Seidenader::SVOGui;

