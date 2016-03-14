//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVToolAdjustmentDialogImagePageClass
//* .File Name       : $Workfile:   SVToolAdjustmentDialogImagePageClass.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   26 Jun 2014 18:29:26  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVOGui\PictureDisplay.h"
#include "SVMFCControls\AvailableObjectListComboBox.h"
#include "SVOGui\ImageController.h"
#include "SVUtilityLibrary\SVGUID.h"
#pragma endregion Includes

namespace Seidenader
{
	namespace SVOGui
	{
		class SVToolAdjustmentDialogImagePageClass : public CPropertyPage, protected ImageController
		{
		public:
			SVToolAdjustmentDialogImagePageClass(const SVGUID & rInspectionID, const SVGUID& rTaskObjectID, int id = IDD );
			virtual ~SVToolAdjustmentDialogImagePageClass();
			HRESULT SetInspectionData();

		protected:
			void refresh();

			//{{AFX_MSG(SVToolAdjustmentDialogImagePageClass)
			virtual BOOL OnInitDialog();
			afx_msg void OnSelchangeCombo1();
			//}}AFX_MSG

			DECLARE_MESSAGE_MAP()

			//{{AFX_VIRTUAL(SVToolAdjustmentDialogImagePageClass)
			virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV
			//}}AFX_VIRTUAL

			//{{AFX_DATA(SVToolAdjustmentDialogImagePageClass)
			enum { IDD = IDD_TA_IMAGE_DIALOG };
			SvMc::AvailableObjectListComboBox m_availableSourceImageListBox;
			PictureDisplay m_dialogImage;
			//}}AFX_DATA
	
			SVGUID m_InspectionID;
			SVGUID m_TaskObjectID;
			SVString m_inputName;
		};
	}
}

namespace SvOg = Seidenader::SVOGui;

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

