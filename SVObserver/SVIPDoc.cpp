//******************************************************************************
//* COPYRIGHT (c) 2008 by Körber Pharma Inspection GmbH. All Rights Reserved, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVIPDoc
//* .File Name       : $Workfile:   SVIPDoc.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.31  $
//* .Check In Date   : $Date:   09 Jan 2015 01:40:54  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
//Moved to precompiled header: #include <comdef.h>
//Moved to precompiled header: #include <algorithm>
#include "SVIPDoc.h"
#include "ExtrasEngine.h"
#include "ResultTabbedView.h"
#include "SVCommandInspectionCollectImageData.h"
#include "SVConfigurationObject.h"
#include "SVDataDefinitionSheet.h"
#include "SVDirectX.h"
#include "SVGuiExtentUpdater.h"
#include "SVGlobal.h"
#include "SVImageViewScroll.h"
#include "SVImageView.h"
#include "SVIODoc.h"
#include "SVIPSplitterFrame.h"
#include "SVInspectionProcess.h"
#include "SVLightReferenceDialog.h"
#include "SVLutDlg.h"
#include "SVMainFrm.h"
#include "SVMultiDocTemplate.h"
#include "SVObserverOuttakes.h"
#include "SVObserver.h"
#include "SVSecurity/SVSecurityManager.h"
#include "SVPPQObject.h"
#include "SVRegressionRunDlg.h"
#include "SVToolAdjustmentDialogSheetClass.h"
#include "SheetForExternalToolAdjustment.h"
#include "SVToolSet.h"
#include "SVUtilities.h"
#include "ToolClipboard.h"
#include "ToolSetView.h"
#include "Definitions/GlobalConst.h"
#include "Definitions/StringTypeDef.h"
#include "Definitions/SVUserMessage.h"
#include "InspectionCommands/CommandExternalHelper.h"
#include "InspectionEngine/SVImageProcessingClass.h"
#include "InspectionEngine/SVAcquisitionClass.h"
#include "ObjectInterfaces/IDependencyManager.h"
#include "ObjectInterfaces/IObjectWriter.h"
#include "ObjectSelectorLibrary/ObjectTreeGenerator.h"
#include "Operators/SVConditional.h"
#include "SVFileSystemLibrary/SVFileNameManagerClass.h"
#include "SVHBitmapUtilitiesLibrary\SVHBitmapUtilities.h"
#include "SVIOLibrary/SVOutputObject.h"
#include "SVMessage/SVMessage.h"
#include "SVMFCControls/SVFileDialog.h"
#include "SVObjectLibrary/SVObjectLevelCreateStruct.h"
#include "SVObjectLibrary/SVObjectManagerClass.h"
#include "SVOGui/InputConflictDlg.h"
#include "SVOGui/ResultTableSelectionDlg.h"
#include "SVOGui/SVAdjustToolSizePositionDlg.h"
#include "SvOGui/SVFormulaEditorSheet.h"
#include "SVOGui/SVSaveToolSetImageDialog.h"
#include "SVOGui/SVShowDependentsDialog.h"
#include "SVOGui/TextDefinesSvOg.h"
#include "SVOResource/ConstGlobalSvOr.h"
#include "SVProtoBuf/InspectionCommands.h"
#include "SVStatusLibrary/SVSVIMStateClass.h"
#include "SVUtilityLibrary/StringHelper.h"
#include "SVXMLLibrary/SVConfigurationTags.h"
#include "SVXMLLibrary/SVNavigateTree.h"
#include "SVXMLLibrary/SVObjectXMLWriter.h"
#include "Tools/LoopTool.h"
#include "Tools/SVTool.h"
#include "Triggering/SVTriggerObject.h"
#include "ObjectInterfaces/ObjectInfo.h"
#include "SVStatusLibrary/MessageTextEnum.h"
#include "SVVisionProcessorHelper.h"
#pragma endregion Includes

#pragma region Declarations

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

union SVViewUnion
{
	CView* pView {nullptr};	//Note for union only one field can be initialized
	SVImageViewScroll* pImageScroll;
	ToolSetView* pToolsetView;
	ResultTabbedView* pResultView;
};

constexpr const char* cSVImageViewScrollName = _T("SVImageViewScroll");
constexpr const char* cSVImageViewName = _T("SVImageView");
constexpr const char* cToolSetViewName = _T("ToolSetView");
constexpr const char* cResultTabbedViewName = _T("ResultTabbedView");
constexpr int MaxImageViews = 8;

IMPLEMENT_DYNCREATE(SVIPDoc, CDocument)

//******************************************************************************
// Message Map Entries
//******************************************************************************
BEGIN_MESSAGE_MAP(SVIPDoc, CDocument)
	//{{AFX_MSG_MAP(SVIPDoc)
	ON_COMMAND(ID_ADD_SHIFTTOOL, OnAddShiftTool)
	ON_COMMAND(ID_ADD_WINDOWTOOL, OnAddWindowTool)
	ON_COMMAND(ID_ADD_CYLINDRICALWARPTOOL, OnAddCylindricalWarpTool)
	ON_COMMAND(ID_EDIT_EDITTOOL, OnEditTool)
	ON_COMMAND(ID_EDIT_EDITTOOLTAB1, OnEditToolTab1)
	ON_COMMAND(ID_EDIT_CUT, OnEditCut)
	ON_UPDATE_COMMAND_UI(ID_EDIT_CUT, OnUpdateEditCutCopy)
	ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
	ON_UPDATE_COMMAND_UI(ID_EDIT_COPY, OnUpdateEditCutCopy)
	ON_COMMAND(ID_EDIT_PASTE, OnEditPaste)
	ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE, OnUpdateEditPaste)
	ON_COMMAND(ID_SHOW_FIRST_ERROR, OnShowFirstError)
	ON_UPDATE_COMMAND_UI(ID_SHOW_FIRST_ERROR, OnUpdateShowFirstError)
	ON_COMMAND(ID_ADD_TOOL_PARA_TO_MONITORLIST, OnAddParameterToMonitorList)
	ON_UPDATE_COMMAND_UI(ID_ADD_TOOL_PARA_TO_MONITORLIST, OnUpdateAddParameterToMonitorList)
	ON_COMMAND(ID_REMOVE_TOOL_PARA_TO_MONITORLIST, OnRemoveParameterToMonitorList)
	ON_UPDATE_COMMAND_UI(ID_REMOVE_TOOL_PARA_TO_MONITORLIST, OnUpdateRemoveParameterToMonitorList)
	ON_COMMAND(ID_EDIT_DELETE, OnEditDelete)
	ON_COMMAND(ID_EDIT_ADJUSTLIGHTREFERENCE, OnAdjustLightReference)
	ON_COMMAND(ID_EDIT_ADJUSTLUT, OnAdjustLut)
	ON_COMMAND(ID_FILE_SAVE_IMAGE, OnFileSaveImage)
	ON_COMMAND(ID_ADD_IMAGETOOL, OnAddImageTool)
	ON_COMMAND(ID_ADD_ARCHIVETOOL, OnAddArchiveTool)
	ON_COMMAND(ID_ADD_MATHTOOL, OnAddMathTool)
	ON_COMMAND(ID_ADD_STATISTICSTOOL, OnAddStatisticsTool)
	ON_COMMAND(ID_EDIT_EDITTOOLSET, OnEditToolSet)
	ON_COMMAND(ID_RESULTS_PICKER, OnResultsPicker)
	ON_COMMAND(ID_RESULTS_TABLE_PICKER, OnResultsTablePicker)
	ON_COMMAND(ID_SAVE_RESULTS_TO_FILE, OnSaveResultsToFile)
	ON_COMMAND(ID_SAVE_RESULTSTABLE_TO_FILE, OnSaveTableResultsToFile)
	ON_COMMAND(ID_PUBLISHED_RESULTS_PICKER, OnPublishedResultsPicker)
	ON_COMMAND(ID_PUBLISHED_RESULT_IMAGES_PICKER, OnPublishedResultImagesPicker)
	ON_COMMAND(ID_ADD_LOADIMAGETOOL, OnAddLoadImageTool)
	ON_COMMAND(ID_RUN_REGRESSIONTEST, RunRegressionTest)
	ON_COMMAND(ID_ADD_ACQUISITIONTOOL, OnAddAcquisitionTool)
	ON_COMMAND(ID_ADD_TRANSFORMATIONTOOL, OnAddTransformationTool)
	ON_UPDATE_COMMAND_UI(ID_VIEW_TOOLSETDRAW_POP_BASE, OnUpdateViewToolSetDraw)
	ON_COMMAND(ID_ADD_POLARUNWRAPTOOL, OnAddPolarUnwrapTool)
	ON_UPDATE_COMMAND_UI(ID_APP_EXIT, OnUpdateFileExit)
	ON_COMMAND(ID_ADD_COLORTOOL, OnAddColorTool)
	ON_COMMAND(ID_ADD_EXTERNAL_TOOL, OnAddExternalTool)
	ON_COMMAND(ID_ADD_LINEARTOOL, OnAddLinearTool)
	ON_COMMAND(ID_EDIT_ADJUSTTOOLPOSITION, OnEditAdjustToolPosition)
	ON_UPDATE_COMMAND_UI(ID_EDIT_ADJUSTTOOLPOSITION, OnUpdateEditAdjustToolPosition)
	ON_COMMAND(ID_ADD_PERSPECTIVEWARPTOOL, OnAddPerspectiveTool)
	ON_COMMAND(ID_VIEW_RESETCOUNTSALLIPS, OnViewResetAllCounts)
	ON_COMMAND(ID_VIEW_RESETCOUNTSCURRENTIP, OnViewResetCountsCurrentIP)
	ON_COMMAND(WM_REGRESSION_TEST_COMPLETE, RegressionTestComplete)
	ON_COMMAND(ID_EDIT_DATA_DEFINITION_LISTS, OnEditDataDefinitionLists)
	ON_UPDATE_COMMAND_UI(ID_VIEW_RESETCOUNTSALLIPS, OnUpdateViewResetCountsAllIPs)
	ON_UPDATE_COMMAND_UI(ID_VIEW_RESETCOUNTSCURRENTIP, OnUpdateViewResetCountsCurrentIP)
	ON_COMMAND(ID_SHOW_RELATIONS, OnShowToolRelations)
	ON_UPDATE_COMMAND_UI(ID_SHOW_RELATIONS, OnUpdateShowToolRelations)
	//}}AFX_MSG_MAP
	ON_UPDATE_COMMAND_UI(ID_RUN_REGRESSIONTEST, OnUpdateRunRegressionTest)
	ON_UPDATE_COMMAND_UI(ID_INDICATOR_INFO, OnUpdateStatusInfo)
	// Dynamic Enumerated Tool Set Draw Flags
	ON_COMMAND_RANGE(ID_VIEW_TOOLSETDRAW_POP_BASE, ID_VIEW_TOOLSETDRAW_POP_MAX, OnChangeToolSetDrawFlag)
	ON_UPDATE_COMMAND_UI_RANGE(ID_VIEW_TOOLSETDRAW_POP_BASE + 1, ID_VIEW_TOOLSETDRAW_POP_MAX, OnUpdateViewToolSetDrawSubMenus)
	ON_UPDATE_COMMAND_UI(ID_EDIT_DATA_DEFINITION_LISTS, &SVIPDoc::OnUpdateEditDataDefinitionLists)
	ON_COMMAND(ID_ADD_RESIZETOOL, &SVIPDoc::OnAddResizetool)
	ON_COMMAND(ID_ADD_RINGBUFFERTOOL, OnAddRingBufferTool)
	ON_COMMAND(ID_ADD_TABLETOOL, OnAddTableTool)
	ON_COMMAND(ID_ADD_TABLEANALYZERTOOL, OnAddTableAnalyzerTool)
	ON_COMMAND(ID_ADD_DRAWTOOL, OnAddDrawTool)
	ON_UPDATE_COMMAND_UI(ID_EDIT_ADJUSTLIGHTREFERENCE, OnAllowAdjustLightReference)
	ON_UPDATE_COMMAND_UI(ID_EDIT_ADJUSTLUT, OnAllowAdjustLut)
	ON_UPDATE_COMMAND_UI(ID_ADD_STARTTOOLGROUPING, OnUpdateAddStartToolGrouping)
	ON_UPDATE_COMMAND_UI(ID_ADD_ENDTOOLGROUPING, OnUpdateAddEndToolGrouping)
	ON_COMMAND(ID_ADD_STARTTOOLGROUPING, OnAddStartToolGrouping)
	ON_COMMAND(ID_ADD_ENDTOOLGROUPING, OnAddEndToolGrouping)
	ON_COMMAND(ID_TOOL_DEPENDENCIES, OnToolDependencies)
	ON_UPDATE_COMMAND_UI(ID_TOOL_DEPENDENCIES, OnUpdateToolDependencies)
	ON_COMMAND(ID_ADD_LOOPTOOL, OnAddLoopTool)
	ON_COMMAND(ID_ADD_GROUPTOOL, OnAddGroupTool)
	ON_UPDATE_COMMAND_UI(ID_ADD_LOADIMAGETOOL, OnUpdateAddGeneralTool)
	ON_UPDATE_COMMAND_UI(ID_ADD_ARCHIVETOOL, OnUpdateAddGeneralTool)
	ON_UPDATE_COMMAND_UI(ID_ADD_MATHTOOL, OnUpdateAddGeneralTool)
	ON_UPDATE_COMMAND_UI(ID_ADD_ACQUISITIONTOOL, OnUpdateAddGeneralTool)
	ON_UPDATE_COMMAND_UI(ID_ADD_STATISTICSTOOL, OnUpdateAddGeneralTool)
	ON_UPDATE_COMMAND_UI(ID_ADD_EXTERNAL_TOOL, OnUpdateAddGeneralTool)
	ON_UPDATE_COMMAND_UI(ID_ADD_RINGBUFFERTOOL, OnUpdateAddGeneralTool)
	ON_UPDATE_COMMAND_UI(ID_ADD_TABLETOOL, OnUpdateAddGeneralTool)
	ON_UPDATE_COMMAND_UI(ID_ADD_TABLEANALYZERTOOL, OnUpdateAddGeneralTool)
	ON_UPDATE_COMMAND_UI(ID_ADD_SHIFTTOOL, OnUpdateAddGeneralTool)
	ON_UPDATE_COMMAND_UI(ID_ADD_WINDOWTOOL, OnUpdateAddGeneralTool)
	ON_UPDATE_COMMAND_UI(ID_ADD_IMAGETOOL, OnUpdateAddGeneralTool)
	ON_UPDATE_COMMAND_UI(ID_ADD_POLARUNWRAPTOOL, OnUpdateAddGeneralTool)
	ON_UPDATE_COMMAND_UI(ID_ADD_LINEARTOOL, OnUpdateAddGeneralTool)
	ON_UPDATE_COMMAND_UI(ID_ADD_RESIZETOOL, OnUpdateAddGeneralTool)
	ON_UPDATE_COMMAND_UI(ID_ADD_PERSPECTIVEWARPTOOL, OnUpdateAddGeneralTool)
	ON_UPDATE_COMMAND_UI(ID_ADD_CYLINDRICALWARPTOOL, OnUpdateAddCylindricalWarpTool)
	ON_UPDATE_COMMAND_UI(ID_ADD_TRANSFORMATIONTOOL, OnUpdateAddTransformationTool)
	ON_UPDATE_COMMAND_UI(ID_ADD_COLORTOOL, OnUpdateAddColorTool)
	ON_UPDATE_COMMAND_UI(ID_ADD_LOOPTOOL, OnUpdateAddToolWithSubTools)
	ON_UPDATE_COMMAND_UI(ID_ADD_GROUPTOOL, OnUpdateAddToolWithSubTools)
	ON_UPDATE_COMMAND_UI(ID_ADD_DRAWTOOL, OnUpdateAddGeneralTool)
	ON_COMMAND(ID_SAVE_CONFIG_REPORT, OnSaveConfigReport)
	ON_UPDATE_COMMAND_UI(ID_SAVE_CONFIG_REPORT, OnUpdateSaveConfigReport)

END_MESSAGE_MAP()

HRESULT RebuildOutputObjectListHelper(SVIODoc* pIODoc);

#pragma endregion Declarations

#pragma region local methods
uint32_t getObjectAfterThis(uint32_t toolId)
{
	auto* pTool {SVObjectManagerClass::Instance().GetObject(toolId)};
	auto* pParent {dynamic_cast<SvIe::SVTaskObjectListClass*>(nullptr != pTool ? pTool->GetParent() : nullptr)};
	if (pParent)
	{
		for (int i = 0; i < pParent->GetSize(); i++)
		{
			const SvIe::SVTaskObjectClass* pCurrentTool = pParent->getTaskObject(i);
			if (pCurrentTool && pCurrentTool->getObjectId() == toolId)
			{
				if (i + 1 < pParent->GetSize())
				{
					return pParent->getTaskObject(i)->getObjectId();
				}
				return SvDef::InvalidObjectId;
			}
		}
	}
	return SvDef::InvalidObjectId;
}

LPCTSTR getViewClassName(CView* pView)
{
	if (nullptr != dynamic_cast<SVImageViewScroll*> (pView))
	{
		return cSVImageViewScrollName;
	}
	else if (nullptr != dynamic_cast<SVImageView*> (pView))
	{
		return cSVImageViewName;
	}
	else if (nullptr != dynamic_cast<ToolSetView*> (pView))
	{
		return cToolSetViewName;
	}
	else if (nullptr != dynamic_cast<ResultTabbedView*> (pView))
	{
		return cResultTabbedViewName;
	}
	return nullptr;
}

std::unique_ptr<SVToolAdjustmentDialogSheetClass> GetToolAdjustmentSheet(SVIPDoc* pSVIPDoc, int tab)
{
	auto taskObjectID = pSVIPDoc->Get1stSelectedToolID();

	SvOi::IObjectClass* pTaskObject = SvOi::getObject(taskObjectID);

	if (nullptr != pTaskObject)
	{
		if (SvPb::SVObjectSubTypeEnum::SVExternalToolObjectType == pTaskObject->GetObjectSubType())
		{
			return std::make_unique<SheetForExternalToolAdjustment>(pSVIPDoc, pSVIPDoc->GetInspectionID(), taskObjectID, _T("Tool Adjustment"), nullptr, tab);
		}
	}

	return std::make_unique <SVToolAdjustmentDialogSheetClass>(pSVIPDoc, pSVIPDoc->GetInspectionID(), taskObjectID, _T("Tool Adjustment"), nullptr, tab);
}

#pragma endregion local methods

#pragma region Constructor
SVIPDoc::SVIPDoc()
	: CDocument()
	, m_NewProductData(1)
	, m_RegisteredImages()
	, m_Images()
	, m_AllViewsUpdated(0)
	, m_pMDIChildWnd(nullptr)
{
	init();
}
#pragma endregion Constructor

uint32_t SVIPDoc::GetInspectionID() const
{
	return m_InspectionID;
}

void SVIPDoc::UpdateAllData()
{
	if (!m_isDestroying)
	{
		SVIPProductStruct l_ProductData;
		if (S_OK == m_NewProductData.PopHead(l_ProductData))
		{
			SVIPImageDataElementMap::const_iterator l_Iter = l_ProductData.m_ImageData.begin();

			while (l_Iter != l_ProductData.m_ImageData.end())
			{
				SetImageData(l_Iter->first, l_Iter->second.m_ImageDIB, l_Iter->second.m_OverlayData);

				++l_Iter;
			}

			m_Results = l_ProductData.m_ResultData;
			m_triggerRecord = l_ProductData.m_triggerRecord;

			if (!(l_ProductData.m_ImageData.empty()))
			{
				::InterlockedExchange(&m_AllViewsUpdated, 0);
			}
			else
			{
				::InterlockedExchange(&m_AllViewsUpdated, 1);

				m_oDisplay.SetIPDocDisplayComplete();
			}

			UpdateAllViews(nullptr);
		}
	}
}

