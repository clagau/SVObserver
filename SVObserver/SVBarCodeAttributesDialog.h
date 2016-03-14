//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVBarCodeAttributesDialog
//* .File Name       : $Workfile:   SVBarCodeAttributesDialog.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 09:40:44  $
//******************************************************************************

#ifndef SVBARCODEATTRIBUTESDIALOG_H
#define SVBARCODEATTRIBUTESDIALOG_H

#pragma warning( disable: 4786 )	// identifier truncation 255 chars

#include <map>
#include "SVContainerLibrary/SVVector.h"
#include "SVValueObjectImpl.h"

/////////////////////////////////////////////////////////////////////////////
// SVBarCodeAttributesDialog dialog

class SVBarCodeAttributesDialog : public CPropertyPage
{
// Construction
public:
	DWORD SetErrCorrection (DWORD dwErrCorrection);
	DWORD SetEncoding (DWORD dwEncoding);
	double m_dErrorCorrection;
	double m_dEncoding;
	void InitSelections();
	void InitSelections (long lBarCodeType);
	double SetErrCorrection (SVDoubleValueObjectClass &svdErrCorrection);
	double SetEncoding (SVDoubleValueObjectClass &svdEncoding);
	double GetErrorCorrection();
	double GetEncoding ();
	BOOL OnSetActive ();
	SVBarCodeAttributesDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(SVBarCodeAttributesDialog)
	enum { IDD = IDD_BARCODE_ATTRIBUTES };
	int		m_iEncoding;
	int		m_iCorrection;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(SVBarCodeAttributesDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(SVBarCodeAttributesDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeBarcodeErrorCorrection();
	afx_msg void OnSelchangeBarcodeEncoding();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	struct SVBarCodeErrorCorrectionEncodingStruct
	{
		long lMil;
		CString strName;

		SVBarCodeErrorCorrectionEncodingStruct() { lMil = 0; }
		SVBarCodeErrorCorrectionEncodingStruct(long l_lMil, const CString& l_strName )
		{
			lMil = l_lMil;
			strName = l_strName;
		}
	};

	class SVBarCodeEccEncArray : public SVVector< SVBarCodeErrorCorrectionEncodingStruct >
	{
	public:
		const SVBarCodeErrorCorrectionEncodingStruct* GetInfoByMil(long lMil);
	};

	SVBarCodeEccEncArray m_aMilEnc;
	SVBarCodeEccEncArray m_aMilEcc;

	std::map< long, std::vector< long > > m_mapBarCodeEcc;
	std::map< long, std::vector< long > > m_mapBarCodeEnc;

	
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SVBARCODEATTRIBUTESDIALOG_H__AF5FDD69_7BE0_11D3_A7D0_00106F010A93__INCLUDED_)

