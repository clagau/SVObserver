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
#include "SVOResource/resource.h"
#include "PictureDisplay.h"
#include "SVMFCControls\AvailableObjectListComboBox.h"
#include "ImageController.h"
#include "ToolSizeController.h"
#pragma endregion Includes


namespace SvOg
{
	class SVToolAdjustmentDialogImagePageClass : public CPropertyPage
	{
	public:
		SVToolAdjustmentDialogImagePageClass(uint32_t inspectionId, uint32_t taskObjectId, SvPb::SVObjectSubTypeEnum SubType = SvPb::SVImageMonoType, int id = IDD );
		virtual ~SVToolAdjustmentDialogImagePageClass();
		HRESULT SetInspectionData();

	protected:
		void refresh();
		ImageController& getImageController() { return m_ImageController; }

		//{{AFX_MSG(SVToolAdjustmentDialogImagePageClass)
		virtual BOOL OnInitDialog() override;
		afx_msg void OnSelchangeCombo1();
		//}}AFX_MSG

		DECLARE_MESSAGE_MAP()

		//{{AFX_VIRTUAL(SVToolAdjustmentDialogImagePageClass)
		virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV
		//}}AFX_VIRTUAL

		//{{AFX_DATA(SVToolAdjustmentDialogImagePageClass)
		enum { IDD = IDD_TA_IMAGE_DIALOG };
		SvMc::AvailableObjectListComboBox<uint32_t> m_availableSourceImageListBox;
		PictureDisplay m_dialogImage;
		//}}AFX_DATA
	
	private:
		std::string m_inputName;
		
		ImageController m_ImageController;
		ToolSizeController m_ToolsizeHelper;
	};
} //namespace SvOg
