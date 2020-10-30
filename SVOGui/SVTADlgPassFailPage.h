//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVTADlgPassFailPage
//* .File Name       : $Workfile:   SVTADlgPassFailPage.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   24 Apr 2013 11:16:30  $
//******************************************************************************
#pragma once

#pragma region Includes
#include "ISVPropertyPageDialog.h"
#include "RangeController.h"
#include "Definitions/RangeEnum.h"
#include "ObjectSelectorController.h"

#pragma endregion Includes

namespace SvOg
{
	class SVTADlgPassFailPage : public CPropertyPage, public SvOg::ISVPropertyPageDialog, public SvOg::RangeController
	{
	public:
#pragma region Constructor
		SVTADlgPassFailPage(uint32_t inspectionID, uint32_t taskID, UINT captionID = 0);
		SVTADlgPassFailPage();
		virtual ~SVTADlgPassFailPage();
#pragma endregion Constructor

#pragma region Public Methods
		virtual bool QueryAllowExit() override;
#pragma endregion Public Methods

#pragma region Protected Methods
	protected:
		DECLARE_MESSAGE_MAP()

		//{{AFX_VIRTUAL(SVTADlgPassFailPage)
	protected:
		virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV-Unterstützung
		virtual BOOL OnInitDialog() override;
		virtual BOOL OnSetActive() override;
		virtual BOOL OnKillActive() override;
		//}}AFX_VIRTUAL

		//{{AFX_MSG(SVTADlgPassFailPage)
		afx_msg void OnBnClickedFailHighIndirect();
		afx_msg void OnBnClickedWarnlHighIndirect();
		afx_msg void OnBnClickedWarnLowIndirect();
		afx_msg void OnBnClickedFailedLowIndirect();
		//{{AFX_MSG
#pragma region Protected Methods

#pragma region Privated Methods
	//************************************
	/// Show an object selector and return the name of the selection.
	/// \param Name [in,out] Name of the object.
	/// \param fieldEnum [in] Enum of the value
	/// \returns bool true if a new object would selected.
	//************************************
		bool ShowObjectSelector(std::string& rName, RangeEnum::ERange fieldEnum);
#pragma region Privated Methods

#pragma region Member Variables
	protected:
		//{{AFX_DATA(SVTADlgPassFailPage)
		enum { IDD = IDD_TA_PASS_FAIL_DIALOG };
		CButton m_ButtonFailHigh;
		CButton m_ButtonWarnHigh;
		CButton m_ButtonWarnLow;
		CButton m_ButtonFailLow;
		CString m_FailHigh;
		CString m_FailLow;
		CString m_WarnHigh;
		CString m_WarnLow;
		//}}AFX_DATA

	private:
		HRESULT SetInspectionData();
		bool UpdateRangeValues();
		void InitData();

		CBitmap m_downArrowBitmap;
		ObjectSelectorController m_objectSelector;
		uint32_t m_toolId;
#pragma endregion Member Variables
	};
} //namespace SvOg