void SVIPDoc::SetInspectionID(uint32_t inspectionID)
{
	m_InspectionID = inspectionID;
	m_oDisplay.SetInspectionID(m_InspectionID, this);
}

void SVIPDoc::init()
{
	mbInitImagesByName = false;

	IsNew = true;

	TheSVObserverApp().setCurrentDocument(this);

	// Default result height and tool set view width.
	m_nWidthToolSetView = SvDef::cDefaultToolsetViewWidth;
	m_nHeightResultView = SvDef::cDefaultToolsetViewHeight;

	m_bAllowRefresh = false;
	m_oDisplay.Create();
}

//******************************************************************************
// Destructor:
//******************************************************************************
SVIPDoc::~SVIPDoc()
{
	m_isDestroying = true;
	::InterlockedExchange(&m_AllViewsUpdated, 1);
	m_oDisplay.SetIPDocDisplayComplete();

	m_oDisplay.Destroy();

	TheSVObserverApp().setCurrentDocument(nullptr);
}

CMDIChildWnd* SVIPDoc::GetMDIChild()
{
	if (nullptr == m_pMDIChildWnd)
	{
		m_pMDIChildWnd = SVSearchForMDIChildWnd(GetToolSetView());
	}

	if (nullptr == m_pMDIChildWnd)
	{
		m_pMDIChildWnd = SVSearchForMDIChildWnd(GetResultView());
	}

	if (nullptr == m_pMDIChildWnd)
	{
		SVImageView* l_pView(GetImageView(0));

		m_pMDIChildWnd = SVSearchForMDIChildWnd(l_pView);
	}

	return m_pMDIChildWnd;
}

void SVIPDoc::SetMDIChild(CMDIChildWnd* p_pMDIChildWnd)
{
	m_pMDIChildWnd = p_pMDIChildWnd;
}

////////////////////////////////////////////////////////////////////////////////
// .Description : "Creates this IPDoc" and his embedded tool and freeze set.
//              : Shall become some kind of CreateObject(...) in future time.
////////////////////////////////////////////////////////////////////////////////
bool SVIPDoc::InitAfterSystemIsDocked()
{
	// Init Utility and Tool Set Draw menus...
	InitMenu();

	SVInspectionProcess* pInspection(GetInspectionProcess());

	bool l_bOk = (nullptr != pInspection);

	if (l_bOk)
	{
		if (IsNew)
		{
			SVImageView* pView = GetImageView();

			if (nullptr != pView)
			{
				uint32_t imageId = SvDef::InvalidObjectId;

				if (nullptr != GetToolSet()->getCurrentImage())
				{
					imageId = GetToolSet()->getCurrentImage()->getObjectId();
				}

				pView->AttachToImage(imageId);

				SVImageViewScroll* pScroll(nullptr);

				pScroll = dynamic_cast<SVImageViewScroll*>(pView->GetParent());

				if (nullptr != pScroll)
				{
					CWnd* pWnd(pScroll->GetParent());

					SVIPSplitterFrame* pSplitterFrame(nullptr);

					do
					{
						pSplitterFrame = dynamic_cast<SVIPSplitterFrame*>(pWnd);

						if (nullptr == pSplitterFrame)
						{
							pWnd = pWnd->GetParent();
						}
					} while (nullptr != pWnd && nullptr == pSplitterFrame);

					if (nullptr != pSplitterFrame)
					{
						RECT IPRect = {0, 0, 0, 0};
						CMDIChildWnd* l_pMDIChild = GetMDIChild();

						if (nullptr != l_pMDIChild)
						{
							l_pMDIChild->GetWindowRect(&IPRect);
						}

						pSplitterFrame->SetDefaultPaneSizes(IPRect);
					}
				}
			}
		}
		else
		{
			for (int i = 0; i < MaxImageViews; ++i)
			{
				SVImageView* pView = GetImageView(i);

				if (nullptr != pView)
				{
					if (mbInitImagesByName)
					{
						pView->AttachToImage(_T(""));
					}
					else
					{
						uint32_t imageId = SvDef::InvalidObjectId;
						if (nullptr != GetToolSet()->getCurrentImage())
						{
							imageId = GetToolSet()->getCurrentImage()->getObjectId();
						}

						pView->AttachToImage(imageId);
					}
				}
			}
		}
	}

	m_bAllowRefresh = true;

	return l_bOk;
}

std::string SVIPDoc::makeNameUnique(const std::string& rToolName, const std::vector<std::string>& rAdditionalNames) const
{
	return m_toolGroupings.makeNameUnique(rToolName, rAdditionalNames);
}

//******************************************************************************
// Operation(s) Of Writing Access:
//******************************************************************************

bool SVIPDoc::AddTool(SvPb::ClassIdEnum classId)
{
	ToolSetView* pToolsetView = GetToolSetView();
	if (nullptr == pToolsetView)
	{
		return false;
	}
	pToolsetView->getListCtrl().EnsureOneIsSelected();

	PtrNavigatorElement pNavElement = pToolsetView->Get1stSelNavigatorElement();

	if (!pNavElement)
	{
		return false;
	}

	std::string Selection(pNavElement->m_DisplayName);
	bool bAddToLoopGroupTool(false);
	bool bAddToToolSet(false);
	switch (pNavElement->m_Type)
	{
		case NavElementType::SubTool:
		case NavElementType::EndDelimiterTool:
		case NavElementType::SubLoopTool:
			bAddToLoopGroupTool = true;
			break;
		case NavElementType::EndDelimiterToolSet:
		case NavElementType::Empty:
			bAddToToolSet = true;
			Selection.clear();
			break;
		case NavElementType::StartGrouping:
		case NavElementType::EndGrouping:
		case NavElementType::Tool:
		case NavElementType::LoopTool:
		case NavElementType::GroupTool:
			bAddToToolSet = true;
			break;
	}
	bool Success(false);

	uint32_t newObjectID = SvDef::InvalidObjectId;
	SVToolSet* pToolSet = GetToolSet();

	uint32_t TaskObjectInsertBeforeID = SvDef::InvalidObjectId;
	uint32_t OwnerID = SvDef::InvalidObjectId;
	if (bAddToToolSet)
	{

		OwnerID = pToolSet->getObjectId();

		TaskObjectInsertBeforeID = GetObjectIdFromToolToInsertBefore(Selection);
	}
	else if (bAddToLoopGroupTool)
	{
		OwnerID = pNavElement->m_OwnerId;
		TaskObjectInsertBeforeID = pNavElement->m_navigatorObjectId;
	}
	SvPb::InspectionCmdRequest requestCmd;
	SvPb::InspectionCmdResponse responseCmd;
	auto* pRequest = requestCmd.mutable_createobjectrequest();
	pRequest->set_ownerid(OwnerID);
	pRequest->set_classid(classId);
	pRequest->set_taskobjectinsertbeforeid(TaskObjectInsertBeforeID);

	HRESULT hr = SvCmd::InspectionCommands(m_InspectionID, requestCmd, &responseCmd);
	if (S_OK == hr && responseCmd.has_createobjectresponse())
	{
		Success = true;
		newObjectID = responseCmd.createobjectresponse().objectid();
		if (bAddToToolSet)
		{
			m_toolGroupings.AddTool(responseCmd.createobjectresponse().name(), Selection);
		}
	}
	else if (responseCmd.has_standardresponse())
	{
		SvStl::MessageContainerVector errorMsgContainer = SvPb::convertProtobufToMessageVector(responseCmd.standardresponse().errormessages());
		if (errorMsgContainer.size() > 0)
		{
			SvStl::MessageManager Msg(SvStl::MsgType::Log | SvStl::MsgType::Display);
			Msg.setMessage(errorMsgContainer.at(0).getMessage());
		}
	}



	if (Success)
	{
		RunOnce();
		// Refresh all views...
		UpdateAllViews(nullptr, SVIPDoc::RefreshView);
		SetSelectedToolID(newObjectID);
		SetModifiedFlag();

		if (nullptr != pToolSet)
		{
			pToolSet->updateToolPosition();
		}
		SVInspectionProcess* pInspection {GetInspectionProcess()};
		if (nullptr != pInspection)
		{
			pInspection->buildValueObjectData();
		}

		return true;
	}
	else
	{
		return false;
	}
}

bool SVIPDoc::AddToolGrouping(bool bStartGroup)
{
	ToolSetView* pToolsetView = GetToolSetView();
	if (nullptr == pToolsetView)
	{
		return false;
	}
	pToolsetView->getListCtrl().EnsureOneIsSelected();

	PtrNavigatorElement  pNavElement = pToolsetView->Get1stSelNavigatorElement();

	if (!pNavElement)
	{
		return false;
	}
	std::string Selection(pNavElement->m_DisplayName);
	switch (pNavElement->m_Type)
	{
		case NavElementType::SubTool:
		case NavElementType::EndDelimiterTool:
			//NO Grouping in LoopTool
			return false;
			break;
		case NavElementType::EndDelimiterToolSet:
		case NavElementType::Empty:
			Selection.clear();
			break;
		case NavElementType::StartGrouping:
		case NavElementType::EndGrouping:
		case NavElementType::Tool:
		case NavElementType::LoopTool:
		case NavElementType::GroupTool:
			break;
	}

	if (bStartGroup)
	{
		m_toolGroupings.AddGroup(m_toolGroupings.GetDefaultName(), Selection);
	}
	else
	{
		// Get Group Name
		std::string GroupName = m_toolGroupings.FindGroup(Selection);
		if (!GroupName.empty())
		{
			// check if has end already ?
			m_toolGroupings.AddEndGroup(GroupName.c_str(), Selection);
		}
	}

	// Refresh all views...
	UpdateAllViews(nullptr, SVIPDoc::RefreshView);

	SetModifiedFlag();

	return true;
}

void SVIPDoc::SetTitle(LPCTSTR LPSZTitle)
{
	CDocument::SetTitle(LPSZTitle);
}

CView* SVIPDoc::getView() const
{
	CView* retVal(nullptr);
	POSITION pos = GetFirstViewPosition();
	if (nullptr != pos)
	{
		retVal = GetNextView(pos);
	}

	return retVal;
}

ToolSetView* SVIPDoc::GetToolSetView() const
{
	ToolSetView* pToolsetView(nullptr);
	POSITION pos(GetFirstViewPosition());

	while (nullptr == pToolsetView && nullptr != pos)
	{
		pToolsetView = dynamic_cast<ToolSetView*>(GetNextView(pos));
	}

	return pToolsetView;
}

SVImageView* SVIPDoc::GetImageView(int p_Index)
{
	SVImageView* pReturnView(nullptr);
	POSITION pos(GetFirstViewPosition());
	long l_Count(0);

	while (nullptr == pReturnView && nullptr != pos)
	{
		SVImageView* pIPView = dynamic_cast<SVImageView*>(GetNextView(pos));
		if (nullptr != pIPView)
		{
			if (l_Count == p_Index)
			{
				pReturnView = pIPView;
			}
			else
			{
				++l_Count;
			}
		}
	}

	return pReturnView;
}

ResultTabbedView* SVIPDoc::GetResultView()
{
	ResultTabbedView* pView(nullptr);
	POSITION pos(GetFirstViewPosition());

	while (nullptr == pView && nullptr != pos)
	{
		pView = dynamic_cast<ResultTabbedView*>(GetNextView(pos));
	}

	return pView;
}

SVInspectionProcess* SVIPDoc::GetInspectionProcess() const
{
	SVObjectClass* l_pObject = SVObjectManagerClass::Instance().GetObject(m_InspectionID);
	SVInspectionProcess* pInspection(dynamic_cast<SVInspectionProcess*>(l_pObject));

	return pInspection;
}

SVToolSet* SVIPDoc::GetToolSet() const
{
	SVToolSet* l_pObject(nullptr);
	SVInspectionProcess* pInspection(GetInspectionProcess());

	if (nullptr != pInspection)
	{
		l_pObject = pInspection->GetToolSet();
	}

	return l_pObject;
}

SvOi::ObjectInfoVector SVIPDoc::GetToolSetInfo() const
{
	SVToolSet* pObject(nullptr);
	SVInspectionProcess* pInspection(GetInspectionProcess());
	int InspectionId {0}, ToolsetId {0};

	if (nullptr != pInspection)
	{
		InspectionId = pInspection->getObjectId();

		pObject = pInspection->GetToolSet();
		if (pObject)
		{
			ToolsetId = pObject->getObjectId();
		}
	}

	SvPb::InspectionCmdRequest requestCmd;
	SvPb::InspectionCmdResponse responseCmd;
	SvPb::TaskObjectListRequest* pTaskObjectListRequest = requestCmd.mutable_taskobjectlistrequest();
	pTaskObjectListRequest->set_taskobjectid(ToolsetId);

	SvCmd::InspectionCommands(InspectionId, requestCmd, &responseCmd, SvCmd::ThreadPref::cur);

	SvOi::ObjectInfoVector  ToolSetInfos;
	SvCmd::ResponseToObjectInfo(responseCmd, ToolSetInfos);
	return ToolSetInfos;
}

SVResultList* SVIPDoc::GetResultList() const
{
	SVResultList* pResultList(nullptr);
	SVToolSet* pToolSet = GetToolSet();
	if (pToolSet)
	{
		pResultList = pToolSet->GetResultList();
	}
	return pResultList;
}

bool SVIPDoc::GoOnline()
{
	return m_oDisplay.GoOnline();
}

bool SVIPDoc::GoOffline()
{
	return m_oDisplay.GoOffline();
}

BOOL SVIPDoc::CanCloseFrame(CFrameWnd* pFrame)
{
	BOOL bCanClose = FALSE;

	if (TheSecurityManager().SVIsDisplayable(SECURITY_POINT_FILE_MENU_CLOSE_CONFIGURATION))
	{
		if (SVSVIMStateClass::CheckState(SV_STATE_CANCELING) || SVSVIMStateClass::CheckState(SV_STATE_CLOSING))
		{
			bCanClose = CDocument::CanCloseFrame(pFrame);
		}
	}

	if (!bCanClose)
	{
		SvStl::MessageManager Msg(SvStl::MsgType::Log | SvStl::MsgType::Display);
		Msg.setMessage(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_SVIPDoc_ClosingImpossible, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_10056);
	}

	return bCanClose;
}

BOOL SVIPDoc::SaveModified()
{
	// Don't save modified
	if (SVSVIMStateClass::CheckState(SV_STATE_CANCELING)) { return TRUE; }

	int index = 0;
	if ((index = m_strTitle.Find(_TCHAR('.'))) >= 0)
	{
		m_strTitle = m_strTitle.Left(index);
	}

	return CDocument::SaveModified();
}

BOOL SVIPDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument()) { return FALSE; }

	// ZU ERLEDIGEN: Hier Code zur Reinitialisierung einfügen
	// (SDI-Dokumente verwenden dieses Dokument)

	return TRUE;
}

void SVIPDoc::CloseDocument()
{
	if (!SVSVIMStateClass::CheckState(SV_STATE_CANCELING | SV_STATE_CLOSING) &&
		!(S_OK == TheSecurityManager().SVValidate(SECURITY_POINT_FILE_MENU_CLOSE_CONFIGURATION)))
	{
		return;
	}

	OnFileClose();
}

void SVIPDoc::OnUpdateStatusInfo(CCmdUI* pCmdUI)
{
	switch (pCmdUI->m_nID)
	{
		case ID_INDICATOR_INFO:
		{
			GetSvoMainFrame()->UpdateStatusInfo(pCmdUI);
			break;
		}
		default:
		{
			// Do nothing.
			break;
		}
	}
}

void SVIPDoc::OnUpdateRunRegressionTest(CCmdUI* PCmdUI)
{
	// @WARNING:  Pointers should be checked before they are dereferenced.
	PCmdUI->Enable(TheSecurityManager().SVIsDisplayable(SECURITY_POINT_MODE_MENU_REGRESSION_TEST));
	PCmdUI->SetCheck(SVSVIMStateClass::CheckState(SV_STATE_REGRESSION));
}

void SVIPDoc::OnAllowAdjustLightReference(CCmdUI* pCmdUI)
{
	bool bEnable = (!SVSVIMStateClass::CheckState(SV_STATE_RUNNING | SV_STATE_REGRESSION) &&
		SVSVIMStateClass::CheckState(SV_STATE_TEST | SV_STATE_EDIT) &&
		TheSecurityManager().SVIsDisplayable(SECURITY_POINT_MODE_MENU_EDIT_TOOLSET));
	if (bEnable)
	{
		SvIe::SVVirtualCameraPtrVector cameraVector;
		SVInspectionProcess* pInspection(GetInspectionProcess());
		if (nullptr != pInspection) { cameraVector = pInspection->GetCamerasForLightReference(); }
		bEnable = (cameraVector.size() > 0) ? true : false;
	}
	pCmdUI->Enable(bEnable);
}

void SVIPDoc::OnAllowAdjustLut(CCmdUI* pCmdUI)
{
	bool bEnable = (!SVSVIMStateClass::CheckState(SV_STATE_RUNNING | SV_STATE_REGRESSION) &&
		SVSVIMStateClass::CheckState(SV_STATE_TEST | SV_STATE_EDIT) &&
		TheSecurityManager().SVIsDisplayable(SECURITY_POINT_MODE_MENU_EDIT_TOOLSET));
	if (bEnable)
	{
		SvIe::SVVirtualCameraPtrVector cameraVector;
		SVInspectionProcess* pInspection(GetInspectionProcess());
		if (nullptr != pInspection) { cameraVector = pInspection->GetCamerasForLut(); }
		bEnable = (cameraVector.size() > 0) ? true : false;
	}
	pCmdUI->Enable(bEnable);
}

void SVIPDoc::OnAdjustLightReference()
{
	SVInspectionProcess* pInspection(GetInspectionProcess());

	assert(GetToolSet());
	assert(GetToolSet()->IsCreated());

	if (nullptr == pInspection || !GetToolSet()->IsCreated()) { return; }

	// NOTE:
	//		 At this time an IPDoc has only one camera image input!!!

	// Show extreme LUT: black --> blue, white --> green...
	GetImageView()->ShowExtremeLUT();

	SVLightReferenceDialogPropertySheetClass dlg(std::string(_T("Light Reference Adjustment - [") + GetTitle() + _T("]")).c_str());

	// obtain copies of the arrays to set if IDOK
	SVLightReferenceVector LightRefVector;

	SvIe::SVVirtualCameraPtrVector cameraVector = pInspection->GetCameras();

	for (const auto* pCamera : cameraVector)
	{
		if (nullptr != pCamera)
		{
			SVLightReference lightRef;
			pCamera->GetAcquisitionDevice()->GetLightReference(lightRef);
			LightRefVector.push_back(lightRef);
		}
	}
	//Make copy for cancel
	SVLightReferenceVector LightRefVectorOriginal {LightRefVector};


	//remove apply button
	dlg.m_psh.dwFlags |= PSH_NOAPPLYNOW;

	if (dlg.CreatePages(cameraVector, LightRefVector))
	{
		SVSVIMStateClass::SetResetState stateEditing {SV_STATE_EDITING};

		SVConfigurationObject* pConfig(nullptr);
		SVObjectManagerClass::Instance().GetConfigurationObject(pConfig);
		assert(nullptr != pConfig);

		SVLightReferenceVector* pResultLightRefVector {nullptr};
		if (nullptr != pConfig && dlg.DoModal() == IDOK)
		{
			SetModifiedFlag();
			pResultLightRefVector = &LightRefVector;
		}
		else
		{
			pResultLightRefVector = &LightRefVectorOriginal;
		}
		if (nullptr != pResultLightRefVector)
		{
			for (size_t i = 0; i < cameraVector.size(); ++i)
			{
				cameraVector[i]->SetLightReference(pResultLightRefVector->at(i));
				SVLightReference lightRef; // get new device lra; camera Set only modifies its band(s)
				cameraVector[i]->GetAcquisitionDevice()->GetLightReference(lightRef);
				std::string deviceName {cameraVector[i]->GetAcquisitionDevice()->DeviceName()};
				pConfig->ModifyAcquisitionDevice(deviceName.c_str(), lightRef);
			}
		}
	}
	// Show default LUT: black --> black, white --> white...
	GetImageView()->ShowExtremeLUT(FALSE);
}

