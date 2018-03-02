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

#include "SVObjectLibrary/SVObjectSynchronousCommandTemplate.h"
#include "ObjectInterfaces/IObjectWriter.h"
#include "SVTimerLibrary/SVClock.h"
#include "SVUtilityLibrary/StringHelper.h"
#include "SVUtilityLibrary/SVGUID.h"

#include "InspectionEngine/SVConditional.h"
#include "SVFileSystemLibrary/SVFileNameManagerClass.h"
#include "SVGlobal.h"
#include "SVImageArchive.h"
#include "SVImageViewScroll.h"
#include "SVImageView.h"
#include "InspectionEngine/SVImageProcessingClass.h"
#include "SVIPChildFrm.h"
#include "SVLightReferenceDialog.h"
#include "SVLutDlg.h"
#include "SVMainFrm.h"
#include "SVXMLLibrary\SVObjectXMLWriter.h"
#include "SVObjectLibrary/SVObjectManagerClass.h"
#include "SVObjectScriptParser.h"
#include "SVObserver.h"
#include "SVToolAdjustmentDialogSheetClass.h"
#include "SVToolSet.h"
#include "SVToolSetAdjustmentDialogSheet.h"
#include "ToolSetView.h"
#include "ResultTabbedView.h"
#include "InspectionEngine/SVTool.h"
#include "SVOGui\SVSaveToolSetImageDialog.h"
#include "SVOGui\SVShowDependentsDialog.h"
#include "SVUtilities.h"
#include "SVInspectionProcess.h"
#include "SVPPQObject.h"
#include "SVXMLLibrary/SVConfigurationTags.h"
#include "SVStatusLibrary/SVSVIMStateClass.h"
#include "SVAcquisitionClass.h"
#include "SVConfigurationObject.h"
#include "SVIODoc.h"
#include "SVXMLLibrary/SVNavigateTree.h"
#include "SVMessage/SVMessage.h"
#include "Definitions/SVUserMessage.h"
#include "ObjectInterfaces/IDependencyManager.h"
#include "SVImageLibrary/SVImagingDeviceParams.h"
#include "SVObjectLibrary/SVObjectLevelCreateStruct.h"
#include "SVLinearToolClass.h"
#include "SVAdjustToolSizePositionDlg.h"
#include "SVDataDefinitionSheet.h"
#include "SVRegressionRunDlg.h"
#include "SVHBitmapUtilitiesLibrary\SVHBitmapUtilities.h"
#include "SVDirectX.h"
#include "SVCommandInspectionCollectImageData.h"
#include "SVGuiExtentUpdater.h"
#include "SVArchiveTool.h"
#include "SVColorTool.h"
#include "SVMathTool.h"
#include "InspectionEngine/SVPolarTransformationTool.h"
#include "InspectionEngine/SVTransformationTool.h"
#include "SVToolAcquisition.h"
#include "SVToolLoadImage.h"
#include "SVToolImage.h"
#include "SVWindowTool.h"
#include "SVCylindricalWarpTool.h"
#include "SVPerspectiveTool.h"
#include "SVStatTool.h"
#include "SVExternalTool.h"
#include "SVRemoteInputTool.h"
#include "SVShiftTool.h"
#include "ResizeTool.h"
#include "ObjectSelectorLibrary/ObjectTreeGenerator.h"
#include "Definitions/GlobalConst.h"
#include "SVContainerLibrary/SelectorItem.h"
#include "ToolClipboard.h"
#include "ExtrasEngine.h"
#include "TextDefinesSvO.h"
#include "SVShiftTool.h"
#include "RingBufferTool.h"
#include "SVOGui/NoSelector.h"
#include "SVOGui/GlobalSelector.h"
#include "SVOGui/PPQSelector.h"
#include "SVOGui/ToolSetItemSelector.h"
#include "TableTool.h"
#include "TableAnalyzerTool.h"
#include "SVOGui/ResultTableSelectionDlg.h"
#include "InspectionCommands/CommandFunctionHelper.h"
#include "InspectionCommands/GetAvailableObjects.h"
#include "InspectionCommands/GetAllowedImageList.h"
#include "SVOGui/TextDefinesSvOg.h"
#include "SVStatusLibrary/GlobalPath.h"
#include "Definitions/StringTypeDef.h"
#include "SVMFCControls/SVFileDialog.h"
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

static const int MaxImageViews = 8;

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
	ON_COMMAND(WM_REGRESSION_MODE_CHANGED, RegressionTestModeChanged)
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
	ON_COMMAND(ID_ADD_REMOTEINPUTTOOL, OnAddRemoteInputTool)
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
	ON_UPDATE_COMMAND_UI(ID_ADD_LOADIMAGETOOL, OnUpdateAddGeneralTool)
	ON_UPDATE_COMMAND_UI(ID_ADD_IMAGETOOL, OnUpdateAddGeneralTool)
	ON_UPDATE_COMMAND_UI(ID_ADD_ARCHIVETOOL, OnUpdateAddGeneralTool)
	ON_UPDATE_COMMAND_UI(ID_ADD_MATHTOOL, OnUpdateAddGeneralTool)
	ON_UPDATE_COMMAND_UI(ID_ADD_STATISTICSTOOL, OnUpdateAddGeneralTool)
	ON_UPDATE_COMMAND_UI(ID_ADD_EXTERNAL_TOOL, OnUpdateAddGeneralTool)
	ON_UPDATE_COMMAND_UI(ID_ADD_POLARUNWRAPTOOL, OnUpdateAddGeneralTool)
	ON_UPDATE_COMMAND_UI(ID_ADD_ACQUISITIONTOOL, OnUpdateAddGeneralTool)
	ON_UPDATE_COMMAND_UI(ID_ADD_LINEARTOOL, OnUpdateAddGeneralTool)
	ON_UPDATE_COMMAND_UI(ID_ADD_REMOTEINPUTTOOL, OnUpdateAddGeneralTool)
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
	SVFileNameManagerClass::Instance().AddItem(&msvFileName);

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

	SVFileNameManagerClass::Instance().RemoveItem(&msvFileName);

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

bool SVIPDoc::AddTool(SVToolClass* pTool)
{
	if (nullptr != pTool)
	{
		ToolSetView* pView = GetToolSetView();
		if (pView)
		{
			SVToolSetClass* pToolSet = GetToolSet();

			const ToolListSelectionInfo& info = pView->GetToolListSelectionInfo();
			int toolListIndex = GetToolToInsertBefore(info.m_Selection, info.m_listIndex);

			// A Color Tool must always be first on a RGB Color System.
			// There can be multiple Color Tools on a RGB Color System, one of them must be first.
			if (0 == toolListIndex)
			{
				if (pToolSet && pToolSet->GetSize())
				{
					SVToolClass* pNextTool = dynamic_cast<SVToolClass*>(pToolSet->GetAt(0));
					if (pNextTool && SV_IS_KIND_OF(pNextTool, SVColorToolClass) &&
						!SV_IS_KIND_OF(pTool, SVColorToolClass))
					{
						SvStl::MessageMgrStd Msg(SvStl::LogAndDisplay);
						Msg.setMessage(SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_ColorToolMustBeFirstMessage, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_10055);
						return false;
					}
				}
			}

			std::string ToolName(pTool->GetName());
			ToolName = CheckName(ToolName);
			pTool->SetName(ToolName.c_str());

			SVAddTool l_AddTool(pTool, toolListIndex);

			if (S_OK == SVObjectManagerClass::Instance().UpdateObserver(GetInspectionID(), l_AddTool))
			{
				m_toolGroupings.AddTool(pTool->GetName(), info.m_Selection);

				// Refresh all views...
				UpdateAllViews(nullptr);

				const SVGUID& rGuid = pTool->GetUniqueObjectID();
				SetSelectedToolID(rGuid);

				SetModifiedFlag();

				pToolSet->updateToolPosition();
				return true;
			}
		}
	}
	return false;
}

