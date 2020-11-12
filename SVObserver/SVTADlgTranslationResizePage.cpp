//*****************************************************************************
/// \copyright COPYRIGHT (c) 2015,2015 by Seidenader Maschinenbau GmbH
/// All Rights Reserved 
/// \Author	Jim Brown ABX umbenennen zu SVTADlgResizePage
//*****************************************************************************

#pragma region Includes
#include "stdafx.h"
//Moved to precompiled header: #include <limits>
#include "SVOResource\resource.h"
#include "SVMessage\SVMessage.h"
#include "SVStatusLibrary/ErrorNumbers.h"
#include "SVStatusLibrary\MessageManager.h"
#include "SVImageLibrary\SVImageBufferHandleImage.h"
#include "SVTADlgTranslationResizePage.h"
#include "SVToolAdjustmentDialogSheetClass.h"
#include "Tools/ResizeTool.h"
#include "Definitions/GlobalConst.h"
#include "Definitions/StringTypeDef.h"
#include "SVUtilityLibrary/StringHelper.h"
#include "SVOGui/DataController.h"
#include "SVMatroxLibrary/SVMatroxEnums.h"
#include "SVRPropertyTree/SVRPropTree.h"
#include "SVRPropertyTree/SVRPropTreeItemCombo.h" //ABXX weg, sobald möglich
#include "SVRPropertyTree/SVRPropTreeItemStatic.h" //ABXX weg, sobald möglich
#pragma endregion Includes

enum
{
	IDC_INPUTLISTTREE = 99,
	IDC_INPUTLISTTREE_ROOT,
	IDC_INPUTLISTTREE_SCALEFACTOR,
	IDC_INPUTLISTTREE_WIDTHSCALEFACTOR,
	IDC_INPUTLISTTREE_ROIHEIGHT,
	IDC_INPUTLISTTREE_ROIWIDTH,
	IDC_INPUTLISTTREE_OUTPUTIMAGE,
	IDC_INPUTLISTTREE_OUTPUTHEIGHT,
	IDC_INPUTLISTTREE_OUTPUTWIDTH,
	IDC_INPUTLISTTREE_OTHER,
	IDC_INPUTLISTTREE_INTERPOLATIONMODE,
	IDC_INPUTLISTTREE_OVERSCAN,
	IDC_INPUTLISTTREE_PERFORMANCE
};


struct AbXxx
{
	double	oldWidthScaleFactor = 0.0;
	double	oldHeightScaleFactor = 0.0;
	SVInterpolationModeOptions::SVInterpolationModeOptionsEnum oldInterpolationValue;
	SVInterpolationModeOptions::SVInterpolationModeOptionsEnum defaultInterpolationValue;
	SVOverscanOptions::SVOverscanOptionsEnum oldOverscanValue;
	SVOverscanOptions::SVOverscanOptionsEnum defaultOverscanValue;
	SVPerformanceOptions::SVPerformanceOptionsEnum oldPerformanceValue;
	SVPerformanceOptions::SVPerformanceOptionsEnum defaultPerformanceValue;

	bool	heightScaleFactorSucceeded = false;
	bool	widthScaleFactorSucceeded = false;
	bool	interpolationModeSucceeded = false;
	bool	overscanSucceeded = false;
	bool	performanceSucceeded = false;

	SVInterpolationModeOptions::SVInterpolationModeOptionsEnum newInterpolationValue =
		SVInterpolationModeOptions::InterpolationModeInitialize;
	SVOverscanOptions::SVOverscanOptionsEnum newOverscanValue =
		SVOverscanOptions::OverscanInitialize;
	SVPerformanceOptions::SVPerformanceOptionsEnum newPerformanceValue =
		SVPerformanceOptions::PerformanceInitialize;

	double	newWidthScaleFactor = 0.0;
	double	newHeightScaleFactor = 0.0;

};



#pragma region Message Map
BEGIN_MESSAGE_MAP(SVTADlgTranslationResizePage, CPropertyPage)
	ON_NOTIFY(PTN_ITEMCHANGED, IDC_INPUTLISTTREE, OnItemChanged)
	ON_EN_KILLFOCUS(IDC_EDIT_WIDTH, OnItemChanged_CEditAbxx)
	ON_EN_KILLFOCUS(IDC_EDIT_HEIGHT, OnItemChanged_CEditAbxx)
	ON_CBN_SELCHANGE(IDC_INTERPOLATION_MODE_COMBO, OnItemChanged_CEditAbxx)
END_MESSAGE_MAP()
#pragma endregion Message Map



SVTADlgTranslationResizePage::SVTADlgTranslationResizePage(uint32_t inspectionID, uint32_t taskObjectID, SVToolAdjustmentDialogSheetClass* Parent, int id)
	: CPropertyPage(id)
	, m_pTool(nullptr)
	, m_ParentDialog(Parent)
	, m_HeightScaleFactor(1.0)
	, m_WidthScaleFactor(1.0)
	, m_HeightScaleFactor_New(1.0)
	, m_WidthScaleFactor_New(1.0)
	, m_SourceHeight(0)
	, m_SourceWidth(0)
	, m_OutputHeight(0)
	, m_OutputWidth(0)
	, m_SourceTabHandle(0)
	, m_ROITabHandle(0)
	, m_OutputTabHandle(0)
	, m_ImageController(inspectionID, taskObjectID)
	, m_inspectionID(inspectionID)
	, m_toolID(taskObjectID)
{
}

SVTADlgTranslationResizePage::~SVTADlgTranslationResizePage()
{
}