void SVIPDoc::OnAdjustLut()
{
	SVInspectionProcess* pInspection(GetInspectionProcess());

	assert(GetToolSet());
	assert(GetToolSet()->IsCreated());

	if (nullptr == pInspection || !GetToolSet()->IsCreated()) // is tool set created
	{
		return;
	}

	// Show extreme LUT: black --> blue, white --> green...
	GetImageView()->ShowExtremeLUT();

	SVLutDlg dlg(std::string(_T("LUT Adjustment - [") + GetTitle() + _T("]")).c_str());

	// obtain copies of the lut to set if IDOK / IDCANCEL
	SVLutDlg::SVLutMap aLut;
	SVLutDlg::SVLutMap aLutOrig;

	SvIe::SVVirtualCameraPtrVector cameraVector = pInspection->GetCameras();

	for (const auto* const pCamera : cameraVector)
	{
		if (nullptr != pCamera)
		{
			pCamera->GetAcquisitionDevice()->GetLut(aLut[pCamera->getObjectId()]);
		}
	}

	aLutOrig = aLut;

	//remove the apply button
	dlg.m_psh.dwFlags |= PSH_NOAPPLYNOW;

	bool bSuccess = dlg.Create(cameraVector, aLut);

	if (bSuccess)
	{
		SVSVIMStateClass::SetResetState stateEditing {SV_STATE_EDITING};

		if (dlg.DoModal() == IDOK)
		{
			SetModifiedFlag();
		}
		else    // if cancel
		{
			// restore old LUT
			for (auto* const pCamera : cameraVector)
			{
				if (nullptr != pCamera)
				{
					pCamera->SetLut(aLutOrig[pCamera->getObjectId()]);
				}
			}
		}
		SVConfigurationObject* pConfig(nullptr);
		SVObjectManagerClass::Instance().GetConfigurationObject(pConfig);
		assert(nullptr != pConfig);

		if (nullptr != pConfig)
		{
			for (auto* const pCamera : cameraVector)
			{
				SVLut lut;
				SvIe::SVAcquisitionClassPtr pDevice = pCamera->GetAcquisitionDevice();
				pDevice->GetLut(lut);
				pConfig->ModifyAcquisitionDevice(pDevice->DeviceName().c_str(), lut);

				// Update DeviceParameters as well...
				SVDeviceParamCollection deviceParams;
				pDevice->GetDeviceParameters(deviceParams);
				pConfig->ModifyAcquisitionDevice(pDevice->DeviceName().c_str(), &deviceParams);
			}
		}
	}

	// Show default LUT: black --> black, white --> white...
	GetImageView()->ShowExtremeLUT(FALSE);
}

void SVIPDoc::OnAddShiftTool()
{
	AddTool(SvPb::ShiftToolClassId);
}

void SVIPDoc::OnAddWindowTool()
{
	AddTool(SvPb::WindowToolClassId);
}

void SVIPDoc::OnAddCylindricalWarpTool()
{
	AddTool(SvPb::CylindricalWarpToolClassId);
}

void SVIPDoc::OnAddPerspectiveTool()
{
	AddTool(SvPb::PerspectiveToolClassId);
}

void SVIPDoc::OnAddImageTool()
{
	AddTool(SvPb::ImageToolClassId);
}

void SVIPDoc::OnAddAcquisitionTool()
{
	AddTool(SvPb::AcquisitionToolClassId);
}

void SVIPDoc::OnAddArchiveTool()
{
	AddTool(SvPb::ArchiveToolClassId);
}

void SVIPDoc::OnAddLinearTool()
{
	AddTool(SvPb::LinearToolClassId);
}

void SVIPDoc::OnAddLoadImageTool()
{
	AddTool(SvPb::LoadImageToolClassId);
}

void SVIPDoc::OnAddMathTool()
{
	AddTool(SvPb::MathToolClassId);
}

void SVIPDoc::OnAddStatisticsTool()
{
	AddTool(SvPb::StatisticsToolClassId);
}

void SVIPDoc::OnAddTransformationTool()
{
	AddTool(SvPb::TransformationToolClassId);
}

void SVIPDoc::OnAddResizetool()
{
	AddTool(SvPb::ResizeToolId);
}

void SVIPDoc::OnAddRingBufferTool()
{
	AddTool(SvPb::RingBufferToolClassId);
}

void SVIPDoc::OnAddTableTool()
{
	AddTool(SvPb::TableToolClassId);
}

void SVIPDoc::OnAddTableAnalyzerTool()
{
	AddTool(SvPb::TableAnalyzerToolClassId);
}

void SVIPDoc::OnAddDrawTool()
{
	AddTool(SvPb::DrawToolClassId);
}

void SVIPDoc::OnAddPolarUnwrapTool()
{
	AddTool(SvPb::PolarTransformationToolClassId);
}

void SVIPDoc::OnAddColorTool()
{
	AddTool(SvPb::ColorToolClassId);
}
void SVIPDoc::OnAddLoopTool()
{
	AddTool(SvPb::LoopToolClassId);
}
void SVIPDoc::OnAddGroupTool()
{
	AddTool(SvPb::GroupToolClassId);
}

void SVIPDoc::OnUpdateAddStartToolGrouping(CCmdUI* pCmdUI)
{
	bool bEnable = (!SVSVIMStateClass::CheckState(SV_STATE_RUNNING | SV_STATE_TEST) && SVSVIMStateClass::CheckState(SV_STATE_EDIT));
	if (bEnable)
	{

		ToolSetView* pView = GetToolSetView();
		if (!pView || pView->IsSubToolSelected())
		{
			bEnable = false;
		}
	}
	pCmdUI->Enable(bEnable);
}

void SVIPDoc::OnUpdateAddEndToolGrouping(CCmdUI* pCmdUI)
{
	bool bEnable = (!SVSVIMStateClass::CheckState(SV_STATE_RUNNING | SV_STATE_TEST) && SVSVIMStateClass::CheckState(SV_STATE_EDIT));
	if (bEnable)
	{
		// check insertion position and ensure there is an unmatched start group before.
		ToolSetView* pView = GetToolSetView();
		if (!pView || pView->IsSubToolSelected() || !pView->IsEndToolGroupAllowed())
		{
			bEnable = false;
		}
	}
	pCmdUI->Enable(bEnable);
}

void SVIPDoc::OnAddStartToolGrouping()
{
	AddToolGrouping(true);
}

void SVIPDoc::OnAddEndToolGrouping()
{
	AddToolGrouping(false);
}

void SVIPDoc::OnEditDelete()
{
	SVInspectionProcess* pInspection(GetInspectionProcess());
	if (nullptr == pInspection)
	{
		return;
	}

	// Don't allow deletion of tools while inspecting
	if (SVSVIMStateClass::CheckState(SV_STATE_RUNNING | SV_STATE_TEST))
	{
		return;
	}
	ToolSetView* pToolsetView = GetToolSetView();
	SVToolSet* pToolSet = GetToolSet();
	if (!pToolSet || !pToolsetView || pToolsetView->IsLabelEditing())
	{
		return;
	}

	if (false == mayDeleteCurrentlySelectedTools(GetSelectedToolIds()))
	{
		return;
	}

	for (const auto ptrNavElem : pToolsetView->GetSelectedNavigatorElements())
	{
		if (!ptrNavElem)
		{
			continue;
		}
		switch (ptrNavElem->m_Type)
		{
			case NavElementType::SubLoopTool:
			case NavElementType::SubTool:
			{
				auto pTool = getCorrespondingToolPointer(ptrNavElem.get(), m_InspectionID);

				if(nullptr != pTool)
				{
					if (deleteTool(pTool))
					{
						UpdateAllViews(nullptr, RefreshDelete);
						SetModifiedFlag();
						RunOnce();
					}
				}
				break;
			}

			case NavElementType::LoopTool:
			case NavElementType::GroupTool:
			case NavElementType::Tool:
			{
				auto pTool = getCorrespondingToolPointer(ptrNavElem.get(), m_InspectionID);

				if (nullptr != pTool)
				{
					if (deleteTool(pTool))
					{
						m_toolGroupings.RemoveTool(ptrNavElem->m_DisplayName);
						RebuildResultsList();
						UpdateAllViews(nullptr, RefreshDelete);
						SetModifiedFlag();
						RunOnce();
					}
				}
			}
			break;

			case NavElementType::StartGrouping:
			case NavElementType::EndGrouping:
			{
				m_toolGroupings.RemoveGroup(ptrNavElem->m_DisplayName);
				UpdateAllViews(nullptr, RefreshDelete);
				SetModifiedFlag();
			}
			break;

		}
	}
}

void SVIPDoc::OnEditCut()
{
	ToolSetView* pToolsetView = GetToolSetView();
	if (nullptr != pToolsetView && !pToolsetView->IsLabelEditing())
	{
		auto toolIDs = pToolsetView->GetAllSelectedToolIds();
		ToolClipboard toolClipboard;
		toolClipboard.writeXmlToolData(toolIDs);
		OnEditDelete();
	}
}

void SVIPDoc::OnEditCopy()
{
	if (!SVSVIMStateClass::CheckState(SV_STATE_READY) || !SVSVIMStateClass::CheckState(SV_STATE_EDIT))
	{
		return;
	}

	ToolSetView* pToolsetView = GetToolSetView();
	if (nullptr != pToolsetView && !pToolsetView->IsLabelEditing())
	{
		auto toolIDs = pToolsetView->GetAllSelectedToolIds();
		ToolClipboard toolClipboard;
		toolClipboard.writeXmlToolData(toolIDs);
	}
}

void SVIPDoc::OnUpdateEditCutCopy(CCmdUI* pCmdUI)
{
	// Check current user access...
	bool Enabled(false);
	ToolSetView* pToolsetView = GetToolSetView();
	SVToolSet* pToolSet = GetToolSet();

	if (TheSVObserverApp().OkToEdit() && nullptr != pToolSet && nullptr != pToolsetView)
	{
		if (!pToolsetView->IsLabelEditing())
		{
			Enabled = true;
			uint16_t numberOfSelectedTools = 0;
			for(auto pNavElement: pToolsetView->GetSelectedNavigatorElements())
			{
				if (pNavElement)
				{
					switch (pNavElement->m_Type)
					{
						case NavElementType::SubTool:
						case NavElementType::Tool:
						case NavElementType::LoopTool:
						case NavElementType::GroupTool:
							numberOfSelectedTools++;
							break;
						default:
							Enabled = false;
							break;
					}
				}
			}
			if (0 == numberOfSelectedTools)
			{
				Enabled = false;
			}
		}
	}
	pCmdUI->Enable(Enabled);
}

void fixInputs(uint32_t inspectionId, const std::vector<uint32_t>& rToolIds)
{
	std::vector<SvPb::FixedInputData> fixedDataVector;
	for (auto toolId : rToolIds)
	{
		SVObjectClass* pToolObject(SVObjectManagerClass::Instance().GetObject(toolId));
		if (nullptr != pToolObject)
		{
			pToolObject->fixInvalidInputs(std::back_inserter(fixedDataVector));
		}
	}
	if (false == fixedDataVector.empty())
	{
		SvOg::InputConflictDlg ConflictDlg(inspectionId, fixedDataVector);
		ConflictDlg.DoModal();
		for (auto toolId : rToolIds)
		{
			auto* pTool = SVObjectManagerClass::Instance().GetObject(toolId);
			if (nullptr != pTool)
			{
				pTool->resetAllObjects();
			}
		}
		//@TODO[MZA][10.20][14.03.2022] Wird ein WindowTool von einer Inspection zu einer anderen Inspection kopiert, wird durch ein Reset die Images nicht richtig angepasst.
		//Deshalb muss man noch ein Reset machen. Wenn das Reset verbessert wird, könnte das zweite Reset vielleicht überflüssig.
		auto* pInsp = SVObjectManagerClass::Instance().GetObject(inspectionId);
		if (nullptr != pInsp)
		{
			pInsp->resetAllObjects();
		}
	}
}

void SVIPDoc::OnEditPaste()
{
	if (!SVSVIMStateClass::CheckState(SV_STATE_READY) || !SVSVIMStateClass::CheckState(SV_STATE_EDIT))
	{
		return;
	}

	SVInspectionProcess* pInspection(GetInspectionProcess());
	ToolSetView* pToolsetView = GetToolSetView();
	if (nullptr == pInspection || nullptr == pToolsetView)
	{
		return;
	}

	auto pNavElement = pToolsetView->Get1stSelNavigatorElement();

	SVToolSet* pToolSet(pInspection->GetToolSet());
	if (nullptr == pNavElement || nullptr == pToolSet)
	{
		return;
	}
	uint32_t postToolId(SvDef::InvalidObjectId);
	uint32_t ownerId(pToolSet->getObjectId());
	switch (pNavElement->m_Type)
	{
		case NavElementType::SubTool:
		case NavElementType::EndDelimiterTool:
		case NavElementType::SubLoopTool:
			ownerId = pNavElement->m_OwnerId;
			if (SvDef::InvalidObjectId == pNavElement->m_navigatorObjectId)
			{	//at the end of the Group/Loop-Tool, get the objectId of the next tool after this tool
				postToolId = getObjectAfterThis(ownerId);
			}
			else
			{
				postToolId = pNavElement->m_navigatorObjectId;
			}
			break;
		case NavElementType::StartGrouping:
		case NavElementType::EndGrouping:
		{
			std::string toolName = m_toolGroupings.GetToolToInsertBefore(pNavElement->m_DisplayName);
			for (int i = 0; i < pToolSet->GetSize(); i++)
			{
				const SvIe::SVTaskObjectClass* pTool = pToolSet->getTaskObject(i);
				if (pTool && pTool->GetName() == toolName)
				{
					postToolId = pTool->getObjectId();
					break;
				}
			}
		}
		break;
		case NavElementType::Tool:
		case NavElementType::LoopTool:
		case NavElementType::GroupTool:
			postToolId = pNavElement->m_navigatorObjectId;
			break;
		case NavElementType::EndDelimiterToolSet:
		case NavElementType::Empty:
		default:
			break;
	}

	ToolClipboard toolClipboard;
	try
	{
		auto XmlData = toolClipboard.readXmlToolData();
		auto pastedToolIDs = toolClipboard.createToolsFromXmlData(XmlData, ownerId);
		if (!pastedToolIDs.empty())
		{
#if defined (TRACE_THEM_ALL) || defined (TRACE_TOOLCLIPBOARD)
			std::stringstream ss;
			ss << _T("Toolids in SVIPDoc::OnEditPaste(): ");

			for (auto id : pastedToolIDs)
			{
				ss << id << _T(" ");


			}
			ss << _T("\n");
			::OutputDebugString(ss.str().c_str());
#endif
			updateToolsetView(pastedToolIDs, postToolId, ownerId, pNavElement->m_DisplayName);
		}
	}
	catch (const SvStl::MessageContainer& rSvE)
	{
		SvStl::MessageManager e(SvStl::MsgType::Log | SvStl::MsgType::Display);
		e.setMessage(rSvE.getMessage());
	}
}

void SVIPDoc::OnUpdateEditPaste(CCmdUI* pCmdUI)
{
	ToolSetView* pToolsetView = GetToolSetView();
	SVToolSet* pToolSet = GetToolSet();
	bool enabled = TheSVObserverApp().OkToEdit() && nullptr != pToolSet && nullptr != pToolsetView && false == pToolsetView->IsLabelEditing();

	if (enabled)
	{
		auto pNavElement = pToolsetView->Get1stSelNavigatorElement();

		//Only if tool list active and a selected index is valid
		enabled = (nullptr != pNavElement && toolClipboardDataPresent());
	}
	pCmdUI->Enable(enabled);
}


void SVIPDoc::updateToolsetView(const std::vector<uint32_t>& rPastedToolIDs, uint32_t postID, uint32_t ownerID, const std::string& rSelectedName)
{
	bool anythingAdded = false;

	SVInspectionProcess* pInspection(GetInspectionProcess());

	if (nullptr == pInspection)
	{
		return;
	}

	for (auto toolID : rPastedToolIDs)
	{
		SVObjectClass* pObject(SVObjectManagerClass::Instance().GetObject(toolID));
		SvTo::SVToolClass* pTool = dynamic_cast<SvTo::SVToolClass*> (pObject);

		if (nullptr != pTool)
		{
			SvPb::InspectionCmdRequest requestCmd;
			auto* pRequest = requestCmd.mutable_moveobjectrequest();
			pRequest->set_parentid(ownerID);
			pRequest->set_objectid(toolID);
			pRequest->set_movepreid(postID);
			pRequest->set_listmode(SvPb::MoveObjectRequest_ListEnum_TaskObjectList);

			HRESULT hr = SvCmd::InspectionCommands(m_InspectionID, requestCmd, nullptr);
			assert(S_OK == hr); UNREFERENCED_PARAMETER(hr);

			SVObjectClass* pOwnerObject(SVObjectManagerClass::Instance().GetObject(ownerID));
			if (pOwnerObject && false == pOwnerObject->isLoopOrGroupTool())
			{
				std::string name;

				if (rSelectedName.empty())
				{
					SVObjectClass* pObjectPost(SVObjectManagerClass::Instance().GetObject(postID));
					if (nullptr != pObjectPost)
					{
						name = pObjectPost->GetName();
					}
				}
				else
				{
					name = rSelectedName;
				}
				m_toolGroupings.AddTool(pTool->GetName(), name);
			}
			anythingAdded = true;

			//@TODO[gra][8.10][13.12.2018]: This is a low risk fix to avoid other problems and should be solved in another way
			//This reset is required when pasting an acquisition tool which otherwise adds a non existent camera to the PPQ
			//The inspection pointer is still nullptr at this stage which avoids adding the camera to the PPQ in SVCameraImageTemplate::RebuildCameraImage
			if (SvPb::SVToolAcquisitionObjectType == pTool->GetObjectSubType())
			{
				pTool->resetAllObjects();
			}
			SVObjectLevelCreateStruct createStruct(nullptr != pOwnerObject ? *pOwnerObject : *pInspection);
			createStruct.m_pInspection = pInspection;
			if (nullptr != pOwnerObject && SvPb::SVToolObjectType == pOwnerObject->GetObjectType())
			{
				createStruct.m_pTool = pOwnerObject;
			}

			pTool->connectAllInputs();
			pTool->createAllObjects(createStruct);
			//Reset only the inserted tool
			pTool->resetAllObjects();
		}
	}

	if (false == anythingAdded)
	{
		return;
	}

	if (nullptr != pInspection->GetToolSet())
	{
		pInspection->GetToolSet()->updateToolPosition();
	}
	fixInputs(m_InspectionID, rPastedToolIDs);

	RunOnce();

	if (false == rSelectedName.empty())
	{
		UpdateAllViews(nullptr, SVIPDoc::RefreshView);
	}

	SetSelectedToolID(rPastedToolIDs[0]);

	SetModifiedFlag();
}


void SVIPDoc::OnShowFirstError()
{
	ToolSetView* pToolsetView = GetToolSetView();
	if (nullptr != pToolsetView)
	{
		pToolsetView->displayFirstCurrentToolError();
	}
}

void SVIPDoc::OnUpdateShowFirstError(CCmdUI* pCmdUI)
{
	BOOL Enabled = false;
	ToolSetView* pToolsetView = GetToolSetView();
	if (nullptr != pToolsetView && pToolsetView->hasCurrentToolErrors())
	{
		Enabled = true;
	}

	pCmdUI->Enable(Enabled);
}

void SVIPDoc::OnAddParameterToMonitorList()
{
	ToolSetView* pToolsetView = GetToolSetView();
	std::string ppqName = GetInspectionProcess()->GetPPQ()->GetName();
	if (nullptr != pToolsetView)
	{
		pToolsetView->addParameter2MonitorList(ppqName.c_str());
	}
}