bool SVIPDoc::AddToolGrouping(bool bStartGroup)
{
	bool bRetVal = false;

	ToolSetView* pView = GetToolSetView();
	if (pView)
	{
		const ToolListSelectionInfo& rInfo = pView->GetToolListSelectionInfo();
		if (bStartGroup)
		{
			m_toolGroupings.AddGroup(m_toolGroupings.GetDefaultName(), rInfo.m_Selection);
		}
		else
		{
			// Get Group Name
			std::string GroupName = m_toolGroupings.FindGroup(rInfo.m_Selection);
			if (!GroupName.empty())
			{
				// check if has end already ?
				m_toolGroupings.AddEndGroup(GroupName.c_str(), rInfo.m_Selection);
			}
		}

		// Refresh all views...
		UpdateAllViews(nullptr, SVIPDoc::RefreshView);

		SetModifiedFlag();

		bRetVal = true;
	}
	return bRetVal;
}

void SVIPDoc::SetTitle(LPCTSTR LPSZTitle)
{
	CDocument::SetTitle(LPSZTitle);
}

void SVIPDoc::SetPathName(LPCTSTR LPSZPathName, BOOL bAddToMRU)
{
	BOOL bMod = GetPathName().CollateNoCase(LPSZPathName) != 0;

	if (bMod)
	{
		msvFileName.SetFullFileName(LPSZPathName);

		SVFileNameManagerClass::Instance().LoadItem(&msvFileName);

		SVSVIMStateClass::AddState(SV_STATE_MODIFIED);
	}

	// Never Add To MRU !!!
	CDocument::SetPathName(msvFileName.GetFullFileName().c_str(), FALSE);
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

SVConditionalClass* SVIPDoc::GetToolSetCondition()
{
	SVConditionalClass* l_pObject(nullptr);
	SVToolSetClass* l_pToolSet(GetToolSet());

	if (nullptr != l_pToolSet)
	{
		l_pObject = l_pToolSet->GetToolSetConditional();
	}

	return l_pObject;
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
		SvStl::MessageMgrStd Msg(SvStl::LogAndDisplay);
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

void SVIPDoc::OnCloseDocument()
{
	if (this)
	{
		// Do not save unsaved IPDoc without updating config
		if (TheSVObserverApp.getConfigFullFileName().empty() &&
			!SVSVIMStateClass::CheckState(SV_STATE_CANCELING))
		{
			TheSVObserverApp.OnFileSaveConfig();
		}

		Sleep(100);
	}

	CDocument::OnCloseDocument();
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
		SVVirtualCameraPtrSet cameraSet;
		SVInspectionProcess* pInspection(GetInspectionProcess());
		HRESULT hr = S_FALSE;
		if (nullptr != pInspection) { hr = pInspection->GetCamerasForLightReference(cameraSet); }
		bEnable = (S_OK == hr && cameraSet.size() > 0) ? true : false;
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
		SVVirtualCameraPtrSet cameraSet;
		SVInspectionProcess* pInspection(GetInspectionProcess());
		HRESULT hr = S_FALSE;
		if (nullptr != pInspection) { hr = pInspection->GetCamerasForLut(cameraSet); }
		bEnable = (S_OK == hr && cameraSet.size() > 0) ? true : false;
	}
	pCmdUI->Enable(bEnable);
}

void SVIPDoc::OnAdjustLightReference()
{
	SVInspectionProcess* pInspection(GetInspectionProcess());

	int i(0);

	ASSERT(GetToolSet());
	ASSERT(GetToolSet()->IsCreated());

	if (nullptr == pInspection || !GetToolSet()->IsCreated()) { return; }

	// NOTE:
	//		 At this time an IPDoc has only one camera image input!!!

	int iChannel = 0;
	// Show extreme LUT: black --> blue, white --> green...
	GetImageView()->ShowExtremeLUT();

	SVLightReferenceDialogPropertySheetClass dlg(std::string(_T("Light Reference Adjustment - [") + GetTitle() + _T("]")).c_str());

	// obtain copies of the arrays to set if IDOK
	SVLightReferencePtrVector apLRA;
	SVLightReferencePtrVector apLRAorig;

	SVVirtualCameraPtrSet setCameras;

	pInspection->GetCameras(setCameras);

	SVVirtualCameraPtrSet::iterator l_Iter = setCameras.begin();

	while (l_Iter != setCameras.end())
	{
		if (nullptr != (*l_Iter))
		{
			SVLightReference* pLRA = new SVLightReference();
			(*l_Iter)->GetAcquisitionDevice()->GetLightReference(*pLRA);
			apLRA.push_back(pLRA);

			// make another copy for cancel... this should move into the propsheet
			SVLightReference* pLRAorig = new SVLightReference();
			(*l_Iter)->GetAcquisitionDevice()->GetLightReference(*pLRAorig);
			apLRAorig.push_back(pLRAorig);
		}

		++l_Iter;
	}

	//remove apply button
	dlg.m_psh.dwFlags |= PSH_NOAPPLYNOW;

	if (dlg.CreatePages(setCameras, apLRA))
	{
		SVSVIMStateClass::AddState(SV_STATE_EDITING);

		SVConfigurationObject* pConfig(nullptr);
		SVObjectManagerClass::Instance().GetConfigurationObject(pConfig);
		ASSERT(nullptr != pConfig);

		if (nullptr != pConfig && dlg.DoModal() == IDOK)
		{
			SetModifiedFlag();

			for (i = 0, l_Iter = setCameras.begin(); l_Iter != setCameras.end(); ++i, ++l_Iter)
			{
				(*l_Iter)->SetLightReference(*(apLRA[i]));
				SVLightReference lra; // get new device lra; camera Set only modifies its band(s)
				(*l_Iter)->GetAcquisitionDevice()->GetLightReference(lra);
				pConfig->ModifyAcquisitionDevice((*l_Iter)->GetAcquisitionDevice()->DeviceName().c_str(), lra);
				delete apLRA[i];
				delete apLRAorig[i];
			}
		}
		else    // if cancel
		{
			for (i = 0, l_Iter = setCameras.begin(); l_Iter != setCameras.end(); ++i, ++l_Iter)
			{
				(*l_Iter)->SetLightReference(*(apLRAorig[i]));
				SVLightReference lra; // get new device lra; camera Set only modifies its band(s)
				(*l_Iter)->GetAcquisitionDevice()->GetLightReference(lra);
				pConfig->ModifyAcquisitionDevice((*l_Iter)->GetAcquisitionDevice()->DeviceName().c_str(), lra);
				delete apLRA[i];
				delete apLRAorig[i];
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

	ASSERT(GetToolSet());
	ASSERT(GetToolSet()->IsCreated());

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

	SVVirtualCameraPtrSet setCameras;

	pInspection->GetCameras(setCameras);

	SVVirtualCameraPtrSet::iterator l_Iter = setCameras.begin();

	while (l_Iter != setCameras.end())
	{
		if (nullptr != *l_Iter)
		{
			(*l_Iter)->GetAcquisitionDevice()->GetLut(aLut[(*l_Iter)->GetUniqueObjectID()]);
		}

		++l_Iter;
	}

	aLutOrig = aLut;

	//remove the apply button
	dlg.m_psh.dwFlags |= PSH_NOAPPLYNOW;

	bool bSuccess = dlg.Create(setCameras, aLut);

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
			for (SVVirtualCameraPtrSet::iterator l_Iter = setCameras.begin(); l_Iter != setCameras.end(); ++l_Iter)
			{
				(*l_Iter)->SetLut(aLutOrig[(*l_Iter)->GetUniqueObjectID()]);
			}
		}
		SVConfigurationObject* pConfig(nullptr);
		SVObjectManagerClass::Instance().GetConfigurationObject(pConfig);
		ASSERT(nullptr != pConfig);

		if (nullptr != pConfig)
		{
			for (SVVirtualCameraPtrSet::iterator l_Iter = setCameras.begin(); l_Iter != setCameras.end(); ++l_Iter)
			{
				SVLut lut;
				SVAcquisitionClassPtr pDevice = (*l_Iter)->GetAcquisitionDevice();
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
	SVToolClass* pTool = new SVShiftTool();

	if (AddTool(pTool)) { return; }

	if (pTool) { delete(pTool); }
}

void SVIPDoc::OnAddWindowTool()
{
	SVToolClass* pTool = new SVWindowToolClass();

	if (AddTool(pTool)) { return; }

	if (pTool) { delete(pTool); }
}

void SVIPDoc::OnAddCylindricalWarpTool()
{
	SVToolClass* pTool = new SVCylindricalWarpToolClass();

	if (AddTool(pTool)) { return; }

	if (pTool) { delete(pTool); }
}

void SVIPDoc::OnAddPerspectiveTool()
{
	SVToolClass* pTool = new SVPerspectiveToolClass();

	if (AddTool(pTool)) { return; }

	if (pTool) { delete(pTool); }
}

void SVIPDoc::OnAddImageTool()
{
	SVToolClass* pTool = new SVImageToolClass();

	if (AddTool(pTool)) { return; }

	if (pTool) { delete(pTool); }
}

void SVIPDoc::OnAddAcquisitionTool()
{
	SVToolClass* pTool = new SVAcquisitionToolClass();

	if (AddTool(pTool)) { return; }

	if (pTool) { delete(pTool); }
}

void SVIPDoc::OnAddArchiveTool()
{
	SVToolClass* pTool = new SVArchiveTool;

	if (AddTool(pTool)) { return; }

	if (pTool) { delete(pTool); }
}

void SVIPDoc::OnAddLinearTool()
{
	SVToolClass* pTool = new SVLinearToolClass();

	if (AddTool(pTool)) { return; }

	if (pTool) { delete(pTool); }
}

void SVIPDoc::OnAddLoadImageTool()
{
	SVToolClass* pTool = new SVLoadImageToolClass();

	if (AddTool(pTool)) { return; }

	if (pTool) { delete(pTool); }
}

void SVIPDoc::OnAddMathTool()
{
	SVToolClass* pTool = new SVMathToolClass;

	if (AddTool(pTool)) { return; }

	if (pTool) { delete(pTool); }
}

void SVIPDoc::OnAddStatisticsTool()
{
	SVToolClass* pTool = new SVStatisticsToolClass();

	if (AddTool(pTool)) { return; }

	if (pTool) { delete(pTool); }
}

void SVIPDoc::OnAddTransformationTool()
{
	SVToolClass* pTool = new SVTransformationToolClass;

	if (AddTool(pTool)) { return; }

	if (pTool) { delete(pTool); }
}

void SVIPDoc::OnAddRemoteInputTool()
{
	SVToolClass* pTool = new SVRemoteInputTool;

	if (AddTool(pTool)) { return; }

	if (pTool) { delete(pTool); }
}

void SVIPDoc::OnAddResizetool()
{
	SVToolClass* pTool = new ResizeTool();

	if (AddTool(pTool)) { return; }

	if (pTool) { delete(pTool); }
}

void SVIPDoc::OnAddRingBufferTool()
{
	SVToolClass* pTool = new RingBufferTool;

	if (AddTool(pTool)) { return; }

	if (pTool) { delete(pTool); }
}

void SVIPDoc::OnAddTableTool()
{
	SVToolClass* pTool = new TableTool;

	if (AddTool(pTool)) { return; }

	if (pTool) { delete(pTool); }
}

void SVIPDoc::OnAddTableAnalyzerTool()
{
	SVToolClass* pTool = new TableAnalyzerTool;

	if (AddTool(pTool)) { return; }

	if (pTool) { delete(pTool); }
}

void SVIPDoc::OnAddPolarUnwrapTool()
{
	SVPolarTransformationToolClass* pTool = new SVPolarTransformationToolClass;

	if (AddTool(pTool))
	{
		// Set the Use Anglular Method2 to the new type.
		pTool->m_svAngularMethod.SetDefaultValue(_T("Angular Method2"), true);

		return;
	}

	if (pTool) { delete(pTool); }
}

void SVIPDoc::OnAddColorTool()
{
	SVToolClass* pTool = new SVColorToolClass();

	if (AddTool(pTool)) { return; }

	if (pTool) { delete(pTool); }
}

void SVIPDoc::OnUpdateAddStartToolGrouping(CCmdUI* pCmdUI)
{
	// @WARNING:  Pointers should be checked before they are dereferenced.
	pCmdUI->Enable(!SVSVIMStateClass::CheckState(SV_STATE_RUNNING | SV_STATE_TEST) && SVSVIMStateClass::CheckState(SV_STATE_EDIT));
}

void SVIPDoc::OnUpdateAddEndToolGrouping(CCmdUI* pCmdUI)
{
	bool bEnable = (!SVSVIMStateClass::CheckState(SV_STATE_RUNNING | SV_STATE_TEST) && SVSVIMStateClass::CheckState(SV_STATE_EDIT));
	if (bEnable)
	{
		// check insertion position and ensure there is an unmatched start group before.
		ToolSetView* pView = GetToolSetView();
		if (pView)
		{
			bEnable = pView->IsEndToolGroupAllowed();
		}
	}
	// @WARNING:  Pointers should be checked before they are dereferenced.
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

	if (nullptr == pInspection) { return; }

	// Don't allow while inspecting
	if (SVSVIMStateClass::CheckState(SV_STATE_RUNNING | SV_STATE_TEST))
	{
		return;
	}
	ToolSetView* pToolSetView = GetToolSetView();
	SVToolSetClass* pToolSet = GetToolSet();
	if (pToolSet && pToolSetView && !pToolSetView->IsLabelEditing())
	{
		// Delete Group or Tool...
		const ToolListSelectionInfo& rInfo = pToolSetView->GetToolListSelectionInfo();
		const SVGUID& rGuid = pToolSetView->GetSelectedTool();
		if (!rGuid.empty())
		{
			// Don't let the first tool be deleted if it is a Color Tool and 
			// the next tool isn't a Color Tool.
			// A Color Tool must always be first on a RGB Color System.
			SVTaskObjectClass* pFirstTaskObject = pToolSet->GetAt(0);
			if (pFirstTaskObject && pFirstTaskObject->GetUniqueObjectID() == rGuid) // If deleting first item
			{
				if (pToolSet->GetSize() > 1)
				{
					SVToolClass* pNextTool = dynamic_cast<SVToolClass*>(pToolSet->GetAt(1));

					if (pFirstTaskObject && SV_IS_KIND_OF(pFirstTaskObject, SVColorToolClass) &&
						pNextTool && !SV_IS_KIND_OF(pNextTool, SVColorToolClass))
					{
						SvStl::MessageMgrStd Msg(SvStl::LogAndDisplay);
						Msg.setMessage(SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_ColorToolMustBeFirstMessage, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_10057);
						return;
					}
				}
			}
			SVTaskObjectClass* pTaskObject = dynamic_cast<SVTaskObjectClass *>(SVObjectManagerClass::Instance().GetObject(rGuid));
			if (pTaskObject)
			{
				int index = GetSelectedToolIndex(rInfo.m_Selection);

				HRESULT hr = DeleteTool(pTaskObject);

				if (S_OK == hr)
				{
					m_toolGroupings.RemoveTool(rInfo.m_Selection);

					// Current behavior is to select the previous tool
					// Select the previous tool or group (item) in the list ?
					--index;
					int iSize = pToolSet->GetSize();
					if (iSize > 0 && index >= 0 && index < iSize)
					{
						pTaskObject = pToolSet->GetAt(index);
					}
					else
					{
						pTaskObject = nullptr;
					}

					pToolSetView->getListCtrl().SaveScrollPos();
					pToolSetView->getListCtrl().Rebuild(); // do not remove as the item must be removed from the list before setting the new selection
					SVGUID selectedToolID = SVGUID();
					if (pTaskObject)
					{
						selectedToolID = pTaskObject->GetUniqueObjectID();
					}
					pToolSetView->SetSelectedTool(selectedToolID);
					pToolSetView->getListCtrl().RestoreScrollPos();
					pToolSetView->getListCtrl().EnsureVisible(index, true);

					SetModifiedFlag();

					RunOnce(); // this will cause rebuild to be called so why are we calling rebuild above?
				}
			}
		}
		else // Deleting Group
		{
			int index = rInfo.m_listIndex;
			index--;
			m_toolGroupings.RemoveGroup(rInfo.m_Selection);

			pToolSetView->getListCtrl().SaveScrollPos();
			pToolSetView->getListCtrl().Rebuild();
			// Select previous..
			if (index >= 0 && index < pToolSetView->getListCtrl().GetItemCount())
			{
				SVGUID selectedToolID = pToolSetView->getListCtrl().getToolGuid(index);

				pToolSetView->SetSelectedTool(selectedToolID);
				pToolSetView->getListCtrl().SetItemState(index, LVIS_SELECTED, LVIS_SELECTED);
				pToolSetView->getListCtrl().EnsureVisible(index, true);
			}
			pToolSetView->getListCtrl().RestoreScrollPos();
			SetModifiedFlag();
		}
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
	bool Enabled = TheSVObserverApp.OkToEdit();

	if (Enabled)
	{
		Enabled = false;
		ToolSetView* pToolSetView = GetToolSetView();
		SVToolSetClass* pToolSet = GetToolSet();
		if (nullptr != pToolSet && nullptr != pToolSetView && !pToolSetView->IsLabelEditing())
		{
			ToolListSelectionInfo ToolListInfo = pToolSetView->GetToolListSelectionInfo();

			const SVGUID& rGuid = pToolSetView->GetSelectedTool();
			//Tool list active and valid tool
			if (!rGuid.empty() && -1 != ToolListInfo.m_listIndex)
			{
				Enabled = true;
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

	if (nullptr != pInspection)
	{
		ToolClipboard Clipboard(*pInspection);
		SVGUID ToolGuid(GUID_NULL);
		ToolSetView* pView = GetToolSetView();

		const ToolListSelectionInfo& info = pView->GetToolListSelectionInfo();
		int ToolListIndex = GetToolToInsertBefore(info.m_Selection, info.m_listIndex);
		Clipboard.readFromClipboard(ToolListIndex, ToolGuid);

		SVObjectClass* pObject(SVObjectManagerClass::Instance().GetObject(ToolGuid));

		SVToolClass* pTool = dynamic_cast<SVToolClass*> (pObject);
		SVToolSetClass* pToolSet(pInspection->GetToolSet());

		if (nullptr != pTool && nullptr != pToolSet)
		{
			//Set the time stamp to 0 to force an update of the tool list
			SetSelectedToolID(ToolGuid);
			m_ToolSetListTimestamp = 0;
			pToolSet->moveTool(ToolListIndex, pTool);
			//If this is the first tool then it has already been inserted otherwise the name is unique
			if (m_toolGroupings.IsNameUnique(pTool->GetName()))
			{
				m_toolGroupings.AddTool(pTool->GetName(), info.m_Selection);
			}

			pTool->SetObjectDepthWithIndex(pInspection->GetObjectDepth(), 1);
			pTool->SetImageDepth(pInspection->GetImageDepth());

			SVObjectLevelCreateStruct createStruct;
			createStruct.OwnerObjectInfo.SetObject(pInspection);
			createStruct.m_pInspection = pInspection;

			pInspection->ConnectObject(createStruct);
			pInspection->ConnectAllInputs();

			SVObjectLevelCreateStruct createObjStruct;

			pInspection->createAllObjects(createObjStruct);
			//Reset only the inserted tool
			pTool->resetAllObjects();

			RunOnce();
			UpdateAllViews(nullptr);
			SetModifiedFlag();
		}
	}
}

void SVIPDoc::OnUpdateEditPaste(CCmdUI* pCmdUI)
{
	// Check current user access...
	bool Enabled = TheSVObserverApp.OkToEdit();

	if (Enabled)
	{
		Enabled = false;
		ToolSetView* pToolSetView = GetToolSetView();
		SVToolSetClass* pToolSet = GetToolSet();
		if (nullptr != pToolSet && nullptr != pToolSetView && !pToolSetView->IsLabelEditing())
		{
			const ToolListSelectionInfo& info = pToolSetView->GetToolListSelectionInfo();
			//Only if tool list active and a selected index is valid
			if (-1 != info.m_listIndex && ToolClipboard::isClipboardDataValid())
			{
				Enabled = true;
			}
		}
	}

	pCmdUI->Enable(Enabled);
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

void SVIPDoc::OnEditTool()
{
	static const int ImageTab = 0;
	return OpenToolAdjustmentDialog(ImageTab);
}

void SVIPDoc::OnEditToolTab1()
{

	static const int SizeTab = 1;
	return OpenToolAdjustmentDialog(SizeTab);
}

void SVIPDoc::OpenToolAdjustmentDialog(int tab)
{
	// Check current user access...
	if (TheSVObserverApp.OkToEdit())
	{
		SVToolClass* l_pTool = dynamic_cast<SVToolClass*>(SVObjectManagerClass::Instance().GetObject(GetSelectedToolID()));
		if (nullptr != l_pTool)
		{
			const SvDef::SVObjectTypeInfoStruct& rToolType = l_pTool->GetObjectInfo().m_ObjectTypeInfo;

			SVSVIMStateClass::AddState(SV_STATE_EDITING);
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
			EditToolSetCondition();
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
	//@TODO [gra][7.50][06.09.2017] This is a work around for the situation when in run mode and the Result Picker is called that
	//this causes the mode to change to SVIM_MODE_CHANGING which means it is no longer in Run mode, which could cause the Inspection to not work properly!
	//This needs to be looked into so that when in run mode no other state can change it
	if (!SVSVIMStateClass::CheckState(SV_STATE_RUNNING))
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

			SvOsl::ObjectTreeGenerator::Instance().setSelectorType(SvOsl::ObjectTreeGenerator::SelectorTypeEnum::TypeMultipleObject);
			SvOsl::ObjectTreeGenerator::Instance().setLocationFilter(SvOsl::ObjectTreeGenerator::FilterInput, InspectionName, std::string(_T("")));

			SvOsl::SelectorOptions BuildOptions(GetInspectionID(), SvDef::SV_VIEWABLE, GUID_NULL, true);
			SvOsl::ObjectTreeGenerator::Instance().BuildSelectableItems<SvOg::GlobalSelector, SvOg::PPQSelector, SvOg::ToolSetItemSelector<>>(BuildOptions);

			const SVObjectReferenceVector& rSelectedObjects(pResultList->GetSelectedObjects());

			SvOsl::ObjectTreeGenerator::Instance().setCheckItems(TranslateSelectedObjects(rSelectedObjects, InspectionName));

			std::string ResultPicker = SvUl::LoadStdString(IDS_RESULT_PICKER);
			std::string Title = SvUl::Format(_T("%s - %s"), ResultPicker.c_str(), InspectionName.c_str());
			std::string Filter = SvUl::LoadStdString(IDS_FILTER);
			INT_PTR Result = SvOsl::ObjectTreeGenerator::Instance().showDialog(Title.c_str(), ResultPicker.c_str(), Filter.c_str());

			if (IDOK == Result)
			{
				const SvCl::SelectorItemVector& SelectedItems = SvOsl::ObjectTreeGenerator::Instance().getSelectedObjects();
				pResultList->Clear();

				for (auto const& rEntry : SelectedItems)
				{
					pResultList->Insert(rEntry.m_Location);
				}
				// Set the Document as modified
				SetModifiedFlag();
				RebuildResultsList();
				UpdateWithLastProduct();
			}
			UpdateAllViews(nullptr);
		}
	}

	if (!SVSVIMStateClass::CheckState(SV_STATE_RUNNING))
	{
		SVSVIMStateClass::RemoveState(SV_STATE_EDITING);
	}
}

void SVIPDoc::OnResultsTablePicker()
{
	//@TODO [gra][7.50][06.09.2017] This is a work around for the situation when in run mode and the Result Table Picker is called that
	//this causes the mode to change to SVIM_MODE_CHANGING which means it is no longer in Run mode, which could cause the Inspection to not work properly!
	//This needs to be looked into so that when in run mode no other state can change it
	if (!SVSVIMStateClass::CheckState(SV_STATE_RUNNING))
	{
		SVSVIMStateClass::AddState(SV_STATE_EDITING); /// do this before calling validate for security as it may display a logon dialog!
	}
	if (S_OK == TheSVObserverApp.m_svSecurityMgr.SVValidate(SECURITY_POINT_EDIT_MENU_RESULT_PICKER))
	{
		SVResultListClass* pResultList(GetResultList());
		assert(nullptr != pResultList);
		if (nullptr != pResultList)
		{
			typedef SvCmd::GetAvailableObjects Command;
			typedef std::shared_ptr<Command> CommandPtr;

			SvUl::NameGuidList availableList;
			std::string selectedItem = SvOg::Table_NoSelected;
			SVGUID selectedGuid = pResultList->getTableGuid();
			CommandPtr commandPtr {new Command(GetInspectionID(), SvDef::SVObjectTypeInfoStruct(SvDef::TableObjectType, SvDef::SVNotSetSubObjectType), SvCmd::IsValidObject(), SvDef::SVToolSetObjectType)};
			SVObjectSynchronousCommandTemplate<CommandPtr> cmd(m_InspectionID, commandPtr);
			HRESULT hr = cmd.Execute(TWO_MINUTE_CMD_TIMEOUT);
			if (S_OK == hr)
			{
				availableList = commandPtr->AvailableObjects();
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
			}
		}
	}

	if (!SVSVIMStateClass::CheckState(SV_STATE_RUNNING))
	{
		SVSVIMStateClass::RemoveState(SV_STATE_EDITING);
	}
}

void SVIPDoc::OnSaveResultsToFile()
{
	static TCHAR Filter[] = _T("CSV Files (*.csv)|*.csv||");
	SvMc::SVFileDialog dlg(false, true, nullptr, nullptr, 0, Filter, nullptr);
	dlg.m_ofn.lpstrTitle = _T("Select File");
	if (dlg.DoModal() == IDOK)
	{
		CFile file;
		BOOL bResult = file.Open(dlg.GetPathName(), CFile::modeCreate | CFile::modeReadWrite | CFile::shareDenyNone, nullptr);

		if (bResult)
		{
			std::string tmpText;
			SVResultDefinitionDeque ResultDefinitions;
			SVIPResultData ResultData;
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

				SVIPResultItemDefinition& l_rDef = ResultDefinitions[i];

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
					NameToType = l_pObject->GetObjectNameToObjectType(SvDef::SVToolObjectType);
				}

				ItemIndex = SvUl::Format(_T("%d"), i);

				SVIPResultData::SVResultDataMap::const_iterator l_Iter = ResultData.m_ResultData.find(l_rDef);

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

				tmpText = Name + ";" /*+ Color + ";"*/ + Value + "; " + NameToType + "; " + ItemIndex + "\n";
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
			SvStl::MessageMgrStd Msg(SvStl::LogAndDisplay);
			Msg.setMessage(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_WriteCSVFileFailed, msgList, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_10058);
		}
	}
}

void SVIPDoc::OnSaveTableResultsToFile()
{
	static TCHAR Filter[] = _T("CSV Files (*.csv)|*.csv||");
	SvMc::SVFileDialog dlg(false, true, nullptr, nullptr, 0, Filter, nullptr);
	dlg.m_ofn.lpstrTitle = _T("Select File");
	if (dlg.DoModal() == IDOK)
	{
		CFile file;
		BOOL bResult = file.Open(dlg.GetPathName(), CFile::modeCreate | CFile::modeReadWrite | CFile::shareDenyNone, nullptr);

		if (bResult)
		{
			std::string tmpText;
			SVResultDefinitionDeque ResultDefinitions;
			SVIPResultData ResultData;
			GetResultData(ResultData);
			GetResultDefinitions(ResultDefinitions);

			tmpText = _T("No.;");
			for (IPResultTableData data : ResultData.m_ResultTableData)
			{
				tmpText += data.m_columnName + ";";
			}
			tmpText += "\n";
			file.Write(tmpText.c_str(), static_cast<int> (tmpText.size()));

			if (0 < ResultData.m_ResultTableData.size())
			{
				int rowCountNew = static_cast<int>(ResultData.m_ResultTableData[0].m_rowData.size());
				for (int i = 0; i < rowCountNew; i++)
				{
					tmpText = SvUl::Format(_T("%d;"), i + 1);
					for (int j = 0; j < ResultData.m_ResultTableData.size(); j++)
					{
						IPResultTableData data = ResultData.m_ResultTableData[j];
						if (data.m_rowData.size() > i)
						{
							tmpText += CString(data.m_rowData[i]);
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
			SvStl::MessageMgrStd Msg(SvStl::LogAndDisplay);
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

		SvOsl::ObjectTreeGenerator::Instance().setSelectorType(SvOsl::ObjectTreeGenerator::SelectorTypeEnum::TypeSetAttributes);
		SvOsl::ObjectTreeGenerator::Instance().setLocationFilter(SvOsl::ObjectTreeGenerator::FilterInput, InspectionName, std::string(_T("")));

		SvOsl::SelectorOptions BuildOptions(GetInspectionID(), SvDef::SV_PUBLISHABLE);
		SvOsl::ObjectTreeGenerator::Instance().BuildSelectableItems<SvOg::NoSelector, SvOg::NoSelector, SvOg::ToolSetItemSelector<>>(BuildOptions);

		std::string PublishableResults = SvUl::LoadStdString(IDS_PUBLISHABLE_RESULTS);
		std::string Title = SvUl::Format(_T("%s - %s"), PublishableResults.c_str(), InspectionName.c_str());
		std::string Filter = SvUl::LoadStdString(IDS_FILTER);
		INT_PTR Result = SvOsl::ObjectTreeGenerator::Instance().showDialog(Title.c_str(), PublishableResults.c_str(), Filter.c_str());

		if (IDOK == Result)
		{
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

		SvOsl::ObjectTreeGenerator::Instance().setSelectorType(SvOsl::ObjectTreeGenerator::SelectorTypeEnum::TypeSetAttributes);

		std::string RootName = SvUl::LoadStdString(IDS_CLASSNAME_ROOTOBJECT);
		SvOsl::ObjectTreeGenerator::Instance().setLocationFilter(SvOsl::ObjectTreeGenerator::FilterInput, RootName, std::string(_T("")));

		SvOsl::SelectorOptions BuildOptions(GetInspectionID(), SvDef::SV_PUBLISH_RESULT_IMAGE);
		SvOsl::ObjectTreeGenerator::Instance().BuildSelectableItems<SvOg::NoSelector, SvOg::NoSelector, SvOg::ToolSetItemSelector<>>(BuildOptions);

		std::string PublishableImages = SvUl::LoadStdString(IDS_PUBLISHABLE_RESULT_IMAGES);
		std::string Title = SvUl::Format(_T("%s - %s"), PublishableImages.c_str(), InspectionName.c_str());
		std::string Filter = SvUl::LoadStdString(IDS_FILTER);

		INT_PTR Result = SvOsl::ObjectTreeGenerator::Instance().showDialog(Title.c_str(), PublishableImages.c_str(), Filter.c_str());

		if (IDOK == Result)
		{
			SetModifiedFlag();

			const SvCl::SelectorItemVector& rSelectedItems = SvOsl::ObjectTreeGenerator::Instance().getSelectedObjects();

			for (auto const& rEntry : rSelectedItems)
			{
				SVGUID ObjectGuid {rEntry.m_ItemKey};
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

void SVIPDoc::EditToolSetCondition()
{
	SVConditionalClass* pCondition = GetToolSetCondition();
	ASSERT(pCondition);
	SVToolSetAdjustmentDialogSheetClass dlg(GetInspectionID(), GetToolSet()->GetUniqueObjectID(), *pCondition, GetToolSet()->GetName());

	dlg.m_psh.dwFlags |= PSH_NOAPPLYNOW;

	INT_PTR dlgResult = dlg.DoModal();
	if (dlgResult == IDOK) { SetModifiedFlag(); }
}

void SVIPDoc::RebuildResultsList()
{
	SVResultListClass* pResultList = GetResultList();

	if (pResultList)
	{
		pResultList->Refresh(GetToolSet());
	}
}

bool SVIPDoc::checkOkToDelete(SVTaskObjectClass* pTaskObject)
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
	bool l_bWasRunMode = false;
	bool l_bWasTestMode = false;

	if (SVSVIMStateClass::CheckState(SV_STATE_REGRESSION))
	{  // already in regression mode, do nothing...
		return;
	}

	//check to see if in Run Mode, if so stop
	if (SVSVIMStateClass::CheckState(SV_STATE_RUNNING)) { l_bWasRunMode = true; }

	if (SVSVIMStateClass::CheckState(SV_STATE_TEST)) { l_bWasTestMode = true; }

	if (l_bWasRunMode || l_bWasTestMode) { TheSVObserverApp.OnStop(); }

	SVInspectionProcess* pInspection(GetInspectionProcess());

	if (nullptr == pInspection) { return; }

	if (!m_bRegressionTestInitEquationText)
	{
		double value;
		SvStl::MessageContainerVector errorMessages;
		m_pRegressionTestPlayEquationController->ValidateEquation(m_RegressionTestLoadEquationText, value, true, errorMessages);
		if (!errorMessages.empty())
		{
			SvStl::MessageMgrStd msg(SvStl::LogOnly);
			msg.setMessage(errorMessages[0].getMessage());
		}
		m_bRegressionTestInitEquationText = true;
	}

	if (pInspection->CanRegressionGoOnline())
	{
		SVImageArchiveClass svImageArchive;
		SVCameraImageTemplate* pMainImage = nullptr;

		bool l_bAllowAccess = false;

		if (l_bWasRunMode)
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
			if (SVSVIMStateClass::CheckState(SV_STATE_TEST))
			{
				SVSVIMStateClass::RemoveState(SV_STATE_TEST);
			}

			if (SVSVIMStateClass::CheckState(SV_STATE_EDIT))
			{
				SVSVIMStateClass::RemoveState(SV_STATE_EDIT);
			}

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
		SvStl::MessageMgrStd Msg(SvStl::LogAndDisplay);
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
			SVEnumerateValueObjectClass* pEnum = pSet->GetDrawFlagObject();
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
		SVEnumerateValueObjectClass* pEnum = pSet->GetDrawFlagObject();
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
			SVEnumerateValueObjectClass* pEnum = GetToolSet()->GetDrawFlagObject();

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

BOOL SVIPDoc::OnOpenDocument(LPCTSTR lpszPathName)
{
	if (lpszPathName)
	{
		msvFileName.SetFullFileName(lpszPathName);
	}
	else
	{
		msvFileName.SetFullFileName(GetPathName());
	}

	BOOL bOk = SVFileNameManagerClass::Instance().LoadItem(&msvFileName);

	if (bOk)
	{
		bOk = CDocument::OnOpenDocument(msvFileName.GetFullFileName().c_str());
	}

	return bOk;
}

BOOL SVIPDoc::OnSaveDocument(LPCTSTR lpszPathName)
{
	msvFileName.SetFullFileName(lpszPathName);
	if (0 != SvUl::CompareNoCase(msvFileName.GetPathName(), SVFileNameManagerClass::Instance().GetRunPathName()))
	{
		msvFileName.SetPathName(SVFileNameManagerClass::Instance().GetRunPathName().c_str());
	}

	if (0 != SvUl::CompareNoCase(msvFileName.GetExtension(), std::string(_T(".ipd"))))
	{
		msvFileName.SetExtension(_T(".ipd"));
	}

	BOOL bOk = CDocument::OnSaveDocument(msvFileName.GetFullFileName().c_str());
	if (bOk)
	{
		bOk = SVFileNameManagerClass::Instance().SaveItem(&msvFileName);

		if (bOk)
		{
			CDocument::SetPathName(msvFileName.GetFullFileName().c_str(), FALSE);
		}
	}

	return bOk;
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

SvTl::SVTimeStamp  SVIPDoc::getResultDefinitionUpdatTimeStamp() const
{
	SvTl::SVTimeStamp Result(0.0);
	SVResultListClass* pResultList = GetResultList();

	if (nullptr != pResultList)
	{
		Result = pResultList->getUpdateTimeStamp();
	}

	return Result;
}

HRESULT SVIPDoc::GetResultDefinitions(SVResultDefinitionDeque& p_rDefinitions) const
{
	HRESULT hres = E_FAIL;
	p_rDefinitions.clear();
	SVResultListClass* pResultList = GetResultList();
	if (nullptr != pResultList)
	{

		hres = pResultList->GetResultDefinitions(p_rDefinitions);

	}

	return hres;
}

HRESULT SVIPDoc::GetResultData(SVIPResultData& p_rResultData) const
{
	p_rResultData = m_Results;

	return S_OK;
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
	SVToolClass* pTool = new SVExternalTool();

	if (AddTool(pTool)) { return; }

	if (pTool) { delete(pTool); }
}

HRESULT SVIPDoc::RemoveImage(SVImageClass* pImage)
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
	publishList.Refresh((SVTaskObjectClass*)GetToolSet());

	SetModifiedFlag();
}

HRESULT SVIPDoc::GetCameras(SVVirtualCameraPtrSet& p_rCameras) const
{
	HRESULT l_Status = S_OK;

	if (nullptr != GetInspectionProcess())
	{
		l_Status = GetInspectionProcess()->GetCameras(p_rCameras);
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

HRESULT SVIPDoc::DeleteTool(SVTaskObjectClass* pTaskObject)
{
	ASSERT(pTaskObject);
	HRESULT l_hr = S_OK;

	SVInspectionProcess* pInspection(GetInspectionProcess());

	if (nullptr == pInspection)
	{
		return E_FAIL;
	}

	if (pTaskObject)
	{
		// Check for Dependents
		bool rc = checkOkToDelete(pTaskObject);

		if (rc)
		{
			CWaitCursor l_cwcMouse;

			// remove image from affected displays
			int i, j;
			SVImageViewClass *pImageView;
			SVOutObjectInfoStruct *pOutputObjectInfo;
			GUID ViewImageUniqueId;

			SVOutputInfoListClass l_OutputList;

			pTaskObject->GetOutputList(l_OutputList);

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

			SetSelectedToolID(SVGUID());

			SVDeleteTool l_DeleteTool(dynamic_cast<SVToolClass*>(pTaskObject));

			SVObjectManagerClass::Instance().UpdateObserver(GetInspectionID(), l_DeleteTool);

			SVConfigurationObject* pConfig(nullptr);
			SVObjectManagerClass::Instance().GetConfigurationObject(pConfig);
			if (nullptr != pConfig)
			{
				pConfig->ValidateRemoteMonitorList();
				TheSVObserverApp.GetIODoc()->UpdateAllViews(nullptr);
			}
			TheSVObserverApp.RebuildOutputList();
		}
		else
		{
			l_hr = S_FALSE;
		}
	}
	else
	{
		l_hr = S_FALSE;
	}

	return l_hr;
}

void SVIPDoc::OnEditAdjustToolPosition()
{
	SVToolClass* pTool = dynamic_cast<SVToolClass*>(SVObjectManagerClass::Instance().GetObject(GetSelectedToolID()));

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
			SVAdjustToolSizePositionDlg dlg(DlgName.c_str(), dynamic_cast<CWnd*>(this->GetMDIChild()), pTool);
			dlg.DoModal();
			SVSVIMStateClass::RemoveState(SV_STATE_EDITING);
		}
	}
}

void SVIPDoc::OnUpdateEditAdjustToolPosition(CCmdUI* pCmdUI)
{
	// Check current user access...
	bool Enabled = TheSVObserverApp.OkToEdit();

	if (Enabled)
	{
		Enabled = false;
		ToolSetView* pToolSetView = GetToolSetView();
		SVToolSetClass* pToolSet = GetToolSet();
		if (nullptr != pToolSet && nullptr != pToolSetView && !pToolSetView->IsLabelEditing())
		{
			ToolListSelectionInfo ToolListInfo = pToolSetView->GetToolListSelectionInfo();

			const SVGUID& rGuid = GetSelectedToolID();
			//Tool list active and valid tool
			if (!rGuid.empty() && -1 != ToolListInfo.m_listIndex)
			{
				SVToolClass* Tool = dynamic_cast<SVToolClass*>(SVObjectManagerClass::Instance().GetObject(rGuid));
				if (Tool)
				{
					//check to see if the tool has extents
					if (Tool->DoesObjectHaveExtents())
					{
						Enabled = true;
					}
				}
			}
		}
	}
	pCmdUI->Enable(Enabled);
}

void SVIPDoc::OnShowToolRelations()
{
	SVGUID selectedToolID = GetSelectedToolID();
	if (GUID_NULL != selectedToolID)
	{
		SVGuidSet DependencySet;
		DependencySet.insert(selectedToolID);
		SvOg::SVShowDependentsDialog Dlg(DependencySet, SvDef::SVToolObjectType, nullptr, SvOg::SVShowDependentsDialog::Show);
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
		SvOi::getToolDependency(std::back_inserter(m_dependencyList), ToolIDSet, SvDef::SVToolObjectType, SvOi::ToolDependencyEnum::Client, FileName);
	}
}

void SVIPDoc::OnUpdateToolDependencies(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(TheSVObserverApp.OkToEdit());
}

void SVIPDoc::OnUpdateAddGeneralTool(CCmdUI* PCmdUI)
{
	bool Enabled = TheSVObserverApp.OkToEdit() && isImageAvailable(SvDef::SVImageMonoType);

	PCmdUI->Enable(Enabled);
}

void SVIPDoc::OnUpdateAddCylindricalWarpTool(CCmdUI* pCmdUI)
{
	bool Enabled = !SVSVIMStateClass::CheckState(SV_STATE_RUNNING | SV_STATE_TEST);

	Enabled = Enabled && TheSVObserverApp.OkToEdit() && isImageAvailable(SvDef::SVImageMonoType);

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
	Enabled = Enabled && TheSVObserverApp.OkToEdit() && isImageAvailable(SvDef::SVImageMonoType);

	pCmdUI->Enable(Enabled);
}

void SVIPDoc::OnUpdateAddColorTool(CCmdUI* PCmdUI)
{
	bool Enabled = !SVSVIMStateClass::CheckState(SV_STATE_RUNNING | SV_STATE_TEST);
	// Check current user access...
	Enabled = Enabled && TheSVObserverApp.OkToEdit() && isImageAvailable(SvDef::SVImageColorType);

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
		Result = l_pToolSet->GetObjectNameToObjectType(SvDef::SVToolObjectType);
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
				SVImageClass* l_pImage = dynamic_cast<SVImageClass*>(SVObjectManagerClass::Instance().GetObject(l_RegisteredIter->first.ToGUID()));

				if ((nullptr != l_pImage))
				{
					SVBitmapInfo l_Info;

					BITMAPINFOHEADER l_Header = l_pImage->GetImageInfo().GetBitmapInfoHeader();

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

	SVImageClass* l_pImage = dynamic_cast<SVImageClass*>(SVObjectManagerClass::Instance().GetObject(p_rImageId.ToGUID()));

	if (nullptr != l_pImage)
	{
		SVImageInfoClass l_svImageInfo = l_pImage->GetImageInfo();

		BITMAPINFOHEADER l_Header = l_pImage->GetImageInfo().GetBitmapInfoHeader();

		p_rBitmapInfo.Assign(l_Header.biWidth, l_Header.biHeight, l_Header.biBitCount, SVBitmapInfo::GetDefaultColorTable(l_Header.biBitCount));
	}
	else
	{
		p_rBitmapInfo.clear();

		l_Status = E_FAIL;
	}

	return l_Status;
}

HRESULT SVIPDoc::GetImageData(const SVGUID& p_rImageId, SVByteVector& p_rImageData, SVExtentMultiLineStructVector& p_rMultiLineArray) const
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

HRESULT SVIPDoc::SetImageData(const SVGUID& p_rImageId, const SVByteVector& p_rImageData, const SVExtentMultiLineStructVector& p_rMultiLineArray)
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

HRESULT SVIPDoc::UpdateExtents(SVTaskObjectClass* p_pTask, const SVImageExtentClass& p_rExtents)
{
	HRESULT l_Status = SVGuiExtentUpdater::SetImageExtent(p_pTask, p_rExtents);
	if (S_OK == l_Status) { SetModifiedFlag(); }
	return l_Status;
}

HRESULT SVIPDoc::UpdateExtentsToFit(SVTaskObjectClass* p_pTask, const SVImageExtentClass& p_rExtents)
{
	HRESULT l_Status = SVGuiExtentUpdater::SetImageExtentToFit(p_pTask, p_rExtents);
	if (S_OK == l_Status) { SetModifiedFlag(); }
	return l_Status;
}

SVImageClass* SVIPDoc::GetImageByName(LPCTSTR ImageName) const
{
	SVImageClass* pImage = nullptr;
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

bool SVIPDoc::RunOnce(SVToolClass* pTool)
{
	SVInspectionProcess* pInspection = GetInspectionProcess();
	bool l_Status = (nullptr != pInspection);

	if (l_Status)
	{
		SvPB::InspectionRunOnceRequest requestMessage;

		SvPB::SetGuidInProtoBytes(requestMessage.mutable_inspectionid(), pInspection->GetUniqueObjectID());

		if (nullptr != pTool)
		{
			SvPB::SetGuidInProtoBytes(requestMessage.mutable_taskid(), pTool->GetUniqueObjectID());

		}

		l_Status = (S_OK == SvCmd::InspectionCommandsSynchronous(pInspection->GetUniqueObjectID(), &requestMessage, nullptr));
	}

	return l_Status;
}

int SVIPDoc::GetSelectedToolIndex(const std::string& rToolName) const
{
	int toolListIndex = 0;
	const SVToolSetClass* pToolSet = GetToolSet();
	if (nullptr != pToolSet)
	{
		toolListIndex = pToolSet->GetSize();
		bool bFound = false;
		// FindTool by Name in SVTaskObjectList
		for (int i = 0; i < pToolSet->GetSize() && !bFound; i++)
		{
			SVTaskObjectClass* pTool = pToolSet->GetAt(i);
			if (nullptr != pTool && pTool->GetName() == rToolName)
			{
				toolListIndex = i;
				bFound = true;
			}
		}
	}
	return toolListIndex;
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
					const SVTaskObjectClass* pTool = pToolSet->GetAt(i);
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

bool SVIPDoc::isImageAvailable(SvDef::SVObjectSubTypeEnum ImageSubType) const
{
	bool Result {false};

	typedef SvCmd::GetAllowedImageList Command;
	typedef std::shared_ptr<Command> CommandPtr;
	SvDef::SVObjectTypeInfoStruct ObjectInfo {SvDef::SVImageObjectType, ImageSubType};
	CommandPtr commandPtr {new Command(m_InspectionID, ObjectInfo, GetSelectedToolID())};
	SVObjectSynchronousCommandTemplate<CommandPtr> cmd(m_InspectionID, commandPtr);
	HRESULT hr = cmd.Execute(TWO_MINUTE_CMD_TIMEOUT);
	if (S_OK == hr)
	{
		Result = (0 < commandPtr->AvailableObjects().size()) ? true : false;
	}

	return Result;
}