void SVTADlgTranslationResizePage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);

	//{{AFX_DATA_MAP(SVTADlgTranslationResizePage)
	DDX_Control(pDX, IDC_DIALOGIMAGE, m_DialogImage);
	DDX_Control(pDX, IDC_EDIT_WIDTH, m_widthEdit);
	DDX_Control(pDX, IDC_EDIT_HEIGHT, m_heightEdit);
	DDX_Control(pDX, IDC_INTERPOLATION_MODE_COMBO, m_InterpolationModeCombo);
	DDX_Control(pDX, IDC_OVERSCAN_COMBO, m_OverscanCombo);
	DDX_Control(pDX, IDC_PERFORMANCE_COMBO, m_PerformanceCombo);
	
	
	//}}AFX_DATA_MAP
}


BOOL SVTADlgTranslationResizePage::OnKillActive()
{
	HRESULT hr = ExitTabValidation();

	if (SUCCEEDED(hr))
	{
		if (false == CPropertyPage::OnKillActive())
		{
			hr = SVMSG_SVO_5077_COULDNOTVALIDATE;
		}
	}

	bool br = SUCCEEDED(hr); // returning false should make it stay on the 
							  // current tab.

	return br;
}


/////////////////////////////////////////////////////////////////////////////
// SVTADlgTranslationResizePage message handlers

BOOL SVTADlgTranslationResizePage::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	HRESULT hr = S_OK;
	m_ImageController.Init();
	
	if (nullptr == m_ParentDialog)
	{
		//	This should not be able to happen.  I'm not using an ASSERT 
		//	because nobody cares about a couple milliseconds in the GUI.
		hr = SVMSG_SVO_5001_NULLPARENT;
	}
	else
	{
		m_pTool = static_cast <SvTo::ResizeTool*> (m_ParentDialog->GetTaskObject());

		if (nullptr == m_pTool)
		{
			hr = SVMSG_SVO_5008_NULLTOOL;
		}
		else
		{

			m_pTool->BackupInspectionParameters();

			hr = SetupResizePropertyTree();
			if (S_OK == hr)
			{
				hr = SetupResizeImageControl();
			}
		}
	}

	if (S_OK != hr)
	{
		SvStl::MessageManager Exception(SvStl::MsgType::Log | SvStl::MsgType::Display);
		Exception.setMessage(hr, SvStl::Tid_Empty, SvStl::SourceFileParams(StdMessageParams), 5015);
	}
	else
	{
		const SvUl::NameObjectIdList& rImageList = m_ImageController.GetResultImages();
		std::string toolName = m_pTool->GetName();
		std::string tmpString = toolName + _T(".") + SvUl::LoadStdString(IDS_OBJECTNAME_ROIIMAGE);
		m_ROIImageID = SvUl::FindObjectId(rImageList, tmpString);
		tmpString = toolName + _T(".") + SvUl::LoadStdString(IDS_OBJECTNAME_IMAGE1);
		m_OutputImageID = SvUl::FindObjectId(rImageList, tmpString);
	}

	UpdateData(FALSE); // dialog being initialized.

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

BOOL SVTADlgTranslationResizePage::OnSetActive()
{
	UpdatePropertyTreeData();
	UpdateImages();
	return CPropertyPage::OnSetActive();
}

HRESULT SVTADlgTranslationResizePage::AddInterpolationMode(SVRPropertyItem* pGroupItem)
{
	HRESULT hr = S_OK;
	SVRPropertyItemCombo* comboItem = static_cast <SVRPropertyItemCombo*> (m_Tree.InsertItem(new SVRPropertyItemCombo(), pGroupItem));
	if (nullptr == comboItem)
	{
		hr = SVMSG_SVO_5035_COULDNOTINSERTINTERPOLATIONMODEITEM;
	}
	else
	{
		comboItem->SetCtrlID(IDC_INPUTLISTTREE_INTERPOLATIONMODE);
		comboItem->SetLabelText(_T("Interpolation Mode"));
		comboItem->SetInfoText(_T("Pulldown menu options: Auto (enlarge is Bilinear, reduce is Average), Bicubic, Bilinear, Nearest neighbor."));

		comboItem->CreateComboBox();

		SvVol::SVEnumerateValueObjectClass& rInterpolationMode = m_pTool->getInterpolationMode();
		std::string enumeratorName;

		rInterpolationMode.GetEnumeratorName(SVInterpolationModeOptions::InterpolationModeAuto, enumeratorName);
		long insertedIndex = comboItem->AddString(enumeratorName.c_str());
		comboItem->SetItemData(insertedIndex, SVInterpolationModeOptions::InterpolationModeAuto);

		// Average will only work with values less than 1.  
		// Auto will already use Average for values less than 1.
		rInterpolationMode.GetEnumeratorName(SVInterpolationModeOptions::InterpolationModeBicubic, enumeratorName);
		insertedIndex = comboItem->AddString(enumeratorName.c_str());
		comboItem->SetItemData(insertedIndex, SVInterpolationModeOptions::InterpolationModeBicubic);

		rInterpolationMode.GetEnumeratorName(SVInterpolationModeOptions::InterpolationModeBilinear, enumeratorName);
		insertedIndex = comboItem->AddString(enumeratorName.c_str());
		comboItem->SetItemData(insertedIndex, SVInterpolationModeOptions::InterpolationModeBilinear);

		rInterpolationMode.GetEnumeratorName(SVInterpolationModeOptions::InterpolationModeNearestNeighbor, enumeratorName);
		insertedIndex = comboItem->AddString(enumeratorName.c_str());
		comboItem->SetItemData(insertedIndex, SVInterpolationModeOptions::InterpolationModeNearestNeighbor);

		comboItem->SetItemValue(SVInterpolationModeOptions::InterpolationModeAuto); // Currently selected value.
	}

	std::string enumeratorName;

	SvVol::SVEnumerateValueObjectClass& rInterpolationMode = m_pTool->getInterpolationMode();
	rInterpolationMode.GetEnumeratorName(SVInterpolationModeOptions::InterpolationModeAuto, enumeratorName);
	m_InterpolationModeCombo.AddString(enumeratorName.c_str());
	rInterpolationMode.GetEnumeratorName(SVInterpolationModeOptions::InterpolationModeBicubic, enumeratorName);
	m_InterpolationModeCombo.AddString(enumeratorName.c_str());
	rInterpolationMode.GetEnumeratorName(SVInterpolationModeOptions::InterpolationModeBilinear, enumeratorName);
	m_InterpolationModeCombo.AddString(enumeratorName.c_str());
	rInterpolationMode.GetEnumeratorName(SVInterpolationModeOptions::InterpolationModeNearestNeighbor, enumeratorName);
	m_InterpolationModeCombo.AddString(enumeratorName.c_str());

	m_InterpolationModeCombo.SetCurSel(0); //ABXX anders, waer: auto


	return hr;
}