void SVIPDoc::OnUpdateAddParameterToMonitorList(CCmdUI* pCmdUI)
{
	BOOL Enabled = false;
	ToolSetView* pToolsetView = GetToolSetView();
	std::string ppqName = GetInspectionProcess()->GetPPQ()->GetName();
	if (nullptr != pToolsetView && pToolsetView->isAddParameter2MonitorListPossible(ppqName.c_str()))
	{
		Enabled = true;
	}

	pCmdUI->Enable(Enabled);
}

void SVIPDoc::OnRemoveParameterToMonitorList()
{
	ToolSetView* pToolsetView = GetToolSetView();
	std::string ppqName = GetInspectionProcess()->GetPPQ()->GetName();
	if (nullptr != pToolsetView)
	{
		pToolsetView->removeParameter2MonitorList(ppqName.c_str());
	}
}

void SVIPDoc::OnUpdateRemoveParameterToMonitorList(CCmdUI* pCmdUI)
{
	BOOL Enabled = false;
	ToolSetView* pToolsetView = GetToolSetView();
	std::string ppqName = GetInspectionProcess()->GetPPQ()->GetName();
	if (nullptr != pToolsetView && pToolsetView->isRemoveParameter2MonitorListPossible(ppqName.c_str()))
	{
		Enabled = true;
	}

	pCmdUI->Enable(Enabled);
}

void SVIPDoc::OnEditTool()
{
	constexpr int ImageTab = 0;
	return OpenToolAdjustmentDialog(ImageTab);
}

void SVIPDoc::OnEditToolTab1()
{

	constexpr int SizeTab = 1;
	return OpenToolAdjustmentDialog(SizeTab);
}


void SVIPDoc::OpenToolAdjustmentDialog(int tab)
{
	// Check current user access...
	if (TheSVObserverApp().OkToEdit())
	{
		SvTo::SVToolClass* pTool = dynamic_cast<SvTo::SVToolClass*> (SVObjectManagerClass::Instance().GetObject(Get1stSelectedToolID()));
		if (nullptr != pTool)
		{
			SVSVIMStateClass::SetResetState stateEditing {SV_STATE_EDITING};
			try
			{
				std::unique_ptr<SVToolAdjustmentDialogSheetClass> pSheet = GetToolAdjustmentSheet(this, tab);

				INT_PTR dlgResult = E_POINTER;

				if (nullptr != pSheet)
				{
					pSheet->init();

					dlgResult = pSheet->DoModal();
				}

				if (IDOK == dlgResult)
				{
					ExtrasEngine::Instance().ExecuteAutoSaveIfAppropriate(false);//after tool was edited: update the autosave timestamp
					SVConfigurationObject* pConfig = nullptr;
					SVObjectManagerClass::Instance().GetConfigurationObject(pConfig);
					if (nullptr != pConfig)
					{
						pConfig->ValidateRemoteMonitorList();
						GetTheIODoc()->UpdateAllViews(nullptr);
					}
				}
				else
				{
					pTool->ResetObject();
				}
			}
			catch (const SvStl::MessageContainer& rExp)
			{
				//This is the topmost catch of the TA for MessageContainer exceptions
				SvStl::MessageManager Exception(SvStl::MsgType::Log | SvStl::MsgType::Display);
				//Set the error code to unhandled exception but use the rest of the data from the original exception
				SvStl::MessageData Msg(rExp.getMessage());
				std::string OrgMessageCode = SvUl::Format(_T("0x%08X"), Msg.m_MessageCode);
				Msg.m_AdditionalTextId = SvStl::Tid_Default;
				SvDef::StringVector msgList;
				msgList.push_back(OrgMessageCode);
				Msg.m_AdditionalTextList = msgList;
				Msg.m_MessageCode = SVMSG_SVO_72_UNHANDLED_EXCEPTION;
				Exception.setMessage(Msg);
			}
			catch (...)
			{
				//This is the topmost catch of TA for other exceptions
				SvStl::MessageManager Exception(SvStl::MsgType::Log | SvStl::MsgType::Display);
				Exception.setMessage(SVMSG_SVO_UNHANDLED_EXCEPTION, SvStl::Tid_Default, SvStl::SourceFileParams(StdMessageParams));
			}
		}
		RebuildResultsList();
		UpdateAllViews(nullptr, RefreshView);
	}
}

void SVIPDoc::OnEditToolSet()
{
	// Check current user access...
	if (TheSVObserverApp().OkToEdit())
	{
		if (GetToolSet())
		{
			SVSVIMStateClass::SetResetState stateEditing {SV_STATE_EDITING};

			SVToolAdjustmentDialogSheetClass toolSetDialog(this, GetInspectionID(), GetToolSet()->getObjectId(), _T("Tool Set Adjustment: "));
			toolSetDialog.init();

			INT_PTR dlgResult = toolSetDialog.DoModal();
			if (dlgResult == IDOK)
			{
				SetModifiedFlag();
			}
		}
	}
}

void SVIPDoc::OnFileSaveImage()
{
	SVSVIMStateClass::SetResetState stateEditing {SV_STATE_EDITING};
	if (S_OK == TheSecurityManager().SVValidate(SECURITY_POINT_FILE_MENU_SAVE_IMAGE))
	{
		SvOg::SVSaveToolSetImageDialogClass dlg(GetInspectionID(), GetToolSet()->getObjectId());
		dlg.DoModal();
	}
}

//******************************************************************************
// Debug Operation(s):
//******************************************************************************

#ifdef _DEBUG
void SVIPDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void SVIPDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

void SVIPDoc::OnResultsPicker()
{

	//This shall change the state to editing only when previously in edit mode
	//This will avoid changing the modes while in Run, Regression or Test mode
	std::unique_ptr<SVSVIMStateClass::SetResetState> pStateEditing {nullptr};
	if (SVSVIMStateClass::CheckState(SV_STATE_EDIT))
	{
		// cppcheck-suppress unreadVariable symbolName=pStateEditing ; RAII variable
		pStateEditing = std::make_unique<SVSVIMStateClass::SetResetState>(SV_STATE_EDITING);  /// do this before calling validate for security as it may display a logon dialog!
	}
	if (S_OK == TheSecurityManager().SVValidate(SECURITY_POINT_EDIT_MENU_RESULT_PICKER))
	{
		SVInspectionProcess* pInspection(GetInspectionProcess());
		SVResultList* pResultList(GetResultList());
		if (nullptr != pInspection && nullptr != pResultList)
		{
			std::string InspectionName(pInspection->GetName());
			SvPb::InspectionCmdRequest requestCmd;
			SvPb::InspectionCmdResponse responseCmd;
			*requestCmd.mutable_getobjectselectoritemsrequest() = SvCmd::createObjectSelectorRequest(
			{SvPb::SearchArea::globalConstantItems, SvPb::SearchArea::cameraObject, SvPb::SearchArea::ppqItems, SvPb::SearchArea::toolsetItems},
				GetInspectionID(), SvPb::viewable, SvDef::InvalidObjectId, true);
			SvCmd::InspectionCommands(m_InspectionID, requestCmd, &responseCmd);

			SvOsl::ObjectTreeGenerator::Instance().setSelectorType(SvOsl::ObjectTreeGenerator::SelectorTypeEnum::TypeMultipleObject, IDD_RESULTS_PICKER + SvOr::HELPFILE_DLG_IDD_OFFSET);
			if (responseCmd.has_getobjectselectoritemsresponse())
			{
				SvOsl::ObjectTreeGenerator::Instance().insertTreeObjects(responseCmd.getobjectselectoritemsresponse().tree());
			}

			const SVObjectReferenceVector& rSelectedObjects(pResultList->GetSelectedObjects());

			SvOsl::ObjectTreeGenerator::Instance().setCheckItems(TranslateSelectedObjects(rSelectedObjects, InspectionName));

			std::string ResultPicker = SvUl::LoadStdString(IDS_RESULT_PICKER);
			std::string Title = SvUl::Format(_T("%s - %s"), ResultPicker.c_str(), InspectionName.c_str());
			std::string Filter = SvUl::LoadStdString(IDS_FILTER);
			INT_PTR Result = SvOsl::ObjectTreeGenerator::Instance().showDialog(Title.c_str(), ResultPicker.c_str(), Filter.c_str());

			if (IDOK == Result)
			{
				const SvDef::StringVector& SelectedItems = SvOsl::ObjectTreeGenerator::Instance().getSelectedObjects();
				pResultList->Clear();

				for (auto const& rEntry : SelectedItems)
				{
					SVObjectReference ObjRef {rEntry};
					pResultList->Insert(ObjRef.GetCompleteName(true));
				}
				// Set the Document as modified
				SetModifiedFlag();
				RebuildResultsList();
				UpdateWithLastProduct();
			}
			UpdateAllViews(nullptr);
		}
	}
}

void SVIPDoc::OnResultsTablePicker()
{
	//This shall change the state to editing only when previously in edit mode
	//This will avoid changing the modes while in Run, Regression or Test mode
	std::unique_ptr<SVSVIMStateClass::SetResetState> pStateEditing {nullptr};
	if (SVSVIMStateClass::CheckState(SV_STATE_EDIT))
	{
		// cppcheck-suppress unreadVariable symbolName=pStateEditing ; RAII variable
		pStateEditing = std::make_unique<SVSVIMStateClass::SetResetState>(SV_STATE_EDITING);  /// do this before calling validate for security as it may display a logon dialog!
	}
	if (S_OK == TheSecurityManager().SVValidate(SECURITY_POINT_EDIT_MENU_RESULT_PICKER))
	{
		SVResultList* pResultList(GetResultList());
		assert(nullptr != pResultList);
		if (nullptr != pResultList)
		{
			SvUl::NameObjectIdList availableList;
			std::string selectedItem = SvOg::Table_NoSelected;
			uint32_t selectedId = pResultList->getTableId();
			SvPb::InspectionCmdRequest requestCmd;
			SvPb::InspectionCmdResponse responseCmd;
			auto* pRequest = requestCmd.mutable_getavailableobjectsrequest();
			auto* pTreeSearchParameter = pRequest->mutable_tree_search();
			pTreeSearchParameter->set_search_start_id(GetInspectionID());
			pTreeSearchParameter->mutable_type_info()->set_objecttype(SvPb::TableObjectType);
			pRequest->set_desired_first_object_type_for_name(SvPb::SVToolSetObjectType);

			HRESULT hr = SvCmd::InspectionCommands(m_InspectionID, requestCmd, &responseCmd);
			if (S_OK == hr && responseCmd.has_getavailableobjectsresponse())
			{
				availableList = SvCmd::convertNameObjectIdList(responseCmd.getavailableobjectsresponse().list());
				for (SvUl::NameObjectIdPair pair : availableList)
				{
					if (pair.second == selectedId)
					{
						selectedItem = pair.first;
					}
				}
			}

			SvOg::ResultTableSelectionDlg TableSelectDlg(availableList, selectedItem);
			if (IDOK == TableSelectDlg.DoModal())
			{
				pResultList->setTableId(TableSelectDlg.getSelectedId());

				// Set the Document as modified
				SetModifiedFlag();
				RebuildResultsList();
				UpdateWithLastProduct();

				ResultTabbedView* pView = GetResultView();
				if (nullptr != pView)
				{
					pView->UpdateTab();
				}
			}
		}
	}
}

void SVIPDoc::OnSaveResultsToFile()
{
	static TCHAR Filter[] = _T("CSV Files (*.csv)|*.csv||");
	SvMc::SVFileDialog dlg(false, true, _T("csv"), nullptr, 0, Filter, nullptr);
	dlg.m_ofn.lpstrTitle = _T("Select File");
	if (dlg.DoModal() == IDOK)
	{
		CFile file;
		BOOL bResult = file.Open(dlg.GetPathName(), CFile::modeCreate | CFile::modeReadWrite | CFile::shareDenyNone, nullptr);

		if (bResult)
		{
			std::string tmpText;
			SVResultDefinitionVector ResultDefinitions;
			SvIe::SVIPResultData ResultData;
			GetResultData(ResultData);
			GetResultDefinitions(ResultDefinitions);

			for (int i = 0; i < 4; i++)
			{
				tmpText += SvUl::LoadStdString(IDS_RESULTVIEW_COLUMN_NAME0 + i) + ";";
			}
			tmpText += "\n";
			file.Write(tmpText.c_str(), static_cast<int> (tmpText.size()));

			for (int i = 0; i < static_cast<int> (ResultDefinitions.size()); ++i)
			{
				std::string Name;
				std::string NameToType;
				std::string ItemIndex;
				std::string Value;
				//std::string Color(_T(""));

				SvIe::SVIPResultItemDefinition& l_rDef = ResultDefinitions[i];

				SVObjectClass* l_pObject = SVObjectManagerClass::Instance().GetObject(l_rDef.GetObjectID());

				if (nullptr != l_pObject)
				{
					if (l_rDef.GetIndexPresent())
					{
						if (0 <= l_rDef.GetIndex())
						{
							Name = SvUl::Format(_T("%s[%d]"), l_pObject->GetName(), l_rDef.GetIndex() + 1);
						}
						else
						{
							Name = SvUl::Format(_T("%s[]"), l_pObject->GetName());
						}
					}
					else
					{
						Name = l_pObject->GetName();
					}
					NameToType = l_pObject->GetObjectNameBeforeObjectType(SvPb::SVToolSetObjectType);
				}

				ItemIndex = SvUl::Format(_T("%d"), i);

				SvIe::SVIPResultData::SVResultDataMap::const_iterator l_Iter = ResultData.m_ResultData.find(l_rDef);

				if (l_Iter != ResultData.m_ResultData.end())
				{
					Value = l_Iter->second.GetValue().c_str();
					//Color = SvUl::Format(_T("0X%X6"), l_Iter->second.GetColor());

					if (l_Iter->second.IsIOTypePresent())
					{
						ItemIndex.clear();

						if (l_Iter->second.GetIOType() == IO_DIGITAL_INPUT)
						{
							NameToType = SvUl::LoadStdString(IDS_OBJECTNAME_DIGITAL_INPUT);
						}
						else if (l_Iter->second.GetIOType() == IO_REMOTE_INPUT)
						{
							NameToType = SvUl::LoadStdString(IDS_OBJECTNAME_REMOTE_INPUT);
						}
					}
				}

				tmpText = Name + ";" /*+ Color + ";"*/ + Value + ";" + NameToType + ";" + ItemIndex + "\n";
				file.Write(tmpText.c_str(), static_cast<int> (tmpText.size()));
			}

			std::string TimeText = SvUl::Format(_T("%.3f ms ( %.3f ms )"), ResultData.m_ToolSetEndTime * 1000, ResultData.m_ToolSetAvgTime * 1000);
			tmpText = _T("Toolset Time;") + TimeText + ";" + GetCompleteToolSetName() + "\n";
			file.Write(tmpText.c_str(), static_cast<int> (tmpText.size()));
			double dTime = ResultData.m_TriggerDistance / 1000.0;
			if (dTime != 0.0)
			{
				TimeText = SvUl::Format(_T("%.3f / sec (%.3f / min)"), 1.0 / dTime, 1.0 / dTime * 60.0);
			}
			else
			{
				TimeText = _T("");
			}
			tmpText = _T("Complete Processes per Second;") + TimeText + ";" + GetCompleteToolSetName() + "\n";
			file.Write(tmpText.c_str(), static_cast<int> (tmpText.size()));
			file.Close();
		}
		else
		{
			SvDef::StringVector msgList;
			msgList.push_back(std::string {dlg.GetPathName()});
			SvStl::MessageManager Msg(SvStl::MsgType::Log | SvStl::MsgType::Display);
			Msg.setMessage(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_WriteCSVFileFailed, msgList, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_10058);
		}
	}
}

void SVIPDoc::OnSaveTableResultsToFile()
{
	static TCHAR Filter[] = _T("CSV Files (*.csv)|*.csv||");
	SvMc::SVFileDialog dlg(false, true, _T("csv"), nullptr, 0, Filter, nullptr);
	dlg.m_ofn.lpstrTitle = _T("Select File");
	if (dlg.DoModal() == IDOK)
	{
		CFile file;
		BOOL bResult = file.Open(dlg.GetPathName(), CFile::modeCreate | CFile::modeReadWrite | CFile::shareDenyNone, nullptr);

		if (bResult)
		{
			std::string tmpText;
			SVResultDefinitionVector ResultDefinitions;
			std::vector <SvIe::IPResultTableData> resultTableData = getResultTableData();
			GetResultDefinitions(ResultDefinitions);

			tmpText = _T("No.;");
			for (SvIe::IPResultTableData data : resultTableData)
			{
				tmpText += data.m_columnName + ";";
			}
			tmpText += "\n";
			file.Write(tmpText.c_str(), static_cast<int> (tmpText.size()));

			if (0 < resultTableData.size())
			{
				int rowCountNew = SvUl::getArraySizeFromOneDim(resultTableData[0].m_rowData);
				for (int i = 0; i < rowCountNew; i++)
				{
					tmpText = SvUl::Format(_T("%d;"), i + 1);
					for (int j = 0; j < resultTableData.size(); j++)
					{
						std::vector<double> values = SvUl::getVectorFromOneDim<double>(resultTableData[j].m_rowData);
						if (values.size() > i)
						{
							tmpText += SvUl::Format("%f", values[i]);
						}
						tmpText += ";";
					}
					tmpText += "\n";
					file.Write(tmpText.c_str(), static_cast<int> (tmpText.size()));
				}
			}
			file.Close();
		}
		else
		{
			SvDef::StringVector msgList;
			msgList.push_back(std::string {dlg.GetPathName()});
			SvStl::MessageManager Msg(SvStl::MsgType::Log | SvStl::MsgType::Display);
			Msg.setMessage(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_WriteCSVFileFailed, msgList, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_10058);
		}
	}
}

void SVIPDoc::OnPublishedResultsPicker()
{
	SVInspectionProcess* pInspection(GetInspectionProcess());
	if (nullptr != pInspection)
	{
		SVSVIMStateClass::SetResetState stateEditing {SV_STATE_EDITING};
		std::string InspectionName(pInspection->GetName());

		SvPb::InspectionCmdRequest requestCmd;
		SvPb::InspectionCmdResponse responseCmd;
		*requestCmd.mutable_getobjectselectoritemsrequest() = SvCmd::createObjectSelectorRequest(
		{SvPb::SearchArea::toolsetItems}, GetInspectionID(), SvPb::publishable);
		SvCmd::InspectionCommands(m_InspectionID, requestCmd, &responseCmd);

		SvOsl::ObjectTreeGenerator::Instance().setSelectorType(SvOsl::ObjectTreeGenerator::SelectorTypeEnum::TypeMultipleObject, IDD_PUBLISHED_RESULTS + SvOr::HELPFILE_DLG_IDD_OFFSET);
		if (responseCmd.has_getobjectselectoritemsresponse())
		{
			SvOsl::ObjectTreeGenerator::Instance().insertTreeObjects(responseCmd.getobjectselectoritemsresponse().tree());
		}

		std::string PublishableResults = SvUl::LoadStdString(IDS_PUBLISHABLE_RESULTS);
		std::string Title = SvUl::Format(_T("%s - %s"), PublishableResults.c_str(), InspectionName.c_str());
		std::string Filter = SvUl::LoadStdString(IDS_FILTER);
		INT_PTR Result = SvOsl::ObjectTreeGenerator::Instance().showDialog(Title.c_str(), PublishableResults.c_str(), Filter.c_str());

		if (IDOK == Result)
		{
			for (auto const& rEntry : SvOsl::ObjectTreeGenerator::Instance().getModifiedObjects())
			{
				SVObjectReference ObjectRef {rEntry};
				bool previousState = SvPb::publishable == (SvPb::publishable & ObjectRef.ObjectAttributesSet());
				SvOi::SetAttributeType attributeType = previousState ? SvOi::SetAttributeType::RemoveAttribute : SvOi::SetAttributeType::AddAttribute;
				ObjectRef.SetObjectAttributesSet(SvPb::publishable, attributeType);
			}

			pInspection->GetPublishList().Refresh(GetToolSet());

			// Set the Document as modified
			SetModifiedFlag();
			SVConfigurationObject* pConfig = nullptr;
			SVObjectManagerClass::Instance().GetConfigurationObject(pConfig);
			if (nullptr != pConfig)
			{
				pConfig->ValidateRemoteMonitorList();

				// Force the PPQs to rebuild
				long lSize = pConfig->GetPPQCount();

				for (long l = 0; l < lSize; l++)
				{
					SVPPQObject* pPPQ = pConfig->GetPPQ(l);
					if (nullptr != pPPQ)
					{
						pPPQ->RebuildOutputList();
					}// end if
				}
			}// end for
			GetTheIODoc()->UpdateAllViews(nullptr);
		}
	}
}

