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

#pragma once

#pragma region Includes
#include "PropertyTree/PropTree.h"
//TODO: MZA(10.Nov 2014): Move this files to SVOGui project and then remove folder from include and Namespace add-on add PictureDisplay declaration.
#include "SVOGui/PictureDisplay.h"
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
	SvOg::PictureDisplay m_dialogImage;
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