HRESULT SVTADlgTranslationResizePage::AddOverScan(SVRPropertyItem* pGroupItem)
{
	HRESULT hr = S_OK;
	SVRPropertyItemCombo* comboItem = static_cast <SVRPropertyItemCombo*> (m_Tree.InsertItem(new SVRPropertyItemCombo(), pGroupItem));
	if (nullptr == comboItem)
	{
		hr = SVMSG_SVO_5036_COULDNOTINSERTOVERSCANITEM;
	}
	else
	{
		comboItem->SetCtrlID(IDC_INPUTLISTTREE_OVERSCAN);
		comboItem->SetLabelText(_T("Overscan"));
		comboItem->SetInfoText(_T("Pulldown menu options: Enabled - use pixels from outside ROI for interpolation (if available), Disabled."));

		comboItem->CreateComboBox();

		SvVol::SVEnumerateValueObjectClass& rOverscan = m_pTool->getOverscan();
		std::string enumeratorName;

		rOverscan.GetEnumeratorName(SVOverscanOptions::OverscanEnable, enumeratorName);
		long insertedIndex = comboItem->AddString(enumeratorName.c_str());
		comboItem->SetItemData(insertedIndex, SVOverscanOptions::OverscanEnable);

		rOverscan.GetEnumeratorName(SVOverscanOptions::OverscanDisable, enumeratorName);
		insertedIndex = comboItem->AddString(enumeratorName.c_str());
		comboItem->SetItemData(insertedIndex, SVOverscanOptions::OverscanDisable);

		comboItem->SetItemValue(SVOverscanOptions::OverscanEnable); // Currently selected value.
	}


	SvVol::SVEnumerateValueObjectClass& rOverscan = m_pTool->getOverscan();
	std::string enumeratorName;

	rOverscan.GetEnumeratorName(SVOverscanOptions::OverscanEnable, enumeratorName);
	m_OverscanCombo.AddString(enumeratorName.c_str());

	rOverscan.GetEnumeratorName(SVOverscanOptions::OverscanDisable, enumeratorName);
	m_OverscanCombo.AddString(enumeratorName.c_str());

	m_OverscanCombo.SetCurSel(1); //ABX anders, war: SVOverscanOptions::OverscanEnable

	return hr;
}

HRESULT SVTADlgTranslationResizePage::AddPerformance(SVRPropertyItem* pGroupItem)
{
	HRESULT hr = S_OK;
	SVRPropertyItemCombo* comboItem = static_cast <SVRPropertyItemCombo*> (m_Tree.InsertItem(new SVRPropertyItemCombo(), pGroupItem));
	if (nullptr == comboItem)
	{
		hr = SVMSG_SVO_5037_COULDNOTINSERTPERFORMANCEITEM;
	}
	else
	{
		comboItem->SetCtrlID(IDC_INPUTLISTTREE_PERFORMANCE);
		comboItem->SetLabelText(_T("Performance"));
		comboItem->SetInfoText(_T("Pulldown menu options: Fast, Precise."));

		comboItem->CreateComboBox();

		SvVol::SVEnumerateValueObjectClass& rPerformance = m_pTool->getPerformance();
		std::string enumeratorName;

		rPerformance.GetEnumeratorName(SVPerformanceOptions::PerformanceFast, enumeratorName);
		long insertedIndex = comboItem->AddString(enumeratorName.c_str());
		comboItem->SetItemData(insertedIndex, SVPerformanceOptions::PerformanceFast);

		rPerformance.GetEnumeratorName(SVPerformanceOptions::PerformancePresice, enumeratorName);
		insertedIndex = comboItem->AddString(enumeratorName.c_str());
		comboItem->SetItemData(insertedIndex, SVPerformanceOptions::PerformancePresice);

		comboItem->SetItemValue(SVPerformanceOptions::PerformancePresice); // Currently selected value.
	}


	SvVol::SVEnumerateValueObjectClass& rPerformance = m_pTool->getPerformance();
	std::string enumeratorName;

	rPerformance.GetEnumeratorName(SVPerformanceOptions::PerformanceFast, enumeratorName);
	m_PerformanceCombo.AddString(enumeratorName.c_str());

	rPerformance.GetEnumeratorName(SVPerformanceOptions::PerformancePresice, enumeratorName);
	m_PerformanceCombo.AddString(enumeratorName.c_str());

	m_PerformanceCombo.SetCurSel(1); //ABXX anders, war: SVPerformanceOptions::PerformancePresice

	return hr;
}