void SVIPDoc::OnPublishedResultImagesPicker()
{
	SVInspectionProcess* pInspection(GetInspectionProcess());
	if (nullptr != pInspection)
	{
		SVSVIMStateClass::SetResetState stateEditing {SV_STATE_EDITING};

		std::string InspectionName(pInspection->GetName());

		SvPb::InspectionCmdRequest requestCmd;
		SvPb::InspectionCmdResponse responseCmd;
		*requestCmd.mutable_getobjectselectoritemsrequest() = SvCmd::createObjectSelectorRequest(
		{SvPb::SearchArea::toolsetItems}, GetInspectionID(), SvPb::publishResultImage, SvDef::InvalidObjectId, false, SvPb::allImageObjects);
		SvCmd::InspectionCommands(m_InspectionID, requestCmd, &responseCmd);

		SvOsl::ObjectTreeGenerator::Instance().setSelectorType(SvOsl::ObjectTreeGenerator::SelectorTypeEnum::TypeMultipleObject, ID_PUBLISHED_RESULT_IMAGES_PICKER + SvOr::HELPFILE_ID_OFFSET);
		if (responseCmd.has_getobjectselectoritemsresponse())
		{
			SvOsl::ObjectTreeGenerator::Instance().insertTreeObjects(responseCmd.getobjectselectoritemsresponse().tree());
		}

		std::string PublishableImages = SvUl::LoadStdString(IDS_PUBLISHABLE_RESULT_IMAGES);
		std::string Title = SvUl::Format(_T("%s - %s"), PublishableImages.c_str(), InspectionName.c_str());
		std::string Filter = SvUl::LoadStdString(IDS_FILTER);

		INT_PTR Result = SvOsl::ObjectTreeGenerator::Instance().showDialog(Title.c_str(), PublishableImages.c_str(), Filter.c_str());

		if (IDOK == Result)
		{
			for (auto const& rEntry : SvOsl::ObjectTreeGenerator::Instance().getModifiedObjects())
			{
				SVObjectReference ObjectRef {rEntry};
				bool previousState = SvPb::publishResultImage == (SvPb::publishResultImage & ObjectRef.ObjectAttributesSet());
				SvOi::SetAttributeType attributeType = previousState ? SvOi::SetAttributeType::RemoveAttribute : SvOi::SetAttributeType::AddAttribute;
				ObjectRef.SetObjectAttributesSet(SvPb::publishResultImage, attributeType);
			}
			SetModifiedFlag();

			for (auto const& rEntry : SvOsl::ObjectTreeGenerator::Instance().getSelectedObjects())
			{
				SVObjectClass* pObject = SVObjectManagerClass::Instance().GetObject(calcObjectId(rEntry));
				if (nullptr != pObject)
				{
					pObject->resetAllObjects();
				}
			}

			SVConfigurationObject* pConfig(nullptr);
			SVObjectManagerClass::Instance().GetConfigurationObject(pConfig);
			if (nullptr != pConfig)
			{
				pConfig->ValidateRemoteMonitorList();
				GetTheIODoc()->UpdateAllViews(nullptr);
			}
		}
	}
}

void SVIPDoc::RebuildResultsList()
{
	SVResultList* pResultList = GetResultList();

	if (pResultList)
	{
		pResultList->Refresh(GetToolSet());
	}
}

void SVIPDoc::RunRegressionTest()
{
	if (SVSVIMStateClass::CheckState(SV_STATE_REGRESSION | SV_STATE_REMOTE_CMD))
	{  // already in regression mode, do nothing...
		return;
	}

	bool hasRunMode = SVSVIMStateClass::CheckState(SV_STATE_RUNNING);

	bool hasTestMode = SVSVIMStateClass::CheckState(SV_STATE_TEST);

	if (hasRunMode || hasTestMode)
	{
		StopSvo();
	}

	SVInspectionProcess* pInspection(GetInspectionProcess());

	if (nullptr == pInspection) { return; }

	m_regTest.resetEquationText();

	if (pInspection->CanRegressionGoOnline())
	{
		bool l_bAllowAccess = false;

		if (hasRunMode)
		{
			// Dual Security access point
			if (S_OK == TheSecurityManager().SVValidate(SECURITY_POINT_MODE_MENU_REGRESSION_TEST,
				SECURITY_POINT_MODE_MENU_EXIT_RUN_MODE))
			{
				l_bAllowAccess = true;
			}
			else
			{
				return;
			}
		}
		else if (S_OK == TheSecurityManager().SVValidate(SECURITY_POINT_MODE_MENU_REGRESSION_TEST))
		{
			l_bAllowAccess = true;
		}

		if (l_bAllowAccess)
		{
			SVSVIMStateClass::changeState(SV_STATE_REGRESSION, SV_STATE_TEST | SV_STATE_EDIT | SV_STATE_STOP);

			TheSVObserverApp().DeselectTool();

			m_oDisplay.CanGoOnline();
			// check to see if it is able to go into Regression mode

			// check to see if the list of files are the same...
			SVMainFrame* pMainFrame = static_cast<SVMainFrame*>(AfxGetApp()->m_pMainWnd);
			HWND hRegressionWnd(nullptr);
			if (nullptr != pMainFrame)
			{
				pMainFrame->m_pregTestDlg = new SVRegressionRunDlg(m_regTest, m_InspectionID);
				pMainFrame->m_pregTestDlg->SetIPDocParent(this);
				pMainFrame->m_pregTestDlg->Create(IDD_DIALOG_REGRESSIONTEST_RUN);
				pMainFrame->m_pregTestDlg->ShowWindow(SW_SHOW);
				hRegressionWnd = pMainFrame->m_pregTestDlg->GetSafeHwnd();
			}


			if (nullptr == pInspection->GetPPQ() || nullptr == pInspection->GetPPQ()->GetTrigger())
			{
				return;
			}
			const SvTrig::ObjectIDParameters& rObjectIDParam = pInspection->GetPPQ()->GetTrigger()->getObjectIDParameters();

			long outputCount {pInspection->GetPPQ()->getOutputCount()};
			SVObjectPtrVector outputList;
			outputList.resize(outputCount);

			for (const SVIOEntryHostStructPtr pOutputEntry : pInspection->GetPPQ()->getUsedOutputs())
			{
				if (m_InspectionID == pOutputEntry->m_inspectionId)
				{
					SVOutputObject* pOutput = dynamic_cast<SVOutputObject*> (SVObjectManagerClass::Instance().GetObject(pOutputEntry->m_IOId));
					if (nullptr != pOutput)
					{
						int outputIndex = pOutput->GetChannel();
						if (SVIOObjectType::IO_PLC_OUTPUT == pOutputEntry->m_ObjectType)
						{
							outputIndex %= outputCount;
						}
						if (outputIndex < outputCount)
						{
							outputList[outputIndex] = pOutputEntry->getObject();
						}
					}
				}
			}

			m_regTest.initialize(pMainFrame->GetSafeHwnd(), hRegressionWnd, pInspection, rObjectIDParam, std::move(outputList));
			DWORD dwThreadID;
			m_RegressionTestHandle = ::CreateThread(nullptr, 0, SVRegressionTestRunThread, (LPVOID)this, 0, &dwThreadID);
		}
	}
	else
	{
		SvStl::MessageManager Msg(SvStl::MsgType::Log | SvStl::MsgType::Display);
		Msg.setMessage(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_SVIPDoc_GoIntoRegTestFailed, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_10058);
	}
}

////////////////////////////////////////////////////////////////////////////////
// .Description : Loads and inits dyn. Menus
//				:	Utility Menu
//				:	Tool Set Draw Menu
////////////////////////////////////////////////////////////////////////////////
void SVIPDoc::InitMenu()
{
	CWnd* pWindow = AfxGetMainWnd();
	if (nullptr != pWindow)
	{
		SVUtilities util;
		// Load and init Utility Menu
		CMenu* pMenu = pWindow->GetMenu();
		pMenu = util.FindSubMenuByName(pMenu, _T("&Utilities"));
		if (nullptr != pMenu)
		{
			util.LoadMenu(pMenu);
		}

		// Load and init Tool Set Draw Menu
		int pos = 0;
		pMenu = ::SVFindMenuByCommand(pWindow->GetMenu(), ID_VIEW_TOOLSETDRAW, TRUE, pos);
		SVToolSet* pSet = GetToolSet();
		if (nullptr != pMenu && nullptr != pSet)
		{
			SvVol::SVEnumerateValueObjectClass* pEnum = pSet->GetDrawFlagObject();
			CMenu myPopUp;
			if (nullptr != pEnum && myPopUp.CreatePopupMenu())
			{
				// Populate pop up with enumerations of tool set draw flag...
				UINT uiCommand = ID_VIEW_TOOLSETDRAW_POP_BASE;
				for (auto const& rEntry : pEnum->GetEnumVector())
				{
					myPopUp.AppendMenu(MF_STRING, uiCommand++, rEntry.first.c_str());
				}

				// Get current name of ID_VIEW_TOOLSETDRAW menu...
				CString strMenu = _T("Display");
				pMenu->GetMenuString(pos, strMenu, MF_BYPOSITION);

				// Transfer pop up...
				if (pMenu->ModifyMenu(pos, MF_BYPOSITION | MF_POPUP, reinterpret_cast<UINT_PTR> (myPopUp.GetSafeHmenu()), strMenu))
				{
					myPopUp.Detach();
				}

				pWindow->DrawMenuBar();
			}
		}
	}
}

////////////////////////////////////////////////////////////////////////////////
// .Description : To deactivate dummy tool set draw menu, if tool set draw menu
//				: cannot be inited.
//				: Refer to InitMenu().
////////////////////////////////////////////////////////////////////////////////
void SVIPDoc::OnUpdateViewToolSetDraw(CCmdUI* pCmdUI)
{
	if (pCmdUI->m_pSubMenu)
	{
		unsigned int l_uiGray = SVSVIMStateClass::CheckState(SV_STATE_EDIT) ? 0 : MF_GRAYED;
		pCmdUI->m_pMenu->EnableMenuItem(pCmdUI->m_nIndex, MF_BYPOSITION | l_uiGray);
	}
	else
	{
		pCmdUI->Enable(SVSVIMStateClass::CheckState(SV_STATE_EDIT));
	}
}

////////////////////////////////////////////////////////////////////////////////
// .Description : Updates all Tool Set Draw menu items, depending on
//				: CheckCurrentUserAccess( ID_VIEW_TOOLSETDRAW ) and
//				: current set draw flag.
//				:
//				: Range of nId must be 
//				: [ ID_VIEW_TOOLSETDRAW_POP_BASE...ID_VIEW_TOOLSETDRAW_POP_MAX ]
////////////////////////////////////////////////////////////////////////////////
void SVIPDoc::OnUpdateViewToolSetDrawSubMenus(CCmdUI* PCmdUI)
{
	PCmdUI->Enable(SVSVIMStateClass::CheckState(SV_STATE_EDIT) && !SVSVIMStateClass::CheckState(SV_STATE_RUNNING));

	SVToolSet* pSet = GetToolSet();

	if (pSet && !SVSVIMStateClass::CheckState(SV_STATE_LOADING | SV_STATE_CANCELING | SV_STATE_CLOSING))
	{
		SvVol::SVEnumerateValueObjectClass* pEnum = pSet->GetDrawFlagObject();
		if (pEnum)
		{
			// Get current value of enumeration object.
			long lValue = 0L;
			pEnum->GetValue(lValue);

			// Get Menu value...
			long lMenuValue = PCmdUI->m_nID - ID_VIEW_TOOLSETDRAW_POP_BASE;

			// Set Radio, if equal current draw flag...
			PCmdUI->SetRadio(lValue == lMenuValue);
		}
	}
}

////////////////////////////////////////////////////////////////////////////////
// .Description : Called if user 'clicks' on a tool set draw menu item.
//				: Range of nId must be 
//				: [ ID_VIEW_TOOLSETDRAW_POP_BASE...ID_VIEW_TOOLSETDRAW_POP_MAX ]
////////////////////////////////////////////////////////////////////////////////
void SVIPDoc::OnChangeToolSetDrawFlag(UINT nId)
{
	assert(nId >= ID_VIEW_TOOLSETDRAW_POP_BASE && nId <= ID_VIEW_TOOLSETDRAW_POP_MAX);

	// Access denied...
	if (!TheSVObserverApp().OkToEdit()) { return; }

	if (nId >= ID_VIEW_TOOLSETDRAW_POP_BASE && nId <= ID_VIEW_TOOLSETDRAW_POP_MAX)
	{
		CWnd* pWindow = AfxGetMainWnd();
		if (GetToolSet() && pWindow)
		{
			// Get Menu...
			int pos = 0;
			CMenu* pMenu = ::SVFindMenuByCommand(pWindow->GetMenu(), nId, TRUE, pos);

			// Get Tool Set Draw Flag...
			SvVol::SVEnumerateValueObjectClass* pEnum = GetToolSet()->GetDrawFlagObject();

			if (pEnum && pMenu)
			{
				// Get correlated enumerator...
				CString Temp;
				pMenu->GetMenuString(pos, Temp, MF_BYPOSITION);
				// Set Flag...
				pEnum->setValue(std::string(Temp));

				// Update Menu...
				pMenu->CheckMenuRadioItem(0, pMenu->GetMenuItemCount() - 1, pos, MF_BYPOSITION);

				// Update views...
				UpdateAllViews(nullptr);
			}
		}
	}
}

void SVIPDoc::RefreshDocument()
{
	std::set<uint32_t> imageIdSet;

	for (int i = 0; i < MaxImageViews; ++i)
	{
		SVImageView* l_pImageView = GetImageView(i);

		if (nullptr != l_pImageView)
		{
			//@TODO[MZA][10.10][09.11.2020] if we want have different overlays depending of direct or over group tool input, this here have to be changed.
			uint32_t imageId = l_pImageView->GetImageID().m_imageId;
			if (SvDef::InvalidObjectId != imageId)
			{
				imageIdSet.insert(imageId);
			}
		}
	}


	CommandInspectionCollectImageData collectImageData(m_InspectionID, imageIdSet);
	CollectImageDataTask collectImageDataTask(std::move(collectImageData));


	std::future<std::shared_ptr<SVIPProductStruct>> futureProductStruct = collectImageDataTask.get_future();

	if (m_InspectionID > 0)
	{

		SvOi::IInspectionProcess* pInspection = dynamic_cast<SvOi::IInspectionProcess*>((SvOi::getObject(m_InspectionID)));
		if (pInspection)
		{
			SvOi::ICommandPtr pCommand = std::make_shared<  SvOi::CTaskWrapper<CollectImageDataTask>>(std::move(collectImageDataTask));
			pInspection->SubmitCommand(pCommand);
		}
		else
		{
			assert(false);

		}

		std::shared_ptr<SVIPProductStruct> pProduct;
		std::future_status status = futureProductStruct.wait_for(std::chrono::milliseconds {120000});
		if (status == std::future_status::ready)
		{
			pProduct = futureProductStruct.get();
			if (m_NewProductData.size() == m_NewProductData.capacity())
			{
				m_NewProductData.PopHead();
			}


			if (S_OK == m_NewProductData.PushTail(*pProduct.get()))
			{
				CWnd* pWnd = GetMDIChild();
				if (nullptr != pWnd && ::IsWindow(pWnd->GetSafeHwnd()))
				{
					pWnd->PostMessage(SV_MDICHILDFRAME_UPDATE_ALL_DATA);
				}
			}


		}
		else
		{

		}
	}

}

void SVIPDoc::RecreateImageSurfaces()
{
	for (int i = 0; i < MaxImageViews; ++i)
	{
		SVImageView* pImageView = GetImageView(i);

		if (pImageView) { pImageView->RecreateImageSurface(); }
	}
}

void SVIPDoc::OnUpdateFileExit(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(!SVSVIMStateClass::CheckState(SV_STATE_RUNNING) && !SVSVIMStateClass::CheckState(SV_STATE_REGRESSION) && !SVSVIMStateClass::CheckState(SV_STATE_TEST)
		&& TheSecurityManager().SVIsDisplayable(SECURITY_POINT_FILE_MENU_EXIT));
}

CFile* SVIPDoc::GetFile(LPCTSTR lpszFileName, UINT nOpenFlags, CFileException* pError)
{
	UINT nNewFlags = nOpenFlags & ~(CFile::shareDenyRead | CFile::shareDenyWrite) |
		CFile::shareDenyNone;

	return CDocument::GetFile(lpszFileName, nNewFlags, pError);
}

bool SVIPDoc::GetParameters(SvOi::IObjectWriter& rWriter)
{
	_variant_t svVariant;

	SVInspectionProcess* pInspection(GetInspectionProcess());

	if (nullptr == pInspection) { return false; }

	SaveViews(rWriter);

	// Save the View Placements
	SaveViewPlacements(rWriter);

	SaveToolGroupings(rWriter);

	SaveViewedVariables(rWriter);

	SaveRegressionTestVariables(rWriter);

	return true;
}

void SVIPDoc::SaveViewedVariables(SvOi::IObjectWriter& rWriter)
{
	SVResultList* pResultList = GetResultList();


	rWriter.StartElement(SvXml::CTAG_VIEWEDVARIABLES);
	if (pResultList)
	{
		pResultList->Save(rWriter);
		//pResultList->m_PPQInputReferences.Save(rWriter);
	}
	rWriter.EndElement();
}

bool SVIPDoc::LoadViewedVariables(SVTreeType& rTree, SVTreeType::SVBranchHandle htiParent)
{
	SVResultList* pResultList = GetResultList();
	if (nullptr == pResultList)
	{
		return false;
	}
	return  pResultList->LoadViewedVariables(rTree, htiParent);

}

void SVIPDoc::SaveRegressionTestVariables(SvOi::IObjectWriter& rWriter)
{
	rWriter.StartElement(SvXml::CTAG_REGRESSIONTEST);
	_variant_t Value(m_regTest.usePlayCondition());
	rWriter.WriteAttribute(SvXml::CTAG_USE_PLAY_CONDITION, Value);
	Value = m_regTest.getPlayEquationController()->GetEquationText().c_str();
	rWriter.WriteAttribute(SvXml::CTAG_PLAY_CONDITION_EQUATION, Value);
	rWriter.EndElement();
}

