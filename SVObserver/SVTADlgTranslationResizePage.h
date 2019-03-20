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

class SVTADlgTranslationResizePage : 
	public CPropertyPage, 
	public SvOg::ISVPropertyPageDialog
{
public:
	SVTADlgTranslationResizePage( const SVGUID& rInspectionID, const SVGUID& rTaskObjectID, SVToolAdjustmentDialogSheetClass* Parent, int id = IDD );
	virtual ~SVTADlgTranslationResizePage();

	HRESULT SetInspectionData(SvStl::MessageContainerVector *pErrorMessages=nullptr);

	bool	QueryAllowExit() override;

protected:
	HRESULT AddScaleFactors(SVRPropertyItem* pRoot);
	HRESULT AddInputImageInfo(SVRPropertyItem* pRoot);
	HRESULT AddOutputImageInfo(SVRPropertyItem* pRoot);
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
	void	DisplayRunError (const SvStl::MessageData& displayMessage, unsigned long programCode);


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
	void UpdateInputImageInfo(long width, long height);
	void UpdateOutputImageInfo(long width, long height);
	void UpdateOtherInfo();

	SVToolAdjustmentDialogSheetClass* m_ParentDialog;

//	The Tool can not be destroyed while the Tool Adjustment Dialog exists.
	SvTo::ResizeTool*	m_pTool;

	double m_HeightScaleFactor;
	double m_WidthScaleFactor;
	long m_SourceHeight;
	long m_SourceWidth;
	long m_OutputHeight;
	long m_OutputWidth;

	long m_SourceTabHandle;
	long m_ROITabHandle;
	long m_OutputTabHandle;

	SVRPropTree	m_Tree;
	SvOg::ImageController m_ImageController;
	SVGUID m_ROIImageID = GUID_NULL;
	SVGUID m_OutputImageID = GUID_NULL;

	const SVGUID m_inspectionID;
	const SVGUID m_toolID;

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
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
#pragma endregion AFX MSG
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