HRESULT SVTADlgTranslationResizePage::AddOtherInfo(SVRPropertyItem* pRoot)
{
	HRESULT hr = S_OK;
	// Add Other group
	SVRPropertyItem* pGroupItem = m_Tree.InsertItem(new SVRPropertyItem(), pRoot);
	if (nullptr == pGroupItem)
	{
		hr = SVMSG_SVO_5038_COULDNOTINSERTOTHERITEM;
	}
	else
	{
		pGroupItem->SetCtrlID(IDC_INPUTLISTTREE_OTHER);
		pGroupItem->SetCanShrink(false);//(true); // Comboboxes in expand/shrink groups do not work correctly when collapsing
		pGroupItem->SetLabelText(_T("Other"));
		pGroupItem->SetInfoText(_T(""));
		pGroupItem->Expand(); // so all the children are visible
		pGroupItem->SetBold(true);
		pGroupItem->SetBackColor(::GetSysColor(COLOR_INACTIVECAPTION));
		pGroupItem->SetForeColor(::GetSysColor(COLOR_HIGHLIGHTTEXT));
		pGroupItem->SetCanHighlight(false);

		hr = AddInterpolationMode(pGroupItem);
		if (S_OK == hr)
		{
			hr = AddOverScan(pGroupItem);

			if (S_OK == hr)
			{
				hr = AddPerformance(pGroupItem);
			}
		}
	}
	return hr;
}

HRESULT SVTADlgTranslationResizePage::SetupResizePropertyTree()
{
	HRESULT	hr = S_OK;

	DWORD dwStyle;
	CRect rc;

	// PTS_NOTIFY - SVRPropTree will send notification messages to the parent window
	dwStyle = WS_CHILD | WS_VISIBLE | PTS_NOTIFY;

	// Init the control's size to cover the entire client area
	// Use dummy control for editor placement of Tree/list control.
	CWnd*	dummyControl = GetDlgItem(IDC_PROPERTY_TREE);
	if (nullptr == dummyControl)
	{
		hr = SVMSG_SVO_5002_NULLPROPERTYTREE_AREA;
	}
	else
	{
		dummyControl->GetWindowRect(rc);
		ScreenToClient(rc);

		// Create SVRPropTree control
		if (FALSE == m_Tree.Create(dwStyle, rc, this, IDC_INPUTLISTTREE))
		{
			hr = SVMSG_SVO_5004_COULDNOTCREATE;
		}
	}

	if (S_OK == hr)
	{
		m_Tree.SetColumn(150);

		SVRPropertyItem* pRoot = m_Tree.InsertItem(new SVRPropertyItem());
		if (nullptr == pRoot)
		{
			hr = SVMSG_SVO_5005_COULDNOTCREATE;
		}
		else
		{
			pRoot->SetCanShrink(false);
			pRoot->SetLabelText(_T(""));
			pRoot->SetInfoText(_T(""));
			pRoot->SetCtrlID(IDC_INPUTLISTTREE_ROOT);

		}
		if (S_OK == hr)
		{
			hr = AddOtherInfo(pRoot);
		}
		if (S_OK == hr)
		{
			hr = UpdatePropertyTreeData();

			if (S_OK == hr)
			{
				if (nullptr == m_ParentDialog)
				{
					//			This should not be able to happen.  I'm not using an ASSERT 
					//			because nobody cares about a couple milliseconds in the GUI.
					hr = SVMSG_SVO_5007_NULLPARENT;
				}
				else
				{
					pRoot->Expand(true);	// needed for redrawing
				}
			}
		}
	}
	return hr;
}


void SVTADlgTranslationResizePage::UpdateScaleFactors(double newWidthScaleFactor, double newHeightScaleFactor)
{
	CString scaleString;
	m_heightEdit.GetWindowText(scaleString);
	double oldHeightScaleFactor = atof(scaleString);

	if (newHeightScaleFactor != oldHeightScaleFactor) // reduces flicker.
	{
		scaleString.Format("%lf", newHeightScaleFactor);
		m_heightEdit.SetWindowText(scaleString);
		m_HeightScaleFactor = newHeightScaleFactor;
	}

	m_widthEdit.GetWindowText(scaleString);
	double oldWidthScaleFactor = atof(scaleString);
	if (newWidthScaleFactor != oldWidthScaleFactor) // reduces flicker.
	{
		scaleString.Format("%lf", newWidthScaleFactor);
		m_widthEdit.SetWindowText(scaleString);
		m_WidthScaleFactor = newWidthScaleFactor;
	}
}


void SVTADlgTranslationResizePage::UpdateOtherInfo()
{
	SVRPropertyItemCombo* comboItem = static_cast <SVRPropertyItemCombo*> (m_Tree.FindItem(IDC_INPUTLISTTREE_INTERPOLATIONMODE));
	SvVol::SVEnumerateValueObjectClass& rInterpolationMode = m_pTool->getInterpolationMode();
	long oldInterpolationValue;
	long newInterpolationValue;
	rInterpolationMode.GetValue(newInterpolationValue);
	comboItem->GetItemValue(oldInterpolationValue);
	if (newInterpolationValue != oldInterpolationValue)
	{
		comboItem->SetItemValue(newInterpolationValue);
	}

	comboItem = static_cast <SVRPropertyItemCombo*> (m_Tree.FindItem(IDC_INPUTLISTTREE_OVERSCAN));
	SvVol::SVEnumerateValueObjectClass& rOverscan = m_pTool->getOverscan();
	long oldOverscanValue;
	long newOverscanValue;
	rOverscan.GetValue(newOverscanValue);
	comboItem->GetItemValue(oldOverscanValue);
	if (newOverscanValue != oldOverscanValue)
	{
		comboItem->SetItemValue(newOverscanValue);
	}

	comboItem = static_cast <SVRPropertyItemCombo*> (m_Tree.FindItem(IDC_INPUTLISTTREE_PERFORMANCE));
	SvVol::SVEnumerateValueObjectClass& rPerformance = m_pTool->getPerformance();
	long oldPerformanceValue;
	long newPerformanceValue;
	rPerformance.GetValue(newPerformanceValue);
	comboItem->GetItemValue(oldPerformanceValue);
	if (newPerformanceValue != oldPerformanceValue)
	{
		comboItem->SetItemValue(newPerformanceValue);
	}
}