SvDef::StringSet SVIPDoc::TranslateSelectedObjects(const SVObjectReferenceVector& rSelectedObjects, const std::string& rInspectionName) const
{
	SvDef::TranslateMap TranslateNames;
	if (!rInspectionName.empty())
	{
		std::string SearchName;
		std::string ReplaceName;
		SearchName = rInspectionName + '.' + SvDef::RemoteInputStr;
		ReplaceName = SvDef::FqnPPQVariables;
		ReplaceName += '.' + SvDef::RemoteInputStr;
		TranslateNames[SearchName] = ReplaceName;
		SearchName = rInspectionName + '.' + SvDef::DioInputStr;
		ReplaceName = SvDef::FqnPPQVariables;
		ReplaceName += '.' + SvDef::DioInputStr;
		TranslateNames[SearchName] = ReplaceName;
		SearchName = rInspectionName + _T(".");
		SearchName += SvUl::LoadedStrings::g_ToolSetName;
		TranslateNames[SearchName] = SvUl::LoadedStrings::g_ToolSetName;
	}

	SvDef::StringSet SelectedObjectNames;
	for (auto const& rItem : rSelectedObjects)
	{
		std::string Name {rItem.GetCompleteName(true)};

		const auto& iter = std::find_if(TranslateNames.begin(), TranslateNames.end(),
			[&Name](auto& rEntry) { return 0 == Name.find(rEntry.first); });
		if (TranslateNames.end() != iter)
		{
			Name.replace(0, iter->first.size(), iter->second.c_str());
		}
		SelectedObjectNames.insert(Name);
	}
	return SelectedObjectNames;
}

void SVIPDoc::LoadRegressionTestVariables(SVTreeType& rTree, SVTreeType::SVBranchHandle htiParent)
{
	SVTreeType::SVBranchHandle htiRegression = nullptr;

	bool bOk = SvXml::SVNavigateTree::GetItemBranch(rTree, SvXml::CTAG_REGRESSIONTEST, htiParent, htiRegression);
	if (bOk)
	{
		_variant_t svVariant;
		bOk = SvXml::SVNavigateTree::GetItem(rTree, SvXml::CTAG_USE_PLAY_CONDITION, htiRegression, svVariant);
		if (bOk)
		{
			m_regTest.usePlayCondition(svVariant);
		}
		bOk = SvXml::SVNavigateTree::GetItem(rTree, SvXml::CTAG_PLAY_CONDITION_EQUATION, htiRegression, svVariant);
		if (bOk)
		{
			m_regTest.setLoadEquationText(SvUl::createStdString(svVariant));
		}
	}
}

void SVIPDoc::SaveViews(SvOi::IObjectWriter& rWriter)
{
	SVViewUnion View;
	_variant_t svVariant;
	rWriter.StartElement(SvXml::CTAG_IPDOC_VIEWS);
	rWriter.StartElement(SvXml::CTAG_VIEWS);

	POSITION vPos = GetFirstViewPosition();

	if (vPos)
	{
		long ViewNumber = 0;

		while (nullptr != (View.pView = GetNextView(vPos)))
		{
			std::string viewClassName = getViewClassName(View.pView);
			assert(false == viewClassName.empty());
			svVariant = ++ViewNumber;
			if (false == viewClassName.empty() && cSVImageViewName != viewClassName)
			{
				rWriter.StartElement(viewClassName.c_str());
				rWriter.WriteAttribute(SvXml::CTAG_VIEW_NUMBER, svVariant);
				svVariant.Clear();

				//Note Image View scroll saves also the Image view parameters
				if (cSVImageViewScrollName == viewClassName)
				{
					View.pImageScroll->GetParameters(rWriter);
				}
				else if (cToolSetViewName == viewClassName)
				{
					View.pToolsetView->GetParameters(rWriter);
				}
				else if (cResultTabbedViewName == viewClassName)
				{
					View.pResultView->GetParameters(rWriter);
				}
				rWriter.EndElement();
			}
		}
	}
	rWriter.EndElement();
	rWriter.EndElement();

	// save height of the Result View
	svVariant = m_nHeightResultView;
	rWriter.WriteAttribute(SvXml::CTAG_HEIGHT_RESULT_VIEW, svVariant);
	svVariant.Clear();

	// save width of the Toolset View
	svVariant = m_nWidthToolSetView;
	rWriter.WriteAttribute(SvXml::CTAG_WIDTH_TOOLSET_VIEW, svVariant);
	svVariant.Clear();
}

void SVIPDoc::SaveViewPlacements(SvOi::IObjectWriter& rWriter)
{
	_variant_t svVariant;
	SVViewUnion View;

	WINDOWPLACEMENT wndpl;
	ZeroMemory(&wndpl, sizeof(WINDOWPLACEMENT));
	wndpl.length = sizeof(WINDOWPLACEMENT);

	View.pToolsetView = dynamic_cast<ToolSetView*>(getView());
	if (View.pToolsetView && View.pToolsetView->GetSafeHwnd())
	{
		CSplitterWnd* pWndSplitter = dynamic_cast<CSplitterWnd*>(View.pToolsetView->GetParent());
		if (pWndSplitter && pWndSplitter->GetSafeHwnd())
		{
			CSplitterWnd* pWndSplitter2 = dynamic_cast<CSplitterWnd*>(pWndSplitter->GetParent());
			assert(pWndSplitter2 && pWndSplitter2->GetSafeHwnd());

			SVIPSplitterFrame* pFrame = dynamic_cast<SVIPSplitterFrame*>(pWndSplitter2->GetParent());
			if (nullptr != pFrame && nullptr != pFrame->GetSafeHwnd())
			{
				pFrame->GetWindowPlacement(&wndpl);
			}
		}
	}

	// Save Window Placement
	rWriter.StartElement(SvXml::CTAG_WINDOW_PLACEMENT);

	svVariant = wndpl.length;
	rWriter.WriteAttribute(SvXml::CTAG_LENGTH, svVariant);
	svVariant.Clear();

	svVariant = wndpl.flags;
	rWriter.WriteAttribute(SvXml::CTAG_FLAGS, svVariant);
	svVariant.Clear();

	svVariant = wndpl.showCmd;
	rWriter.WriteAttribute(SvXml::CTAG_SHOW_COMMAND, svVariant);
	svVariant.Clear();

	// Save Minimum Position
	rWriter.StartElement(SvXml::CTAG_MINIMUM_POSITION);
	svVariant = wndpl.ptMinPosition.x;
	rWriter.WriteAttribute(SvXml::CTAG_X, svVariant);
	svVariant.Clear();

	svVariant = wndpl.ptMinPosition.y;
	rWriter.WriteAttribute(SvXml::CTAG_Y, svVariant);
	svVariant.Clear();
	rWriter.EndElement();

	// Save Maximum Position
	rWriter.StartElement(SvXml::CTAG_MAXIMUM_POSITION);
	svVariant = wndpl.ptMaxPosition.x;
	rWriter.WriteAttribute(SvXml::CTAG_X, svVariant);
	svVariant.Clear();

	svVariant = wndpl.ptMaxPosition.y;
	rWriter.WriteAttribute(SvXml::CTAG_Y, svVariant);
	svVariant.Clear();
	rWriter.EndElement();

	// Save Normal Position
	rWriter.StartElement(SvXml::CTAG_NORMAL_POSITION);
	svVariant = wndpl.rcNormalPosition.left;
	rWriter.WriteAttribute(SvXml::CTAG_LEFT, svVariant);
	svVariant.Clear();

	svVariant = wndpl.rcNormalPosition.top;
	rWriter.WriteAttribute(SvXml::CTAG_TOP, svVariant);
	svVariant.Clear();

	svVariant = wndpl.rcNormalPosition.right;
	rWriter.WriteAttribute(SvXml::CTAG_RIGHT, svVariant);
	svVariant.Clear();

	svVariant = wndpl.rcNormalPosition.bottom;
	rWriter.WriteAttribute(SvXml::CTAG_BOTTOM, svVariant);
	svVariant.Clear();
	rWriter.EndElement();

	rWriter.EndElement();
}

void SVIPDoc::SaveToolGroupings(SvOi::IObjectWriter& rWriter)
{
	m_toolGroupings.GetParameters(rWriter);
}


bool SVIPDoc::SetParameters(SVTreeType& rTree, SVTreeType::SVBranchHandle htiParent)
{
	bool bOk = false;

	SVViewUnion View;
	_variant_t svVariant;

	SVInspectionProcess* pInspection(GetInspectionProcess());

	if (nullptr == pInspection) { return false; }

	bOk = SvXml::SVNavigateTree::GetItem(rTree, SvXml::CTAG_HEIGHT_RESULT_VIEW, htiParent, svVariant);
	if (bOk)
	{
		m_nHeightResultView = svVariant;

		bOk = SvXml::SVNavigateTree::GetItem(rTree, SvXml::CTAG_WIDTH_TOOLSET_VIEW, htiParent, svVariant);
		if (bOk)
		{
			m_nWidthToolSetView = svVariant;
		}
	}

	if (bOk)
	{
		WINDOWPLACEMENT wndpl;

		SVTreeType::SVBranchHandle htiWindow = nullptr;

		bOk = SvXml::SVNavigateTree::GetItemBranch(rTree, SvXml::CTAG_WINDOW_PLACEMENT, htiParent, htiWindow);
		if (bOk)
		{
			bOk = SvXml::SVNavigateTree::GetItem(rTree, SvXml::CTAG_LENGTH, htiWindow, svVariant);
			if (bOk)
			{
				wndpl.length = svVariant;
				bOk = SvXml::SVNavigateTree::GetItem(rTree, SvXml::CTAG_FLAGS, htiWindow, svVariant);
				if (bOk) { wndpl.flags = svVariant; }
			}

			if (bOk)
			{
				bOk = SvXml::SVNavigateTree::GetItem(rTree, SvXml::CTAG_SHOW_COMMAND, htiWindow, svVariant);
				if (bOk) { wndpl.showCmd = svVariant; }
			}

			if (bOk)
			{
				SVTreeType::SVBranchHandle htiData = nullptr;

				bOk = SvXml::SVNavigateTree::GetItemBranch(rTree, SvXml::CTAG_MINIMUM_POSITION, htiWindow, htiData);
				if (bOk)
				{
					bOk = SvXml::SVNavigateTree::GetItem(rTree, SvXml::CTAG_X, htiData, svVariant);
					if (bOk)
					{
						wndpl.ptMinPosition.x = svVariant;
						bOk = SvXml::SVNavigateTree::GetItem(rTree, SvXml::CTAG_Y, htiData, svVariant);
						if (bOk) { wndpl.ptMinPosition.y = svVariant; }
					}
				}
			}

			if (bOk)
			{
				SVTreeType::SVBranchHandle htiData = nullptr;

				bOk = SvXml::SVNavigateTree::GetItemBranch(rTree, SvXml::CTAG_MAXIMUM_POSITION, htiWindow, htiData);
				if (bOk)
				{
					bOk = SvXml::SVNavigateTree::GetItem(rTree, SvXml::CTAG_X, htiData, svVariant);
					if (bOk)
					{
						wndpl.ptMaxPosition.x = svVariant;
						bOk = SvXml::SVNavigateTree::GetItem(rTree, SvXml::CTAG_Y, htiData, svVariant);
						if (bOk) { wndpl.ptMaxPosition.y = svVariant; }
					}
				}
			}

			if (bOk)
			{
				SVTreeType::SVBranchHandle htiData = nullptr;

				bOk = SvXml::SVNavigateTree::GetItemBranch(rTree, SvXml::CTAG_NORMAL_POSITION, htiWindow, htiData);
				if (bOk)
				{
					bOk = SvXml::SVNavigateTree::GetItem(rTree, SvXml::CTAG_LEFT, htiData, svVariant);
					if (bOk)
					{
						wndpl.rcNormalPosition.left = svVariant;
						bOk = SvXml::SVNavigateTree::GetItem(rTree, SvXml::CTAG_TOP, htiData, svVariant);
						if (bOk) { wndpl.rcNormalPosition.top = svVariant; }
					}

					if (bOk)
					{
						bOk = SvXml::SVNavigateTree::GetItem(rTree, SvXml::CTAG_RIGHT, htiData, svVariant);
						if (bOk) { wndpl.rcNormalPosition.right = svVariant; }
					}

					if (bOk)
					{
						bOk = SvXml::SVNavigateTree::GetItem(rTree, SvXml::CTAG_BOTTOM, htiData, svVariant);
						if (bOk) { wndpl.rcNormalPosition.bottom = svVariant; }
					}
				}
			}
		}

		if (bOk)
		{
			//
			// The first view is the Tool Set view.
			//
			View.pToolsetView = dynamic_cast<ToolSetView*>(getView());
			if (View.pToolsetView && View.pToolsetView->GetSafeHwnd())
			{
				CSplitterWnd* pWndSplitter = dynamic_cast<CSplitterWnd*>(View.pToolsetView->GetParent());
				if (pWndSplitter && pWndSplitter->GetSafeHwnd())
				{
					//
					// This the one we want to retrieve the size and position.
					//
					CSplitterWnd* pWndSplitter2 = dynamic_cast<CSplitterWnd*>(pWndSplitter->GetParent());
					assert(pWndSplitter2 && pWndSplitter2->GetSafeHwnd());

					SVIPSplitterFrame* pFrame = dynamic_cast<SVIPSplitterFrame*>(pWndSplitter2->GetParent());

					if (nullptr != pFrame && nullptr != pFrame->GetSafeHwnd())
					{
						bOk = pFrame->SetWindowPlacement(&wndpl) ? true : false;
					}
				}
			}
		}
	}

	if (bOk)
	{
		SVTreeType::SVBranchHandle htiIPViews = nullptr;
		SVTreeType::SVBranchHandle htiViews = nullptr;

		bOk = SvXml::SVNavigateTree::GetItemBranch(rTree, SvXml::CTAG_IPDOC_VIEWS, htiParent, htiIPViews);
		if (bOk)
		{
			// Serialze View Data...
			mbInitImagesByName = true;
			POSITION vPos;
			std::string Name;

			bOk = SvXml::SVNavigateTree::GetItemBranch(rTree, SvXml::CTAG_VIEWS, htiIPViews, htiViews);
			if (bOk)
			{
				SVTreeType::SVBranchHandle htiItem(nullptr);

				htiItem = rTree.getFirstBranch(htiViews);

				while (bOk && nullptr != htiItem)
				{
					Name = rTree.getBranchName(htiItem);

					// The class SVToolSetTabView was changed to ToolSetView when the Tool Set Tree View was removed.
					SvUl::searchAndReplace(Name, SvXml::CTAG_SVTOOLSET_TAB_VIEW_CLASS, SvXml::CTAG_TOOLSET_VIEW);

					bOk = SvXml::SVNavigateTree::GetItem(rTree, SvXml::CTAG_VIEW_NUMBER, htiItem, svVariant);
					if (bOk)
					{
						vPos = GetFirstViewPosition();
						if (vPos)
						{
							long lViewNumber = svVariant;
							// get the view specified by 'ViewNumber'
							for (long lNumberOfViews = 0; lNumberOfViews < lViewNumber; lNumberOfViews++)
							{
								View.pView = GetNextView(vPos);
								if (nullptr == View.pView)
								{
									break;  // if there are not enough views, exit the loop
								}
							}

							std::string viewClassName = getViewClassName(View.pView);
							if (viewClassName != Name)
							{
								viewClassName.clear();
							}

							if (nullptr != View.pView && false == viewClassName.empty() && cSVImageViewName != viewClassName)
							{
								//Note Image View scroll saves also the Image view parameters
								if (cSVImageViewScrollName == viewClassName)
								{
									bOk = View.pImageScroll->SetParameters(rTree, htiItem);
								}
								else if (cToolSetViewName == viewClassName)
								{
									bOk = View.pToolsetView->SetParameters(rTree, htiItem);
								}
								else if (cResultTabbedViewName == viewClassName)
								{
									bOk = View.pResultView->SetParameters(rTree, htiItem);
								}
							}
						}
					}
					htiItem = rTree.getNextBranch(htiViews, htiItem);
				}
			}

			bOk = SvXml::SVNavigateTree::GetItemBranch(rTree, SvXml::CTAG_VIEWS, htiIPViews, htiViews);
			if (bOk)
			{
				SVTreeType::SVBranchHandle htiItem(nullptr);

				htiItem = rTree.getFirstBranch(htiViews);

				while (bOk && nullptr != htiItem)
				{
					Name = rTree.getBranchName(htiItem);

					bOk = SvXml::SVNavigateTree::GetItem(rTree, SvXml::CTAG_VIEW_NUMBER, htiItem, svVariant);
					if (bOk)
					{
						vPos = GetFirstViewPosition();
						if (vPos)
						{
							long lViewNumber = svVariant;
							// get the view specified by 'ViewNumber'
							for (long lNumberOfViews = 0; lNumberOfViews < lViewNumber; lNumberOfViews++)
							{
								View.pView = GetNextView(vPos);
								if (!View.pView)
									break;  // if there are not enough views, exit the loop
							}

							std::string viewClassName = getViewClassName(View.pView);
							if (viewClassName != Name)
							{
								viewClassName.clear();
							}

							if (nullptr != View.pView && false == viewClassName.empty() && cSVImageViewName != viewClassName)
							{
								//Note Image View scroll saves also the Image view parameters
								if (cSVImageViewScrollName == viewClassName)
								{
									bOk = View.pImageScroll->CheckParameters(rTree, htiItem);
								}
								else if (cToolSetViewName == viewClassName)
								{
									bOk = View.pToolsetView->CheckParameters(rTree, htiItem);
								}
								else if (cResultTabbedViewName == viewClassName)
								{
									bOk = View.pResultView->CheckParameters(rTree, htiItem);
								}
							}
						}
					}
					htiItem = rTree.getNextBranch(htiViews, htiItem);
				}
			}
		}
		if (bOk)
		{
			HRESULT hr = m_toolGroupings.SetParameters(rTree, htiParent);
			bOk = (S_OK == hr) ? true : false;
			if (bOk)
			{
				//correct corrupt configuration with missing tools in toolgrouping
				// Initialize the tool Groupings (for older saved configurations)

				SvOi::ObjectInfoVector toolsetinfo = GetToolSetInfo();
				int nchanged(0);
				if (m_toolGroupings.Correct(toolsetinfo, nchanged))
				{
					std::vector<std::string>  ssv;
					ssv.push_back(std::to_string(nchanged));
					TRACE1("Correct groupings add : %i  Items", nchanged);
					SvStl::MessageManager Msg(SvStl::MsgType::Log);
					Msg.setMessage(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_Correct_Grouping_Add_S_Items, ssv, SvStl::SourceFileParams(StdMessageParams));
				}
			}
		}

		if (bOk)
		{
			bOk = LoadViewedVariables(rTree, htiParent);
		}

		if (bOk)
		{
			LoadRegressionTestVariables(rTree, htiParent);
		}
	}
	return bOk;
}

void SVIPDoc::SetModifiedFlag(BOOL bModified /*= TRUE*/)
{
	CDocument::SetModifiedFlag(bModified);
	if (bModified)
	{
		SVSVIMStateClass::AddState(SV_STATE_MODIFIED);
	}
}

double  SVIPDoc::getResultDefinitionUpdatTimeStamp() const
{
	double Result(0.0);
	SVResultList* pResultList = GetResultList();

	if (nullptr != pResultList)
	{
		Result = pResultList->getUpdateTimeStamp();
	}

	return Result;
}

HRESULT SVIPDoc::GetResultDefinitions(SVResultDefinitionVector& rDefinitions) const
{
	HRESULT hres = E_FAIL;
	rDefinitions.clear();
	SVResultList* pResultList = GetResultList();
	if (nullptr != pResultList)
	{

		hres = pResultList->GetResultDefinitions(rDefinitions);

	}

	return hres;
}

void SVIPDoc::GetResultData(SvIe::SVIPResultData& rResultData) const
{
	rResultData = m_Results;
}

std::vector <SvIe::IPResultTableData> SVIPDoc::getResultTableData() const
{
	SVInspectionProcess* pInspection(GetInspectionProcess());
	if (nullptr != pInspection)
	{
		SVResultList* pResultList = pInspection->GetResultList();
		if (nullptr != pResultList && nullptr != m_triggerRecord && m_triggerRecord->isObjectUpToTime())
		{
			return pResultList->getResultTableData(*m_triggerRecord);
		}
	}
	return {};
}



void SVIPDoc::OnAddExternalTool()
{
	AddTool(SvPb::ExternalToolClassId);
}

HRESULT SVIPDoc::RemoveImage(SvIe::SVImageClass* pImage)
{
	return RemoveImage(pImage->getObjectId());
}

