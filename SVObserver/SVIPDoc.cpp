//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision, Inc., Harrisburg
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
#include "SVIPChildFrm.h"
#include "SVInspectionProcess.h"
#include "SVLightReferenceDialog.h"
#include "SVLutDlg.h"
#include "SVMainFrm.h"
#include "SVObjectScriptParser.h"
#include "SVObserver.h"
#include "SVPPQObject.h"
#include "SVRegressionRunDlg.h"
#include "SVToolAdjustmentDialogSheetClass.h"
#include "SVToolSet.h"
#include "SVUtilities.h"
#include "ToolClipboard.h"
#include "ToolSetView.h"
#include "Definitions/GlobalConst.h"
#include "Definitions/ObjectDefines.h"
#include "Definitions/StringTypeDef.h"
#include "Definitions/SVUserMessage.h"
#include "InspectionCommands/CommandExternalHelper.h"
#include "InspectionEngine/SVImageProcessingClass.h"
#include "InspectionEngine/SVAcquisitionClass.h"
#include "ObjectInterfaces/IDependencyManager.h"
#include "ObjectInterfaces/IObjectWriter.h"
#include "ObjectSelectorLibrary/ObjectTreeGenerator.h"
#include "Operators/SVConditional.h"
#include "SVCommandLibrary/SVObjectSynchronousCommandTemplate.h"
#include "SVFileSystemLibrary/SVFileNameManagerClass.h"
#include "SVHBitmapUtilitiesLibrary\SVHBitmapUtilities.h"
#include "SVMessage/SVMessage.h"
#include "SVMFCControls/SVFileDialog.h"
#include "SVObjectLibrary/SVObjectLevelCreateStruct.h"
#include "SVObjectLibrary/SVObjectManagerClass.h"
#include "SVOGui/ResultTableSelectionDlg.h"
#include "SVOGui/SVAdjustToolSizePositionDlg.h"
#include "SvOGui/SVFormulaEditorSheet.h"
#include "SVOGui/SVSaveToolSetImageDialog.h"
#include "SVOGui/SVShowDependentsDialog.h"
#include "SVOGui/TextDefinesSvOg.h"
#include "SVOResource/ConstGlobalSvOr.h"
#include "SVProtoBuf/InspectionCommands.h"
#include "SVStatusLibrary/GlobalPath.h"
#include "SVStatusLibrary/SVSVIMStateClass.h"
#include "SVTimerLibrary/SVClock.h"
#include "SVUtilityLibrary/StringHelper.h"
#include "SVUtilityLibrary/SVGUID.h"
#include "SVXMLLibrary/SVConfigurationTags.h"
#include "SVXMLLibrary/SVNavigateTree.h"
#include "SVXMLLibrary/SVObjectXMLWriter.h"
#include "Tools/LoopTool.h"
#include "Tools/SVColorTool.h"
#include "Tools/SVTool.h"
#pragma endregion Includes

#pragma region Declarations

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

union SVViewUnion
{
	CView *pView;
	SVImageViewScroll *pImageScroll;
	SVImageViewClass *pImageView;
	ToolSetView *pToolSetView;
	ResultTabbedView *pResultView;
};

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
	ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
	ON_UPDATE_COMMAND_UI(ID_EDIT_COPY, OnUpdateEditCopy)
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
	ON_COMMAND(ID_EDIT_EDITTOOLSETCONDITION, OnEditToolSetCondition)
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
	ON_UPDATE_COMMAND_UI(ID_EDIT_ADJUSTLIGHTREFERENCE, OnAllowAdjustLightReference)
	ON_UPDATE_COMMAND_UI(ID_EDIT_ADJUSTLUT, OnAllowAdjustLut)
	ON_UPDATE_COMMAND_UI(ID_ADD_STARTTOOLGROUPING, OnUpdateAddStartToolGrouping)
	ON_UPDATE_COMMAND_UI(ID_ADD_ENDTOOLGROUPING, OnUpdateAddEndToolGrouping)
	ON_COMMAND(ID_ADD_STARTTOOLGROUPING, OnAddStartToolGrouping)
	ON_COMMAND(ID_ADD_ENDTOOLGROUPING, OnAddEndToolGrouping)
	ON_COMMAND(ID_TOOL_DEPENDENCIES, OnToolDependencies)
	ON_COMMAND(ID_ADD_LOOPTOOL, OnAddLoopTool)
	ON_UPDATE_COMMAND_UI(ID_ADD_LOADIMAGETOOL, OnUpdateAddGeneralTool)
	ON_UPDATE_COMMAND_UI(ID_ADD_IMAGETOOL, OnUpdateAddGeneralTool)
	ON_UPDATE_COMMAND_UI(ID_ADD_ARCHIVETOOL, OnUpdateAddGeneralTool)
	ON_UPDATE_COMMAND_UI(ID_ADD_MATHTOOL, OnUpdateAddGeneralTool)
	ON_UPDATE_COMMAND_UI(ID_ADD_STATISTICSTOOL, OnUpdateAddGeneralTool)
	ON_UPDATE_COMMAND_UI(ID_ADD_EXTERNAL_TOOL, OnUpdateAddGeneralTool)
	ON_UPDATE_COMMAND_UI(ID_ADD_POLARUNWRAPTOOL, OnUpdateAddGeneralTool)
	ON_UPDATE_COMMAND_UI(ID_ADD_ACQUISITIONTOOL, OnUpdateAddGeneralTool)
	ON_UPDATE_COMMAND_UI(ID_ADD_LINEARTOOL, OnUpdateAddGeneralTool)	
	ON_UPDATE_COMMAND_UI(ID_ADD_RESIZETOOL, OnUpdateAddGeneralTool)
	ON_UPDATE_COMMAND_UI(ID_ADD_RINGBUFFERTOOL, OnUpdateAddGeneralTool)
	ON_UPDATE_COMMAND_UI(ID_ADD_TABLETOOL, OnUpdateAddGeneralTool)
	ON_UPDATE_COMMAND_UI(ID_ADD_TABLEANALYZERTOOL, OnUpdateAddGeneralTool)
	ON_UPDATE_COMMAND_UI(ID_ADD_PERSPECTIVEWARPTOOL, OnUpdateAddGeneralTool)
	ON_UPDATE_COMMAND_UI(ID_ADD_SHIFTTOOL, OnUpdateAddGeneralTool)
	ON_UPDATE_COMMAND_UI(ID_ADD_WINDOWTOOL, OnUpdateAddGeneralTool)
	ON_UPDATE_COMMAND_UI(ID_ADD_CYLINDRICALWARPTOOL, OnUpdateAddCylindricalWarpTool)
	ON_UPDATE_COMMAND_UI(ID_ADD_TRANSFORMATIONTOOL, OnUpdateAddTransformationTool)
	ON_UPDATE_COMMAND_UI(ID_ADD_COLORTOOL, OnUpdateAddColorTool)
	ON_UPDATE_COMMAND_UI(ID_ADD_LOOPTOOL, OnUpdateAddLoopTool)



END_MESSAGE_MAP()
#pragma endregion Declarations

#pragma region Constructor
SVIPDoc::SVIPDoc()
	: CDocument()
	, m_NewProductData(1)
	, m_RegisteredImages()
	, m_Images()
	, m_AllViewsUpdated(0)
	, m_pMDIChildWnd(nullptr)
	, m_ToolSetListTimestamp(0)
	, m_PPQListTimestamp(0)
	, m_bRegressionTestInitEquationText(false)
	, m_RegressionTestLoadEquationText(_T(""))
{
	init();
}
#pragma endregion Constructor

const GUID& SVIPDoc::GetInspectionID() const
{
	return m_InspectionID;
}

void SVIPDoc::UpdateAllData()
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

void SVIPDoc::SetInspectionID(const SVGUID& p_InspectionID)
{
	m_InspectionID = p_InspectionID;
	m_oDisplay.SetInspectionID(m_InspectionID, this);
}

void SVIPDoc::init()
{
	mbInitImagesByName = false;

	IsNew = true;

	TheSVObserverApp.setCurrentDocument(this);

	// Default result height and tool set view width.
	m_nWidthToolSetView = SvDef::cDefaultToolsetViewWidth;
	m_nHeightResultView = SvDef::cDefaultToolsetViewHeight;

	m_bAllowRefresh = false;
	m_bRegressionTestRunning = false;
	m_bRegressionTestUsePlayCondition = false;

	m_oDisplay.Create();
}

//******************************************************************************
// Destructor:
//******************************************************************************
SVIPDoc::~SVIPDoc()
{
	::InterlockedExchange(&m_AllViewsUpdated, 1);
	m_oDisplay.SetIPDocDisplayComplete();

	m_oDisplay.Destroy();

	ClearRegressionTestStructures();

	TheSVObserverApp.setCurrentDocument(nullptr);
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
		SVImageViewClass* l_pView(GetImageView(0));

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
		SVImageViewClass* pView = nullptr;
		int i;

		if (IsNew)
		{
			pView = GetImageView();

			if (nullptr != pView)
			{
				SVGUID l_ImageId;

				if (nullptr != GetToolSet()->getCurrentImage())
				{
					l_ImageId = GetToolSet()->getCurrentImage()->GetUniqueObjectID();
				}

				pView->AttachToImage(l_ImageId);

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
			for (i = 0; i < MaxImageViews; ++i)
			{
				pView = GetImageView(i);

				if (nullptr != pView)
				{
					if (mbInitImagesByName)
					{
						pView->AttachToImage(_T(""));
					}
					else
					{
						SVGUID l_ImageId;

						if (nullptr != GetToolSet()->getCurrentImage())
						{
							l_ImageId = GetToolSet()->getCurrentImage()->GetUniqueObjectID();
						}

						pView->AttachToImage(l_ImageId);
					}
				}
			}
		}
	}

	m_bAllowRefresh = true;

	return l_bOk;
}

std::string SVIPDoc::CheckName(const std::string& rToolName) const
{
	std::string NewName(rToolName);
	if (!m_toolGroupings.IsNameUnique(rToolName.c_str()))
	{
		NewName = m_toolGroupings.MakeNumericUniqueName(rToolName.c_str()).c_str();
	}

	return NewName;
}