HRESULT SVTADlgTranslationResizePage::UpdatePropertyTreeData()
{
	::google::protobuf::RepeatedPtrField< ::SvPb::ExtentParameter > extents;
	SvPb::InspectionCmdRequest requestCmd;
	SvPb::InspectionCmdResponse responseCmd;
	auto* pRequest = requestCmd.mutable_getextentparameterrequest();
	pRequest->set_objectid(m_toolID);

	HRESULT hr = SvCmd::InspectionCommands(m_inspectionID, requestCmd, &responseCmd);
	if (S_OK == hr && responseCmd.has_getextentparameterresponse())
	{
		extents = responseCmd.getextentparameterresponse().parameters();
	}
	else
	{
		return E_POINTER;
	}

	double newHeightScaleFactor = SvCmd::getValueForProperties<double>(extents, SvPb::SVExtentPropertyHeightScaleFactor);
	double newWidthScaleFactor = SvCmd::getValueForProperties<double>(extents, SvPb::SVExtentPropertyWidthScaleFactor);
	UpdateScaleFactors(newWidthScaleFactor, newHeightScaleFactor);

	m_WidthScaleFactor_New = newWidthScaleFactor;
	m_HeightScaleFactor_New = newHeightScaleFactor;

	m_widthEdit.SetWindowText(std::to_string(newWidthScaleFactor).c_str());
	m_heightEdit.SetWindowText(std::to_string(newHeightScaleFactor).c_str());

	UpdateOtherInfo();
	return S_OK;
}


HRESULT SVTADlgTranslationResizePage::SetInspectionData(SvStl::MessageContainerVector *pErrorMessages)
{
	AbXxx abx;

	// Retrieve current values --------------------------------------------------
	::google::protobuf::RepeatedPtrField< ::SvPb::ExtentParameter > extents;
	SvPb::InspectionCmdRequest requestCmd;
	SvPb::InspectionCmdResponse responseCmd;
	auto* pRequest = requestCmd.mutable_getextentparameterrequest();
	pRequest->set_objectid(m_toolID);

	HRESULT hr1 = SvCmd::InspectionCommands(m_inspectionID, requestCmd, &responseCmd);
	if (S_OK == hr1 && responseCmd.has_getextentparameterresponse())
	{
		extents = responseCmd.getextentparameterresponse().parameters();
	}
	else
	{
		return E_POINTER;
	}

	abx.oldHeightScaleFactor = SvCmd::getValueForProperties<double>(extents, SvPb::SVExtentPropertyHeightScaleFactor);
	abx.oldWidthScaleFactor = SvCmd::getValueForProperties<double>(extents, SvPb::SVExtentPropertyWidthScaleFactor);

	SvVol::SVEnumerateValueObjectClass& rInterpolationMode = m_pTool->getInterpolationMode();
	long lValue{0L};
	rInterpolationMode.GetValue(lValue);
	abx.oldInterpolationValue = static_cast<SVInterpolationModeOptions::SVInterpolationModeOptionsEnum> (lValue);
	lValue = rInterpolationMode.GetDefaultValue();
	abx.defaultInterpolationValue = static_cast<SVInterpolationModeOptions::SVInterpolationModeOptionsEnum> (lValue);

	SvVol::SVEnumerateValueObjectClass& rOverscan = m_pTool->getOverscan();
	rOverscan.GetValue(lValue);
	abx.oldOverscanValue = static_cast<SVOverscanOptions::SVOverscanOptionsEnum> (lValue);
	lValue = rOverscan.GetDefaultValue();
	abx.defaultOverscanValue = static_cast<SVOverscanOptions::SVOverscanOptionsEnum> (lValue);

	SvVol::SVEnumerateValueObjectClass& rPerformance = m_pTool->getPerformance();
	rPerformance.GetValue(lValue);
	abx.oldPerformanceValue = static_cast<SVPerformanceOptions::SVPerformanceOptionsEnum> (lValue);
	lValue = rPerformance.GetDefaultValue();
	abx.defaultPerformanceValue = static_cast<SVPerformanceOptions::SVPerformanceOptionsEnum> (lValue);

	// Validate new values --------------------------------------------------


	if (!ValidateNewScaleValues(abx, pErrorMessages))
	{
		return SVMSG_SVO_5002_NULLPROPERTYTREE_AREA; //ABXX sollte eigntlich nur 'false' sein
	}


	if (!ValidateNewValuesComboBoxesABxx(abx, pErrorMessages))
	{
		return SVMSG_SVO_5002_NULLPROPERTYTREE_AREA; //ABXX sollte eigntlich nur 'false' sein
	}

	if (!ValidateNewValuesTreeAbxx(abx, pErrorMessages))
	{
		return SVMSG_SVO_5002_NULLPROPERTYTREE_AREA; //ABXX sollte eigntlich nur 'false' sein
	}

	return CommitNewValuesAbXxx(abx) ? S_OK : SVMSG_SVO_5002_NULLPROPERTYTREE_AREA; //ABXX sollte eigntlich nur 'false' sein
}