HRESULT SVIPDoc::RemoveImage(uint32_t imageId)
{
	HRESULT l_Status = S_FALSE;

	for (int i = 0; i < MaxImageViews; ++i)
	{
		SVImageView* l_pView = GetImageView(i);

		if (l_pView->GetImageID().m_imageId == imageId)
		{
			l_pView->DetachFromImage();

			l_Status = S_OK;
		}
	}

	return l_Status;
}

void SVIPDoc::RefreshPublishedList()
{
	SVPublishList& publishList = GetInspectionProcess()->GetPublishList();
	publishList.Refresh(dynamic_cast<SvIe::SVTaskObjectClass*> (GetToolSet()));

	SetModifiedFlag();
}

SvIe::SVVirtualCameraPtrVector SVIPDoc::GetCameras() const
{
	SvIe::SVVirtualCameraPtrVector cameraVector;

	if (nullptr != GetInspectionProcess())
	{
		cameraVector = GetInspectionProcess()->GetCameras();
	}
	return cameraVector;
}


bool SVIPDoc::deleteTool(SvTo::SVToolClass* pTool)
{
	CWaitCursor waitCursor;

	// remove image from affected displays
	std::vector<SvOi::IObjectClass*> outputList;
	pTool->getOutputList(std::back_inserter(outputList));

	for (int i = 0; i < MaxImageViews; i++)
	{
		SVImageView* pImageView = GetImageView(i);
		if (pImageView->GetImage())
		{
			uint32_t viewImageUniqueId = pImageView->GetImage()->getObjectId();
			if (std::any_of(outputList.begin(), outputList.end(), [viewImageUniqueId](const auto* pObject)
			{ return nullptr != pObject && viewImageUniqueId == pObject->getObjectId(); }))
			{
				// Close Display resources...
				pImageView->DetachFromImage();
			}
		}
	}

	SvPb::InspectionCmdRequest requestCmd;
	SvPb::InspectionCmdResponse responseCmd;
	auto* pRequest = requestCmd.mutable_deleteobjectrequest();
	pRequest->set_flag(SvPb::DeleteObjectRequest::Flag_SetDefaultInputs_And_ResetInspection);
	pRequest->set_objectid(pTool->getObjectId());
	SvCmd::InspectionCommands(m_InspectionID, requestCmd, &responseCmd);
	if (S_OK == responseCmd.hresult())
	{
		RebuildOutputObjectListHelper(GetTheIODoc());
	}
	else if (responseCmd.has_standardresponse())
	{
		SvStl::MessageContainerVector errorMsgContainer = SvPb::convertProtobufToMessageVector(responseCmd.standardresponse().errormessages());
		if (errorMsgContainer.size() > 0)
		{
			SvStl::MessageManager Msg(SvStl::MsgType::Log | SvStl::MsgType::Display);
			Msg.setMessage(errorMsgContainer.at(0).getMessage());
		}

		return false;
	}


	return true;
}



void SVIPDoc::OnEditAdjustToolPosition()
{
	SvTo::SVToolClass* pTool = dynamic_cast<SvTo::SVToolClass*> (SVObjectManagerClass::Instance().GetObject(Get1stSelectedToolID()));

	if (nullptr != pTool)
	{
		//------ Warp tool hands back a SvDef::SVPolarTransformObjectType. Sub type 1792.
		//------ Window tool, Luminance hands back a SvDef::SVImageObjectType. Sub type 0.
		if (GetImageView())
		{
			SVSVIMStateClass::SetResetState stateEditing {SV_STATE_EDITING};
			std::string DlgName = SvUl::Format(_T("Adjust Tool Size and Position - %s"), pTool->GetName());
			SvOg::SVAdjustToolSizePositionDlg dlg(m_InspectionID, pTool->getObjectId(), DlgName.c_str(), dynamic_cast<CWnd*>(this->GetMDIChild()));
			dlg.DoModal();
		}
	}
}

void SVIPDoc::OnUpdateEditAdjustToolPosition(CCmdUI* pCmdUI)
{
	// Check current user access...
	if (!TheSVObserverApp().OkToEdit())
	{
		return pCmdUI->Enable(false);
	}
	ToolSetView* pToolsetView = GetToolSetView();
	SVToolSet* pToolSet = GetToolSet();
	if (nullptr == pToolSet && nullptr == pToolsetView || pToolsetView->IsLabelEditing())
	{
		return pCmdUI->Enable(false);
	}

	auto pNavElement = pToolsetView->Get1stSelNavigatorElement();

	if (!pNavElement || nullptr == pNavElement)
	{
		return pCmdUI->Enable(false);
	}
	std::string Selection(pNavElement->m_DisplayName);
	bool enabled(false);
	switch (pNavElement->m_Type)
	{
		case NavElementType::SubTool:
		case NavElementType::Tool:
		case NavElementType::LoopTool:
		case NavElementType::GroupTool:
			enabled = true;
			break;
	}
	uint32_t SelectedId(pNavElement->m_navigatorObjectId);
	if (!enabled || SvDef::InvalidObjectId == SelectedId)
	{
		return pCmdUI->Enable(false);
	}

	//Tool list active and valid tool
	SvTo::SVToolClass* Tool = dynamic_cast<SvTo::SVToolClass*> (SVObjectManagerClass::Instance().GetObject(SelectedId));
	if (Tool)
	{
		//check to see if the tool has extents
		if (Tool->DoesObjectHaveExtents() && SvPb::TransformationToolClassId != Tool->GetClassID())
		{
			return pCmdUI->Enable(true);
		}
	}
	pCmdUI->Enable(false);
}

void SVIPDoc::OnShowToolRelations()
{
	SvOg::SVShowDependentsDialog Dlg(GetSelectedToolIds(), SvPb::SVToolObjectType, nullptr, SvOg::SVShowDependentsDialog::Show);
	Dlg.DoModal();
}

void SVIPDoc::OnUpdateShowToolRelations(CCmdUI* pCmdUI)
{
	// Check current user access...
	bool Enabled = TheSVObserverApp().OkToEdit();

	pCmdUI->Enable(Enabled);
}

void SVIPDoc::OnToolDependencies()
{
	SVToolSet* pToolSet = GetToolSet();
	if (nullptr != pToolSet)
	{
		SVSVIMStateClass::SetResetState stateEditing {SV_STATE_EDITING};
		bool bFullAccess = TheSecurityManager().SVIsDisplayable(SECURITY_POINT_UNRESTRICTED_FILE_ACCESS);
		constexpr const TCHAR* Filter = _T("GraphViz Files (*.dot)|*.dot||");
		SvMc::SVFileDialog fileDlg(false, bFullAccess, _T("dot"), nullptr, 0, Filter, nullptr);
		fileDlg.m_ofn.lpstrTitle = _T("Select File");
		if (fileDlg.DoModal() == IDOK)
		{
			CWaitCursor MouseBusy;
			std::set<uint32_t> ToolIDSet;
			pToolSet->GetToolIds(std::inserter(ToolIDSet, ToolIDSet.end()));
			SvDef::StringPairVector dependencyList;
			SvOi::getToolDependency(std::back_inserter(dependencyList), ToolIDSet, SvPb::SVToolObjectType, SvOi::ToolDependencyEnum::Client, false, fileDlg.GetPathName().GetString());
		}
	}
}

void SVIPDoc::OnUpdateToolDependencies(CCmdUI* PCmdUI)
{
	bool Enabled = TheSVObserverApp().OkToEdit();

	PCmdUI->Enable(Enabled);
}

void SVIPDoc::OnSaveConfigReport()
{
	SVConfigurationObject* pConfig(nullptr);
	SVObjectManagerClass::Instance().GetConfigurationObject(pConfig);

	if (nullptr != pConfig)
	{
		if (pConfig->IsConfigurationLoaded())
		{
			bool bFullAccess = TheSecurityManager().SVIsDisplayable(SECURITY_POINT_UNRESTRICTED_FILE_ACCESS);
			constexpr const TCHAR* Filter = _T("Extensible Markup Language (*.xml)|*.xml||");
			SvMc::SVFileDialog fileDlg(false, bFullAccess, _T("xml"), nullptr, 0, Filter, nullptr);
			fileDlg.m_ofn.lpstrTitle = _T("Export Configuration Report");
			if (fileDlg.DoModal() == IDOK)
			{
				std::string report;
				SVVisionProcessorHelper::Instance().GetConfigurationPrintReport(report);
				std::ofstream exportFile(fileDlg.GetPathName().GetString());
				exportFile << report;
			}
		}
	}
}

void SVIPDoc::OnUpdateSaveConfigReport(CCmdUI* PCmdUI)
{
	bool Enabled = TheSVObserverApp().OkToEdit();

	PCmdUI->Enable(Enabled);
}

void SVIPDoc::OnUpdateAddGeneralTool(CCmdUI* PCmdUI)
{
	bool Enabled = TheSVObserverApp().OkToEdit();

	PCmdUI->Enable(Enabled);
}

void SVIPDoc::OnUpdateAddCylindricalWarpTool(CCmdUI* pCmdUI)
{
	bool Enabled = !SVSVIMStateClass::CheckState(SV_STATE_RUNNING | SV_STATE_TEST);

	Enabled = Enabled && TheSVObserverApp().OkToEdit() && isImageAvailable(SvPb::SVImageMonoType);

	if (pCmdUI->m_pSubMenu)
	{
		unsigned int l_uiGray = Enabled ? 0 : MF_GRAYED;
		pCmdUI->m_pMenu->EnableMenuItem(pCmdUI->m_nIndex, MF_BYPOSITION | l_uiGray);
	}
	else
	{
		pCmdUI->Enable(Enabled);
	}
}

void SVIPDoc::OnUpdateAddTransformationTool(CCmdUI* pCmdUI)
{
	bool Enabled = !SVSVIMStateClass::CheckState(SV_STATE_RUNNING | SV_STATE_TEST);
	Enabled = Enabled && TheSVObserverApp().OkToEdit() && isImageAvailable(SvPb::SVImageMonoType);

	pCmdUI->Enable(Enabled);
}

void SVIPDoc::OnUpdateAddColorTool(CCmdUI* PCmdUI)
{
	bool Enabled = !SVSVIMStateClass::CheckState(SV_STATE_RUNNING | SV_STATE_TEST);
	// Check current user access...
	Enabled = Enabled && TheSVObserverApp().OkToEdit() && isImageAvailable(SvPb::SVImageColorType);

	PCmdUI->Enable(Enabled);
}

afx_msg void SVIPDoc::OnUpdateAddToolWithSubTools(CCmdUI* PCmdUI)
{
	bool Enabled = !SVSVIMStateClass::CheckState(SV_STATE_RUNNING | SV_STATE_TEST);
	// Check current user access...
	Enabled = Enabled && TheSVObserverApp().OkToEdit();
	ToolSetView* pView = GetToolSetView();
	if (!pView || pView->IsSubToolSelected())
	{
		//no Loop tool as sub tool
		Enabled = false;
	}

	PCmdUI->Enable(Enabled);
}

void SVIPDoc::OnViewResetAllCounts()
{
	if (S_OK == TheSecurityManager().SVValidate(SECURITY_POINT_VIEW_MENU_RESET_COUNTS_ALL))
	{
		TheSVObserverApp().ResetAllCounts();
	}
}

void SVIPDoc::OnViewResetCountsCurrentIP()
{
	if (S_OK == TheSecurityManager().SVValidate(SECURITY_POINT_VIEW_MENU_RESET_COUNTS_CURRENT))
	{
		SVInspectionProcess* pInspection = GetInspectionProcess();

		// @TODO:  What should we do if there's an error?  (Method is void.)
		if (nullptr != pInspection && nullptr != pInspection->GetToolSet())
		{
			pInspection->GetToolSet()->ResetCounts();
		}

		if (!SVSVIMStateClass::CheckState(SV_STATE_RUNNING | SV_STATE_REGRESSION))
		{
			RunOnce();
		}
	}
}

DWORD WINAPI SVIPDoc::SVRegressionTestRunThread(LPVOID lpParam)
{
	SVIPDoc* pIPDoc = reinterpret_cast<SVIPDoc*> (lpParam);

	return pIPDoc->m_regTest.runThread();
}

void SVIPDoc::RegressionTestComplete()
{
	SVMainFrame* pMainFrm(dynamic_cast<SVMainFrame*> (AfxGetApp()->m_pMainWnd));
	if (nullptr != pMainFrm && nullptr != pMainFrm->m_pregTestDlg)
	{
		pMainFrm->m_pregTestDlg->DestroyWindow();
		delete pMainFrm->m_pregTestDlg;
		pMainFrm->m_pregTestDlg = nullptr;
	}

	CloseHandle(m_RegressionTestHandle);
	SVSVIMStateClass::RemoveState(SV_STATE_REGRESSION);
}

void SVIPDoc::RegressionTestModeChanged()
{

	// @WARNING:  Pointers should be checked before they are dereferenced.
	::BringWindowToTop(AfxGetMainWnd()->GetSafeHwnd());
	m_regTest.stopRunning();

	SVSVIMStateClass::RemoveState(SV_STATE_REGRESSION);

	SVToolSet* pToolSet = GetToolSet();
	if (nullptr != pToolSet)
	{
		pToolSet->goingOffline(); // Mainly used to tell Archive Tool to shutdown
		pToolSet->ResetObject();
	}
	RunOnce();
}

bool SVIPDoc::IsRegressionTestRunning()
{
	return m_regTest.isRunning();
}

std::set<uint32_t> SVIPDoc::GetSelectedToolIds() const
{
	std::set<uint32_t> ids;
	ToolSetView* pView = GetToolSetView();
	if (pView)
	{
		for (auto id : pView->GetAllSelectedToolIds())
		{
			ids.insert(id);
		}
	}

	return ids;
}

uint32_t SVIPDoc::Get1stSelectedToolID() const
{
	ToolSetView* pView = GetToolSetView();
	if (pView)
	{
		return pView->Get1stSelToolId();
	}
	else
	{
		return SvDef::InvalidObjectId;
	}
}

void SVIPDoc::SetSelectedToolID(uint32_t toolID)
{
	ToolSetView* pView = GetToolSetView();
	if (pView)
	{
		pView->selectTool(toolID);
	}
}

bool SVIPDoc::IsToolPreviousToSelected(uint32_t toolID) const
{
	bool l_Status = false;
	if (nullptr != GetToolSet())
	{
		l_Status = GetToolSet()->IsToolPreviousToSelected(toolID);
	}

	return l_Status;
}

std::string SVIPDoc::GetCompleteToolSetName() const
{
	std::string Result;

	SVToolSet* pToolSet = GetToolSet();

	if (nullptr != pToolSet)
	{
		Result = pToolSet->GetObjectNameToObjectType(SvPb::SVToolObjectType);
	}

	return Result;
}

const SVToolGrouping& SVIPDoc::GetToolGroupings() const
{
	return m_toolGroupings;
}

SVToolGrouping& SVIPDoc::GetToolGroupings()
{
	return m_toolGroupings;
}

void SVIPDoc::OnUpdateViewResetCountsAllIPs(CCmdUI* pCmdUI)
{
	// @WARNING:  Pointers should be checked before they are dereferenced.
	pCmdUI->Enable(TheSecurityManager().SVIsDisplayable(SECURITY_POINT_VIEW_MENU_RESET_COUNTS_ALL));
}

void SVIPDoc::OnUpdateViewResetCountsCurrentIP(CCmdUI* pCmdUI)
{
	// @WARNING:  Pointers should be checked before they are dereferenced.
	pCmdUI->Enable(TheSecurityManager().SVIsDisplayable(SECURITY_POINT_VIEW_MENU_RESET_COUNTS_CURRENT));
}

void SVIPDoc::OnEditDataDefinitionLists()
{
	SVInspectionProcess* pInspection = GetInspectionProcess();

	if (nullptr != pInspection)
	{
		SVSVIMStateClass::SetResetState stateEditing {SV_STATE_EDITING};
		std::string inspectionName = pInspection->GetName();
		std::string Title = _T("Data Definition Lists - ");
		Title += inspectionName;
		SVDataDefinitionSheet sheet(this, Title.c_str(), inspectionName, pInspection->getObjectId());

		//remove apply button
		sheet.m_psh.dwFlags |= PSH_NOAPPLYNOW;

		sheet.DoModal();
	}
}

void SVIPDoc::OnUpdateEditDataDefinitionLists(CCmdUI* pCmdUI)
{
	// @WARNING:  Pointers should be checked before they are dereferenced.
	pCmdUI->Enable(SVSVIMStateClass::CheckState(SV_STATE_READY) && SVSVIMStateClass::CheckState(SV_STATE_EDIT));
}

HRESULT SVIPDoc::RebuildImages()
{
	HRESULT l_Status = S_OK;

	SVImageIdImageDataStructMap::iterator l_Iter = m_Images.begin();

	while (l_Iter != m_Images.end())
	{
		SVMasterImageRegisterMap::iterator l_RegisteredIter = m_RegisteredImages.find(l_Iter->first);

		if (l_RegisteredIter != m_RegisteredImages.end())
		{
			// If image from m_Images map is in m_RegisteredImages map
			SVImageViewPtrImageViewStatusMap::iterator l_ViewIter = l_Iter->second.m_ImageViews.begin();

			// While a view is found for the m_Images image 
			while (l_ViewIter != l_Iter->second.m_ImageViews.end())
			{
				// try to find the a view in the m_RegisteredImages map for the
				// associated image that matched the m_Images based image.
				SVImageViewPtrSet::iterator l_RegisteredViewIter = l_RegisteredIter->second.find(l_ViewIter->first);

				if (l_RegisteredViewIter != l_RegisteredIter->second.end())
				{
					// If the view was found in the m_RegisteredImage, then 
					// move to the next m_Images view.
					++l_ViewIter;
				}
				else
				{
					// else remove the m_Images view.
					l_ViewIter = l_Iter->second.m_ImageViews.erase(l_ViewIter);
				}
			}

			++l_Iter;  // m_Images iterator
		}
		else
		{
			// If image from m_Images map is not in m_RegisteredImages map 
			// then delete from m_Images map.
			l_Iter = m_Images.erase(l_Iter);
		}
	}

	SVMasterImageRegisterMap::iterator l_RegisteredIter = m_RegisteredImages.begin();

	while (l_RegisteredIter != m_RegisteredImages.end())
	{
		SVImageViewPtrSet::iterator l_RegisteredViewIter = l_RegisteredIter->second.begin();

		while (l_RegisteredViewIter != l_RegisteredIter->second.end())
		{
			SVImageDataStruct& l_rImage = m_Images[l_RegisteredIter->first];

			SVImageViewPtrImageViewStatusMap::iterator l_ViewIter = l_rImage.m_ImageViews.find(*l_RegisteredViewIter);

			if (l_ViewIter == l_rImage.m_ImageViews.end())
			{
				l_rImage.m_ImageViews[*l_RegisteredViewIter] = SVImageViewStatusStruct();
			}

			if (l_rImage.m_ImageData.empty())
			{
				SvIe::SVImageClass* pImage = dynamic_cast<SvIe::SVImageClass*>(SVObjectManagerClass::Instance().GetObject(l_RegisteredIter->first));

				if ((nullptr != pImage))
				{
					SVBitmapInfo l_Info;

					BITMAPINFOHEADER l_Header = pImage->GetImageInfo().GetBitmapInfoHeader();

					l_Info.Assign(l_Header.biWidth, l_Header.biHeight, l_Header.biBitCount, SVBitmapInfo::GetDefaultColorTable(l_Header.biBitCount));

					l_rImage.m_ImageData.resize(l_Info.GetBitmapInfoSizeInBytes() + l_Info.GetBitmapImageSizeInBytes(), 0);

					::memcpy(&(l_rImage.m_ImageData[0]), l_Info.GetBitmapInfo(), l_Info.GetBitmapInfoSizeInBytes());
				}
			}

			++l_RegisteredViewIter;
		}

		++l_RegisteredIter;
	}

	::InterlockedExchange(&m_AllViewsUpdated, 0);

	return l_Status;
}

