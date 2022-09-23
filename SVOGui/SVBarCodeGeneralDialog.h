//******************************************************************************
//* COPYRIGHT (c) 2003 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVBarCodeGeneralDialog
//* .File Name       : $Workfile:   svbarcodegeneraldialog.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 09:42:26  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVMFCControls/SVEnumerateCombo.h"
#include "SVProtoBuf/InspectionCommands.h"
#pragma endregion Includes

namespace SvOg
{
	// Enum for threshold type radio
	enum
	{
		SVBCThresholdNormal = 0,
		SVBCThresholdAuto = 1,
		SVBCThresholdAdaptive = 2,
	};

	/////////////////////////////////////////////////////////////////////////////
	// SVBarCodeGeneralDialog dialog

	class SVBarCodeGeneralDialog : public CPropertyPage
	{
		// Construction
	public:
		void SetThreshold(long threshold);
		double GetThreshold();
		void SetSkewPositive(long skewPositive);
		double GetSkewPositive();
		void SetSkewNegative(long skewNegative);
		double GetSkewNegative();
		void SetOrientation(long orientation);
		double GetOrientation();
		void SetForegroundColor(long foregroundColor);
		double GetForegroundColor();
		void SetBarCodeSearchSpeed(long searchSpeed);
		double GetBarCodeSearchSpeed();
		void SetBarCodeStringSize(long stringSize);
		double GetBarCodeStringSize();

		void setBarCodeTypeInfos(const SvPb::GetBarCodeTypeInfosResponse& barCodeTypeInfos) { m_barCodeTypeInfos = barCodeTypeInfos; };
		void SetBarcodeStringFormat(SvDef::NameValueVector&& rStringFormatList, long stringFormatPos);
		CString GetBarcodeStringFormat();

		void SetBarcodeThresholdType(long thresholdType);
		int GetBarcodeThresholdType();

		void SetBarCodeType(long barCodeType);
		long GetBarCodeType();
		BOOL GetWarnedOnFail();
		void SetWarnOnFail(BOOL bWarnOnFail);
		void SetTimeout(long lTimeout);
		long GetTimeout();

		void SetUnEvenGrid(bool unEvenGrid);
		BOOL GetUnEvenGrid();

		void UpdateUnEvenGrid();

		SVBarCodeGeneralDialog(CWnd* pParent = nullptr);   // standard constructor
		virtual ~SVBarCodeGeneralDialog();
		BOOL	m_bWarnOnFail = false;

		// Dialog Data
			//{{AFX_DATA(SVBarCodeGeneralDialog)
		enum { IDD = IDD_BARCODE_GENERAL };
		CSpinButtonCtrl	m_spnTimeout;
		CEdit	m_ctlTimeout;
		CComboBox	m_cbBarcodeType;
		int		m_iBarCodeColor = -1;
		int		m_iOrientation = 0;
		CString	m_szOrientation { _T("") };
		int		m_iSkewNegative = 0;
		CString	m_szSkewNegative { _T("") };
		int		m_iSkewPositive = 0;
		CString	m_szSkewPositive { _T("") };
		CString	m_szStringSize { _T("") };
		int		m_iThreshold = 0;
		CString	m_szThreshold { _T("") };
		int		m_iBarCodeType = -1;
		int		m_iSearchSpeed = -1;
		long	m_lTimeout = 0;
		//}}AFX_DATA

	// Overrides
		// ClassWizard generated virtual function overrides
		//{{AFX_VIRTUAL(SVBarCodeGeneralDialog)
	protected:
		virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support
		//}}AFX_VIRTUAL

	// Implementation
	protected:

		// Generated message map functions
		//{{AFX_MSG(SVBarCodeGeneralDialog)
		virtual BOOL OnInitDialog() override;
		afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
		afx_msg void OnUpdateBarcodeOrientationEdit();
		afx_msg void OnUpdateBarcodeSkewNegativeEdit();
		afx_msg void OnUpdateBarcodeSkewPositiveEdit();
		afx_msg void OnUpdateBarcodeThresholdEdit();
		afx_msg void OnEditChangeBarCodeStringSize();
		afx_msg void OnSelChangeBarCodeColor();
		afx_msg void OnSelChangeSearchSpeed();
		afx_msg void OnSelChangeBarCodeType();
		afx_msg void OnSelChangeBarCodeStringSize();
		afx_msg void OnSelChangeStringFormat();
		afx_msg void OnDeltaposSpinTimeout(NMHDR* pNMHDR, LRESULT* pResult);
		//}}AFX_MSG
		DECLARE_MESSAGE_MAP()

	private:
		const SvPb::BarCodeTypeParameter* GetInfoByMilID(long MilID) const;
		long GetIndexByMilID(long MilID) const;

		SvPb::GetBarCodeTypeInfosResponse m_barCodeTypeInfos;
		long m_lInitialBarCodeType;
		SvMc::SVEnumerateCombo m_StringFormatCombo;
		SvDef::NameValueVector m_stringFormatList;
		long m_stringFormatPos = -1;
		CString m_StringFormat;

		bool  m_bUnEvenGrid = false;

		// Threshold Type Radio
		CButton m_ThresholdRadio;
		int m_iThresholdRadio;
		afx_msg void OnBnClickedThresholdNormalRadio(UINT nID);
		CButton m_UnevenCheck;
		afx_msg void OnBnClickedChkUnevenGrid();
	};
}