//*****************************************************************************
/// \copyright COPYRIGHT (c) 2015,2015 by Seidenader Maschinenbau GmbH
/// All Rights Reserved 
/// \Author	Jim Brown
//*****************************************************************************

/// Classes and functions to ....
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVRPropertyTree/SVRPropTree.h"
#include "SVOGui/PictureDisplay.h"
#include "SVOGui\ISVPropertyPageDialog.h"  // for QueryAllowExit()
#include "SVOGui\ImageController.h"
#pragma endregion Includes

namespace SvTo
{
class ResizeTool;
}
class SVToolAdjustmentDialogSheetClass;
class SVRPropertyItem;


struct AbXxx;

class SVTADlgTranslationResizePage : 
	public CPropertyPage, 
	public SvOg::ISVPropertyPageDialog
{
public:
	SVTADlgTranslationResizePage(uint32_t inspectionID, uint32_t taskObjectID, SVToolAdjustmentDialogSheetClass* Parent, int id = IDD);
	virtual ~SVTADlgTranslationResizePage();

	HRESULT SetInspectionData(SvStl::MessageContainerVector *pErrorMessages=nullptr);

	bool	QueryAllowExit() override;

protected:
	HRESULT AddOtherInfo(SVRPropertyItem* pRoot);
	HRESULT AddInterpolationMode(SVRPropertyItem* pGroupItem);
	HRESULT AddOverScan(SVRPropertyItem* pGroupItem);
	HRESULT AddPerformance(SVRPropertyItem* pGroupItem);
	HRESULT SetupResizePropertyTree();
	HRESULT SetupResizeImageControl();
	HRESULT	ExitTabValidation();
	HRESULT ValidateCurrentTreeData (SVRPropertyItem* item);
	// DisplayRunError () is intended to deal with scenarios where an error 
	// may have already been displayed earlier in the tracking. 
	void DisplayRunError (const SvStl::MessageData& displayMessage, unsigned long programCode);

	bool ValidateNewScaleValues(AbXxx& rAbx, SvStl::MessageContainerVector* pErrorMessages); //ABXX neu!
	bool ValidateNewValuesTreeAbxx(AbXxx& rAbx, SvStl::MessageContainerVector* pErrorMessages); //ABXX neu!
	bool ValidateNewValuesComboBoxesABxx(AbXxx& rAbx, SvStl::MessageContainerVector* pErrorMessages); //ABXX neu!
	bool CommitNewValuesAbXxx(AbXxx& rAbx); //ABXX neu!


// Note:
//	UpdateImages () ------------------------------------------------------------
//  OnInitDialog () must be called first in order to initialize members.
	void UpdateImages();

// Note:
//	UpdatePropertyTreeData () ------------------------------------------------------------
//  OnInitDialog () must be called first in order to initialize members.
	HRESULT UpdatePropertyTreeData();
	HRESULT RestorePropertyTreeItemFromBackup (SVRPropertyItem* pItem);

	void UpdateScaleFactors(double widthScaleFactor, double heightScaleFactor);
	void UpdateOtherInfo();

	SVToolAdjustmentDialogSheetClass* m_ParentDialog;

//	The Tool can not be destroyed while the Tool Adjustment Dialog exists.
	SvTo::ResizeTool*	m_pTool;

	double m_HeightScaleFactor;
	double m_WidthScaleFactor;
	double m_HeightScaleFactor_New;
	double m_WidthScaleFactor_New;
	long m_SourceHeight;
	long m_SourceWidth;
	long m_OutputHeight;
	long m_OutputWidth;

	long m_SourceTabHandle;
	long m_ROITabHandle;
	long m_OutputTabHandle;

	SVRPropTree	m_Tree;
	SvOg::ImageController m_ImageController;
	uint32_t m_ROIImageID = SvDef::InvalidObjectId;
	uint32_t m_OutputImageID = SvDef::InvalidObjectId;

	const uint32_t m_inspectionID;
	const uint32_t m_toolID;

	CEdit	m_widthEdit;
	CEdit	m_heightEdit;
	CComboBox	m_InterpolationModeCombo;
	CComboBox	m_OverscanCombo;
	CComboBox	m_PerformanceCombo;


	//{{AFX_DATA(SVTADlgTranslationResizePage)
	enum { IDD = IDD_TA_RESIZE_DIALOG };
	SvOg::PictureDisplay m_DialogImage;
	//}}AFX_DATA

	// Overrides
#pragma region AFX Virtual Methods
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(SVTADlgTranslationResizePage)
	virtual BOOL OnInitDialog() override;
	virtual BOOL OnSetActive() override;
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support
	virtual BOOL OnKillActive() override;
	//}}AFX_VIRTUAL
#pragma endregion AFX Virtual Methods

#pragma region AFX MSG
	// Generated message map functions
	//{{AFX_MSG(SVTADlgTranslationResizePage)
	afx_msg void OnItemChanged(NMHDR* pNotifyStruct, LRESULT* plResult);
	afx_msg void OnItemChanged_CEditAbxx();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
#pragma endregion AFX MSG
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
