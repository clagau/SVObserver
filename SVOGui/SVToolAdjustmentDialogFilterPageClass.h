//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVToolAdjustmentDialogFilterPageClass
//* .File Name       : $Workfile:   SVToolAdjustmentDialogFilterPageClass.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   26 Jun 2014 18:29:26  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVOResource/resource.h"
#include "PictureDisplay.h"
#include "SVMFCControls\AvailableObjectListComboBox.h"
#include "ImageController.h"
#include "SVMFCControls\ObjectsListBox.h"
#pragma endregion Includes

namespace SvOg
{
	class SVToolAdjustmentDialogFilterPageClass : public CPropertyPage
	{
		//******************************************************************************
		// Class Wizard Generated Message Map Entries:
		//******************************************************************************
		// Generated message map functions
	protected:
		//{{AFX_MSG(SVToolAdjustmentDialogFilterPageClass)
		virtual BOOL OnInitDialog() override;
		afx_msg void OnButtonProperties();
		afx_msg void OnButtonClearAll();
		afx_msg void OnButtonDeleteCurrentFilter();
		afx_msg void OnButtonInsertNewFilter();
		afx_msg void OnSelchangeList1();
		//}}AFX_MSG

		DECLARE_MESSAGE_MAP()

	public:
		SVToolAdjustmentDialogFilterPageClass(const SVGUID& rInspectionID, const SVGUID& rTaskObjectID);
		virtual ~SVToolAdjustmentDialogFilterPageClass();

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
		//{{AFX_VIRTUAL(SVToolAdjustmentDialogFilterPageClass)
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
		//{{AFX_DATA(SVToolAdjustmentDialogFilterPageClass)
		enum { IDD = IDD_TA_FILTER_DIALOG };
		CButton	m_btnProperties;
		CButton	insertFilter;
		SvMc::ObjectsListBox m_filterListBox;
		PictureDisplay	dialogImage;
		SvMc::AvailableObjectListComboBox<SvPb::ClassIdEnum> m_availableFilterCB;
		//}}AFX_DATA

		//******************************************************************************
		// Data Element(s):
		//******************************************************************************
	protected:
		const SVGUID m_InspectionID;
		const SVGUID m_TaskObjectID;
		SVGUID m_UnaryImageOperatorID;
		ImageController m_ImageController;
	};
} //namespace SvOg
