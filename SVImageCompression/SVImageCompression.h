// SVImageCompression.h : main header file for the SVIMAGECOMPRESSION DLL
//

#if !defined(AFX_SVIMAGECOMPRESSION_H__10698A12_2606_463D_AB54_37ED30E58D60__INCLUDED_)
#define AFX_SVIMAGECOMPRESSION_H__10698A12_2606_463D_AB54_37ED30E58D60__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CSVImageCompressionApp
// See SVImageCompression.cpp for the implementation of this class
//

class CSVImageCompressionApp : public CWinApp
{
public:
	CSVImageCompressionApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSVImageCompressionApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CSVImageCompressionApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SVIMAGECOMPRESSION_H__10698A12_2606_463D_AB54_37ED30E58D60__INCLUDED_)
