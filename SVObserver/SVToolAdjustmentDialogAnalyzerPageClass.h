//******************************************************************************
//* COPYRIGHT (c) 2005 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVToolAdjustmentDialogAnalyzerPageClass
//* .File Name       : $Workfile:   SVToolAdjustmentDialogAnalyzerPageClass.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.3  $
//* .Check In Date   : $Date:   17 Jul 2014 20:58:12  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVMFCControls\svmaskeditor.h"
//TODO: MZA(10.Nov 2014): Move this files to SVOGui project and then remove folder from include and Namespace add-on add PictureDisplay declaration.
#include "SVOGui\PictureDisplay.h"
#include "SVMFCControls\AvailableObjectListComboBox.h"
#include "SVEnumerateCombo.h"
#pragma endregion Includes

class SVToolClass;
class SVAnalyzerClass;
class SVToolAdjustmentDialogSheetClass;
class SVUnaryImageOperatorListClass;

struct SVToolPropertyEntryStruct;

class SVToolAdjustmentDialogAnalyzerPageClass : public CPropertyPage
{
	//******************************************************************************
	// Class Wizard Generated Message Map Entries:
	//******************************************************************************
	// Generated message map functions
protected:
	//{{AFX_MSG(SVToolAdjustmentDialogAnalyzerPageClass)
	afx_msg void OnButtonDetails();
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeCurrentAnalyzer();
	afx_msg void OnResultButton();
	afx_msg void OnPublishButton();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

public:
	SVToolAdjustmentDialogAnalyzerPageClass( const SVGUID& rInspectionID, const SVGUID& rTaskObjectID, SVToolAdjustmentDialogSheetClass* pParent );
	virtual ~SVToolAdjustmentDialogAnalyzerPageClass();

protected:
	void updateButtons();
	void DestroyAnalyzer();
	//************************************
	// Method:    setImages
	// Description: Set image to the activeX-control. 
	// Access:    public 
	// Returns:   BOOL -FALSE when image not exist
	//************************************
	BOOL setImages();

	//******************************************************************************
	// Class Wizard Generated Virtual Function(s):
	//******************************************************************************
	//{{AFX_VIRTUAL(SVToolAdjustmentDialogAnalyzerPageClass)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV
	//}}AFX_VIRTUAL
	//************************************
	// Method:    OnSetActive
	// Description: is called when propertypage get active. set the image 
	// Access:    public 
	// Returns:   BOOL 
	//************************************
	virtual BOOL OnSetActive();

	//******************************************************************************
	// Class Wizard Generated Dialog Data:
	//******************************************************************************
protected:
	//{{AFX_DATA(SVToolAdjustmentDialogAnalyzerPageClass)
	enum { IDD = IDD_TA_ANALYZER_DIALOG };
	SvMc::AvailableObjectListComboBox	m_availableAnalyzerCombobox;
	SvOg::PictureDisplay	dialogImage;
	//}}AFX_DATA

	//******************************************************************************
	// Data Element(s):
	//******************************************************************************
public:
protected:
	SVToolAdjustmentDialogSheetClass*	m_pParentDialog;
	SVToolClass*                    m_pTool;
	SVAnalyzerClass*				m_pCurrentAnalyzer;

	/// If a analyzer additional added to the combobox, because it is normally not available but now selected, it is saved here. 
	/// If the selection of this get lost it will deleted from the combobox
	SVGUID m_additionalAnalyzerId; 
	const SVGUID m_InspectionID;
	const SVGUID m_TaskObjectID;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