bool SVTADlgTranslationResizePage::ValidateNewScaleValues(AbXxx& rAbx, SvStl::MessageContainerVector* pErrorMessages)
{
	CString scaleString;
	m_heightEdit.GetWindowText(scaleString);
	rAbx.newHeightScaleFactor = atof(scaleString);

	if (!m_pTool->ValidateScaleFactor(rAbx.newHeightScaleFactor, pErrorMessages))
	{
		rAbx.newHeightScaleFactor = rAbx.oldHeightScaleFactor;
		if (!m_pTool->ValidateScaleFactor(rAbx.newHeightScaleFactor, pErrorMessages))
		{
			// old values were also invalid.  Currently this can happen 
			// with corruption or with Remote Access putting in an 
			// invalid value.
			rAbx.newHeightScaleFactor = SvDef::cDefaultWindowToolHeightScaleFactor;
		}

		scaleString.Format("%lf", rAbx.newHeightScaleFactor);
		m_heightEdit.SetWindowText(scaleString);
		return false;
	}

	if (rAbx.newHeightScaleFactor != rAbx.oldHeightScaleFactor)
	{
		rAbx.heightScaleFactorSucceeded = true;
	}

	m_widthEdit.GetWindowText(scaleString);
	rAbx.newWidthScaleFactor = atof(scaleString);

	if (!m_pTool->ValidateScaleFactor(rAbx.newWidthScaleFactor, pErrorMessages))
	{
		// Expected codes include...
		//  SVMSG_SVO_5061_SFOUTSIDERANGE
		rAbx.newWidthScaleFactor = rAbx.oldWidthScaleFactor;
		if (!m_pTool->ValidateScaleFactor(rAbx.newWidthScaleFactor, pErrorMessages))
		{
			// old values were also invalid.  Currently this can happen 
			// with corruption or with Remote Access putting in an 
			// invalid value.
			rAbx.newWidthScaleFactor = SvDef::cDefaultWindowToolWidthScaleFactor;
		}

		scaleString.Format("%lf", rAbx.newWidthScaleFactor);
		m_widthEdit.SetWindowText(scaleString);
		return false;
	}

	if (rAbx.newWidthScaleFactor != rAbx.oldWidthScaleFactor)
	{
		rAbx.widthScaleFactorSucceeded = true;
	}

	return true;
}


#define ABxxNew

bool SVTADlgTranslationResizePage::ValidateNewValuesTreeAbxx(AbXxx& rAbx, SvStl::MessageContainerVector* pErrorMessages)
{
#ifndef ABxxNew  //ABXXXX
	SVRPropertyItemCombo* pComboItem = static_cast <SVRPropertyItemCombo*> (m_Tree.FindItem(IDC_INPUTLISTTREE_INTERPOLATIONMODE));
	pComboItem->GetItemValue(*(reinterpret_cast <long*> (&rAbx.newInterpolationValue)));

	if (!m_pTool->ValidateInterpolation(rAbx.newInterpolationValue, pErrorMessages))
	{
		rAbx.newInterpolationValue = rAbx.oldInterpolationValue;
		if (!m_pTool->ValidateInterpolation(rAbx.newInterpolationValue, pErrorMessages))
		{
			// old values were also invalid.  Currently this can happen 
			// with corruption or with Remote Access putting in an 
			// invalid value.
			rAbx.newInterpolationValue = rAbx.defaultInterpolationValue;
		}
		pComboItem->SetItemValue(rAbx.newInterpolationValue);
		return false;
	}

	if (rAbx.newInterpolationValue != rAbx.oldInterpolationValue)
	{
		rAbx.interpolationModeSucceeded = true;
	}
		
	pComboItem = static_cast <SVRPropertyItemCombo*> (m_Tree.FindItem(IDC_INPUTLISTTREE_OVERSCAN));
#else
	SVRPropertyItemCombo* pComboItem = static_cast <SVRPropertyItemCombo*> (m_Tree.FindItem(IDC_INPUTLISTTREE_OVERSCAN));
#endif

	pComboItem->GetItemValue(*(reinterpret_cast <long*> (&(rAbx.newOverscanValue))));

	if (!m_pTool->ValidateOverscan(rAbx.newOverscanValue, pErrorMessages))
	{
		rAbx.newOverscanValue = rAbx.oldOverscanValue;
		if (!m_pTool->ValidateOverscan(rAbx.newOverscanValue, pErrorMessages))
		{
			// old values were also invalid.  Currently this can happen 
			// with corruption or with Remote Access putting in an 
			// invalid value.
			rAbx.newOverscanValue = rAbx.defaultOverscanValue;
		}
		pComboItem->SetItemValue(rAbx.newOverscanValue);
		return false;
	}

	if (rAbx.newOverscanValue != rAbx.oldOverscanValue)
	{
		rAbx.overscanSucceeded = true;
	}

	pComboItem = static_cast <SVRPropertyItemCombo*> (m_Tree.FindItem(IDC_INPUTLISTTREE_PERFORMANCE));
	pComboItem->GetItemValue(*(reinterpret_cast <long*> (&rAbx.newPerformanceValue)));
	if (!m_pTool->ValidatePerformance(rAbx.newPerformanceValue, pErrorMessages))
	{
		rAbx.newPerformanceValue = rAbx.oldPerformanceValue;
		if (!m_pTool->ValidatePerformance(rAbx.newPerformanceValue, pErrorMessages))
		{
			// old values were also invalid.  Currently this can happen 
			// with corruption or with Remote Access putting in an 
			// invalid value.
			rAbx.newPerformanceValue = rAbx.defaultPerformanceValue;
		}
		pComboItem->SetItemValue(rAbx.oldPerformanceValue);
		return false;
	}

	if (rAbx.newPerformanceValue != rAbx.oldPerformanceValue)
	{
		rAbx.performanceSucceeded = true;
	}

	return true;
}


