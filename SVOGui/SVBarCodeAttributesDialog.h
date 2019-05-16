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
#pragma once

#pragma warning( disable: 4786 )	// identifier truncation 255 chars

#pragma region Includes
//Moved to precompiled header: #include <map>
//Moved to precompiled header: #include <vector>
#pragma endregion Includes

namespace SvVol
{
class SVDoubleValueObjectClass;
}

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
	double SetErrCorrection (SvVol::SVDoubleValueObjectClass &svdErrCorrection);
	double SetEncoding (SvVol::SVDoubleValueObjectClass &svdEncoding);
	double GetErrorCorrection();
	double GetEncoding ();
	BOOL OnSetActive ();
	SVBarCodeAttributesDialog(CWnd* pParent = nullptr);   // standard constructor
	virtual ~SVBarCodeAttributesDialog();

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
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(SVBarCodeAttributesDialog)
	virtual BOOL OnInitDialog() override;
	afx_msg void OnSelchangeBarcodeErrorCorrection();
	afx_msg void OnSelchangeBarcodeEncoding();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	struct SVBarCodeErrorCorrectionEncodingStruct
	{
		long m_Mil;
		std::string m_Name;

		SVBarCodeErrorCorrectionEncodingStruct() { m_Mil = 0; }
		SVBarCodeErrorCorrectionEncodingStruct(long Mil, const std::string& rName )
		{
			m_Mil = Mil;
			m_Name = rName;
		}
	};
	typedef std::vector<SVBarCodeErrorCorrectionEncodingStruct> SVBarCodeEccEncVector;


	std::string GetInfoNameByMil(const SVBarCodeEccEncVector& rMilVector, long MilID);
	SVBarCodeEccEncVector m_aMilEnc;
	SVBarCodeEccEncVector m_aMilEcc;

	std::map<long, std::vector< long > > m_mapBarCodeEcc;
	std::map<long, std::vector< long > > m_mapBarCodeEnc;
};