HRESULT SVIPDoc::CheckImages()
{
	HRESULT l_Status = S_OK;

	if (m_AllViewsUpdated == 0)
	{
		bool l_Refresh = true;

		SVImageIdImageDataStructMap::iterator l_Iter = m_Images.begin();

		while (l_Refresh && l_Iter != m_Images.end())
		{
			SVImageViewPtrImageViewStatusMap::iterator l_ViewIter = l_Iter->second.m_ImageViews.begin();

			while (l_Refresh && l_ViewIter != l_Iter->second.m_ImageViews.end())
			{
				l_Refresh &= l_ViewIter->second.m_DisplayComplete;

				++l_ViewIter;
			}

			++l_Iter;
		}

		if (l_Refresh)
		{
			::InterlockedExchange(&m_AllViewsUpdated, 1);

			m_oDisplay.SetIPDocDisplayComplete();
		}
	}

	return l_Status;
}

HRESULT SVIPDoc::RegisterImage(uint32_t imageId, SVImageView* p_pImageView)
{
	HRESULT l_Status = S_OK;

	SVMasterImageRegisterMap::iterator l_Iter = m_RegisteredImages.begin();

	while (l_Iter != m_RegisteredImages.end())
	{
		l_Iter->second.erase(p_pImageView);

		if (l_Iter->second.empty())
		{
			l_Iter = m_RegisteredImages.erase(l_Iter);
		}
		else
		{
			++l_Iter;
		}
	}

	if (SvDef::InvalidObjectId != imageId)
	{
		m_RegisteredImages[imageId].insert(p_pImageView);

		l_Status = RebuildImages();
	}
	else
	{
		l_Status = E_FAIL;
	}

	SVInspectionProcess* pInspection(GetInspectionProcess());

	if (nullptr != pInspection) { pInspection->LastProductNotify(); }

	return l_Status;
}

HRESULT SVIPDoc::UnregisterImageView(SVImageView* p_pImageView)
{
	HRESULT l_Status = S_OK;

	SVMasterImageRegisterMap::iterator l_Iter = m_RegisteredImages.begin();

	while (l_Iter != m_RegisteredImages.end())
	{
		l_Iter->second.erase(p_pImageView);

		if (l_Iter->second.empty())
		{
			l_Iter = m_RegisteredImages.erase(l_Iter);
		}
		else
		{
			++l_Iter;
		}
	}

	l_Status = RebuildImages();

	return l_Status;
}

HRESULT SVIPDoc::IsImageDataUpdated(uint32_t imageId, SVImageView* p_pImageView) const
{
	HRESULT l_Status = S_OK;

	if (m_AllViewsUpdated == 0)
	{
		SVImageIdImageDataStructMap::const_iterator l_Iter = m_Images.find(imageId);

		if (l_Iter != m_Images.end())
		{
			SVImageViewPtrImageViewStatusMap::const_iterator l_ViewIter = l_Iter->second.m_ImageViews.find(p_pImageView);

			if (l_ViewIter != l_Iter->second.m_ImageViews.end())
			{
				if (!(l_ViewIter->second.m_ViewDataUpdated))
				{
					l_Status = S_FALSE;
				}
			}
		}
	}

	return l_Status;
}

HRESULT SVIPDoc::GetBitmapInfo(uint32_t imageId, SVBitmapInfo& p_rBitmapInfo) const
{
	HRESULT l_Status = S_OK;

	SvIe::SVImageClass* pImage = dynamic_cast<SvIe::SVImageClass*>(SVObjectManagerClass::Instance().GetObject(imageId));

	if (nullptr != pImage)
	{
		BITMAPINFOHEADER l_Header = pImage->GetImageInfo().GetBitmapInfoHeader();

		p_rBitmapInfo.Assign(l_Header.biWidth, l_Header.biHeight, l_Header.biBitCount, SVBitmapInfo::GetDefaultColorTable(l_Header.biBitCount));
	}
	else
	{
		p_rBitmapInfo.clear();

		l_Status = E_FAIL;
	}

	return l_Status;
}

HRESULT SVIPDoc::GetImageData(uint32_t imageId, std::string& p_rImageData, SVExtentMultiLineStructVector& p_rMultiLineArray) const
{
	HRESULT l_Status = S_OK;

	SVImageIdImageDataStructMap::const_iterator l_Iter = m_Images.find(imageId);

	if (l_Iter != m_Images.end())
	{
		p_rImageData = l_Iter->second.m_ImageData;
		p_rMultiLineArray = l_Iter->second.m_OverlayData;
	}
	else
	{
		p_rImageData.clear();
		p_rMultiLineArray.clear();

		l_Status = E_FAIL;
	}

	return l_Status;
}

HRESULT SVIPDoc::SetImageData(uint32_t imageId, const std::string& p_rImageData, const SVExtentMultiLineStructVector& p_rMultiLineArray)
{
	HRESULT l_Status = S_OK;

	SVImageIdImageDataStructMap::iterator l_Iter = m_Images.find(imageId);

	if (l_Iter != m_Images.end())
	{
		l_Iter->second.m_ImageData = p_rImageData;
		l_Iter->second.m_OverlayData = p_rMultiLineArray;

		SVImageViewPtrImageViewStatusMap::iterator l_ViewIter = l_Iter->second.m_ImageViews.begin();

		while (l_ViewIter != l_Iter->second.m_ImageViews.end())
		{
			l_ViewIter->second.m_ViewNotified = false;
			l_ViewIter->second.m_ViewDataUpdated = false;
			l_ViewIter->second.m_DisplayComplete = false;

			++l_ViewIter;
		}
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

HRESULT SVIPDoc::MarkImageDataUpdated(uint32_t imageId, SVImageView* p_pImageView)
{
	HRESULT l_Status = S_OK;

	if (m_AllViewsUpdated == 0)
	{
		SVImageIdImageDataStructMap::iterator l_Iter = m_Images.find(imageId);

		if (l_Iter != m_Images.end())
		{
			SVImageViewPtrImageViewStatusMap::iterator l_ViewIter = l_Iter->second.m_ImageViews.find(p_pImageView);

			if (l_ViewIter != l_Iter->second.m_ImageViews.end())
			{
				l_ViewIter->second.m_ViewDataUpdated = true;
			}
			else
			{
				l_Status = E_FAIL;
			}
		}
		else
		{
			l_Status = E_FAIL;
		}
	}

	return l_Status;
}

HRESULT SVIPDoc::MarkImageDataDisplayed(uint32_t imageId, SVImageView* p_pImageView)
{
	HRESULT l_Status = S_OK;

	if (m_AllViewsUpdated == 0)
	{
		SVImageIdImageDataStructMap::iterator l_Iter = m_Images.find(imageId);

		if (l_Iter != m_Images.end())
		{
			SVImageViewPtrImageViewStatusMap::iterator l_ViewIter = l_Iter->second.m_ImageViews.find(p_pImageView);

			if (l_ViewIter != l_Iter->second.m_ImageViews.end())
			{
				if (!(l_ViewIter->second.m_DisplayComplete))
				{
					l_ViewIter->second.m_DisplayComplete = true;

					l_Status = CheckImages();
				}
			}
			else
			{
				l_Status = E_FAIL;
			}
		}
		else
		{
			l_Status = E_FAIL;
		}
	}

	return l_Status;
}

HRESULT SVIPDoc::UpdateExtents(SvIe::SVTaskObjectClass* pTask, const SVImageExtentClass& rExtents)
{
	HRESULT l_Status = SVGuiExtentUpdater::UpdateImageExtent(pTask, rExtents);
	if (S_OK == l_Status) { SetModifiedFlag(); }
	return l_Status;
}

HRESULT SVIPDoc::UpdateExtentsToFit(SvIe::SVTaskObjectClass* pTask, const SVImageExtentClass& rExtents)
{
	HRESULT l_Status = SVGuiExtentUpdater::SetImageExtentToFit(pTask, rExtents);
	if (S_OK == l_Status) { SetModifiedFlag(); }
	return l_Status;
}

HRESULT SVIPDoc::UpdateWithLastProduct()
{
	HRESULT result {S_OK};
	SVInspectionProcess* pInspection = GetInspectionProcess();

	if (nullptr != pInspection)
	{
		pInspection->LastProductNotify();
	}
	else
	{
		result = E_FAIL;
	}

	return result;
}

bool SVIPDoc::RunOnce()
{
	SVInspectionProcess* pInspection = GetInspectionProcess();
	bool l_Status = (nullptr != pInspection);

	if (l_Status)
	{
		l_Status = (S_OK == SvCmd::RunOnceSynchronous(pInspection->getObjectId()));
	}

	return l_Status;
}

uint32_t SVIPDoc::GetObjectIdFromToolToInsertBefore(const std::string& rName) const
{
	uint32_t result = SvDef::InvalidObjectId;

	const SVToolSet* pToolSet = GetToolSet();
	if (nullptr != pToolSet)
	{
		if (!rName.empty())
		{
			std::string ToolName = m_toolGroupings.GetToolToInsertBefore(rName);
			if (!ToolName.empty())
			{
				bool bFound = false;
				// FindTool by Name in SVTaskObjectList
				for (int i = 0; i < pToolSet->GetSize() && !bFound; i++)
				{
					const SvIe::SVTaskObjectClass* pTool = pToolSet->getTaskObject(i);
					if (pTool && pTool->GetName() == ToolName)
					{
						result = pTool->getObjectId();
						bFound = true;
					}
				}
			}
		}
	}
	return result;
}


bool SVIPDoc::isImageAvailable(SvPb::SVObjectSubTypeEnum ImageSubType) const
{
	bool Result {false};

	SvPb::InspectionCmdRequest requestCmd;
	SvPb::InspectionCmdResponse responseCmd;
	auto* pRequest = requestCmd.mutable_getavailableobjectsrequest();
	auto* pTreeSearchParameter = pRequest->mutable_tree_search();
	pTreeSearchParameter->set_search_start_id(m_InspectionID);
	pTreeSearchParameter->mutable_type_info()->set_objecttype(SvPb::SVImageObjectType);
	pTreeSearchParameter->mutable_type_info()->set_subtype(ImageSubType);
	pTreeSearchParameter->mutable_isbeforetoolmethod()->set_toolid(Get1stSelectedToolID());

	HRESULT hr = SvCmd::InspectionCommands(m_InspectionID, requestCmd, &responseCmd);
	SvUl::NameObjectIdList availableList;
	if (S_OK == hr && responseCmd.has_getavailableobjectsresponse())
	{
		Result = (0 < responseCmd.getavailableobjectsresponse().list().size());
	}

	return Result;
}


SVIPDoc* NewSVIPDoc(LPCTSTR DocName, SVInspectionProcess& Inspection)
{
	SVIPDoc* pDoc = nullptr;
	CDocTemplate* pDocTemplate = nullptr;
	POSITION pos = AfxGetApp()->GetFirstDocTemplatePosition();
	if (pos)
	{
		pDocTemplate = AfxGetApp()->GetNextDocTemplate(pos);
		if (pDocTemplate)
		{
			pDoc = dynamic_cast<SVIPDoc*>(pDocTemplate->OpenDocumentFile(nullptr, TRUE));   // Make visible

			if (nullptr != pDoc)
			{
				pDoc->SetInspectionID(Inspection.getObjectId());
				pDoc->SetRegressionTestPlayEquationController(Inspection.getRegressionTestPlayConditionController());

				pDoc->SetTitle(DocName);
			}
		}
	}

	return pDoc;
}


SVIPDoc* GetIPDocByInspectionID(uint32_t inspectionID)
{
	SVIPDoc* pIPDoc(nullptr);
	POSITION pos = AfxGetApp()->GetFirstDocTemplatePosition();
	while (pos && !pIPDoc)
	{
		CDocTemplate* pDocTemplate = AfxGetApp()->GetNextDocTemplate(pos);
		if (pDocTemplate)
		{
			POSITION posDoc = pDocTemplate->GetFirstDocPosition();
			while (posDoc && !pIPDoc)
			{
				SVIPDoc* pDoc = dynamic_cast <SVIPDoc*>(pDocTemplate->GetNextDoc(posDoc));
				if (nullptr != pDoc)
				{
					if (pDoc->GetInspectionID() == inspectionID)
					{
						pIPDoc = pDoc;
					}
				}
			}
		}
	}
	return pIPDoc;
}


void ResetAllIPDocModifyFlag(BOOL bModified)
{
	POSITION pos = AfxGetApp()->GetFirstDocTemplatePosition();
	if (pos)
	{
		do
		{
			CDocTemplate* pDocTemplate = AfxGetApp()->GetNextDocTemplate(pos); //@TODO [Arvid][10.20][27.10.2021] similar control structures occur multiple times. Use std::algorithm?
			if (pDocTemplate)
			{
				POSITION posDoc = pDocTemplate->GetFirstDocPosition();
				if (posDoc)
				{
					do
					{
						CDocument* newDoc = pDocTemplate->GetNextDoc(posDoc);
						if (newDoc)
						{
							SVIPDoc* pTmpDoc = dynamic_cast <SVIPDoc*> (newDoc);

							if (nullptr != pTmpDoc)
							{
								pTmpDoc->SetModifiedFlag(bModified);
							}
						}
					} while (posDoc);
				}
			}
		} while (pos);
	}
	//also set the IODoc Modified flag
	SVIODoc* pIODoc = GetTheIODoc();
	if (pIODoc)
	{
		pIODoc->SetModifiedFlag(bModified);
	}
}


void SetAllIPDocumentsOffline()
{
	//get list of IPDoc's.

	POSITION pos = AfxGetApp()->GetFirstDocTemplatePosition();
	if (pos)
	{
		do
		{
			CDocTemplate* pDocTemplate = AfxGetApp()->GetNextDocTemplate(pos);
			if (pDocTemplate)
			{
				POSITION posDoc = pDocTemplate->GetFirstDocPosition();
				if (posDoc)
				{
					do
					{
						CDocument* newDoc = pDocTemplate->GetNextDoc(posDoc);
						if (newDoc)
						{
							SVIPDoc* pTmpDoc = dynamic_cast <SVIPDoc*> (newDoc);

							if (nullptr != pTmpDoc)
							{
								pTmpDoc&& pTmpDoc->GoOffline();
							}
						}
					} while (posDoc);
				}
			}
		} while (pos);
	}
}




HRESULT RebuildOutputObjectListHelper(SVIODoc* pIODoc)
{
	HRESULT l_Status = S_OK;

	SVConfigurationObject* pConfig(nullptr);
	SVObjectManagerClass::Instance().GetConfigurationObject(pConfig);

	if (nullptr != pConfig)
	{
		pConfig->ValidateRemoteMonitorList();
		pConfig->RebuildOutputObjectList();

		if (pIODoc)
		{
			pIODoc->UpdateAllViews(nullptr);
		}
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}


CDocTemplate* CreateIpDocMultiDocTemplate()
{
	// Dokumentvorlagen der Anwendung registrieren. Dokumentvorlagen
	//  dienen als Verbindung zwischen Dokumenten, Rahmenfenstern und Ansichten.
	return new SVMultiDocTemplate(IDR_SVOBSERVER_IPDOCTYPE,
		RUNTIME_CLASS(SVIPDoc),			 // Doc
		RUNTIME_CLASS(SVIPSplitterFrame),  // Frame
		RUNTIME_CLASS(SVImageViewScroll));// View
}


void RefreshAllIPDocuments()
{
	//get list of IPDoc's.

	POSITION pos = AfxGetApp()->GetFirstDocTemplatePosition();
	if (pos)
	{
		do
		{
			CDocTemplate* pDocTemplate = AfxGetApp()->GetNextDocTemplate(pos);
			if (pDocTemplate)
			{
				POSITION posDoc = pDocTemplate->GetFirstDocPosition();
				if (posDoc)
				{
					do
					{
						CDocument* newDoc = pDocTemplate->GetNextDoc(posDoc);
						if (newDoc)
						{
							SVIPDoc* pTmpDoc = dynamic_cast <SVIPDoc*> (newDoc);
							if (nullptr != pTmpDoc)
							{
								pTmpDoc->UpdateWithLastProduct();
							}
						}
					} while (posDoc);
				}
			}
		} while (pos);
	}
}

void RunAllIPDocuments()
{
	//get list of IPDoc's.
	SVSVIMStateClass::SVRCBlocker block;
	POSITION pos = AfxGetApp()->GetFirstDocTemplatePosition();
	if (pos)
	{
		do
		{
			CDocTemplate* pDocTemplate = AfxGetApp()->GetNextDocTemplate(pos);
			if (pDocTemplate)
			{
				POSITION posDoc = pDocTemplate->GetFirstDocPosition();
				if (posDoc)
				{
					do
					{
						CDocument* newDoc = pDocTemplate->GetNextDoc(posDoc);
						if (newDoc)
						{
							SVIPDoc* pTmpDoc = dynamic_cast <SVIPDoc*> (newDoc);
							if (nullptr != pTmpDoc)
							{
								pTmpDoc->RunOnce();
							}
						}
					} while (posDoc);
				}
			}
		} while (pos);
	}
}

void SetAllIPDocumentsOnline()
{
	//get list of IPDoc's.
	SVSVIMStateClass::SVRCBlocker block;
	POSITION pos = AfxGetApp()->GetFirstDocTemplatePosition();
	if (pos)
	{
		do
		{
			CDocTemplate* pDocTemplate = AfxGetApp()->GetNextDocTemplate(pos);
			if (pDocTemplate)
			{
				POSITION posDoc = pDocTemplate->GetFirstDocPosition();
				if (posDoc)
				{
					do
					{
						CDocument* newDoc = pDocTemplate->GetNextDoc(posDoc);
						if (newDoc)
						{
							SVIPDoc* pTmpDoc = dynamic_cast <SVIPDoc*> (newDoc);

							if (nullptr != pTmpDoc)
							{
								pTmpDoc&& pTmpDoc->GoOnline();
							}
						}
					} while (posDoc);
				}
			}
		} while (pos);
	}
}

bool mayDeleteCurrentlySelectedTools(const std::set<uint32_t>& rIdsOfObjectsDependedOn)
{
	SVSVIMStateClass::SetResetState stateEditing {SV_STATE_EDITING};

	std::string FormatText = SvUl::LoadStdString(IDS_DELETE_CHECK_DEPENDENCIES);
	std::string DisplayText = SvUl::Format(FormatText.c_str(), "the selected tools'", "the selected tools", "the selected tools", "the selected tools");

	return IDOK == SvOg::showDependentsDialogIfNecessary(rIdsOfObjectsDependedOn, DisplayText);
}


SvTo::SVToolClass* getCorrespondingToolPointer(NavigatorElement* pNaviElement, uint32_t inspectionID)
{
	if (NavElementType::SubTool == pNaviElement->m_Type || NavElementType::SubLoopTool == pNaviElement->m_Type)
	{
		SvPb::InspectionCmdRequest requestCmd;
		SvPb::InspectionCmdResponse responseCmd;
		auto* pRequest = requestCmd.mutable_getobjectparametersrequest();
		pRequest->set_objectid(pNaviElement->m_OwnerId);

		HRESULT hr = SvCmd::InspectionCommands(inspectionID, requestCmd, &responseCmd);
		if (S_OK == hr && responseCmd.has_getobjectparametersresponse())
		{
			if (SvPb::SVToolObjectType != responseCmd.getobjectparametersresponse().typeinfo().objecttype() ||
				(responseCmd.getobjectparametersresponse().typeinfo().subtype() && SvPb::GroupToolObjectType != responseCmd.getobjectparametersresponse().typeinfo().subtype()))
			{
				return nullptr;
			}
		}
		else
		{
			return nullptr;
		}
	}

	return dynamic_cast<SvTo::SVToolClass*> (SVObjectManagerClass::Instance().GetObject(pNaviElement->m_navigatorObjectId));
}


