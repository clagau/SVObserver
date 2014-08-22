//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVPatModelPageClass
//* .File Name       : $Workfile:   SVPatSelectModelPageClass.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   21 Aug 2014 10:40:42  $
//******************************************************************************

#ifndef SVPATMODELPAGECLASS_H
#define SVPATMODELPAGECLASS_H

#pragma region Includes
#include "SVLibrary/SVBitmap.h"
#include "SVImageLibrary/SVImageInfoClass.h"
#include "SVImageLibrary/SVImageBufferHandleInterface.h"
#include "PropertyTree/PropTree.h"
#include "PictureDisplay.h"
#pragma endregion Includes

#pragma region Declarations
class SVToolClass;
class SVPatternAnalyzerClass;
class SVPatAnalyzeSetupDlgSheet;
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
#pragma endregion Declarations

class SVPatModelPageClass : public CPropertyPage
{
	DECLARE_DYNCREATE(SVPatModelPageClass)

// Construction
#pragma region Constructor
public:
	SVPatModelPageClass();
	virtual ~SVPatModelPageClass();
#pragma endregion Constructor

#pragma region Public Members
public:
	//@HACK: These Member Variables are used outside this class.  It should be changed to make them private.
// Dialog Data
	//{{AFX_DATA(SVPatModelPageClass)
	enum { IDD = IDD_PAT_SETUP_MODEL };
	CButton m_CircularOverscanCheckbox;
	PictureDisplay m_dialogImage;
	CString	m_strModelName;
	BOOL m_bCircularOverscan;
	SVRPropTree          m_Tree;
	//}}AFX_DATA

	int		m_nXPos;
	int		m_nYPos;
	long	m_lModelWidth;
	long	m_lModelHeight;
	int m_nMaxX;
	int m_nMaxY;
	
	SVPatternAnalyzerClass* m_pPatAnalyzer;
#pragma endregion Public Members

#pragma region Protected Methods
protected:
// Overrides
#pragma region AFX Virtual Methods
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(SVPatModelPageClass)
	virtual void OnCancel();
	virtual void OnOK();
	virtual BOOL OnApply();
	virtual BOOL OnSetActive();
	virtual BOOL OnKillActive();
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
#pragma endregion AFX Virtual Methods

#pragma region AFX MSG
	// Generated message map functions
	//{{AFX_MSG(SVPatModelPageClass)
	afx_msg void OnFileButton();
	afx_msg void OnCreateModel();
	afx_msg void OnKillFileName();
	afx_msg void OnCircularOverscanClicked();
	afx_msg void OnItemChanged(NMHDR* pNotifyStruct, LRESULT* plResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	DECLARE_EVENTSINK_MAP()
#pragma endregion AFX MSG

// Implementation
protected:
	//************************************
	// Method:    ObjectChangedExDialogimage
	// Description: Event-methods, called if overlay-object is changed. Set the new values to the mask properties.
	//************************************
	void ObjectChangedExDialogImage(long Tab, long Handle, VARIANT* ParameterList, VARIANT* ParameterValue);
#pragma endregion Protected Methods

#pragma region Private Methods
private:
	// Validation routines
	bool ValidateModelParameters(UINT& nMsgID);
	bool ValidateModelWidth(UINT& nMsgID);
	bool ValidateModelHeight(UINT& nMsgID);
	bool ValidateModelFilename(UINT& nMsgID);

	void InitializeData(); 
	BOOL ProcessOnKillFocus(UINT nId);
	UINT RestoreModelFromFile();
	BOOL GetModelFile(BOOL bMode);

	HRESULT BuildPropertyList();
	HRESULT RefreshProperties();
	void setImages();
	void setOverlay();
	void setCircularToolOverlay();
	void setCircularModelOverlay();
	bool m_bAllowExit;
	//************************************
	// Method:    setCircularOverscanCheckboxState
	// Description: Enable or Disable the checkbox if the height and width large enough or not.
	//				Uncheck the checkbox, if it is not big enough
	// Returns:   void
	//************************************
	void setCircularOverscanCheckboxState();
#pragma endregion Private Methods

#pragma region Member Variables
private:
	enum 
	{ 
		ModelWidth_Property,
		ModelHeight_Property,
		ModelOriginX_Property,
		ModelOriginY_Property
	} Model_Property_Enums;

	SVPatAnalyzeSetupDlgSheet* m_pSheet;

	long m_sourceImageWidth;
	long m_sourceImageHeight;

	CString m_strOldModelName;
	long m_handleToModelOverlayObject;
	long m_handleToCircleOverlayObject;
	long m_handleToSquareOverlayObject;
	long m_handleToCircleOverlayObject2;
	long m_handleToSquareOverlayObject2;
#pragma endregion Member Variables
};

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVPatSelectModelPageClass.h_v  $
 * 
 *    Rev 1.2   21 Aug 2014 10:40:42   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  912
 * SCR Title:  Fix issue with Pattern Analyzer if pattern is larger than the ROI of tool
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   added OnApply and a check to see if the user wants to exit if model is larger then ROI
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   26 Jun 2014 18:09:44   mziegler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  885
 * SCR Title:  Replace image display in TA-dialogs with activeX SVPictureDisplay
 * Checked in by:  mZiegler;  Marc Ziegler
 * Change Description:  
 *   use SVPictureDisplay-control
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 13:20:10   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0.1.10   10 Feb 2011 15:09:04   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to now use shared pointer instead of untracked structure.  This fixes destruction, tracking and rebuilding issues.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0.1.9   16 Dec 2009 12:25:08   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  677
 * SCR Title:  Fix problem in camera notify thread
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fix issues with includes and comments.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0.1.8   31 Dec 2008 11:23:06   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  644
 * SCR Title:  Pattern Analyzer Enhancements and Bug Fixes
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised to support Circular Overscan.
 * Revised Field validation logic.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0.1.7   19 Jan 2004 10:45:28   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  370
 * SCR Title:  Display buffers have reached maxium number of available
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Removed all non-statndard paint functionality in replace of SVDlgImageClass functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0.1.6   29 Oct 2003 14:04:32   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  393
 * SCR Title:  Create Model for Pattern Analyzer will save even if you press cancel
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   changed return type of GetModelFile to return FALSE if the user pressed cancel.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0.1.5   22 Apr 2003 13:51:52   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0.1.4   06 Mar 2003 13:17:18   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  328
 * SCR Title:  Upgrade SVObserver to MIL 7.1
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Updated to work with MIL 7.1
 * MvgaDispXXX no longer works. Create a copy of the DIB instead.
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0.1.3   30 Jan 2003 15:36:32   joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  301
 * SCR Title:  Implement Result Image Circle Buffer
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated OnPaint, InitModelImage, OnInitDialog, destructor and OnCreateModel methods to use the new image circle buffer methodology.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/