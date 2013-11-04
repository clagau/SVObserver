// ******************************************************************************
// * COPYRIGHT (c) 2002 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVOIOFrameWndClass
// * .File Name       : $Workfile:   SVOIOFrameWndClass.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   25 Apr 2013 11:33:56  $
// ******************************************************************************

#if !defined(AFX_SVOIOFRAMEWNDCLASS_H__CDF9EB40_A939_4B1A_AF5C_D45CE408BB2A__INCLUDED_)
#define AFX_SVOIOFRAMEWNDCLASS_H__CDF9EB40_A939_4B1A_AF5C_D45CE408BB2A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// SVOIOFrameWndClass frame

class SVOIOFrameWndClass : public CFrameWnd
{
	DECLARE_DYNCREATE(SVOIOFrameWndClass)
protected:
	SVOIOFrameWndClass();           // protected constructor used by dynamic creation

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(SVOIOFrameWndClass)
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~SVOIOFrameWndClass();

	// Generated message map functions
	//{{AFX_MSG(SVOIOFrameWndClass)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SVOIOFRAMEWNDCLASS_H__CDF9EB40_A939_4B1A_AF5C_D45CE408BB2A__INCLUDED_)

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVOFileConfig\SVOIOFrameWndClass.h_v  $
 * 
 *    Rev 1.0   25 Apr 2013 11:33:56   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   18 Nov 2002 10:01:46   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  226
 * SCR Title:  Monochrome SVIM configuration compatibility between ViperQUAD and ViperDUAL
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   first version of file
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/