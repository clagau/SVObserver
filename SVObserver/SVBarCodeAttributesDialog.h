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

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVBarCodeAttributesDialog.h_v  $
 * 
 *    Rev 1.0   23 Apr 2013 09:40:44   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.6   28 May 2010 14:30:18   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  694
 * SCR Title:  Upgrade SVObserver version for Chromis release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix issues with includes and make them consistant.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.5   03 Mar 2010 11:44:20   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  686
 * SCR Title:  Update Laptop version to load all valid configurations
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated to remove duplicate containers.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.4   08 Aug 2005 15:26:40   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Update Value Object header info.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.3   17 Apr 2003 16:45:00   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.2   05 Mar 2003 11:48:56   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  329
 * SCR Title:  Add bar codes to SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Added PVCS header
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
