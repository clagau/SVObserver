//******************************************************************************
//* COPYRIGHT (c) 2005 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : TADlgAnalyzerPageClass
//* .File Name       : $Workfile:   TADlgAnalyzerPageClass.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.3  $
//* .Check In Date   : $Date:   17 Jul 2014 20:58:12  $
//******************************************************************************

#pragma once

#pragma region Includes
//TODO: MZA(10.Nov 2014): Move this files to SVOGui project and then remove folder from include and Namespace add-on add PictureDisplay declaration.
#include "Definitions/ObjectDefines.h"
#include "SVOGuiUtility/PictureDisplay.h"
#include "SVOGuiUtility/ImageController.h"
#include "SVProtoBuf/SVO-Enum.h"
#include "SVMFCControls/AvailableObjectListComboBox.h"
#pragma endregion Includes

namespace SvAo
{
class Analyzer;
}
namespace SvTo
{
class SVToolClass;
}
class TADlgSheetClass;
class SVUnaryImageOperatorList;

struct SVToolPropertyEntryStruct;

class TADlgAnalyzerPageClass : public CPropertyPage
{
	//******************************************************************************
	// Class Wizard Generated Message Map Entries:
	//******************************************************************************
	// Generated message map functions
protected:
	//{{AFX_MSG(TADlgAnalyzerPageClass)
	afx_msg void OnButtonDetails();
	virtual BOOL OnInitDialog() override;
	afx_msg void OnSelchangeCurrentAnalyzer();
	afx_msg void OnResultButton();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

public:
	TADlgAnalyzerPageClass(uint32_t inspectionID, uint32_t taskObjectID, TADlgSheetClass* pParent );
	virtual ~TADlgAnalyzerPageClass();

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
	//{{AFX_VIRTUAL(TADlgAnalyzerPageClass)
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
	//{{AFX_DATA(TADlgAnalyzerPageClass)
	enum { IDD = IDD_TA_ANALYZER_DIALOG };
	SvMc::AvailableObjectListComboBox<SvPb::ClassIdEnum>	m_availableAnalyzerCombobox;
	SvOgu::PictureDisplay	m_dialogImage;
	//}}AFX_DATA

	//******************************************************************************
	// Data Element(s):
	//******************************************************************************
public:
protected:
	TADlgSheetClass*	m_pParentDialog;
	SvTo::SVToolClass* m_pTool;
	SvAo::Analyzer*	m_pCurrentAnalyzer;

	/// If a analyzer additional added to the combobox, because it is normally not available but now selected, it is saved here. 
	/// If the selection of this get lost it will deleted from the combobox
	SvPb::ClassIdEnum m_additionalAnalyzerId = SvPb::NoObjectClassId;
	const uint32_t m_InspectionID;
	const uint32_t m_TaskObjectID;

	SvOgu::ImageController m_ImageController;
	uint32_t m_resultImageID = SvDef::InvalidObjectId;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

