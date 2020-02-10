//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVTADlgColorToolPage
//* .File Name       : $Workfile:   SVTADlgColorToolPage.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   26 Jun 2014 18:21:16  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVOResource/resource.h"
#include "SVMFCControls\AvailableObjectListComboBox.h"
#include "PictureDisplay.h"
#include "ImageController.h"
#include "BoundValue.h"
#include "ValuesAccessor.h"
#include "DataController.h"
#pragma endregion Includes

namespace SvOg
{
	class SVTADlgColorToolPage : public CPropertyPage
	{
		typedef SvOg::DataController<SvOg::ValuesAccessor, SvOg::ValuesAccessor::value_type> ValueController;

	public:
		SVTADlgColorToolPage( const SVGUID& rInspectionID, const SVGUID& rTaskObjectID );
		virtual ~SVTADlgColorToolPage();

		HRESULT SetInspectionData();

	// Overrides
		// ClassWizard generate virtual function overrides
		//{{AFX_VIRTUAL(SVTADlgColorToolPageClass)
		protected:
		virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support
		//}}AFX_VIRTUAL

	// Implementation
	protected:
		// Generated message map functions
		//{{AFX_MSG(SVTADlgColorToolPageClass)
		virtual BOOL OnInitDialog() override;
		afx_msg void OnSelchangeCombo1();
		afx_msg void OnConvertToHsi();
		afx_msg void OnTrainColor();
		//}}AFX_MSG
		DECLARE_MESSAGE_MAP()

		void SetImage();

	protected:
		enum { IDD = IDD_TA_COLOR_TOOL_DIALOG };
		SvOg::PictureDisplay	m_dialogImage;
		SvMc::AvailableObjectListComboBox<SVGUID> m_availableSourceImageListBox;

		BOOL	m_convertToHSI;
		std::string m_InputName;

		const SVGUID m_InspectionID;
		const SVGUID m_TaskObjectID;
		SVGUID m_ResultImageID = GUID_NULL;
		
		ImageController m_Images;
		ValueController m_Values;
	};
} //namespace SvOg

