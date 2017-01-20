//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVToolAdjustmentDialogPassFailPageClass
//* .File Name       : $Workfile:   SVTADlgPassFailPage.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   24 Apr 2013 11:16:30  $
//******************************************************************************
#pragma once

#pragma region Includes
#include "SVMFCControls\SVEditNumbers.h"
#include "ISVPropertyPageDialog.h"
#include "RangeController.h"
#include "ObjectInterfaces\RangeEnum.h"
#include "ObjectSelectorController.h"
#include "SVUtilityLibrary/SVString.h"
#pragma endregion Includes

namespace Seidenader
{ 
	namespace SVOGui
	{
		class SVToolAdjustmentDialogPassFailPageClass : public CPropertyPage, public SvOg::ISVPropertyPageDialog, public SvOg::RangeController
		{
		public:
		#pragma region Constructor
			SVToolAdjustmentDialogPassFailPageClass(const GUID& rInspectionID, const GUID& rTaskObjectID, UINT captionID = 0);
			SVToolAdjustmentDialogPassFailPageClass();
			virtual ~SVToolAdjustmentDialogPassFailPageClass();
		#pragma endregion Constructor

		#pragma region Public Methods
			virtual bool QueryAllowExit() override;
		#pragma endregion Public Methods

		#pragma region Protected Methods
		protected:
			DECLARE_MESSAGE_MAP()

			//{{AFX_VIRTUAL(SVToolAdjustmentDialogPassFailPageClass)
		protected:
			virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV-Unterstützung
			virtual BOOL OnInitDialog() override;
			virtual BOOL OnSetActive() override;
			virtual BOOL OnKillActive() override;
			//}}AFX_VIRTUAL

			//{{AFX_MSG(SVToolAdjustmentDialogPassFailPageClass)
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
			bool ShowObjectSelector(SVString& rName, RangeEnum::ERange fieldEnum);
		#pragma region Privated Methods

		#pragma region Member Variables
		protected:
			//{{AFX_DATA(SVToolAdjustmentDialogPassFailPageClass)
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
			void SetInspectionData();
			bool UpdateRangeValues();
			void InitData();

			CBitmap m_downArrowBitmap;
			ObjectSelectorController m_objectSelector;
		#pragma endregion Member Variables
		};
	}
}

namespace SvOg = Seidenader::SVOGui;