//******************************************************************************
// Operation(s) Of Writing Access:
//******************************************************************************
bool SVIPDoc::AddTool(const SVGUID& rClassId)
{
	ToolSetView* pView = GetToolSetView();
	if (nullptr == pView)
	{
		return false;
	}
	pView->getListCtrl().EnsureOneIsSelected();

	int SelectedListIndex(-1);
	PtrNavigatorElement  NavElement = pView->GetSelectedNavigatorElement(&SelectedListIndex);
	if (!NavElement)
	{
		return false;
	}
	std::string Selection(NavElement->m_DisplayName);
	bool bAddToLoopTool(false);
	bool bAddToToolSet(false);
	switch (NavElement->m_Type)
	{
		case NavElementType::SubTool:
		case NavElementType::EndDelimiterLoopTool:
		case NavElementType::SubLoopTool:
			bAddToLoopTool = true;
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
			bAddToToolSet = true;
			break;
	}
	bool Success(false);
	SVGUID newObjectID = GUID_NULL;
	if (bAddToToolSet)
	{
		int toolListIndex = GetToolToInsertBefore(Selection, SelectedListIndex);
		SVToolSetClass* pToolSet = GetToolSet();
		// A Color Tool must always be first on a RGB Color System.
		// There can be multiple Color Tools on a RGB Color System, one of them must be first.
		if (0 == toolListIndex && SVColorToolClassGuid != rClassId)
		{
			if (nullptr != pToolSet && 0 < pToolSet->GetSize())
			{
				SvTo::SVToolClass* pNextTool = dynamic_cast<SvTo::SVToolClass*>(pToolSet->GetAt(0));
				if (pNextTool && SV_IS_KIND_OF(pNextTool, SvTo::SVColorToolClass))
				{
					SvStl::MessageMgrStd Msg(SvStl::MsgType::Log | SvStl::MsgType::Display);
					Msg.setMessage(SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_ColorToolMustBeFirstMessage, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_10055);
					return false;
				}
			}
		}

		SvPb::InspectionCmdMsgs requestMessage, responseMessage;
		SvPb::ConstructAndInsertRequest* pRequest = requestMessage.mutable_constructandinsertrequest();
		SvPb::SetGuidInProtoBytes(pRequest->mutable_ownerid(), pToolSet->GetUniqueObjectID());
		SvPb::SetGuidInProtoBytes(pRequest->mutable_classid(), rClassId);
		pRequest->set_taskobjectpos(toolListIndex);

		HRESULT hr = SvCmd::InspectionCommands(m_InspectionID, requestMessage, &responseMessage);
		if (S_OK == hr && responseMessage.has_constructandinsertresponse())
		{
			Success = true;
			newObjectID = SvPb::GetGuidFromProtoBytes(responseMessage.constructandinsertresponse().objectid());
			m_toolGroupings.AddTool(responseMessage.constructandinsertresponse().name(), Selection);
		}
	}
	else if (bAddToLoopTool)
	{
		SvPb::InspectionCmdMsgs requestMessage, responseMessage;
		SvPb::ConstructAndInsertRequest* pRequest = requestMessage.mutable_constructandinsertrequest();
		SvPb::SetGuidInProtoBytes(pRequest->mutable_ownerid(), NavElement->m_OwnerGuid);
		SvPb::SetGuidInProtoBytes(pRequest->mutable_classid(), rClassId);
		SvPb::SetGuidInProtoBytes(pRequest->mutable_taskobjectafterid(), NavElement->m_Guid);

		HRESULT hr = SvCmd::InspectionCommands(m_InspectionID, requestMessage, &responseMessage);
		if (S_OK == hr && responseMessage.has_constructandinsertresponse())
		{
			Success = true;
			newObjectID = SvPb::GetGuidFromProtoBytes(responseMessage.constructandinsertresponse().objectid());
		}
	}
	if (Success)
	{
		RunOnce();
		// Refresh all views...
		UpdateAllViews(nullptr, SVIPDoc::RefreshView);
		SetSelectedToolID(newObjectID);
		SetModifiedFlag();
		SVToolSetClass* pToolSet{GetToolSet()};
		if(nullptr != pToolSet)
		{
			pToolSet->updateToolPosition();
		}
		SVInspectionProcess* pInspection{GetInspectionProcess()};
		if(nullptr != pInspection)
		{
			pInspection->buildValueObjectDefList();
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
	bool bRetVal = false;
	ToolSetView* pView = GetToolSetView();
	if (nullptr == pView)
	{
		return false;
	}
	pView->getListCtrl().EnsureOneIsSelected();
	
	int SelectedListIndex(-1);
	auto NavElement = pView->GetSelectedNavigatorElement(&SelectedListIndex);
	if (!NavElement)
	{
		return false;
	}
	std::string Selection(NavElement->m_DisplayName);
	switch (NavElement->m_Type)
	{
		case NavElementType::SubTool:
		case NavElementType::EndDelimiterLoopTool:
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

	bRetVal = true;

	return bRetVal;
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
	ToolSetView* pView(nullptr);
	POSITION pos(GetFirstViewPosition());

	while (nullptr == pView && nullptr != pos)
	{
		pView = dynamic_cast<ToolSetView*>(GetNextView(pos));
	}

	return pView;
}

SVImageViewClass* SVIPDoc::GetImageView(int p_Index)
{
	SVImageViewClass* pReturnView(nullptr);
	POSITION pos(GetFirstViewPosition());
	long l_Count(0);

	while (nullptr == pReturnView && nullptr != pos)
	{
		SVImageViewClass* pIPView = dynamic_cast<SVImageViewClass*>(GetNextView(pos));
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

SVToolSetClass* SVIPDoc::GetToolSet() const
{
	SVToolSetClass* l_pObject(nullptr);
	SVInspectionProcess* pInspection(GetInspectionProcess());

	if (nullptr != pInspection)
	{
		l_pObject = pInspection->GetToolSet();
	}

	return l_pObject;
}

bool SVIPDoc::shouldPauseRegressionTestByCondition()
{
	SVInspectionProcess* pInspection(GetInspectionProcess());

	if (RegModePlay == m_regtestRunMode && nullptr != pInspection && m_bRegressionTestUsePlayCondition)
	{
		return pInspection->shouldPauseRegressionTestByCondition();
	}

	return false;
}

SVResultListClass* SVIPDoc::GetResultList() const
{
	SVResultListClass* pResultList(nullptr);
	SVToolSetClass* pToolSet = GetToolSet();
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

	if (TheSVObserverApp.m_svSecurityMgr.SVIsDisplayable(SECURITY_POINT_FILE_MENU_CLOSE_CONFIGURATION))
	{
		if (SVSVIMStateClass::CheckState(SV_STATE_CANCELING) || SVSVIMStateClass::CheckState(SV_STATE_CLOSING))
		{
			bCanClose = CDocument::CanCloseFrame(pFrame);
		}
	}

	if (!bCanClose)
	{
		SvStl::MessageMgrStd Msg(SvStl::MsgType::Log | SvStl::MsgType::Display);
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
		!(S_OK == TheSVObserverApp.m_svSecurityMgr.SVValidate(SECURITY_POINT_FILE_MENU_CLOSE_CONFIGURATION)))
	{
		return;
	}

	OnFileClose();
}

void SVIPDoc::OnUpdateStatusInfo(CCmdUI *pCmdUI)
{
	switch (pCmdUI->m_nID)
	{
		case ID_INDICATOR_INFO:
		{
			((SVMainFrame*)AfxGetApp()->m_pMainWnd)->OnUpdateStatusInfo(pCmdUI);
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
	PCmdUI->Enable(TheSVObserverApp.m_svSecurityMgr.SVIsDisplayable(SECURITY_POINT_MODE_MENU_REGRESSION_TEST));
	PCmdUI->SetCheck(SVSVIMStateClass::CheckState(SV_STATE_REGRESSION));
}

void SVIPDoc::OnAllowAdjustLightReference(CCmdUI* pCmdUI)
{
	bool bEnable = (!SVSVIMStateClass::CheckState(SV_STATE_RUNNING | SV_STATE_REGRESSION) &&
		SVSVIMStateClass::CheckState(SV_STATE_TEST | SV_STATE_EDIT) &&
		TheSVObserverApp.m_svSecurityMgr.SVIsDisplayable(SECURITY_POINT_MODE_MENU_EDIT_TOOLSET));
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
		TheSVObserverApp.m_svSecurityMgr.SVIsDisplayable(SECURITY_POINT_MODE_MENU_EDIT_TOOLSET));
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
	SVLightReferenceVector LightRefVectorOriginal{LightRefVector};


	//remove apply button
	dlg.m_psh.dwFlags |= PSH_NOAPPLYNOW;

	if (dlg.CreatePages(cameraVector, LightRefVector))
	{
		SVSVIMStateClass::AddState(SV_STATE_EDITING);

		SVConfigurationObject* pConfig(nullptr);
		SVObjectManagerClass::Instance().GetConfigurationObject(pConfig);
		assert(nullptr != pConfig);

		SVLightReferenceVector* pResultLightRefVector{nullptr};
		if (nullptr != pConfig && dlg.DoModal() == IDOK)
		{
			SetModifiedFlag();
			pResultLightRefVector = &LightRefVector;
		}
		else
		{
			pResultLightRefVector = &LightRefVectorOriginal;
		}
		if(nullptr != pResultLightRefVector)
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

		SVSVIMStateClass::RemoveState(SV_STATE_EDITING);
	}
	// Show default LUT: black --> black, white --> white...
	GetImageView()->ShowExtremeLUT(FALSE);
}

void SVIPDoc::OnAdjustLut()
{
	int i(0);

	SVInspectionProcess* pInspection(GetInspectionProcess());

	assert(GetToolSet());
	assert(GetToolSet()->IsCreated());

	if (nullptr == pInspection || !GetToolSet()->IsCreated()) // is tool set created
	{
		return;
	}

	int iChannel = 0;
	// Show extreme LUT: black --> blue, white --> green...
	GetImageView()->ShowExtremeLUT();

	SVLutDlg dlg(std::string(_T("LUT Adjustment - [") + GetTitle() + _T("]")).c_str());

	// obtain copies of the luts to set if IDOK / IDCANCEL
	SVLutDlg::SVLutMap aLut;
	SVLutDlg::SVLutMap aLutOrig;

	SvIe::SVVirtualCameraPtrVector cameraVector = pInspection->GetCameras();

	for (const auto* const pCamera : cameraVector)
	{
		if (nullptr != pCamera)
		{
			pCamera->GetAcquisitionDevice()->GetLut(aLut[pCamera->GetUniqueObjectID()]);
		}
	}

	aLutOrig = aLut;

	//remove the apply button
	dlg.m_psh.dwFlags |= PSH_NOAPPLYNOW;

	bool bSuccess = dlg.Create(cameraVector, aLut);

	if (bSuccess)
	{
		SVSVIMStateClass::AddState(SV_STATE_EDITING);


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
					pCamera->SetLut(aLutOrig[pCamera->GetUniqueObjectID()]);
				}
			}
		}
		SVConfigurationObject* pConfig(nullptr);
		SVObjectManagerClass::Instance().GetConfigurationObject(pConfig);
		ASSERT(nullptr != pConfig);

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
		SVSVIMStateClass::RemoveState(SV_STATE_EDITING);
	}

	// Show default LUT: black --> black, white --> white...
	GetImageView()->ShowExtremeLUT(FALSE);
}

void SVIPDoc::OnAddShiftTool()
{
	AddTool(SVShiftToolClassGuid);
}

void SVIPDoc::OnAddWindowTool()
{
	AddTool(SVWindowToolClassGuid);
}

void SVIPDoc::OnAddCylindricalWarpTool()
{
	AddTool(SVCylindricalWarpToolClassGuid);
}

void SVIPDoc::OnAddPerspectiveTool()
{
	AddTool(SVPerspectiveToolClassGuid);
}

void SVIPDoc::OnAddImageTool()
{
	AddTool(SVImageToolClassGuid);
}

void SVIPDoc::OnAddAcquisitionTool()
{
	AddTool(SVAcquisitionToolClassGuid);
}

void SVIPDoc::OnAddArchiveTool()
{
	AddTool(SVArchiveToolClassGuid);
}

void SVIPDoc::OnAddLinearTool()
{
	AddTool(SVLinearToolClassGuid);
}

void SVIPDoc::OnAddLoadImageTool()
{
	AddTool(SVLoadImageToolClassGuid);
}

void SVIPDoc::OnAddMathTool()
{
	AddTool(SVMathToolClassGuid);
}

void SVIPDoc::OnAddStatisticsTool()
{
	AddTool(SVStatisticsToolClassGuid);
}

void SVIPDoc::OnAddTransformationTool()
{
	AddTool(SVTransformationToolClassGuid);
}


void SVIPDoc::OnAddResizetool()
{
	AddTool(SVResizeToolGuid);
}

void SVIPDoc::OnAddRingBufferTool()
{
	AddTool(RingBufferToolGuid);
}

void SVIPDoc::OnAddTableTool()
{
	AddTool(TableToolGuid);
}

void SVIPDoc::OnAddTableAnalyzerTool()
{
	AddTool(TableAnalyzerToolGuid);
}

void SVIPDoc::OnAddPolarUnwrapTool()
{
	AddTool(SVPolarTransformationToolClassGuid);
}

void SVIPDoc::OnAddColorTool()
{
	AddTool(SVColorToolClassGuid);
}
void SVIPDoc::OnAddLoopTool()
{
	AddTool(LoopToolClassGuid);
}

void SVIPDoc::OnUpdateAddStartToolGrouping(CCmdUI* pCmdUI)
{
	bool bEnable = (!SVSVIMStateClass::CheckState(SV_STATE_RUNNING | SV_STATE_TEST) && SVSVIMStateClass::CheckState(SV_STATE_EDIT));
	if (bEnable)
	{

		ToolSetView* pView = GetToolSetView();
		if (!pView || pView->IsLoopToolSelected())
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
		if (!pView || pView->IsLoopToolSelected() || !pView->IsEndToolGroupAllowed())
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

	// Don't allow while inspecting
	if (SVSVIMStateClass::CheckState(SV_STATE_RUNNING | SV_STATE_TEST))
	{
		return;
	}
	ToolSetView* pToolSetView = GetToolSetView();
	SVToolSetClass* pToolSet = GetToolSet();
	if (!pToolSet || !pToolSetView || pToolSetView->IsLabelEditing())
	{
		return;
	}
	int SelectedIndex(-1);
	PtrNavigatorElement pNavElement = pToolSetView->GetSelectedNavigatorElement(&SelectedIndex);
	if (!pNavElement)
	{
		return;
	}
	switch (pNavElement->m_Type)
	{
		case NavElementType::SubLoopTool:
		case NavElementType::SubTool:


			if (deleteTool(pNavElement.get()))
			{
				UpdateAllViews(nullptr, RefreshDelete);
				SetModifiedFlag();
				RunOnce();
			}
			break;

		case NavElementType::LoopTool:
		case NavElementType::Tool:
		{

			SvIe::SVTaskObjectClass* pFirstTaskObject = pToolSet->GetAt(0);
			// If deleting first item in a color IPD first tool must be a color tool
			if (nullptr != pFirstTaskObject && pFirstTaskObject->GetUniqueObjectID() == pNavElement->m_Guid && pInspection->IsColorCamera())
			{
				if (pToolSet->GetSize() > 1)
				{
					SvTo::SVToolClass* pNextTool = dynamic_cast<SvTo::SVToolClass*> (pToolSet->GetAt(1));

					if (pFirstTaskObject && SV_IS_KIND_OF(pFirstTaskObject, SvTo::SVColorToolClass) &&
						pNextTool && !SV_IS_KIND_OF(pNextTool, SvTo::SVColorToolClass))
					{
						SvStl::MessageMgrStd Msg(SvStl::MsgType::Log | SvStl::MsgType::Display);
						Msg.setMessage(SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_ColorToolMustBeFirstMessage, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_10057);
						return;
					}
				}
			}

			if (deleteTool(pNavElement.get()))
			{
				m_toolGroupings.RemoveTool(pNavElement->m_DisplayName);
				UpdateAllViews(nullptr, RefreshDelete);
				SetModifiedFlag();
				RunOnce(); // this will cause rebuild to be called so why are we calling rebuild above?
			}


		}
		break;

		case NavElementType::StartGrouping:
		case NavElementType::EndGrouping:
		{
			m_toolGroupings.RemoveGroup(pNavElement->m_DisplayName);
			UpdateAllViews(nullptr, RefreshDelete);
			SetModifiedFlag();
		}
		break;

	}
}
void SVIPDoc::OnEditCopy()
{
	if (!SVSVIMStateClass::CheckState(SV_STATE_READY) || !SVSVIMStateClass::CheckState(SV_STATE_EDIT))
	{
		return;
	}

	SVInspectionProcess* pInspection(GetInspectionProcess());

	if (nullptr != pInspection)
	{
		ToolSetView* pToolSetView = GetToolSetView();
		if (nullptr != pToolSetView && !pToolSetView->IsLabelEditing())
		{
			const SVGUID& rGuid = pToolSetView->GetSelectedTool();
			if (!rGuid.empty())
			{
				ToolClipboard Clipboard(*pInspection);

				Clipboard.writeToClipboard(rGuid);
			}
		}
	}
}

void SVIPDoc::OnUpdateEditCopy(CCmdUI* pCmdUI)
{
	// Check current user access...
	bool Enabled(false);
	ToolSetView* pToolSetView = GetToolSetView();
	SVToolSetClass* pToolSet = GetToolSet();

	if (TheSVObserverApp.OkToEdit() && nullptr != pToolSet && nullptr != pToolSetView)
	{
		if (!pToolSetView->IsLabelEditing())
		{
			int SelectedListIndex(-1);
			auto NavElement = pToolSetView->GetSelectedNavigatorElement(&SelectedListIndex);
			if (NavElement)
			{
				switch (NavElement->m_Type)
				{
					case NavElementType::EndDelimiterLoopTool:
						break;
					case NavElementType::SubTool:
					case NavElementType::Tool:
					case NavElementType::LoopTool:
						if (-1 != SelectedListIndex)
						{
							Enabled = true;
						}
						break;
				}
			}
		}
	}
	pCmdUI->Enable(Enabled);
}

void SVIPDoc::OnEditPaste()
{
	if (!SVSVIMStateClass::CheckState(SV_STATE_READY) || !SVSVIMStateClass::CheckState(SV_STATE_EDIT))
	{
		return;
	}

	SVInspectionProcess* pInspection(GetInspectionProcess());
	ToolSetView* pView = GetToolSetView();
	if (nullptr == pInspection || nullptr == pView)
	{
		return;
	}
	int selectedListIndex(-1);
	auto pNavElement = pView->GetSelectedNavigatorElement(&selectedListIndex);
	SVToolSetClass* pToolSet(pInspection->GetToolSet());
	if (nullptr == pNavElement || nullptr == pToolSet)
	{
		return;
	}
	SVGUID postToolGuid(GUID_NULL);
	SVGUID ownerGuid(pToolSet->GetUniqueObjectID());
	switch (pNavElement->m_Type)
	{
		case NavElementType::SubTool:
		case NavElementType::EndDelimiterLoopTool:
		case NavElementType::SubLoopTool:
			ownerGuid = pNavElement->m_OwnerGuid;
			postToolGuid = pNavElement->m_Guid;
			break;
		case NavElementType::StartGrouping:
		case NavElementType::EndGrouping:
		{
			std::string toolName = m_toolGroupings.GetToolToInsertBefore(pNavElement->m_DisplayName);
			for (int i = 0; i < pToolSet->GetSize(); i++)
			{
				const SvIe::SVTaskObjectClass* pTool = pToolSet->GetAt(i);
				if (pTool && pTool->GetName() == toolName)
				{
					postToolGuid = pTool->GetUniqueObjectID();
					break;
				}
			}
		}
		break;
		case NavElementType::Tool:
		case NavElementType::LoopTool:
			postToolGuid = pNavElement->m_Guid;
			break;
		case NavElementType::EndDelimiterToolSet:
		case NavElementType::Empty:
		default:
			break;
	}

	ToolClipboard Clipboard(*pInspection);
	SVGUID toolGuid(GUID_NULL);
	Clipboard.readFromClipboard(postToolGuid, ownerGuid, toolGuid);

	SVObjectClass* pObject(SVObjectManagerClass::Instance().GetObject(toolGuid));

	SvTo::SVToolClass* pTool = dynamic_cast<SvTo::SVToolClass*> (pObject);

	if (nullptr != pTool)
	{
		//Set the time stamp to 0 to force an update of the tool list
		m_ToolSetListTimestamp = 0;
		
		SvPb::InspectionCmdMsgs Request, Response;
		SvPb::MoveObjectRequest* pMovetaskobjectrequest = Request.mutable_moveobjectrequest();
		SvPb::SetGuidInProtoBytes(pMovetaskobjectrequest->mutable_parentid(), ownerGuid);
		SvPb::SetGuidInProtoBytes(pMovetaskobjectrequest->mutable_objectid(), toolGuid);
		SvPb::SetGuidInProtoBytes(pMovetaskobjectrequest->mutable_movepreid(), postToolGuid);
		pMovetaskobjectrequest->set_listmode(SvPb::MoveObjectRequest_ListEnum_TaskObjectList);

		HRESULT hr = SvCmd::InspectionCommands(m_InspectionID, Request,&Response);
		assert(S_OK == hr);
		
		bool OwnerIsLooptool {false};
		SVObjectClass*  pOwnwerObject(SVObjectManagerClass::Instance().GetObject(ownerGuid));
		if (pOwnwerObject && SvPb::LoopToolObjectType == pOwnwerObject->GetObjectSubType())
		{
			OwnerIsLooptool = true;
		}
	
		if (!OwnerIsLooptool)
		{
			
			m_toolGroupings.AddTool(pTool->GetName(), pNavElement->m_DisplayName);
			
		}

		//@TODO[gra][8.10][13.12.2018]: This is a low risk fix to avoid other problems and should be solved in another way
		//This reset is required when pasting an acquisition tool which otherwise adds a non existent camera to the PPQ
		//The inspection pointer is still nullptr at this stage which avoids adding the camera to the PPQ in SVCameraImageTemplate::RebuildCameraImage
		if(SvPb::SVToolAcquisitionObjectType == pTool->GetObjectSubType())
		{
			pTool->resetAllObjects();
		}

		SVObjectLevelCreateStruct createStruct;
		createStruct.OwnerObjectInfo.SetObject(pInspection);
		createStruct.m_pInspection = pInspection;

		pInspection->ConnectObject(createStruct);
		pInspection->ConnectAllInputs();

		SVObjectLevelCreateStruct createObjStruct;

		pInspection->createAllObjects(createObjStruct);
		//Reset only the inserted tool
		pTool->resetAllObjects();
		pToolSet->updateToolPosition();

		RunOnce();
		UpdateAllViews(nullptr);
		SetSelectedToolID(toolGuid);
		SetModifiedFlag();
	}
	pToolSet->updateToolPosition();

}

void SVIPDoc::OnUpdateEditPaste(CCmdUI* pCmdUI)
{
	ToolSetView* pToolSetView = GetToolSetView();
	SVToolSetClass* pToolSet = GetToolSet();
	bool enabled = !TheSVObserverApp.OkToEdit() || nullptr != pToolSet || nullptr != pToolSetView || pToolSetView->IsLabelEditing();

	if (enabled)
	{
		int SelectedListIndex(-1);
		auto pNavElement = pToolSetView->GetSelectedNavigatorElement(&SelectedListIndex);

		//Only if tool list active and a selected index is valid
		enabled = (nullptr != pNavElement && -1 != SelectedListIndex && ToolClipboard::isClipboardDataValid());
	}
	pCmdUI->Enable(enabled);
}

void SVIPDoc::OnShowFirstError()
{
	ToolSetView* pToolSetView = GetToolSetView();
	if (nullptr != pToolSetView)
	{
		pToolSetView->displayFirstCurrentToolError();
	}
}

void SVIPDoc::OnUpdateShowFirstError(CCmdUI* pCmdUI)
{
	BOOL Enabled = false;
	ToolSetView* pToolSetView = GetToolSetView();
	if (nullptr != pToolSetView && pToolSetView->hasCurrentToolErrors())
	{
		Enabled = true;
	}

	pCmdUI->Enable(Enabled);
}

void SVIPDoc::OnAddParameterToMonitorList()
{
	ToolSetView* pToolSetView = GetToolSetView();
	std::string ppqName = GetInspectionProcess()->GetPPQ()->GetName();
	if (nullptr != pToolSetView)
	{
		pToolSetView->addParameter2MonitorList(ppqName.c_str());
	}
}

void SVIPDoc::OnUpdateAddParameterToMonitorList(CCmdUI* pCmdUI)
{
	BOOL Enabled = false;
	ToolSetView* pToolSetView = GetToolSetView();
	std::string ppqName = GetInspectionProcess()->GetPPQ()->GetName();
	if (nullptr != pToolSetView && pToolSetView->isAddParameter2MonitorListPossible(ppqName.c_str()))
	{
		Enabled = true;
	}

	pCmdUI->Enable(Enabled);
}

void SVIPDoc::OnRemoveParameterToMonitorList()
{
	ToolSetView* pToolSetView = GetToolSetView();
	std::string ppqName = GetInspectionProcess()->GetPPQ()->GetName();
	if (nullptr != pToolSetView)
	{
		pToolSetView->removeParameter2MonitorList(ppqName.c_str());
	}
}

void SVIPDoc::OnUpdateRemoveParameterToMonitorList(CCmdUI* pCmdUI)
{
	BOOL Enabled = false;
	ToolSetView* pToolSetView = GetToolSetView();
	std::string ppqName = GetInspectionProcess()->GetPPQ()->GetName();
	if (nullptr != pToolSetView && pToolSetView->isRemoveParameter2MonitorListPossible(ppqName.c_str()))
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
	if (TheSVObserverApp.OkToEdit())
	{
		SvTo::SVToolClass* l_pTool = dynamic_cast<SvTo::SVToolClass*> (SVObjectManagerClass::Instance().GetObject(GetSelectedToolID()));
		if (nullptr != l_pTool)
		{
			const SvDef::SVObjectTypeInfoStruct& rToolType = l_pTool->GetObjectInfo().m_ObjectTypeInfo;
			SVSVIMStateClass::AddState(SV_STATE_EDITING);
			try
			{
				SVToolAdjustmentDialogSheetClass toolAdjustmentDialog(this, GetInspectionID(), GetSelectedToolID(), _T("Tool Adjustment"), nullptr, tab);
				INT_PTR dlgResult = toolAdjustmentDialog.DoModal();
				if (IDOK == dlgResult)
				{
					ExtrasEngine::Instance().ExecuteAutoSaveIfAppropriate(false);//Arvid: after tool was edited: update the autosave timestamp
					SVConfigurationObject* pConfig = nullptr;
					SVObjectManagerClass::Instance().GetConfigurationObject(pConfig);
					if (nullptr != pConfig)
					{
						pConfig->ValidateRemoteMonitorList();
						TheSVObserverApp.GetIODoc()->UpdateAllViews(nullptr);
					}
				}
				else
				{
					l_pTool->ResetObject();
				}
			}
			catch (const SvStl::MessageContainer& rExp)
			{
				//This is the topmost catch of the TA for MessageContainer exceptions
				SvStl::MessageMgrStd Exception(SvStl::MsgType::Log | SvStl::MsgType::Display);
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
				SvStl::MessageMgrStd Exception(SvStl::MsgType::Log | SvStl::MsgType::Display);
				Exception.setMessage(SVMSG_SVO_UNHANDLED_EXCEPTION, SvStl::Tid_Default, SvStl::SourceFileParams(StdMessageParams));
			}
			SVSVIMStateClass::RemoveState(SV_STATE_EDITING);
		}
		UpdateAllViews(nullptr, RefreshView);
	}
}

void SVIPDoc::OnEditToolSetCondition()
{
	// Check current user access...
	if (TheSVObserverApp.OkToEdit())
	{
		if (GetToolSet())
		{
			SVSVIMStateClass::AddState(SV_STATE_EDITING);

			SvDef::SVObjectTypeInfoStruct Info(SvPb::SVEquationObjectType, SvPb::SVConditionalObjectType);
			std::string Title {_T("ToolSet Adjustment: ")};
			Title += GetToolSet()->GetName();
			SvOg::SVFormulaEditorSheetClass dlg(GetInspectionID(), GetToolSet()->GetUniqueObjectID(), Info, Title.c_str());

			INT_PTR dlgResult = dlg.DoModal();
			if (dlgResult == IDOK)
			{
				SetModifiedFlag();
			}

			SVSVIMStateClass::RemoveState(SV_STATE_EDITING);
		}
	}
}

void SVIPDoc::OnFileSaveImage()
{
	SVSVIMStateClass::AddState(SV_STATE_EDITING); /// do this before calling validate for security as it may display a logon dialog!
	if (S_OK == TheSVObserverApp.m_svSecurityMgr.SVValidate(SECURITY_POINT_FILE_MENU_SAVE_IMAGE))
	{
		SvOg::SVSaveToolSetImageDialogClass dlg(GetInspectionID(), GetToolSet()->GetUniqueObjectID());
		dlg.DoModal();
	}
	SVSVIMStateClass::RemoveState(SV_STATE_EDITING);
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
	if (SVSVIMStateClass::CheckState(SV_STATE_EDIT))
	{
		SVSVIMStateClass::AddState(SV_STATE_EDITING); /// do this before calling validate for security as it may display a logon dialog!
	}
	if (S_OK == TheSVObserverApp.m_svSecurityMgr.SVValidate(SECURITY_POINT_EDIT_MENU_RESULT_PICKER))
	{
		SVInspectionProcess* pInspection(GetInspectionProcess());
		SVResultListClass* pResultList(GetResultList());
		if (nullptr != pInspection && nullptr != pResultList)
		{
			std::string InspectionName(pInspection->GetName());
			SvPb::InspectionCmdMsgs request, response;
			*request.mutable_getobjectselectoritemsrequest() = SvCmd::createObjectSelectorRequest(
				{SvPb::ObjectSelectorType::globalConstantItems, SvPb::ObjectSelectorType::ppqItems, SvPb::ObjectSelectorType::toolsetItems},
				GetInspectionID(), SvPb::viewable, GUID_NULL, true);
			SvCmd::InspectionCommands(m_InspectionID, request, &response);

			SvOsl::ObjectTreeGenerator::Instance().setSelectorType(SvOsl::ObjectTreeGenerator::SelectorTypeEnum::TypeMultipleObject, IDD_RESULTS_PICKER + SvOr::HELPFILE_DLG_IDD_OFFSET);
			if (response.has_getobjectselectoritemsresponse())
			{
				SvOsl::ObjectTreeGenerator::Instance().insertTreeObjects(response.getobjectselectoritemsresponse().tree());
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
					SVObjectReference ObjRef{rEntry};
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

	if (SVSVIMStateClass::CheckState(SV_STATE_EDIT))
	{
		SVSVIMStateClass::RemoveState(SV_STATE_EDITING);
	}
}

void SVIPDoc::OnResultsTablePicker()
{
	//This shall change the state to editing only when previously in edit mode
	//This will avoid changing the modes while in Run, Regression or Test mode
	if (SVSVIMStateClass::CheckState(SV_STATE_EDIT))
	{
		SVSVIMStateClass::AddState(SV_STATE_EDITING); /// do this before calling validate for security as it may display a logon dialog!
	}
	if (S_OK == TheSVObserverApp.m_svSecurityMgr.SVValidate(SECURITY_POINT_EDIT_MENU_RESULT_PICKER))
	{
		SVResultListClass* pResultList(GetResultList());
		assert(nullptr != pResultList);
		if (nullptr != pResultList)
		{
			SvUl::NameGuidList availableList;
			std::string selectedItem = SvOg::Table_NoSelected;
			SVGUID selectedGuid = pResultList->getTableGuid();
			SvPb::InspectionCmdMsgs request, response;
			SvPb::GetAvailableObjectsRequest* pGetAvailableObjectsRequest = request.mutable_getavailableobjectsrequest();

			SvPb::SetGuidInProtoBytes(pGetAvailableObjectsRequest->mutable_objectid(), GetInspectionID());
			pGetAvailableObjectsRequest->mutable_typeinfo()->set_objecttype(SvPb::TableObjectType);
			pGetAvailableObjectsRequest->set_objecttypetoinclude(SvPb::SVToolSetObjectType);
			HRESULT hr = SvCmd::InspectionCommands(m_InspectionID, request, &response);
			if (S_OK == hr && response.has_getavailableobjectsresponse())
			{
				availableList = SvCmd::convertNameGuidList(response.getavailableobjectsresponse().list());
				for (SvUl::NameGuidPair pair : availableList)
				{
					if (pair.second == selectedGuid)
					{
						selectedItem = pair.first;
					}
				}
			}

			SvOg::ResultTableSelectionDlg TableSelectDlg(availableList, selectedItem);
			if (IDOK == TableSelectDlg.DoModal())
			{
				pResultList->setTableGuid(TableSelectDlg.getSelectedGuid());

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

	if(SVSVIMStateClass::CheckState(SV_STATE_EDIT))
	{
		SVSVIMStateClass::RemoveState(SV_STATE_EDITING);
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
			SVResultDefinitionDeque ResultDefinitions;
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
			SvStl::MessageMgrStd Msg(SvStl::MsgType::Log | SvStl::MsgType::Display);
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
			SVResultDefinitionDeque ResultDefinitions;
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
			SvStl::MessageMgrStd Msg(SvStl::MsgType::Log | SvStl::MsgType::Display);
			Msg.setMessage(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_WriteCSVFileFailed, msgList, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_10058);
		}
	}
}

void SVIPDoc::OnPublishedResultsPicker()
{
	SVInspectionProcess* pInspection(GetInspectionProcess());
	if (nullptr != pInspection)
	{
		SVSVIMStateClass::AddState(SV_STATE_EDITING);
		std::string InspectionName(pInspection->GetName());

		SvPb::InspectionCmdMsgs request, response;
		*request.mutable_getobjectselectoritemsrequest() = SvCmd::createObjectSelectorRequest(
			{SvPb::ObjectSelectorType::toolsetItems}, GetInspectionID(), SvPb::publishable);
		SvCmd::InspectionCommands(m_InspectionID, request, &response);

		SvOsl::ObjectTreeGenerator::Instance().setSelectorType(SvOsl::ObjectTreeGenerator::SelectorTypeEnum::TypeMultipleObject, IDD_PUBLISHED_RESULTS + SvOr::HELPFILE_DLG_IDD_OFFSET);
		if (response.has_getobjectselectoritemsresponse())
		{
			SvOsl::ObjectTreeGenerator::Instance().insertTreeObjects(response.getobjectselectoritemsresponse().tree());
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
			SVPPQObject* pPPQ(nullptr);

			SVConfigurationObject* pConfig = nullptr;
			SVObjectManagerClass::Instance().GetConfigurationObject(pConfig);
			if (nullptr != pConfig)
			{
				pConfig->ValidateRemoteMonitorList();

				// Force the PPQs to rebuild
				long lSize = pConfig->GetPPQCount();

				for (long l = 0; l < lSize; l++)
				{
					pPPQ = pConfig->GetPPQ(l);
					if (nullptr != pPPQ)
					{
						pPPQ->RebuildOutputList();
					}// end if
				}
			}// end for
			TheSVObserverApp.GetIODoc()->UpdateAllViews(nullptr);
		}
		SVSVIMStateClass::RemoveState(SV_STATE_EDITING);
	}
}

void SVIPDoc::OnPublishedResultImagesPicker()
{
	SVInspectionProcess* pInspection(GetInspectionProcess());
	if (nullptr != pInspection)
	{
		SVSVIMStateClass::AddState(SV_STATE_EDITING);

		std::string InspectionName(pInspection->GetName());

		SvPb::InspectionCmdMsgs request, response;
		*request.mutable_getobjectselectoritemsrequest() = SvCmd::createObjectSelectorRequest(
			{SvPb::ObjectSelectorType::toolsetItems}, GetInspectionID(), SvPb::publishResultImage);
		SvCmd::InspectionCommands(m_InspectionID, request, &response);

		SvOsl::ObjectTreeGenerator::Instance().setSelectorType(SvOsl::ObjectTreeGenerator::SelectorTypeEnum::TypeMultipleObject, ID_PUBLISHED_RESULT_IMAGES_PICKER + SvOr::HELPFILE_ID_OFFSET);
		if (response.has_getobjectselectoritemsresponse())
		{
			SvOsl::ObjectTreeGenerator::Instance().insertTreeObjects(response.getobjectselectoritemsresponse().tree());
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
				SVGUID ObjectGuid {rEntry};
				SVObjectClass* pObject = SVObjectManagerClass::Instance().GetObject(ObjectGuid);
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
				TheSVObserverApp.GetIODoc()->UpdateAllViews(nullptr);
			}
		}

		SVSVIMStateClass::RemoveState(SV_STATE_EDITING);
	}
}

void SVIPDoc::RebuildResultsList()
{
	SVResultListClass* pResultList = GetResultList();

	if (pResultList)
	{
		pResultList->Refresh(GetToolSet());
	}
}

bool SVIPDoc::checkOkToDelete(SvIe::SVTaskObjectClass* pTaskObject)
{
	bool bRetVal = false;

	SVSVIMStateClass::AddState(SV_STATE_EDITING);
	// show dependents dialog

	if (pTaskObject)
	{
		SVGUID TaskObjectID = pTaskObject->GetUniqueObjectID();

		INT_PTR dlgResult = SvOg::SVShowDependentsDialog::StandardDialog(pTaskObject->GetName(), TaskObjectID);

		bRetVal = (IDOK == dlgResult);
	}
	SVSVIMStateClass::RemoveState(SV_STATE_EDITING);
	return bRetVal;
}

void SVIPDoc::RunRegressionTest()
{

	if (SVSVIMStateClass::CheckState(SV_STATE_REGRESSION))
	{  // already in regression mode, do nothing...
		return;
	}

	bool hasRunMode = SVSVIMStateClass::CheckState(SV_STATE_RUNNING);

	bool hasTestMode = SVSVIMStateClass::CheckState(SV_STATE_TEST);

	if (hasRunMode || hasTestMode)
	{
		TheSVObserverApp.OnStop();
	}

	SVInspectionProcess* pInspection(GetInspectionProcess());

	if (nullptr == pInspection) { return; }

	if (!m_bRegressionTestInitEquationText)
	{
		double value;
		SvStl::MessageContainerVector errorMessages;
		m_pRegressionTestPlayEquationController->ValidateEquation(m_RegressionTestLoadEquationText, value, true, errorMessages);
		if (!errorMessages.empty())
		{
			SvStl::MessageMgrStd msg(SvStl::MsgType::Log);
			msg.setMessage(errorMessages[0].getMessage());
		}
		m_bRegressionTestInitEquationText = true;
	}

	if (pInspection->CanRegressionGoOnline())
	{
		bool l_bAllowAccess = false;

		if (hasRunMode)
		{
			// Dual Security access point
			if (S_OK == TheSVObserverApp.m_svSecurityMgr.SVValidate(SECURITY_POINT_MODE_MENU_REGRESSION_TEST,
				SECURITY_POINT_MODE_MENU_EXIT_RUN_MODE))
			{
				l_bAllowAccess = true;
			}
			else
			{
				return;
			}
		}
		else if (S_OK == TheSVObserverApp.m_svSecurityMgr.SVValidate(SECURITY_POINT_MODE_MENU_REGRESSION_TEST))
		{
			l_bAllowAccess = true;
		}

		if (l_bAllowAccess)
		{
			SVSVIMStateClass::RemoveState(SV_STATE_TEST | SV_STATE_EDIT | SV_STATE_STOP);

			TheSVObserverApp.DeselectTool();

			SVSVIMStateClass::AddState(SV_STATE_REGRESSION);

			m_oDisplay.CanGoOnline();
			// check to see if it is able to go into Regression mode

			// check to see if the list of files are the same...
			m_bRegressionTestRunning = true;
			int iNumCameras = static_cast<int>(m_listRegCameras.GetCount());

			((SVMainFrame*)AfxGetApp()->m_pMainWnd)->m_pregTestDlg = new CSVRegressionRunDlg(m_pRegressionTestPlayEquationController);
			((SVMainFrame*)AfxGetApp()->m_pMainWnd)->m_pregTestDlg->SetUsePlayCondition(m_bRegressionTestUsePlayCondition);

			((SVMainFrame*)AfxGetApp()->m_pMainWnd)->m_pregTestDlg->SetIPDocParent(this);

			((SVMainFrame*)AfxGetApp()->m_pMainWnd)->m_pregTestDlg->Create(IDD_DIALOG_REGRESSIONTEST_RUN);

			((SVMainFrame*)AfxGetApp()->m_pMainWnd)->m_pregTestDlg->ShowWindow(SW_SHOW);

			DWORD dwThreadID;

			m_hRegressionHandle = ::CreateThread(nullptr, 0, SVRegressionTestRunThread, (LPVOID)this, 0, &dwThreadID);
		}
	}
	else
	{
		SvStl::MessageMgrStd Msg(SvStl::MsgType::Log | SvStl::MsgType::Display);
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
	// Load Utilities Menu
	SVUtilitiesClass util;
	CWnd *pWindow(nullptr);
	CMenu *pMenu(nullptr);

	pWindow = AfxGetMainWnd();
	if (nullptr != pWindow)
	{
		// Load and init Utility Menu
		pMenu = pWindow->GetMenu();
		if (pMenu = util.FindSubMenuByName(pMenu, _T("&Utilities"))) { util.LoadMenu(pMenu); }

		// Load and init Tool Set Draw Menu
		int pos = 0;
		pMenu = ::SVFindMenuByCommand(pWindow->GetMenu(), ID_VIEW_TOOLSETDRAW, TRUE, pos);
		SVToolSetClass* pSet = GetToolSet();
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

	SVToolSetClass* pSet = GetToolSet();

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
	ASSERT(nId >= ID_VIEW_TOOLSETDRAW_POP_BASE && nId <= ID_VIEW_TOOLSETDRAW_POP_MAX);

	// Access denied...
	if (!TheSVObserverApp.OkToEdit()) { return; }

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
	SVGuidSet l_ImageIds;

	for (int i = 0; i < MaxImageViews; ++i)
	{
		SVImageViewClass* l_pImageView = GetImageView(i);

		if (nullptr != l_pImageView)
		{
			SVGUID l_ImageId = l_pImageView->GetImageID();

			if (!(l_ImageId.empty()))
			{
				l_ImageIds.insert(l_ImageId);
			}
		}
	}
	SVCommandInspectionCollectImageDataPtr l_DataPtr {new SVCommandInspectionCollectImageData(m_InspectionID, l_ImageIds)};
	SVObjectSynchronousCommandTemplate< SVCommandInspectionCollectImageDataPtr > l_Command(m_InspectionID, l_DataPtr);

	if (S_OK == l_Command.Execute(120000))
	{
		if (m_NewProductData.size() == m_NewProductData.capacity())
		{
			m_NewProductData.PopHead();
		}

		if (S_OK == m_NewProductData.PushTail(l_DataPtr->GetProduct()))
		{
			CWnd* pWnd = GetMDIChild();
			if (nullptr != pWnd && ::IsWindow(pWnd->GetSafeHwnd()))
			{
				pWnd->PostMessage(SV_MDICHILDFRAME_UPDATE_ALL_DATA);
			}
		}
	}
}

void SVIPDoc::RecreateImageSurfaces()
{
	for (int i = 0; i < MaxImageViews; ++i)
	{
		SVImageViewClass* pImageView = GetImageView(i);

		if (pImageView) { pImageView->RecreateImageSurface(); }
	}
}

void SVIPDoc::OnUpdateFileExit(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(!SVSVIMStateClass::CheckState(SV_STATE_RUNNING) && !SVSVIMStateClass::CheckState(SV_STATE_REGRESSION) && !SVSVIMStateClass::CheckState(SV_STATE_TEST)
		&& TheSVObserverApp.m_svSecurityMgr.SVIsDisplayable(SECURITY_POINT_FILE_MENU_EXIT));
}

CFile* SVIPDoc::GetFile(LPCTSTR lpszFileName, UINT nOpenFlags, CFileException* pError)
{
	UINT nNewFlags = nOpenFlags & ~(CFile::shareDenyRead | CFile::shareDenyWrite) |
		CFile::shareDenyNone;

	return CDocument::GetFile(lpszFileName, nNewFlags, pError);
}

bool SVIPDoc::IsColorInspectionDocument() const
{
	bool bRetVal = false;

	SVInspectionProcess* pInspection = GetInspectionProcess();

	if (nullptr != pInspection)
	{
		bRetVal = pInspection->IsColorCamera();
	}

	return bRetVal;
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
	SVResultListClass* pResultList = GetResultList();


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
	SVResultListClass* pResultList = GetResultList();
	if (nullptr == pResultList)
	{
		return false;
	}
	return  pResultList->LoadViewedVariables(rTree, htiParent);

}

void SVIPDoc::SaveRegressionTestVariables(SvOi::IObjectWriter& rWriter)
{
	rWriter.StartElement(SvXml::CTAG_REGRESSIONTEST);
	_variant_t Value(m_bRegressionTestUsePlayCondition);
	rWriter.WriteAttribute(SvXml::CTAG_USE_PLAY_CONDITION, Value);
	Value = m_pRegressionTestPlayEquationController->GetEquationText().c_str();
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
		SearchName = rInspectionName + SvDef::FqnRemoteInput;
		ReplaceName = SvDef::FqnPPQVariables;
		ReplaceName += SvDef::FqnRemoteInput;
		TranslateNames[SearchName] = ReplaceName;
		SearchName = rInspectionName + SvDef::FqnDioInput;
		ReplaceName = SvDef::FqnPPQVariables;
		ReplaceName += SvDef::FqnDioInput;
		TranslateNames[SearchName] = ReplaceName;
		ReplaceName = SvUl::LoadStdString(IDS_CLASSNAME_SVTOOLSET);
		SearchName = rInspectionName + _T(".");
		SearchName += ReplaceName;
		TranslateNames[SearchName] = ReplaceName;
	}

	SvDef::StringSet SelectedObjectNames;
	for (auto const& rItem : rSelectedObjects)
	{
		std::string Name {rItem.GetCompleteName(true)};

		for (auto const& rTranslateName : TranslateNames)
		{
			size_t Pos = Name.find(rTranslateName.first);
			//Check only that the start of the dotted name is found
			if (0 == Pos)
			{
				Name.replace(Pos, rTranslateName.first.size(), rTranslateName.second.c_str());
				break;
			}
		}
		SelectedObjectNames.insert(Name);
	}
	return SelectedObjectNames;
}

void SVIPDoc::LoadRegressionTestVariables(SVTreeType& rTree, SVTreeType::SVBranchHandle htiParent)
{
	SVTreeType::SVBranchHandle htiRegression = nullptr;
	SVTreeType::SVBranchHandle htiViews = nullptr;

	bool bOk = SvXml::SVNavigateTree::GetItemBranch(rTree, SvXml::CTAG_REGRESSIONTEST, htiParent, htiRegression);
	if (bOk)
	{
		_variant_t svVariant;
		bOk = SvXml::SVNavigateTree::GetItem(rTree, SvXml::CTAG_USE_PLAY_CONDITION, htiRegression, svVariant);
		if (bOk)
		{
			m_bRegressionTestUsePlayCondition = svVariant;
		}
		bOk = SvXml::SVNavigateTree::GetItem(rTree, SvXml::CTAG_PLAY_CONDITION_EQUATION, htiRegression, svVariant);
		if (bOk)
		{
			m_RegressionTestLoadEquationText = SvUl::createStdString(svVariant);
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

		while (View.pView = GetNextView(vPos))
		{
			rWriter.StartElement(View.pView->GetRuntimeClass()->m_lpszClassName);

			svVariant = ++ViewNumber;
			rWriter.WriteAttribute(SvXml::CTAG_VIEW_NUMBER, svVariant);
			svVariant.Clear();

			if (View.pView->IsKindOf(RUNTIME_CLASS(SVImageViewScroll)))
			{
				View.pImageScroll->GetParameters(rWriter);
			}
			else if (View.pView->IsKindOf(RUNTIME_CLASS(SVImageViewClass)))
			{
				View.pImageView->GetParameters(rWriter);
			}
			else if (View.pView->IsKindOf(RUNTIME_CLASS(ToolSetView)))
			{
				View.pToolSetView->GetParameters(rWriter);
			}
			else if (View.pView->IsKindOf(RUNTIME_CLASS(ResultTabbedView)))
			{
				View.pResultView->GetParameters(rWriter);
			}
			rWriter.EndElement();
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

	View.pToolSetView = dynamic_cast<ToolSetView*>(getView());
	if (View.pToolSetView && View.pToolSetView->GetSafeHwnd())
	{
		CSplitterWnd* pWndSplitter = dynamic_cast<CSplitterWnd*>(View.pToolSetView->GetParent());
		if (pWndSplitter && pWndSplitter->GetSafeHwnd())
		{
			CSplitterWnd* pWndSplitter2 = dynamic_cast<CSplitterWnd*>(pWndSplitter->GetParent());
			ASSERT(pWndSplitter2 && pWndSplitter2->GetSafeHwnd());

			SVIPSplitterFrame* pFrame = dynamic_cast<SVIPSplitterFrame*>(pWndSplitter2->GetParent());

			CRuntimeClass* pClass = pFrame->GetRuntimeClass();
			assert(_T("SVIPSplitterFrame") == std::string(pClass->m_lpszClassName));

			if (pFrame &&pFrame->GetSafeHwnd())
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


void SVIPDoc::correctToolGrouping()
{
	SVToolSetClass* pToolSet = GetToolSet();
	if (!pToolSet)
	{
		return;
	}
	for (int i = 0; i < pToolSet->GetSize(); i++)
	{
		std::string name = pToolSet->GetAt(i)->GetName();
		if (m_toolGroupings.find(name) == m_toolGroupings.end())
		{
			m_toolGroupings.clear();
			break;
		}
	}
	
	if (m_toolGroupings.empty())
	{
		std::string insertAtEnd;
		for (int i = 0; i < pToolSet->GetSize(); i++)
		{
			std::string name = pToolSet->GetAt(i)->GetName();
			m_toolGroupings.AddTool(name.c_str(), insertAtEnd.c_str());
		}
	}

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
			if (bOk) { wndpl.length = svVariant; }

			if (bOk)
			{
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
					if (bOk) { wndpl.ptMinPosition.x = svVariant; }

					if (bOk)
					{
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
					if (bOk) { wndpl.ptMaxPosition.x = svVariant; }

					if (bOk)
					{
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
					if (bOk) { wndpl.rcNormalPosition.left = svVariant; }

					if (bOk)
					{
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
			View.pToolSetView = dynamic_cast<ToolSetView*>(getView());
			if (View.pToolSetView && View.pToolSetView->GetSafeHwnd())
			{
				CSplitterWnd* pWndSplitter = dynamic_cast<CSplitterWnd*>(View.pToolSetView->GetParent());
				if (pWndSplitter && pWndSplitter->GetSafeHwnd())
				{
					//
					// This the one we want to retrieve the size and position.
					//
					CSplitterWnd* pWndSplitter2 = dynamic_cast<CSplitterWnd*>(pWndSplitter->GetParent());
					ASSERT(pWndSplitter2 && pWndSplitter2->GetSafeHwnd());

					SVIPSplitterFrame* pFrame = dynamic_cast<SVIPSplitterFrame*>(pWndSplitter2->GetParent());

					CRuntimeClass* pClass = pFrame->GetRuntimeClass();
					assert(_T("SVIPSplitterFrame") == std::string(pClass->m_lpszClassName));

					if (pFrame && pFrame->GetSafeHwnd())
					{
						bOk = pFrame->SetWindowPlacement(&wndpl) ? true : false; // WINDOWPLACEMENT* lpwndpl
					}// end if
				} // if(pFrame && pFrame->GetSafeHwnd())
			} // if(pWndSplitter && pWndSplitter->GetSafeHwnd())
		} // if( View.pToolSetView && View.pToolSetView->GetSafeHwnd() )
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
			_variant_t svVariant;

			long lViewNumber = 0;
			long lNumberOfViews = 0;

			SVTreeType::SVBranchHandle htiBranch = nullptr;

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
					if (bOk) { lViewNumber = svVariant; }

					if (bOk)
					{
						vPos = GetFirstViewPosition();

						if (vPos)
						{
							// get the view specified by 'ViewNumber'
							for (lNumberOfViews = 0; lNumberOfViews < lViewNumber; lNumberOfViews++)
							{
								View.pView = GetNextView(vPos);
								if (!View.pView)
									break;  // if there are not enough views, exit the loop
							}

							// if there were not enough views or the view found is the
							// wrong class, find the first matching view
							if (!View.pView ||
								Name.compare(View.pView->GetRuntimeClass()->m_lpszClassName))
							{
								vPos = GetFirstViewPosition();
								while ((View.pView = GetNextView(vPos)) &&
									(Name.compare(View.pView->GetRuntimeClass()->m_lpszClassName)));
							}

							if (View.pView)  // this should never fail, but if it does, we'll try to continue
							{
								if (View.pView->IsKindOf(RUNTIME_CLASS(SVImageViewScroll)))
								{
									bOk = View.pImageScroll->SetParameters(rTree, htiItem);
								}
								else if (View.pView->IsKindOf(RUNTIME_CLASS(SVImageViewClass)))
								{
									bOk = View.pImageView->SetParameters(rTree, htiItem);
								}
								else if (View.pView->IsKindOf(RUNTIME_CLASS(ToolSetView)))
								{
									bOk = View.pToolSetView->SetParameters(rTree, htiItem);
								}
								else if (View.pView->IsKindOf(RUNTIME_CLASS(ResultTabbedView)))
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
					if (bOk) { lViewNumber = svVariant; }

					if (bOk)
					{
						vPos = GetFirstViewPosition();

						if (vPos)
						{
							// get the view specified by 'ViewNumber'
							for (lNumberOfViews = 0; lNumberOfViews < lViewNumber; lNumberOfViews++)
							{
								View.pView = GetNextView(vPos);
								if (!View.pView)
									break;  // if there are not enough views, exit the loop
							}

							// if there were not enough views or the view found is the
							// wrong class, find the first matching view
							if (!View.pView ||
								Name.compare(View.pView->GetRuntimeClass()->m_lpszClassName))
							{
								vPos = GetFirstViewPosition();
								while ((View.pView = GetNextView(vPos)) &&
									(Name.compare(View.pView->GetRuntimeClass()->m_lpszClassName)));
							}

							if (View.pView)  // this should never fail, but if it does, we'll try to continue
							{
								if (View.pView->IsKindOf(RUNTIME_CLASS(SVImageViewScroll)))
								{
									bOk = View.pImageScroll->CheckParameters(rTree, htiItem);
								}
								else if (View.pView->IsKindOf(RUNTIME_CLASS(SVImageViewClass)))
								{
									bOk = View.pImageView->CheckParameters(rTree, htiItem);
								}
								else if (View.pView->IsKindOf(RUNTIME_CLASS(ToolSetView)))
								{
									bOk = View.pToolSetView->CheckParameters(rTree, htiItem);
								}
								else if (View.pView->IsKindOf(RUNTIME_CLASS(ResultTabbedView)))
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
				correctToolGrouping();
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
	SVResultListClass* pResultList = GetResultList();

	if (nullptr != pResultList)
	{
		Result = pResultList->getUpdateTimeStamp();
	}

	return Result;
}

HRESULT SVIPDoc::GetResultDefinitions(SVResultDefinitionDeque& rDefinitions) const
{
	HRESULT hres = E_FAIL;
	rDefinitions.clear();
	SVResultListClass* pResultList = GetResultList();
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
		SVResultListClass* pResultList = pInspection->GetResultList();
		if (nullptr != pResultList)
		{
			return pResultList->getResultTableData(m_triggerRecord);
		}
	}
	return {};
}

HRESULT SVIPDoc::IsToolSetListUpdated() const
{
	HRESULT l_Status = S_OK;

	SVToolSetClass* pToolSet = GetToolSet();
	if (pToolSet)
	{
		if (pToolSet->GetLastListUpdateTimestamp() < m_ToolSetListTimestamp)
		{
			l_Status = S_FALSE;
		}
		else
		{
			m_ToolSetListTimestamp = SvTl::GetTimeStamp();
		}
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

void SVIPDoc::OnAddExternalTool()
{
	AddTool(SVExternalToolGuid);
}

HRESULT SVIPDoc::RemoveImage(SvIe::SVImageClass* pImage)
{
	GUID sourceID = pImage->GetUniqueObjectID();

	return RemoveImage(sourceID);
}

HRESULT SVIPDoc::RemoveImage(const SVGUID& p_rImageId)
{
	HRESULT l_Status = S_FALSE;

	for (int i = 0; i < MaxImageViews; ++i)
	{
		SVImageViewClass *l_pView = GetImageView(i);

		if (l_pView->GetImageID() == p_rImageId)
		{
			l_pView->DetachFromImage();

			l_Status = S_OK;
		}
	}

	return l_Status;
}

void SVIPDoc::RefreshPublishedList()
{
	SVPublishListClass& publishList = GetInspectionProcess()->GetPublishList();
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

bool SVIPDoc::deleteTool(NavigatorElement* pNaviElement)
{
	SVInspectionProcess* pInspection(GetInspectionProcess());
	if (nullptr == pInspection)
	{
		return false;
	}
	SvTo::LoopTool* pLoopTool(nullptr);
	SVGUID parentGuid = GUID_NULL;
	if (NavElementType::SubTool == pNaviElement->m_Type || NavElementType::SubLoopTool == pNaviElement->m_Type)
	{
		SvPb::InspectionCmdMsgs request, response;
		SvPb::GetObjectParametersRequest* pGetObjectNameRequest = request.mutable_getobjectparametersrequest();

		SvPb::SetGuidInProtoBytes(pGetObjectNameRequest->mutable_objectid(), pNaviElement->m_OwnerGuid);
		HRESULT hr = SvCmd::InspectionCommands(m_InspectionID, request, &response);
		if (S_OK == hr && response.has_getobjectparametersresponse())
		{
			parentGuid = pNaviElement->m_OwnerGuid;
			if (SvPb::SVToolObjectType != response.getobjectparametersresponse().typeinfo().objecttype() ||
				SvPb::LoopToolObjectType != response.getobjectparametersresponse().typeinfo().subtype())
			{
				return false;
			}
		}
		else
		{
			return false;
		}
	}
	else
	{
		SVToolSetClass* pToolSet = GetToolSet();
		if (nullptr != pToolSet)
		{
			parentGuid = pToolSet->GetUniqueObjectID();
		}
	}
	if (GUID_NULL == parentGuid)
	{
		return false;
	}

	SVObjectClass* pObject2(SVObjectManagerClass::Instance().GetObject(pNaviElement->m_Guid));
	SvTo::SVToolClass* pTool = dynamic_cast<SvTo::SVToolClass*> (pObject2);
	if (nullptr == pTool)
	{
		return false;
	}
	if (false == checkOkToDelete(pTool))
	{
		return false;
	}
	CWaitCursor l_cwcMouse;

	// remove image from affected displays
	int i, j;
	SVImageViewClass *pImageView;
	SVOutObjectInfoStruct *pOutputObjectInfo;
	GUID ViewImageUniqueId;

	SVOutputInfoListClass l_OutputList;

	pTool->GetOutputList(l_OutputList);

	for (i = 0; i < MaxImageViews; i++)
	{
		pImageView = GetImageView(i);
		if (pImageView->GetImage())
		{
			ViewImageUniqueId = pImageView->GetImage()->GetUniqueObjectID();

			for (j = 0; j < l_OutputList.GetSize(); j++)
			{
				pOutputObjectInfo = l_OutputList.GetAt(j);
				if (ViewImageUniqueId == pOutputObjectInfo->getUniqueObjectID())
				{
					// Close Display resources...
					pImageView->DetachFromImage();
					break;
				}
			}
		}
	}

	SvPb::InspectionCmdMsgs Request, Response;
	SvPb::DestroyChildRequest* pDestroyChildRequest = Request.mutable_destroychildrequest();

	
	pDestroyChildRequest->set_flag(SvPb::DestroyChildRequest::Flag_SetDefaultInputs_And_ResetInspection);
	SvPb::SetGuidInProtoBytes(pDestroyChildRequest->mutable_taskobjectlistid(), parentGuid);
	SvPb::SetGuidInProtoBytes(pDestroyChildRequest->mutable_objectid(), pTool->GetUniqueObjectID());

	SvCmd::InspectionCommands(m_InspectionID, Request, &Response);
	SVConfigurationObject* pConfig(nullptr);
	SVObjectManagerClass::Instance().GetConfigurationObject(pConfig);
	if (nullptr != pConfig)
	{
		pConfig->ValidateRemoteMonitorList();
		TheSVObserverApp.GetIODoc()->UpdateAllViews(nullptr);
	}
	TheSVObserverApp.RebuildOutputList();
	return true;
}



void SVIPDoc::OnEditAdjustToolPosition()
{
	SvTo::SVToolClass* pTool = dynamic_cast<SvTo::SVToolClass*> (SVObjectManagerClass::Instance().GetObject(GetSelectedToolID()));

	if (nullptr != pTool)
	{
		SVObjectInfoStruct info = pTool->GetObjectInfo();
		SvDef::SVObjectTypeInfoStruct typeInfo = info.m_ObjectTypeInfo;
		//------ Warp tool hands back a SvDef::SVPolarTransformObjectType. Sub type 1792.
		//------ Window tool, Luminance hands back a SvDef::SVImageObjectType. Sub type 0.
		if (SVImageViewClass* pImageView = GetImageView())
		{
			SVSVIMStateClass::AddState(SV_STATE_EDITING);
			std::string DlgName = SvUl::Format(_T("Adjust Tool Size and Position - %s"), pTool->GetName());
			SvOg::SVAdjustToolSizePositionDlg dlg(m_InspectionID, pTool->GetUniqueObjectID(), DlgName.c_str(), dynamic_cast<CWnd*>(this->GetMDIChild()));
			dlg.DoModal();
			SVSVIMStateClass::RemoveState(SV_STATE_EDITING);
		}
	}
}

void SVIPDoc::OnUpdateEditAdjustToolPosition(CCmdUI* pCmdUI)
{
	// Check current user access...
	if (!TheSVObserverApp.OkToEdit())
	{
		return pCmdUI->Enable(false);
	}
	ToolSetView* pToolSetView = GetToolSetView();
	SVToolSetClass* pToolSet = GetToolSet();
	if (nullptr == pToolSet && nullptr == pToolSetView || pToolSetView->IsLabelEditing())
	{
		return pCmdUI->Enable(false);
	}

	int SelectedListIndex(-1);
	auto NavElement = pToolSetView->GetSelectedNavigatorElement(&SelectedListIndex);
	if (!NavElement || SelectedListIndex == -1)
	{
		return pCmdUI->Enable(false);
	}
	std::string Selection(NavElement->m_DisplayName);
	bool enabled(false);
	switch (NavElement->m_Type)
	{
		case NavElementType::SubTool:
		case NavElementType::Tool:
		case NavElementType::LoopTool:
			enabled = true;
			break;
	}
	SVGUID SelectedGuid(NavElement->m_Guid);
	if (!enabled || SelectedGuid.empty())
	{
		return pCmdUI->Enable(false);
	}

	//Tool list active and valid tool
	SvTo::SVToolClass* Tool = dynamic_cast<SvTo::SVToolClass*> (SVObjectManagerClass::Instance().GetObject(SelectedGuid));
	if (Tool)
	{
		//check to see if the tool has extents
		if (Tool->DoesObjectHaveExtents())
		{
			return pCmdUI->Enable(true);
		}
	}
	pCmdUI->Enable(false);
}

void SVIPDoc::OnShowToolRelations()
{
	SVGUID selectedToolID = GetSelectedToolID();
	if (GUID_NULL != selectedToolID)
	{
		SVGuidSet DependencySet;
		DependencySet.insert(selectedToolID);
		SvOg::SVShowDependentsDialog Dlg(DependencySet, SvPb::SVToolObjectType, nullptr, SvOg::SVShowDependentsDialog::Show);
		Dlg.DoModal();
	}
}

void SVIPDoc::OnUpdateShowToolRelations(CCmdUI* pCmdUI)
{
	// Check current user access...
	bool Enabled = TheSVObserverApp.OkToEdit();

	pCmdUI->Enable(Enabled);
}

void SVIPDoc::OnToolDependencies()
{
	SVToolSetClass* pToolSet = GetToolSet();
	if (nullptr != pToolSet)
	{
		CWaitCursor MouseBusy;
		SVGuidSet ToolIDSet;
		pToolSet->GetToolIds(std::inserter(ToolIDSet, ToolIDSet.end()));
		SvDef::StringPairVector m_dependencyList;
		std::string FileName;
		//Don't need to check inspection pointer because ToolSet pointer is valid
		FileName = SvUl::Format(_T("%s\\%s.dot"), SvStl::GlobalPath::Inst().GetTempPath().c_str(), GetInspectionProcess()->GetName());
		SvOi::getToolDependency(std::back_inserter(m_dependencyList), ToolIDSet, SvPb::SVToolObjectType, SvOi::ToolDependencyEnum::Client, FileName);
	}
}

void SVIPDoc::OnUpdateToolDependencies(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(TheSVObserverApp.OkToEdit());
}

void SVIPDoc::OnUpdateAddGeneralTool(CCmdUI* PCmdUI)
{
	bool Enabled = TheSVObserverApp.OkToEdit() && isImageAvailable(SvPb::SVImageMonoType);

	PCmdUI->Enable(Enabled);
}

void SVIPDoc::OnUpdateAddCylindricalWarpTool(CCmdUI* pCmdUI)
{
	bool Enabled = !SVSVIMStateClass::CheckState(SV_STATE_RUNNING | SV_STATE_TEST);

	Enabled = Enabled && TheSVObserverApp.OkToEdit() && isImageAvailable(SvPb::SVImageMonoType);

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
	Enabled = Enabled && TheSVObserverApp.OkToEdit() && isImageAvailable(SvPb::SVImageMonoType);

	pCmdUI->Enable(Enabled);
}

void SVIPDoc::OnUpdateAddColorTool(CCmdUI* PCmdUI)
{
	bool Enabled = !SVSVIMStateClass::CheckState(SV_STATE_RUNNING | SV_STATE_TEST);
	// Check current user access...
	Enabled = Enabled && TheSVObserverApp.OkToEdit() && isImageAvailable(SvPb::SVImageColorType);

	PCmdUI->Enable(Enabled);
}
afx_msg void SVIPDoc::OnUpdateAddLoopTool(CCmdUI* PCmdUI)
{
	bool Enabled = !SVSVIMStateClass::CheckState(SV_STATE_RUNNING | SV_STATE_TEST);
	// Check current user access...
	Enabled = Enabled && TheSVObserverApp.OkToEdit() && isImageAvailable(SvPb::SVImageMonoType);;
	ToolSetView* pView = GetToolSetView();
	if (!pView || pView->IsLoopToolSelected())
	{
		//no LOOPTOOL in LOOPTOOL!
		Enabled = false;
	}

	PCmdUI->Enable(Enabled);
}

void SVIPDoc::OnViewResetAllCounts()
{
	if (S_OK == TheSVObserverApp.m_svSecurityMgr.SVValidate(SECURITY_POINT_VIEW_MENU_RESET_COUNTS_ALL))
	{
		TheSVObserverApp.ResetAllCounts();
	}
}

void SVIPDoc::OnViewResetCountsCurrentIP()
{
	if (S_OK == TheSVObserverApp.m_svSecurityMgr.SVValidate(SECURITY_POINT_VIEW_MENU_RESET_COUNTS_CURRENT))
	{
		SVInspectionProcess* pInspection = GetInspectionProcess();

		// @TODO:  What should we do if there's an error?  (Method is void.)
		if (nullptr != pInspection && nullptr != pInspection->GetToolSet())
		{
			pInspection->GetToolSet()->ResetCounts();
		}

		if (!SVSVIMStateClass::CheckState(SV_STATE_RUNNING))
		{
			RunOnce();
		}
	}
}

void SVIPDoc::SetRegressionTestRunMode(RegressionRunModeEnum newMode)
{
	m_regtestRunMode = static_cast<RegressionRunModeEnum>(newMode);
}

void SVIPDoc::SetRegressionTestPlayMode(RegressionPlayModeEnum newPlayMode)
{
	m_regtestRunPlayMode = newPlayMode;
}

DWORD WINAPI SVIPDoc::SVRegressionTestRunThread(LPVOID lpParam)
{
	SVIPDoc* pIPDoc = reinterpret_cast<SVIPDoc*> (lpParam);
	SVMainFrame* pMainFrm(dynamic_cast<SVMainFrame*> (AfxGetApp()->m_pMainWnd));
	HWND hRegressionWnd(nullptr);

	if (nullptr != pMainFrm && nullptr != pMainFrm->m_pregTestDlg)
	{
		hRegressionWnd = pMainFrm->m_pregTestDlg->GetSafeHwnd();
	}
	if (nullptr == pIPDoc->GetInspectionProcess() || nullptr == hRegressionWnd)
	{
		return E_FAIL;
	}

	bool l_bFirst = true;
	pIPDoc->m_bRegressionTestRunning = true;

	pIPDoc->m_regtestRunMode = RegModePause;
	pIPDoc->m_bRegressionTestStopping = false;

	bool l_bUsingSingleFile = false;
	bool bRunFlag = false;
	bool bModeReset = false;

	while (pIPDoc->m_regtestRunMode != RegModeStopExit)
	{
		//while in Pause mode, sleep
		bModeReset = false;
		if (pIPDoc->m_regtestRunMode == RegModePause)
		{
			::Sleep(50);
			continue;
		}

		CList<RegressionRunFileStruct*, RegressionRunFileStruct*> l_lstFileNames;
		RegressionRunFileStruct *ptmpRunFileStruct;
		RegressionTestStruct *ptmpRegTestStruct;

		INT_PTR iListCnt = pIPDoc->m_listRegCameras.GetCount();
		bool l_bDone = false;
		bool l_bListDone = false;
		int l_iNumSingleFile = 0;

		for (INT_PTR i = 0; i < iListCnt; i++)
		{
			POSITION posCamera = pIPDoc->m_listRegCameras.FindIndex(i);

			if (nullptr == posCamera)
			{
				continue;
			}
			l_bDone = false;

			ptmpRunFileStruct = new RegressionRunFileStruct;
			ptmpRegTestStruct = pIPDoc->m_listRegCameras.GetAt(posCamera);

			if (nullptr == ptmpRegTestStruct)
			{
				continue;
			}

			if (ptmpRegTestStruct->iFileMethod == RegSingleFile)
			{
				l_bUsingSingleFile = true;
				l_iNumSingleFile++;
			}

			ptmpRunFileStruct->CameraName = ptmpRegTestStruct->Camera;
			switch (pIPDoc->m_regtestRunMode)
			{
				case RegModePlay:
				{
					bRunFlag = true;
					if (!l_bFirst)
					{
						ptmpRegTestStruct->stdIteratorCurrent++;
					}
					if (ptmpRegTestStruct->stdIteratorCurrent == ptmpRegTestStruct->stdVectorFile.end())
					{
						ptmpRegTestStruct->stdIteratorCurrent = ptmpRegTestStruct->stdVectorFile.begin();
					}

					if (ptmpRegTestStruct->iFileMethod != RegSingleFile)
					{
						if (pIPDoc->m_regtestRunPlayMode != Continue && !l_bFirst)
						{
							if (ptmpRegTestStruct->stdIteratorCurrent == ptmpRegTestStruct->stdIteratorStart)
							{
								l_bListDone = true;
								bRunFlag = false;
							}
						}
					}

					if (pIPDoc->m_regtestRunPlayMode != Continue)
					{
						if (l_bUsingSingleFile)
						{
							if (iListCnt == l_iNumSingleFile) //all using single file
							{
								pIPDoc->m_regtestRunMode = RegModePause;
								SendMessage(hRegressionWnd, WM_REGRESSION_TEST_SET_PLAYPAUSE, reinterpret_cast<LPARAM> (&pIPDoc->m_regtestRunMode), 0);
								bModeReset = true;
								l_bFirst = true;
							}
							else
							{
								//not all using single file...
								//check to see if at end of list.  if so stop...
								if (l_bListDone && ((iListCnt - 1) == i))
								{
									pIPDoc->m_regtestRunMode = RegModePause;
									SendMessage(hRegressionWnd, WM_REGRESSION_TEST_SET_PLAYPAUSE, reinterpret_cast<LPARAM> (&pIPDoc->m_regtestRunMode), 0);
									bModeReset = true;
									l_bFirst = true;
									bRunFlag = false;
								}
							}
						}
						else
						{  //not using single files.
							if (l_bListDone && ((iListCnt - 1) == i))
							{
								pIPDoc->m_regtestRunMode = RegModePause;
								SendMessage(hRegressionWnd, WM_REGRESSION_TEST_SET_PLAYPAUSE, reinterpret_cast<LPARAM> (&pIPDoc->m_regtestRunMode), 0);
								bModeReset = true;
								l_bFirst = true;
								bRunFlag = false;
							}
						}
					}

					ptmpRunFileStruct->FileName = *ptmpRegTestStruct->stdIteratorCurrent;
					break;
				}
				case RegModeSingleStepForward:
				{
					if (!l_bFirst)
					{
						ptmpRegTestStruct->stdIteratorCurrent++;
					}

					if (ptmpRegTestStruct->stdIteratorCurrent == ptmpRegTestStruct->stdVectorFile.end())
					{
						ptmpRegTestStruct->stdIteratorCurrent = ptmpRegTestStruct->stdVectorFile.begin();
					}
					ptmpRunFileStruct->FileName = *ptmpRegTestStruct->stdIteratorCurrent;
					bRunFlag = true;
					break;
				}
				case RegModeSingleStepBack:
				{
					if (!l_bFirst)
					{
						if (ptmpRegTestStruct->stdIteratorCurrent == ptmpRegTestStruct->stdVectorFile.begin())
						{
							ptmpRegTestStruct->stdIteratorCurrent = ptmpRegTestStruct->stdVectorFile.end() - 1;
						}
						else
						{
							ptmpRegTestStruct->stdIteratorCurrent--;
						}
					}

					if (ptmpRegTestStruct->stdIteratorCurrent == ptmpRegTestStruct->stdVectorFile.end())
					{
						ptmpRegTestStruct->stdIteratorCurrent = ptmpRegTestStruct->stdVectorFile.end() - 1;
					}

					ptmpRunFileStruct->FileName = *ptmpRegTestStruct->stdIteratorCurrent;
					bRunFlag = true;

					break;
				}
				case RegModeBackToBeginningPlay:
				{
					ptmpRegTestStruct->stdIteratorCurrent = ptmpRegTestStruct->stdIteratorStart;
					ptmpRunFileStruct->FileName = *ptmpRegTestStruct->stdIteratorCurrent;
					bRunFlag = true;
					break;
				}
				case RegModeBackToBeginningStop:
				{
					ptmpRegTestStruct->stdIteratorCurrent = ptmpRegTestStruct->stdIteratorStart;
					ptmpRunFileStruct->FileName = *ptmpRegTestStruct->stdIteratorCurrent;
					bRunFlag = true;
					break;
				}
				case RegModeResetSettings:
				{
					l_bFirst = true;
					ptmpRegTestStruct->stdIteratorCurrent = ptmpRegTestStruct->stdIteratorStart;

					if (i == (iListCnt - 1)) //last item in list
					{
						//set back to pause
						pIPDoc->m_regtestRunMode = RegModePause;
					}
					l_bUsingSingleFile = false; //reset this value
					l_iNumSingleFile = 0;
					bRunFlag = false;
					bModeReset = true;
					break;
				}
				default:
				{
					break;
				}
			}//end switch

			if (bRunFlag)
			{
				pIPDoc->m_listRegCameras.SetAt(posCamera, ptmpRegTestStruct);

				l_lstFileNames.AddTail(ptmpRunFileStruct);

				if (nullptr != pIPDoc->GetInspectionProcess())
				{
					pIPDoc->GetInspectionProcess()->AddInputImageRequestByCameraName(ptmpRunFileStruct->CameraName, ptmpRunFileStruct->FileName);
				}
				else
				{
					break;
				}
			}
		}//end for i to num cameras...
		if (pIPDoc->m_regtestRunMode == RegModeBackToBeginningPlay)
		{
			pIPDoc->m_regtestRunMode = RegModePlay;
		}

		if (pIPDoc->m_regtestRunMode == RegModeBackToBeginningStop)
		{
			pIPDoc->m_regtestRunMode = RegModePause;
			SendMessage(hRegressionWnd, WM_REGRESSION_TEST_SET_PLAYPAUSE, reinterpret_cast<LPARAM> (&pIPDoc->m_regtestRunMode), 0);
		}
		l_bDone = false;
		//send image info to the dialog...
		if (bRunFlag)
		{
			bRunFlag = false;

			SendMessage(hRegressionWnd, WM_REGRESSION_TEST_SET_FILENAMES, reinterpret_cast<LPARAM> (&l_lstFileNames), 0);

			pIPDoc->RunOnce();

			//////////////////////////////////////////////////////////////////////////////////
			//clean up
			INT_PTR iNumList = l_lstFileNames.GetCount();
			//delete l_lstFileNames
			for (int iNumDel = static_cast<int>(iNumList) - 1; iNumDel >= 0; --iNumDel)
			{
				POSITION posDel = l_lstFileNames.FindIndex(iNumDel);
				if (nullptr != posDel)
				{
					RegressionRunFileStruct *pDeleteStruct = l_lstFileNames.GetAt(posDel);
					if (pDeleteStruct)
					{
						delete pDeleteStruct;
					}
				}
			} //end of loop to delete temp structures...
		}

		if (!pIPDoc->m_bRegressionTestStopping)
		{
			if (pIPDoc->m_regtestRunMode == RegModePlay)
			{
				int iMS = pIPDoc->m_iRegessionTimeoutMS;
				if (iMS < MinRegressionTime)
				{
					iMS = MinRegressionTime;
				}
				Sleep(iMS);
			}
			else
			{
				pIPDoc->m_regtestRunMode = RegModePause;
			}
		}

		if (!bModeReset)
		{
			l_bFirst = false;
		}

		if (pIPDoc->shouldPauseRegressionTestByCondition())
		{
			pIPDoc->m_regtestRunMode = RegModePause;
			SendMessage(hRegressionWnd, WM_REGRESSION_TEST_SET_PLAYPAUSE, reinterpret_cast<LPARAM> (&pIPDoc->m_regtestRunMode), 0);
			bRunFlag = false;
			if (pIPDoc->m_regtestRunPlayMode != Continue)
			{
				//Check if end of list reached and if then set it to begin.
				bool bEndOfList = false;
				for (INT_PTR i = 0; i < iListCnt; i++)
				{
					POSITION posCamera = pIPDoc->m_listRegCameras.FindIndex(i);
					ptmpRegTestStruct = pIPDoc->m_listRegCameras.GetAt(posCamera);
					if (ptmpRegTestStruct->iFileMethod != RegSingleFile)
					{
						if (ptmpRegTestStruct->stdVectorFile.end() == ptmpRegTestStruct->stdIteratorCurrent + 1)
						{
							bEndOfList = true;
						}
					}
				}
				if (bEndOfList)
				{
					l_bFirst = true;
					for (INT_PTR i = 0; i < iListCnt; i++)
					{
						POSITION posCamera = pIPDoc->m_listRegCameras.FindIndex(i);
						ptmpRegTestStruct = pIPDoc->m_listRegCameras.GetAt(posCamera);
						ptmpRegTestStruct->stdIteratorCurrent = ptmpRegTestStruct->stdVectorFile.begin();
					}
				}
			}

		}
	}//end of while loop

	//let the IP know that the regression test is done.
	PostMessage(pMainFrm->GetSafeHwnd(), WM_COMMAND, WM_REGRESSION_TEST_COMPLETE, 0L);
	//let the RegressionRunDlg know that it is to shut down...
	SendMessage(hRegressionWnd, WM_REGRESSION_TEST_CLOSE_REGRESSION, 0, 0);

	return 0L;
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

	SVSVIMStateClass::RemoveState(SV_STATE_REGRESSION);
}

void SVIPDoc::RegressionTestModeChanged()
{
	// @WARNING:  Pointers should be checked before they are dereferenced.
	::BringWindowToTop(AfxGetMainWnd()->GetSafeHwnd());
	m_bRegressionTestRunning = false;
	m_regtestRunPlayMode = RunToEnd;
	m_bRegressionTestStopping = true;
	m_regtestRunMode = RegModeStopExit;  //should cause dialog to go away...

	SVSVIMStateClass::RemoveState(SV_STATE_REGRESSION);

	SVToolSetClass* pToolSet = GetToolSet();
	if (nullptr != pToolSet)
	{
		pToolSet->goingOffline(); // Mainly used to tell Archive Tool to shutdown
		pToolSet->ResetObject();
	}
	RunOnce();
}

RegressionRunModeEnum SVIPDoc::GetRegressionRunMode()
{
	return m_regtestRunMode;
}

void SVIPDoc::SetRegressionTimeoutPeriod(int p_TimeoutMS)
{
	m_iRegessionTimeoutMS = p_TimeoutMS;
}

bool SVIPDoc::IsRegressionTestRunning()
{
	return m_bRegressionTestRunning;
}

SVGUID SVIPDoc::GetSelectedToolID() const
{
	ToolSetView* pView = GetToolSetView();
	if (pView)
	{
		return pView->GetSelectedTool();
	}
	else
	{
		return SVGUID();
	}
}

void SVIPDoc::SetSelectedToolID(const SVGUID& p_rToolID)
{
	ToolSetView* pView = GetToolSetView();
	if (pView)
	{
		pView->SetSelectedTool(p_rToolID);
	}
}

bool SVIPDoc::IsToolPreviousToSelected(const SVGUID& p_rToolID) const
{
	bool l_Status = false;
	if (nullptr != GetToolSet())
	{
		l_Status = GetToolSet()->IsToolPreviousToSelected(p_rToolID);
	}

	return l_Status;
}

std::string SVIPDoc::GetCompleteToolSetName() const
{
	std::string Result;

	SVToolSetClass* l_pToolSet = GetToolSet();

	if (nullptr != l_pToolSet)
	{
		Result = l_pToolSet->GetObjectNameToObjectType(SvPb::SVToolObjectType);
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
	pCmdUI->Enable(TheSVObserverApp.m_svSecurityMgr.SVIsDisplayable(SECURITY_POINT_VIEW_MENU_RESET_COUNTS_ALL));
}

void SVIPDoc::OnUpdateViewResetCountsCurrentIP(CCmdUI* pCmdUI)
{
	// @WARNING:  Pointers should be checked before they are dereferenced.
	pCmdUI->Enable(TheSVObserverApp.m_svSecurityMgr.SVIsDisplayable(SECURITY_POINT_VIEW_MENU_RESET_COUNTS_CURRENT));
}

void SVIPDoc::ClearRegressionTestStructures()
{
	INT_PTR iCnt = m_listRegCameras.GetCount();
	if (iCnt > 0)
	{
		//empty list
		RegressionTestStruct* pTmpStruct = nullptr;
		for (INT_PTR i = iCnt - 1; i >= 0; --i)
		{
			POSITION pos = m_listRegCameras.FindIndex(i);
			pTmpStruct = m_listRegCameras.GetAt(pos);
			if (pTmpStruct)
			{
				delete pTmpStruct;
				pTmpStruct = nullptr;
				m_listRegCameras.RemoveAt(pos);
			}
		}
	}
}

void SVIPDoc::OnEditDataDefinitionLists()
{
	SVInspectionProcess* pInspection = GetInspectionProcess();

	if (nullptr != pInspection)
	{
		SVSVIMStateClass::AddState(SV_STATE_EDITING);
		std::string inspectionName = pInspection->GetName();
		const SVGUID& inspectionID = pInspection->GetUniqueObjectID();
		std::string Title = _T("Data Definition Lists - ");
		Title += inspectionName;
		SVDataDefinitionSheet sheet(this, Title.c_str(), inspectionName, inspectionID);

		//remove apply button
		sheet.m_psh.dwFlags |= PSH_NOAPPLYNOW;

		sheet.DoModal();
		SVSVIMStateClass::RemoveState(SV_STATE_EDITING);
	}
}

void SVIPDoc::OnUpdateEditDataDefinitionLists(CCmdUI *pCmdUI)
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
			// If image from m_Images map is in m_RegisteredImages map (GUID)

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
				SvIe::SVImageClass* pImage = dynamic_cast<SvIe::SVImageClass*>(SVObjectManagerClass::Instance().GetObject(l_RegisteredIter->first.ToGUID()));

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

HRESULT SVIPDoc::RegisterImage(const SVGUID& p_rImageId, SVImageViewClass* p_pImageView)
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

	if (!(p_rImageId.empty()))
	{
		m_RegisteredImages[p_rImageId].insert(p_pImageView);

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

HRESULT SVIPDoc::UnregisterImage(const SVGUID& p_rImageId, SVImageViewClass* p_pImageView)
{
	HRESULT l_Status = S_OK;

	SVMasterImageRegisterMap::iterator l_Iter = m_RegisteredImages.find(p_rImageId);

	if (l_Iter != m_RegisteredImages.end())
	{
		l_Iter->second.erase(p_pImageView);

		if (l_Iter->second.empty()) { m_RegisteredImages.erase(l_Iter); }

		l_Status = RebuildImages();
	}

	return l_Status;
}

HRESULT SVIPDoc::UnregisterImageView(SVImageViewClass* p_pImageView)
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

HRESULT SVIPDoc::IsImageDataSent(const SVGUID& p_rImageId, SVImageViewClass* p_pImageView) const
{
	HRESULT l_Status = S_OK;

	if (m_AllViewsUpdated == 0)
	{
		SVImageIdImageDataStructMap::const_iterator l_Iter = m_Images.find(p_rImageId);

		if (l_Iter != m_Images.end())
		{
			SVImageViewPtrImageViewStatusMap::const_iterator l_ViewIter = l_Iter->second.m_ImageViews.find(p_pImageView);

			if (l_ViewIter != l_Iter->second.m_ImageViews.end())
			{
				if (!(l_ViewIter->second.m_ViewNotified))
				{
					l_Status = S_FALSE;
				}
			}
		}
	}

	return l_Status;
}

HRESULT SVIPDoc::IsImageDataUpdated(const SVGUID& p_rImageId, SVImageViewClass* p_pImageView) const
{
	HRESULT l_Status = S_OK;

	if (m_AllViewsUpdated == 0)
	{
		SVImageIdImageDataStructMap::const_iterator l_Iter = m_Images.find(p_rImageId);

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

HRESULT SVIPDoc::IsImageDataDisplayed(const SVGUID& p_rImageId, SVImageViewClass* p_pImageView) const
{
	HRESULT l_Status = S_OK;

	if (m_AllViewsUpdated == 0)
	{
		SVImageIdImageDataStructMap::const_iterator l_Iter = m_Images.find(p_rImageId);

		if (l_Iter != m_Images.end())
		{
			SVImageViewPtrImageViewStatusMap::const_iterator l_ViewIter = l_Iter->second.m_ImageViews.find(p_pImageView);

			if (l_ViewIter != l_Iter->second.m_ImageViews.end())
			{
				if (!(l_ViewIter->second.m_DisplayComplete))
				{
					l_Status = S_FALSE;
				}
			}
		}
	}

	return l_Status;
}

HRESULT SVIPDoc::GetBitmapInfo(const SVGUID& p_rImageId, SVBitmapInfo& p_rBitmapInfo) const
{
	HRESULT l_Status = S_OK;

	SvIe::SVImageClass* pImage = dynamic_cast<SvIe::SVImageClass*>(SVObjectManagerClass::Instance().GetObject(p_rImageId.ToGUID()));

	if (nullptr != pImage)
	{
		SVImageInfoClass l_svImageInfo = pImage->GetImageInfo();

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

HRESULT SVIPDoc::GetImageData(const SVGUID& p_rImageId, std::string& p_rImageData, SVExtentMultiLineStructVector& p_rMultiLineArray) const
{
	HRESULT l_Status = S_OK;

	SVImageIdImageDataStructMap::const_iterator l_Iter = m_Images.find(p_rImageId);

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

HRESULT SVIPDoc::SetImageData(const SVGUID& p_rImageId, const std::string& p_rImageData, const SVExtentMultiLineStructVector& p_rMultiLineArray)
{
	HRESULT l_Status = S_OK;

	SVImageIdImageDataStructMap::iterator l_Iter = m_Images.find(p_rImageId);

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

HRESULT SVIPDoc::MarkImageDataSent(const SVGUID& p_rImageId, SVImageViewClass* p_pImageView)
{
	HRESULT l_Status = S_OK;

	if (m_AllViewsUpdated == 0)
	{
		SVImageIdImageDataStructMap::iterator l_Iter = m_Images.find(p_rImageId);

		if (l_Iter != m_Images.end())
		{
			SVImageViewPtrImageViewStatusMap::iterator l_ViewIter = l_Iter->second.m_ImageViews.find(p_pImageView);

			if (l_ViewIter != l_Iter->second.m_ImageViews.end())
			{
				l_ViewIter->second.m_ViewNotified = true;
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

HRESULT SVIPDoc::MarkImageDataUpdated(const SVGUID& p_rImageId, SVImageViewClass* p_pImageView)
{
	HRESULT l_Status = S_OK;

	if (m_AllViewsUpdated == 0)
	{
		SVImageIdImageDataStructMap::iterator l_Iter = m_Images.find(p_rImageId);

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

HRESULT SVIPDoc::MarkImageDataDisplayed(const SVGUID& p_rImageId, SVImageViewClass* p_pImageView)
{
	HRESULT l_Status = S_OK;

	if (m_AllViewsUpdated == 0)
	{
		SVImageIdImageDataStructMap::iterator l_Iter = m_Images.find(p_rImageId);

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
	HRESULT l_Status = SVGuiExtentUpdater::SetImageExtent(pTask, rExtents);
	if (S_OK == l_Status) { SetModifiedFlag(); }
	return l_Status;
}

HRESULT SVIPDoc::UpdateExtentsToFit(SvIe::SVTaskObjectClass* pTask, const SVImageExtentClass& rExtents)
{
	HRESULT l_Status = SVGuiExtentUpdater::SetImageExtentToFit(pTask, rExtents);
	if (S_OK == l_Status) { SetModifiedFlag(); }
	return l_Status;
}

SvIe::SVImageClass* SVIPDoc::GetImageByName(LPCTSTR ImageName) const
{
	SvIe::SVImageClass* pImage = nullptr;
	SVInspectionProcess* pInspection = GetInspectionProcess();

	if (nullptr != pInspection)
	{
		SVObjectManagerClass::Instance().GetObjectByDottedName(ImageName, pImage);
	}

	return pImage;
}

HRESULT SVIPDoc::UpdateWithLastProduct()
{
	HRESULT l_Status = S_OK;

	SVInspectionProcess* pInspection = GetInspectionProcess();

	if (nullptr != pInspection)
	{
		l_Status = pInspection->LastProductNotify();
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

bool SVIPDoc::RunOnce()
{
	SVInspectionProcess* pInspection = GetInspectionProcess();
	bool l_Status = (nullptr != pInspection);

	if (l_Status)
	{
		l_Status = (S_OK == SvCmd::RunOnceSynchronous(pInspection->GetUniqueObjectID()));
	}

	return l_Status;
}


int SVIPDoc::GetToolToInsertBefore(const std::string& rName, int listIndex) const
{
	int toolListIndex = 0;

	const SVToolSetClass* pToolSet = GetToolSet();
	if (pToolSet)
	{
		toolListIndex = pToolSet->GetSize();
		if (!rName.empty())
		{
			bool bFound = false;
			std::string ToolName = m_toolGroupings.GetToolToInsertBefore(rName);
			if (!ToolName.empty())
			{
				// FindTool by Name in SVTaskObjectList
				for (int i = 0; i < pToolSet->GetSize() && !bFound; i++)
				{
					const SvIe::SVTaskObjectClass* pTool = pToolSet->GetAt(i);
					if (pTool && pTool->GetName() == ToolName)
					{
						toolListIndex = i;
						bFound = true;
					}
				}
			}
			if (!bFound && 0 == listIndex)
			{
				toolListIndex = 0;
			}
		}
	}
	return toolListIndex;
}

bool SVIPDoc::isImageAvailable(SvPb::SVObjectSubTypeEnum ImageSubType) const
{
	bool Result {false};

	SvPb::InspectionCmdMsgs request, response;
	SvPb::GetAvailableObjectsRequest* pGetAvailableObjectsRequest = request.mutable_getavailableobjectsrequest();

	SvPb::SetGuidInProtoBytes(pGetAvailableObjectsRequest->mutable_objectid(), m_InspectionID);
	pGetAvailableObjectsRequest->mutable_typeinfo()->set_objecttype(SvPb::SVImageObjectType);
	pGetAvailableObjectsRequest->mutable_typeinfo()->set_subtype(ImageSubType);
	SvPb::SetGuidInProtoBytes(pGetAvailableObjectsRequest->mutable_isbeforetoolmethod()->mutable_toolid(), GetSelectedToolID());
	HRESULT hr = SvCmd::InspectionCommands(m_InspectionID, request, &response);
	SvUl::NameGuidList availableList;
	if (S_OK == hr && response.has_getavailableobjectsresponse())
	{
		Result = ( 0 < response.getavailableobjectsresponse().list().size());
	}

	return Result;
}
