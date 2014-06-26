//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVToolAdjustmentDialogFileImageSourcePage
//* .File Name       : $Workfile:   SVTADlgFileImageSourcePage.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   26 Jun 2014 18:21:18  $
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

#pragma region Includes
#include "SVFileNameClass.h"
#include "PictureDisplay.h"
#include "SVTaskObjectInterfaceClass.h"
#pragma endregion Includes

#pragma region Declarations
class SVToolAdjustmentDialogSheetClass;
class SVToolClass;
class SVBoolValueObjectClass;
class SVFileNameValueObjectClass;
#pragma endregion Declarations
//******************************************************************************
//* CLASS(ES) & TYPE(S):
//* STRUCTURE(S) & TYPE(S):
//* UNION(S) & TYPE(S):
//******************************************************************************

class SVToolAdjustmentDialogFileImageSourcePageClass : public CPropertyPage, public SVTaskObjectInterfaceClass
{
#pragma region Constructor
public:
	SVToolAdjustmentDialogFileImageSourcePageClass( SVToolAdjustmentDialogSheetClass* PParent );
	virtual ~SVToolAdjustmentDialogFileImageSourcePageClass();
#pragma endregion Constructor

#pragma region Public Methods
public:
	virtual HRESULT SetInspectionData();
#pragma endregion Public Methods

#pragma region Protected Methods
protected:
	//{{AFX_MSG(SVToolAdjustmentDialogFileImageSourcePageClass)
	afx_msg void OnBrowseButton();
	afx_msg void OnReloadCheck();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	//{{AFX_VIRTUAL(SVToolAdjustmentDialogFileImageSourcePageClass)
	protected:
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL

protected:
	void refresh();
	void setImages();
#pragma endregion Protected Methods

#pragma region Member variables
private:
	SVToolAdjustmentDialogSheetClass*	m_pSheet;
	SVToolClass*						m_pTool;
	SVImageClass*						m_pImage;
	SVBoolValueObjectClass*				m_pReloadObject;
	SVFileNameValueObjectClass*			m_pPathNameObject;
	SVFileNameClass						m_svfncImageSourceFile;

	//{{AFX_DATA(SVToolAdjustmentDialogFileImageSourcePageClass)
	enum { IDD = IDD_TA_FILE_IMAGE_DIALOG };
	PictureDisplay	m_imageCtrl;
	CString	m_StrPathName;
	BOOL	m_BContinuousReload;
	//}}AFX_DATA
#pragma endregion Member variables
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
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVTADlgFileImageSourcePage.h_v  $
 * 
 *    Rev 1.1   26 Jun 2014 18:21:18   mziegler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  885
 * SCR Title:  Replace image display in TA-dialogs with activeX SVPictureDisplay
 * Checked in by:  mZiegler;  Marc Ziegler
 * Change Description:  
 *   use SVPictureDisplay-control
 * cleanup
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
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