bool SVTADlgTranslationResizePage::ValidateNewValuesComboBoxesABxx(AbXxx& rAbx, SvStl::MessageContainerVector* pErrorMessages)
{
#ifdef ABxxNew  //ABXXXX

	SvVol::SVEnumerateValueObjectClass& rInterpolationMode = m_pTool->getInterpolationMode();

	CString abxT;
	m_InterpolationModeCombo.GetLBText(m_InterpolationModeCombo.GetCurSel(), abxT);

	long ABxx = -1;
	rInterpolationMode.GetEnumerator(abxT.GetBuffer(0), ABxx);

	rAbx.newInterpolationValue = (SVInterpolationModeOptions::SVInterpolationModeOptionsEnum) ABxx; //ABXX cast


	if (!m_pTool->ValidateInterpolation(rAbx.newInterpolationValue, pErrorMessages))
	{
		rAbx.newInterpolationValue = rAbx.oldInterpolationValue;
		if (!m_pTool->ValidateInterpolation(rAbx.newInterpolationValue, pErrorMessages))
		{
			// old values were also invalid.  Currently this can happen 
			// with corruption or with Remote Access putting in an 
			// invalid value.
			rAbx.newInterpolationValue = rAbx.defaultInterpolationValue;
		}
		m_InterpolationModeCombo.SetCurSel((int)rAbx.newInterpolationValue);//ABXX C-Style cast!
		return false;
	}

	if (rAbx.newInterpolationValue != rAbx.oldInterpolationValue)
	{
		rAbx.interpolationModeSucceeded = true;
	}
	

#endif
	return true;
}

bool SVTADlgTranslationResizePage::CommitNewValuesAbXxx(AbXxx& rAbx)
{
	//@TODO[gra][8.00][15.01.2018]: The data controller should be used like the rest of SVOGui
	SvOg::ValueController Values{ SvOg::BoundValues{ m_inspectionID, m_toolID } };
	Values.Init();

	// Set new values -------------------------------------------------------
// Want to do as much error checking as possible before calling 
// commit, which can not be easily backed out.  Restoring from
// an error prior to this point is easier than restoring from an error 
// after this point.

	bool	extentChanged = false;
	bool	embeddedChanged = false;
	::google::protobuf::RepeatedPtrField< ::SvPb::ExtentParameter > extents;
	SvPb::InspectionCmdRequest requestCmd;
	SvPb::InspectionCmdResponse responseCmd;
	auto* pRequest = requestCmd.mutable_getextentparameterrequest();
	pRequest->set_objectid(m_toolID);

	HRESULT hr1 = SvCmd::InspectionCommands(m_inspectionID, requestCmd, &responseCmd);
	if (S_OK == hr1 && responseCmd.has_getextentparameterresponse())
	{
		extents = responseCmd.getextentparameterresponse().parameters();
	}
	else
	{
		return false;
	}

	if (rAbx.heightScaleFactorSucceeded) // reduces flicker.
	{
		SvCmd::setValueForProperties<double>(extents, SvPb::SVExtentPropertyHeightScaleFactor, rAbx.newHeightScaleFactor);
		extentChanged = true;
	}

	if (rAbx.widthScaleFactorSucceeded) // reduces flicker.
	{
		SvCmd::setValueForProperties<double>(extents, SvPb::SVExtentPropertyWidthScaleFactor, rAbx.newWidthScaleFactor);
		extentChanged = true;
	}

	if (rAbx.interpolationModeSucceeded)
	{
		SVObjectClass* pObject = dynamic_cast<SVObjectClass*> (&m_pTool->getInterpolationMode());
		Values.Set<long>(pObject->GetEmbeddedID(), rAbx.newInterpolationValue);
		embeddedChanged = true;
	}

	if (rAbx.overscanSucceeded)
	{
		SVObjectClass* pObject = dynamic_cast<SVObjectClass*> (&m_pTool->getOverscan());
		Values.Set<long>(pObject->GetEmbeddedID(), rAbx.newOverscanValue);
		embeddedChanged = true;
	}

	if (rAbx.performanceSucceeded)
	{
		SVObjectClass* pObject = dynamic_cast<SVObjectClass*> (&m_pTool->getPerformance());
		Values.Set<long>(pObject->GetEmbeddedID(), rAbx.newPerformanceValue);
		embeddedChanged = true;
	}

	if (embeddedChanged)
	{
		Values.Commit();
	}

	if (extentChanged)
	{
		requestCmd.Clear();
		auto* pSetExtentRequest = requestCmd.mutable_setextentparameterrequest();
		pSetExtentRequest->set_objectid(m_toolID);
		pSetExtentRequest->mutable_extentlist()->mutable_extentlist()->MergeFrom(extents);
		SvCmd::InspectionCommands(m_inspectionID, requestCmd, nullptr);
	}

	if (extentChanged || embeddedChanged)
	{
		SvCmd::RunOnceSynchronous(m_inspectionID);
		UpdateImages();
		m_pTool->BackupInspectionParameters();
	}


	return true;
}


bool SVTADlgTranslationResizePage::QueryAllowExit()
{
	HRESULT hr = ExitTabValidation();

	bool br = SUCCEEDED(hr); // returning false should make it stay on the 
							  // current tab.
	return br;
}


HRESULT	SVTADlgTranslationResizePage::ExitTabValidation()
{
	// if changes in the property are pending, CommitChanges() will fire off 
	// OnItemChanged(). Because OnItemChanged() will in turn go into 
	// RunOnce() and ResetObject() logic, we will attempt to retrieve 
	// we will only be able to track results from the Run Error Code.
	m_pTool->clearTaskMessages();

	// It is possible for no item to be in focus if the table was not clicked 
	// on.  In which case nothing has changed.
	SVRPropertyItem* item = m_Tree.GetFocusedItem();
	if (nullptr != item)
	{
		item->CommitChanges();
	}

	SvStl::MessageContainer message = m_pTool->getFirstTaskMessage();

	// above validates the parameters from the translation tab.
	// below validates the whole tool.
	if (SUCCEEDED(message.getMessage().m_MessageCode))
	{
		bool br = m_pTool->resetAllObjects();
		message = m_pTool->getFirstTaskMessage();
		if (br != SUCCEEDED(message.getMessage().m_MessageCode))
		{
			message.clearMessage();
			message.setMessage(SvStl::MessageData(SVMSG_SVO_5078_INCONSISTENTDATA));
		}
	}

	if (SUCCEEDED(message.getMessage().m_MessageCode))
	{
		SvCmd::RunOnceSynchronous(m_inspectionID);
	}

	if (!SUCCEEDED(message.getMessage().m_MessageCode))
	{
		SvStl::MessageManager Exception(SvStl::MsgType::Log | SvStl::MsgType::Display);
		Exception.setMessage(message.getMessage().m_MessageCode, SvStl::Tid_Empty, SvStl::SourceFileParams(StdMessageParams), SvStl::ProgCode_5068_ValidateTabData);
	}

	return message.getMessage().m_MessageCode;
}


