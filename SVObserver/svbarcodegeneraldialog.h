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

#ifndef SVBARCODEANALYZERDIALOG_H
#define SVBARCODEANALYZERDIALOG_H

#include "SVContainerLibrary/SVVector.h"
#include "SVValueObjectImpl.h"

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
	long GetBarcodeStringFormat( );

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

	SVBarCodeGeneralDialog(CWnd* pParent = NULL);   // standard constructor
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
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(SVBarCodeGeneralDialog)
	virtual BOOL OnInitDialog();
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


	class SVBarCodeInfoArray : public SVVector< SVBarCodeInfoStruct >
	{
	public:
		const SVBarCodeInfoStruct* GetInfoByIndex(int iIndex);
		const SVBarCodeInfoStruct* GetInfoByMilID(long lMilID);
	};

	SVBarCodeInfoArray m_aBarCodeInfo;
	long m_lInitialBarCodeType;
	int m_lStringFormat;
	CComboBox m_StringFormatCombo;
	SVEnumerateVector m_EnumVect;

	BOOL  m_bUnEvenGrid;

	// Threshold Type Radio
	CButton m_ThresholdRadio;
	int m_iThresholdRadio;
	afx_msg void OnBnClickedThresholdNormalRadio(UINT nID );
	CButton m_UnevenCheck;
	afx_msg void OnBnClickedChkUnevenGrid();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SVBARCODEANALYZERDIALOG_H__AF5FDD68_7BE0_11D3_A7D0_00106F010A93__INCLUDED_)

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\svbarcodegeneraldialog.h_v  $
 * 
 *    Rev 1.0   23 Apr 2013 09:42:26   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.13   29 Jun 2012 14:56:20   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  771
 * SCR Title:  Upgrade to the Next Version of Matrox Imaging Library (MIL)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated resource file to add the new barcode parameter to fix an issues with MIL 9 processing.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.12   03 Mar 2010 11:44:22   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  686
 * SCR Title:  Update Laptop version to load all valid configurations
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated to remove duplicate containers.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.11   06 Jun 2008 11:22:58   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  627
 * SCR Title:  Add RawData object to the Barcode Analyzer and format read string
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Removed include afxwin. It is already in stdafx.h
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.10   03 Jun 2008 14:24:34   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  627
 * SCR Title:  Add RawData object to the Barcode Analyzer and format read string
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added Threshold Type Radio to Dialog.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.9   30 May 2008 14:44:12   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  627
 * SCR Title:  Add RawData object to the Barcode Analyzer and format read string
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Dialog changes to support Adaptive threshold check box.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.8   29 May 2008 09:53:10   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  627
 * SCR Title:  Add RawData object to the Barcode Analyzer and format read string
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Modified barcode general page to include String Output Format combo box.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.7   24 Jul 2007 11:26:12   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  597
 * SCR Title:  Upgrade Matrox Imaging Library to version 8.0
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Upgrade to Matrox Imaging Library 8.0
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.6   29 Sep 2005 09:49:04   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  498
 * SCR Title:  Add individual timeout capability to Bar Code analyzers
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   new field on the dialog for timeout
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.5   08 Aug 2005 15:26:40   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Update Value Object header info.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.4   08 Sep 2004 11:04:28   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  444
 * SCR Title:  Change Bar Code Analyzer Pass / Fail Result
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added m_bWarnOnFail.
 * Added accessor functions for m_bWarnOnFail.
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.3   17 Apr 2003 16:54:06   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.2   05 Mar 2003 11:48:54   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  329
 * SCR Title:  Add bar codes to SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Added PVCS header
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
