//******************************************************************************
//* COPYRIGHT (c) 2003 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : TADlgFilterPageClass
//* .File Name       : $Workfile:   TADlgFilterPageClass.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   26 Jun 2014 18:29:26  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVOResource/resource.h"
#include "SVOGuiUtility/ImageController.h"
#include "SVOGuiUtility/PictureDisplay.h"
#include "SVMFCControls/AvailableObjectListComboBox.h"
#include "SVMFCControls/ObjectsListBox.h"
#pragma endregion Includes

namespace SvOg
{
	class TADlgFilterPageClass : public CPropertyPage
	{
		//******************************************************************************
		// Class Wizard Generated Message Map Entries:
		//******************************************************************************
		// Generated message map functions
	protected:
		//{{AFX_MSG(TADlgFilterPageClass)
		virtual BOOL OnInitDialog() override;
		afx_msg void OnButtonProperties();
		afx_msg void OnButtonClearAll();
		afx_msg void OnButtonDeleteCurrentFilter();
		afx_msg void OnButtonInsertNewFilter();
		afx_msg void OnSelchangeList1();
		//}}AFX_MSG

		DECLARE_MESSAGE_MAP()

	public:
		TADlgFilterPageClass(uint32_t inspectionID, uint32_t taskObjectID);
		virtual ~TADlgFilterPageClass();

		//************************************
		// Method:    setImages
		// Description: Set image to the activeX-control. 
		//************************************
		void setImages();

	protected:
		HRESULT SetInspectionData();
		void refresh();

		//******************************************************************************
		// Class Wizard Generated Virtual Function(s):
		//******************************************************************************
		//{{AFX_VIRTUAL(TADlgFilterPageClass)
	protected:
		virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV-Unterstützung
		//}}AFX_VIRTUAL

		//************************************
		// Method:    OnSetActive
		// Description: is called when propertypage get active. set the image 
		// Access:    public 
		// Returns:   BOOL 
		//************************************
		virtual BOOL OnSetActive() override;

		//******************************************************************************
		// Class Wizard Generated Dialog Data:
		//******************************************************************************
	protected:
		//{{AFX_DATA(TADlgFilterPageClass)
		enum { IDD = IDD_TA_FILTER_DIALOG };
		CButton	m_btnProperties;
		CButton	insertFilter;
		SvMc::ObjectsListBox m_filterListBox;
		SvOgu::PictureDisplay	dialogImage;
		SvMc::AvailableObjectListComboBox<SvPb::ClassIdEnum> m_availableFilterCB;
		//}}AFX_DATA

		//******************************************************************************
		// Data Element(s):
		//******************************************************************************
	protected:
		const uint32_t m_InspectionID;
		const uint32_t m_TaskObjectID;
		uint32_t m_UnaryImageOperatorID;
		SvOgu::ImageController m_ImageController;
	};
} //namespace SvOg