HRESULT SVTADlgTranslationResizePage::SetupResizeImageControl()
{
	HRESULT	hr = S_OK;

	if (S_OK != m_DialogImage.AddTab(_T("Source ROI"), &m_ROITabHandle))
	{
		hr = SVMSG_SVO_5016_COULDNOTADDTAB;
	}
	else
	{
		if (S_OK != m_DialogImage.AddTab(_T("Result"), &m_OutputTabHandle))
		{
			hr = SVMSG_SVO_5017_COULDNOTADDTAB;
		}
		else
		{
			UpdateImages();
		}
	}
	return hr;
}

void SVTADlgTranslationResizePage::UpdateImages()
{
	if (SvDef::InvalidObjectId != m_ROIImageID && SvDef::InvalidObjectId != m_OutputImageID)
	{
		IPictureDisp* pROIImage = m_ImageController.GetImage(m_ROIImageID);
		IPictureDisp* pOutputImage = m_ImageController.GetImage(m_OutputImageID);
		m_DialogImage.setImage(pROIImage, m_ROITabHandle);
		m_DialogImage.setImage(pOutputImage, m_OutputTabHandle);
	}
	m_DialogImage.Refresh();
}


void SVTADlgTranslationResizePage::OnItemChanged(NMHDR* pNotifyStruct, LRESULT* plResult)
{
	LPNMPROPTREE pNMPropTree = reinterpret_cast<LPNMPROPTREE>(pNotifyStruct);
	*plResult = S_OK;  // the OS (DefWindowProc) is not required to process this 
					   // message.

	ValidateCurrentTreeData(pNMPropTree->pItem);
}


void SVTADlgTranslationResizePage::OnItemChanged_CEditAbxx()
{
	ValidateCurrentTreeData(nullptr);
}



HRESULT SVTADlgTranslationResizePage::ValidateCurrentTreeData(SVRPropertyItem* item)
{
	SvStl::MessageContainer message;
	SvStl::MessageContainerVector messageList;

	// Some items are checked before the RunOnce, and then the RunOnce is 
	// bypassed.  So the clear must be explicitly called.
	m_pTool->clearTaskMessages();

	HRESULT	hr = SetInspectionData(&messageList);

	if (!SUCCEEDED(hr))
	{
		if (messageList.empty())
		{
			// SetInspectionData() ends up calling RunOnce and ResetObjects.  
			// Error handling within these processes is not complete. The 
			// maintains members to help pass error data outside these 
			// operations.
			message = m_pTool->getFirstTaskMessage();

			if (((SVMSG_SVO_5067_IMAGEALLOCATIONFAILED == hr) ||
				(SVMSG_SVO_5061_SFOUTSIDERANGE == hr)) &&
				(nullptr != item))
			{
				if (SVMSG_SVO_5067_IMAGEALLOCATIONFAILED == hr)
				{
					// more specific message for this context.
					hr = SVMSG_SVO_5070_IMAGEALLOCATIONFAILED;
				}

				SvDef::StringVector msgList;
				msgList.push_back(item->GetLabelText());
				message.setMessage(hr, SvStl::Tid_Default, msgList, SvStl::SourceFileParams(StdMessageParams));
			}
		}
		else
		{
			message = messageList[0];
		}

		SvStl::MessageManager Exception(SvStl::MsgType::Log | SvStl::MsgType::Display);
		Exception.setMessage(message.getMessage());
	}

	if (!SUCCEEDED(hr))
	{
		if (nullptr != item)
		{
			// Attempt to restore to previous values.
			RestorePropertyTreeItemFromBackup(item);
		}

		// It is understood that we are losing the previous error.
		hr = SetInspectionData();
	}

	if (SUCCEEDED(hr))
	{
		hr = UpdatePropertyTreeData();
	}

	return hr;
}

HRESULT SVTADlgTranslationResizePage::RestorePropertyTreeItemFromBackup(SVRPropertyItem* pItem)
{
	HRESULT hr = S_OK;

	UINT controlID = pItem->GetCtrlID();

	double	oldWidthScaleFactor = 0.0;
	double	oldHeightScaleFactor = 0.0;
	long	oldInterpolationModeValue = 0;
	long	oldOverscanValue = 0;
	long	oldPerformanceValue = 0;

	m_pTool->GetBackupInspectionParameters(&oldHeightScaleFactor, //ABXXX was ist mit diesen beiden?
		&oldWidthScaleFactor,
		&oldInterpolationModeValue,
		&oldOverscanValue,
		&oldPerformanceValue);

	switch (controlID)
	{
		case IDC_INPUTLISTTREE_INTERPOLATIONMODE:
		{
			pItem->SetItemValue(oldInterpolationModeValue);
			break;
		}
		case IDC_INPUTLISTTREE_OVERSCAN:
		{
			pItem->SetItemValue(oldOverscanValue);
			break;
		}

		case IDC_INPUTLISTTREE_PERFORMANCE:
		{
			pItem->SetItemValue(oldPerformanceValue);
			break;
		}
		default:
		{
			//  ingore for now.
			break;
		}
	}

	return hr;
}


