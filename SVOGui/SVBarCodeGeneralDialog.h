//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
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
#include "SVValueObjectLibrary/SVValueObject.h"
#include "SVMFCControls/SVEnumerateCombo.h"
#pragma endregion Includes

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
	double SetThreshold (SVDoubleValueObjectClass& svdThreshold);
	double GetThreshold ();
	double SetSkewPositive (SVDoubleValueObjectClass& svdSkewPositive);
	double GetSkewPositive ();
	double SetSkewNegative (SVDoubleValueObjectClass& svdSkewNegative);
	double GetSkewNegative ();
	double SetOrientation (SVDoubleValueObjectClass& svdOrientation);
	double GetOrientation ();
	double SetForegroundColor (SVDoubleValueObjectClass& svdForegroundColor);
	double GetForegroundColor ();
	double SetBarCodeSearchSpeed (SVDoubleValueObjectClass& svdSearchSpeed);
	double GetBarCodeSearchSpeed ();
	double SetBarCodeStringSize (SVDoubleValueObjectClass& svdStringSize);
	double GetBarCodeStringSize ();

	bool SetBarcodeStringFormat( SVEnumerateValueObjectClass& p_sveStringFormat );
	CString GetBarcodeStringFormat( );

	bool SetBarcodeThresholdType( SVLongValueObjectClass& p_svThresholdType );
	int GetBarcodeThresholdType( );

	long SetBarCodeType (SVLongValueObjectClass& svlBarCodeType);
	long GetBarCodeType();
	BOOL GetWarnedOnFail();
	void SetWarnOnFail( BOOL bWarnOnFail );
	long SetTimeout( SVLongValueObjectClass &svlTimeout );
	long GetTimeout();

	bool SetUnEvenGrid( SVBoolValueObjectClass p_svbUnEvenGrid );
	BOOL GetUnEvenGrid ();

	void UpdateUnEvenGrid();

	SVBarCodeGeneralDialog(CWnd* pParent = nullptr);   // standard constructor
	virtual ~SVBarCodeGeneralDialog();
	BOOL	m_bWarnOnFail;

// Dialog Data
	//{{AFX_DATA(SVBarCodeGeneralDialog)
	enum { IDD = IDD_BARCODE_GENERAL };
	CSpinButtonCtrl	m_spnTimeout;
	CEdit	m_ctlTimeout;
	CComboBox	m_cbBarcodeType;
	int		m_iBarCodeColor;
	int		m_iOrientation;
	CString	m_szOrientation;
	int		m_iSkewNegative;
	CString	m_szSkewNegative;
	int		m_iSkewPositive;
	CString	m_szSkewPositive;
	CString	m_szStringSize;
	int		m_iThreshold;
	CString	m_szThreshold;
	int		m_iBarCodeType;
	int		m_iSearchSpeed;
	long	m_lTimeout;
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

public:
	struct SVBarCodeInfoStruct
	{
		SVBarCodeInfoStruct()
		{
			iIndex = CB_ERR;
			lMil = 0;
			lDefaultEncoding = 0;
			lDefaultErrorCorrection = 0;
		};
		SVBarCodeInfoStruct(long l_lMil, const CString& l_strName, long l_DefEnc, long l_DefECC )
		{
			iIndex = CB_ERR;
			lMil = l_lMil;
			strName = l_strName;
			lDefaultEncoding = l_DefEnc;
			lDefaultErrorCorrection = l_DefECC;
		}
		long lMil;
		CString strName;
		long lDefaultEncoding;
		long lDefaultErrorCorrection;
		int iIndex;
	};
	typedef std::vector<SVBarCodeInfoStruct> SVBarCodeInfoVector;

	const SVBarCodeInfoStruct* GetInfoByIndex(int iIndex);
	const SVBarCodeInfoStruct* GetInfoByMilID(long lMilID);

	SVBarCodeInfoVector m_aBarCodeInfo;
	long m_lInitialBarCodeType;
	SvMc::SVEnumerateComboClass m_StringFormatCombo;
	CString m_StringFormat;
	SvOi::NameValueVector m_EnumVect;

	bool  m_bUnEvenGrid;

	// Threshold Type Radio
	CButton m_ThresholdRadio;
	int m_iThresholdRadio;
	afx_msg void OnBnClickedThresholdNormalRadio(UINT nID );
	CButton m_UnevenCheck;
	afx_msg void OnBnClickedChkUnevenGrid();
};


