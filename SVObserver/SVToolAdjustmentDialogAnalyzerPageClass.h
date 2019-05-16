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
//TODO: MZA(10.Nov 2014): Move this files to SVOGui project and then remove folder from include and Namespace add-on add PictureDisplay declaration.
#include "SVOGui\PictureDisplay.h"
#include "SVOGui/ImageController.h"
#include "SVMFCControls\AvailableObjectListComboBox.h"
#include "SVMFCControls/SVEnumerateCombo.h"
#pragma endregion Includes

namespace SvAo
{
class SVAnalyzerClass;
}
namespace SvTo
{
class SVToolClass;
}
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
	virtual BOOL OnInitDialog() override;
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
	//************************************
	void setImages();

	//******************************************************************************
	// Class Wizard Generated Virtual Function(s):
	//******************************************************************************
	//{{AFX_VIRTUAL(SVToolAdjustmentDialogAnalyzerPageClass)
protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV
	//}}AFX_VIRTUAL
	//************************************
	// Method:    OnSetActive
	// Description: is called when propertypage get active. set the image 
	// Access:    public 
	// Returns:   BOOL 
	//************************************
	virtual BOOL OnSetActive() override;

	//******************************************************************************
	// Class Wizard Generated Dialog Data:
	//******************************************************************************
protected:
	//{{AFX_DATA(SVToolAdjustmentDialogAnalyzerPageClass)
	enum { IDD = IDD_TA_ANALYZER_DIALOG };
	SvMc::AvailableObjectListComboBox	m_availableAnalyzerCombobox;
	SvOg::PictureDisplay	m_dialogImage;
	//}}AFX_DATA

	//******************************************************************************
	// Data Element(s):
	//******************************************************************************
public:
protected:
	SVToolAdjustmentDialogSheetClass*	m_pParentDialog;
	SvTo::SVToolClass* m_pTool;
	SvAo::SVAnalyzerClass*	m_pCurrentAnalyzer;

	/// If a analyzer additional added to the combobox, because it is normally not available but now selected, it is saved here. 
	/// If the selection of this get lost it will deleted from the combobox
	SVGUID m_additionalAnalyzerId; 
	const SVGUID m_InspectionID;
	const SVGUID m_TaskObjectID;

	SvOg::ImageController m_ImageController;
	SVGUID m_resultImageID = GUID_NULL;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

