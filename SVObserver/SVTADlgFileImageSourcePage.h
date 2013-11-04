//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVToolAdjustmentDialogFileImageSourcePage
//* .File Name       : $Workfile:   SVTADlgFileImageSourcePage.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   24 Apr 2013 11:14:38  $
//******************************************************************************

//******************************************************************************
//* INCLUDE CONTROL:
//******************************************************************************

#ifndef SVTADLGFILEIMAGESOURCEPAGE_H
#define SVTADLGFILEIMAGESOURCEPAGE_H

//******************************************************************************
//* INCLUDE(S):
//******************************************************************************

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SVFileNameClass.h"
#include "SVDlgImage.h"

#include "SVTaskObjectInterfaceClass.h"

class SVToolAdjustmentDialogSheetClass;
class SVToolClass;
class SVBoolValueObjectClass;
class SVFileNameValueObjectClass;

//******************************************************************************
//* CLASS(ES) & TYPE(S):
//* STRUCTURE(S) & TYPE(S):
//* UNION(S) & TYPE(S):
//******************************************************************************

class SVToolAdjustmentDialogFileImageSourcePageClass : public CPropertyPage, public SVTaskObjectInterfaceClass
{
//******************************************************************************
// Constructor(s):
//******************************************************************************
public:
	SVToolAdjustmentDialogFileImageSourcePageClass( SVToolAdjustmentDialogSheetClass* PParent );

//******************************************************************************
// Destructor(s):
//******************************************************************************
public:
	virtual ~SVToolAdjustmentDialogFileImageSourcePageClass();

	virtual HRESULT SetInspectionData();

//******************************************************************************
// Message Handler(s):
//******************************************************************************
protected:
	//{{AFX_MSG(SVToolAdjustmentDialogFileImageSourcePageClass)
	virtual BOOL OnInitDialog();
	afx_msg void OnBrowseButton();
	afx_msg void OnReloadCheck();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()


//******************************************************************************
// Virtual(s):
//******************************************************************************

	//{{AFX_VIRTUAL(SVToolAdjustmentDialogFileImageSourcePageClass)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL


//******************************************************************************
// Operation(s) Of Representation:
//******************************************************************************
protected:
	void refresh();

//******************************************************************************
// Data Element(s):
//******************************************************************************
public:
protected:
	SVToolAdjustmentDialogSheetClass*	pSheet;
	SVToolClass*						pTool;
	SVImageClass*						pImage;
	SVBoolValueObjectClass*				pReloadObject;
	SVFileNameValueObjectClass*			pPathNameObject;
	SVFileNameClass						svfncImageSourceFile;

	//{{AFX_DATA(SVToolAdjustmentDialogFileImageSourcePageClass)
	enum { IDD = IDD_TA_FILE_IMAGE_DIALOG };
	SVDlgImageClass	imageCtrl;
	CString	StrPathName;
	BOOL	BContinuousReload;
	//}}AFX_DATA

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

//******************************************************************************
//* INCLUDE CONTROL:
//******************************************************************************
#endif	// SVTADLGFILEIMAGESOURCEPAGE_H

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVTADlgFileImageSourcePage.h_v  $
 * 
 *    Rev 1.0   24 Apr 2013 11:14:38   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.4   09 Aug 2005 08:18:14   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated objects and dialogs to use the ActiveX Interface to update inspection data.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.3   22 Apr 2003 16:12:00   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.2   13 Jun 2001 10:14:18   ed
 * Project:  SVObserver
 * Change Request (SCR) nbr:  200
 * SCR Title:  Update code to implement SVIM COM Server
 * Checked in by:  Ed;  Ed Chobanoff
 * Change Description:  
 *   Modified the following functions: OnInitDialog.
 * Changed pPathNameObject from SVStringValueObjectClass* to SVFileNameValueObjectClass*.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.1   06 Feb 2001 13:44:26   cSchmittinger
 * Project:  SVObserver
 * Change Request (SCR) nbr:  191
 * SCR Title:  Restructure File Management
 * Checked in by:  cSchmittinger;  Carl Schmittinger
 * Change Description:  
 *   Modified source code to use new file management classes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.0   23 Nov 1999 13:40:34   mike
 * Project:  SVObserver
 * Change Request (SCR) nbr:  61
 * SCR Title:  Update PVCS versioning to version 3.0.
 * Checked in by:  Mike;  Mike McCarl
 * Change Description:  
 *   
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   Sep 24 1999 08:59:16   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  4
 * SCR Title:  Add "Load image" option
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Provides for tool for loading an image from a file.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
