//******************************************************************************
//* COPYRIGHT (c) 2005 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVToolAdjustmentDialogSheetClass
//* .File Name       : $Workfile:   SVToolAdjustmentDialogSheetClass.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.7  $
//* .Check In Date   : $Date:   17 Jul 2014 20:51:50  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVToolAdjustmentDialogSheetClass.h"
#include "SVIPDoc.h"
#include "SVInspectionProcess.h"
#include "SVExternalToolDlg.h"
#include "SVOGui/SVPerspectiveWarpPage.h"
#include "SVTADlgAcquisitionSourcePage.h"
#include "SVTADlgArchiveImagePage.h"
#include "SVTADlgArchiveResultsPage.h"
#include "SVTADlgPolarTransformPage.h"
#include "SVTADlgRotationPage.h"
#include "SVTADlgStatisticsPage.h"
#include "SVTADlgThresholdPage.h"
#include "SVTADlgTransformationLearnPage.h"
#include "SVTADlgTranslationPage.h"
#include "SVTADlgTranslationResizePage.h"
#include "SVTADlgTranslationShiftPage.h"
#include "SVToolAdjustmentDialogAnalyzerPageClass.h"
#include "SVToolAdjustmentDialogMaskPageClass.h"
#include "Definitions/StringTypeDef.h"
#include "ObjectInterfaces/IDependencyManager.h"
#include "SVOGui/SVToolAdjustmentDialogCommentPage.h"
#include "SVOGui/SVToolAdjustmentDialogSizePage.h"
#include "SVOGui/ISVPropertyPageDialog.h"
#include "SVOGui/SVCylindricalWarpDlg.h"
#include "SvOGui/SVFormulaEditorPage.h"
#include "SVOGui/SVToolAdjustmentDialogFilterPageClass.h"
#include "SVOGui/SVTADlgColorToolPage.h"
#include "SVOGui/SVTADlgFileImageSourcePage.h"
#include "SvOGui/SVTADlgGeneralPage.h"
#include "SVOGui/SVTADlgPassFailPage.h"
#include "SVOGui/SVTADlgTransformImagePage.h"
#include "SVOGui/SVToolAdjustmentDialogImagePageClass.h"
#include "SVOGui/SVToolAdjustmentDialogLinearSpecialPage.h"
#include "SVOGui/SVToolAdjustmentDialogTwoImagePage.h"
#include "SVOGui/SVToolAdjustmentLUTPage.h"
#include "SVOGui/TADialogLoopToolParameterPage.h"
#include "SVOGui/TADialogRingBufferParameterPage.h"
#include "SVOGui/TADialogTableParameterPage.h"
#include "SVOGui/TADialogTableDefinesPage.h"
#include "SVOGui/TATableAnalyzerPage.h"
#include "SVOGui/TATableSourcePage.h"
#include "SVOGui/ToolAdjustToolSetPage.h"
#include "SVStatusLibrary/MessageManager.h"
#include "SVStatusLibrary/ErrorNumbers.h"
#include "SVStatusLibrary/MessageManager.h"
#include "Tools/SVTool.h"
#pragma endregion Includes

#pragma region Declarations
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

IMPLEMENT_DYNAMIC(SVToolAdjustmentDialogSheetClass, CPropertySheet)

BEGIN_MESSAGE_MAP(SVToolAdjustmentDialogSheetClass, CPropertySheet)
	//{{AFX_MSG_MAP(SVToolAdjustmentDialogSheetClass)
	ON_WM_DESTROY()
	ON_COMMAND(IDOK, OnOK)
	ON_WM_SYSCOMMAND()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

SVToolAdjustmentDialogSheetClass::SVToolAdjustmentDialogSheetClass(SVIPDoc* p_pIPDoc, const GUID& rInspectionID, const GUID& rTaskObjectID, UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
	: CPropertySheet(nIDCaption, pParentWnd, iSelectPage)
	, m_pIPDoc(p_pIPDoc)
	, m_InspectionID(rInspectionID)
	, m_TaskObjectID(rTaskObjectID)
{
	init();
}

SVToolAdjustmentDialogSheetClass::SVToolAdjustmentDialogSheetClass(SVIPDoc* p_pIPDoc, const GUID& rInspectionID, const GUID& rTaskObjectID, LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
	: CPropertySheet(pszCaption, pParentWnd, iSelectPage)
	, m_pIPDoc(p_pIPDoc)
	, m_InspectionID(rInspectionID)
	, m_TaskObjectID(rTaskObjectID)
{
	init();
}

void SVToolAdjustmentDialogSheetClass::init()
{
	//This will remove the APPLY Button from the Tool Adjustment Dialogs.
	m_psh.dwFlags |= PSH_NOAPPLYNOW;

	addPages();
}

SVToolAdjustmentDialogSheetClass::~SVToolAdjustmentDialogSheetClass()
{
	// Remove and delete all pages...
	for (int i = GetPageCount() - 1; i >= 0; --i)
	{
		CPropertyPage* pPage = GetPage(i);
		RemovePage(i);
		if (pPage)
		{
			delete pPage;
		}
	}
}

void SVToolAdjustmentDialogSheetClass::addPages()
{
	SvDef::SVObjectTypeInfoStruct ObjectInfo;
	ObjectInfo.ObjectType = SvPb::SVEquationObjectType;
	ObjectInfo.SubType = SvPb::SVConditionalObjectType;
	SvOi::IFormulaControllerPtr pFormularController {new SvOg::FormulaController(m_InspectionID, m_TaskObjectID, ObjectInfo)};
	UINT disableExtensionID = (SvPb::SVObjectTypeEnum::SVToolSetObjectType == GetTaskObject()->GetObjectType()) ? IDS_CLASSNAME_SVTOOLSET : IDS_TOOL_STRING;
	SvOg::SVFormulaEditorPageClass* pConditionalDlg = new SvOg::SVFormulaEditorPageClass {pFormularController, true, IDS_CONDITIONAL_STRING, disableExtensionID};

	SvOi::IObjectClass* pLUTEquation {nullptr};
	ObjectInfo.ObjectType = SvPb::SVUnaryImageOperatorObjectType;
	ObjectInfo.SubType = SvPb::SVLUTOperatorObjectType;
	SvOi::IObjectClass* pLUTOperator = GetTaskObject()->getFirstObject(ObjectInfo);
	if (nullptr != pLUTOperator)
	{
		ObjectInfo.ObjectType = SvPb::SVEquationObjectType;
		ObjectInfo.SubType = SvPb::SVLUTEquationObjectType;
		pLUTEquation = pLUTOperator->getFirstObject(ObjectInfo);
	}

	ObjectInfo.ObjectType = SvPb::SVUnaryImageOperatorObjectType;
	ObjectInfo.SubType = SvPb::SVUserMaskOperatorObjectType;
	SvOi::IObjectClass* pMaskOperator = GetTaskObject()->getFirstObject(ObjectInfo);

	ObjectInfo.ObjectType = SvPb::SVEquationObjectType;
	ObjectInfo.SubType = SvPb::SVMathEquationObjectType;
	SvOi::IObjectClass* pMathEquation = GetTaskObject()->getFirstObject(ObjectInfo);

	ObjectInfo.ObjectType = SvPb::SVTransformObjectType;
	ObjectInfo.SubType = SvPb::SVImageTransformObjectType;
	SvOi::IObjectClass* pImageTransform = GetTaskObject()->getFirstObject(ObjectInfo);

	bool bHasSize = false;
	ObjectInfo.ObjectType = SvPb::SVToolSizeAdjustTaskType;
	ObjectInfo.SubType = SvPb::SVNotSetSubObjectType;

	if (nullptr != GetTaskObject()->getFirstObject(ObjectInfo))
	{
		bHasSize = true;
	}

	switch (GetTaskObject()->GetObjectSubType())
	{
		case SvPb::SVObjectSubTypeEnum::SVToolAcquisitionObjectType:
			AddPage(new SVToolAdjustmentDialogAcquisitionSourcePageClass(m_InspectionID, m_TaskObjectID, this));
			// NOTE:
			// No Conditional Execution for Acquisition Tool !!!!
			delete pConditionalDlg;
			pConditionalDlg = nullptr;
			// NOTE:
			// No General page for acquisition tool!
			// AddPage( new SvOg::SVToolAdjustmentDialogGeneralPageClass( m_InspectionID, m_TaskObjectID ) );
			break;

		case SvPb::SVObjectSubTypeEnum::SVToolImageObjectType:	// Image Tool
			AddPage(new SvOg::SVToolAdjustmentDialogTwoImagePageClass(m_InspectionID, m_TaskObjectID));
			if (bHasSize)
			{
				AddPage(new SvOg::SVToolAdjustmentDialogSizePage(m_InspectionID, m_TaskObjectID));
			}
			// New image tool has also this pages...RO_22Mar2000
			if (nullptr != pLUTOperator && nullptr != pLUTEquation)
			{
				AddPage(new SvOg::SVToolAdjustmentDialogFilterPageClass(m_InspectionID, m_TaskObjectID));
				AddPage(new SVToolAdjustmentDialogThresholdPageClass(m_InspectionID, m_TaskObjectID, this));
				if (nullptr != pMaskOperator)
				{
					AddPage(new SVToolAdjustmentDialogMaskPageClass(m_InspectionID, m_TaskObjectID, pMaskOperator->GetUniqueObjectID()));
				}
				AddPage(new SvOg::SVToolAdjustmentLUTPage(m_InspectionID, pLUTOperator->GetUniqueObjectID(), pLUTEquation->GetUniqueObjectID()));
			}
			AddPage(pConditionalDlg);
			AddPage(new SvOg::SVToolAdjustmentDialogGeneralPageClass(m_InspectionID, m_TaskObjectID));
			break;

		case SvPb::SVObjectSubTypeEnum::SVWindowToolObjectType:
			AddPage(new SvOg::SVToolAdjustmentDialogImagePageClass(m_InspectionID, m_TaskObjectID));
			if (bHasSize)
			{
				AddPage(new SvOg::SVToolAdjustmentDialogSizePage(m_InspectionID, m_TaskObjectID));
			}
			AddPage(new SvOg::SVToolAdjustmentDialogFilterPageClass(m_InspectionID, m_TaskObjectID));
			AddPage(new SVToolAdjustmentDialogThresholdPageClass(m_InspectionID, m_TaskObjectID, this));
			if (nullptr != pMaskOperator)
			{
				AddPage(new SVToolAdjustmentDialogMaskPageClass(m_InspectionID, m_TaskObjectID, pMaskOperator->GetUniqueObjectID()));
			}
			if (nullptr != pLUTOperator && nullptr != pLUTEquation)
			{
				// To be compatible to old Window Tool...RO_22Mar2000
				AddPage(new SvOg::SVToolAdjustmentLUTPage(m_InspectionID, pLUTOperator->GetUniqueObjectID(), pLUTEquation->GetUniqueObjectID()));
			}
			AddPage(new SVToolAdjustmentDialogAnalyzerPageClass(m_InspectionID, m_TaskObjectID, this));
			AddPage(pConditionalDlg);
			AddPage(new SvOg::SVToolAdjustmentDialogGeneralPageClass(m_InspectionID, m_TaskObjectID));
			break;

		case SvPb::SVObjectSubTypeEnum::SVLinearToolObjectType:
			AddPage(new SvOg::SVToolAdjustmentDialogImagePageClass(m_InspectionID, m_TaskObjectID));
			if (bHasSize)
			{
				AddPage(new SvOg::SVToolAdjustmentDialogSizePage(m_InspectionID, m_TaskObjectID));
			}
			AddPage(new SvOg::SVToolAdjustmentDialogFilterPageClass(m_InspectionID, m_TaskObjectID));
			AddPage(new SVToolAdjustmentDialogThresholdPageClass(m_InspectionID, m_TaskObjectID, this));
			AddPage(new SVToolAdjustmentDialogAnalyzerPageClass(m_InspectionID, m_TaskObjectID, this));
			//add the new page.
			AddPage(new SvOg::SVToolAdjustmentDialogLinearSpecialPage(m_InspectionID, m_TaskObjectID));
			AddPage(pConditionalDlg);
			AddPage(new SvOg::SVToolAdjustmentDialogGeneralPageClass(m_InspectionID, m_TaskObjectID));
			break;

		case SvPb::SVObjectSubTypeEnum::SVToolLoadImageObjectType:
			AddPage(new SvOg::SVToolAdjustmentDialogFileImageSourcePageClass(m_InspectionID, m_TaskObjectID));
			if (bHasSize)
			{
				AddPage(new SvOg::SVToolAdjustmentDialogSizePage(m_InspectionID, m_TaskObjectID));
			}
			AddPage(pConditionalDlg);
			AddPage(new SvOg::SVToolAdjustmentDialogGeneralPageClass(m_InspectionID, m_TaskObjectID));
			break;

		case SvPb::SVObjectSubTypeEnum::SVToolArchiveObjectType:   // Archive tool
			AddPage(new SVTADlgArchiveResultsPage(m_InspectionID, m_TaskObjectID, this));
			AddPage(new SVTADlgArchiveImagePage(m_InspectionID, m_TaskObjectID, this));
			AddPage(pConditionalDlg);
			break;

		case SvPb::SVObjectSubTypeEnum::SVMathToolObjectType:
		{
			if (nullptr != pMathEquation)
			{
				SvOi::IFormulaControllerPtr pFormularController {new SvOg::FormulaController(m_InspectionID, m_TaskObjectID, pMathEquation->GetUniqueObjectID())};
				AddPage(new SvOg::SVFormulaEditorPageClass(pFormularController, false));
			}
			AddPage(new SvOg::SVToolAdjustmentDialogPassFailPageClass(m_InspectionID, m_TaskObjectID));
			AddPage(pConditionalDlg);
		}
		break;

		case SvPb::SVObjectSubTypeEnum::SVStatisticsToolObjectType:
			AddPage(new SVToolAdjustmentDialogStatisticsPageClass(m_InspectionID, m_TaskObjectID, this));
			AddPage(pConditionalDlg);
			break;

		case SvPb::SVObjectSubTypeEnum::SVShiftToolObjectType:
			AddPage(new SvOg::SVToolAdjustmentDialogImagePageClass(m_InspectionID, m_TaskObjectID, SvPb::SVNotSetSubObjectType)); 
			//@WARNING [Arvid][12.12.2019]: for enabling colour images for this tool, SVNotSetSubObjectType is used here instead of
			// SVImageMonoType to indicate that color _or_ monochrome images are acceptable here.
			// However, it would be better if there was a way to specify "any type of image" instead of "any type of objects",
			// i.e. something like 'SVImageMonoOrColorType'. Maybe a new type of enum is needed for this, since SVObjectSubTypeEnum
			// is meant to describe properties of individual images (which of course are always either 'mono' or 'color')

			if (bHasSize)
			{
				AddPage(new SvOg::SVToolAdjustmentDialogSizePage(m_InspectionID, m_TaskObjectID));
			}
			AddPage(new SVTADlgTranslationShiftPageClass(m_InspectionID, m_TaskObjectID, this));
			AddPage(pConditionalDlg);
			AddPage(new SvOg::SVToolAdjustmentDialogGeneralPageClass(m_InspectionID, m_TaskObjectID));
			break;

		case SvPb::SVObjectSubTypeEnum::SVResizeToolObjectType:
			AddPage(new SvOg::SVToolAdjustmentDialogImagePageClass(m_InspectionID, m_TaskObjectID));
			if (bHasSize)
			{
				AddPage(new SvOg::SVToolAdjustmentDialogSizePage(m_InspectionID, m_TaskObjectID));
			}
			AddPage(new SVTADlgTranslationResizePage(m_InspectionID, m_TaskObjectID, this));
			AddPage(pConditionalDlg);
			AddPage(new SvOg::SVToolAdjustmentDialogGeneralPageClass(m_InspectionID, m_TaskObjectID));
			break;

		case SvPb::SVObjectSubTypeEnum::SVTransformationToolObjectType:
			if (nullptr != pImageTransform)
			{
				AddPage(new SvOg::SVToolAdjustmentDialogTransformImagePageClass(m_InspectionID, m_TaskObjectID, pImageTransform->GetUniqueObjectID()));
			}
			AddPage(new SVToolAdjustmentDialogTranslationPageClass(m_InspectionID, m_TaskObjectID));
			AddPage(new SVToolAdjustmentDialogRotationPageClass(m_InspectionID, m_TaskObjectID));
			AddPage(new SVToolAdjustmentDialogTransformationLearnPageClass(m_InspectionID, m_TaskObjectID, this));
			AddPage(pConditionalDlg);
			AddPage(new SvOg::SVToolAdjustmentDialogGeneralPageClass(m_InspectionID, m_TaskObjectID));
			break;

		case SvPb::SVObjectSubTypeEnum::SVPolarTransformationToolObjectType:
			AddPage(new SvOg::SVToolAdjustmentDialogImagePageClass(m_InspectionID, m_TaskObjectID));
			AddPage(new SVToolAdjustmentDialogPolarTransformPageClass(m_InspectionID, m_TaskObjectID, this));
			AddPage(pConditionalDlg);
			AddPage(new SvOg::SVToolAdjustmentDialogGeneralPageClass(m_InspectionID, m_TaskObjectID));
			break;

		case SvPb::SVObjectSubTypeEnum::SVColorToolObjectType:
			AddPage(new SvOg::SVTADlgColorToolPage(m_InspectionID, m_TaskObjectID));
			if (bHasSize)
			{
				AddPage(new SvOg::SVToolAdjustmentDialogSizePage(m_InspectionID, m_TaskObjectID));
			}
			AddPage(pConditionalDlg);
			AddPage(new SvOg::SVToolAdjustmentDialogGeneralPageClass(m_InspectionID, m_TaskObjectID));
			break;

		case SvPb::SVObjectSubTypeEnum::SVToolCylindricalObjectType:
			AddPage(new SvOg::SVToolAdjustmentDialogImagePageClass(m_InspectionID, m_TaskObjectID));
			AddPage(new SvOg::SVCylindricalWarpDlg(m_InspectionID, m_TaskObjectID));
			AddPage(pConditionalDlg);
			AddPage(new SvOg::SVToolAdjustmentDialogGeneralPageClass(m_InspectionID, m_TaskObjectID));
			break;

		case SvPb::SVObjectSubTypeEnum::SVPerspectiveToolObjectType:
			AddPage(new SvOg::SVToolAdjustmentDialogImagePageClass(m_InspectionID, m_TaskObjectID));
			AddPage(new SvOg::SVPerspectiveWarpPage(m_InspectionID, m_TaskObjectID));
			AddPage(pConditionalDlg);
			AddPage(new SvOg::SVToolAdjustmentDialogGeneralPageClass(m_InspectionID, m_TaskObjectID));
			break;

		case SvPb::SVObjectSubTypeEnum::SVExternalToolObjectType:
			AddPage(new SVExternalToolDlg(m_InspectionID, m_TaskObjectID, this));
			AddPage(pConditionalDlg);
			AddPage(new SvOg::SVToolAdjustmentDialogGeneralPageClass(m_InspectionID, m_TaskObjectID));
			break;


		case SvPb::SVObjectSubTypeEnum::SVRingBufferToolObjectType:
			// When the Object sub type is not set, then both mono and color images are placed into the selection list
			AddPage(new SvOg::SVToolAdjustmentDialogImagePageClass(m_InspectionID, m_TaskObjectID, SvPb::SVNotSetSubObjectType));
			AddPage(new SvOg::TADialogRingBufferParameterPage(m_InspectionID, m_TaskObjectID));
			AddPage(pConditionalDlg);
			break;

		case SvPb::SVObjectSubTypeEnum::SVTableToolObjectType:
			AddPage(new SvOg::TADialogTableParameterPage(m_InspectionID, m_TaskObjectID));
			AddPage(new SvOg::TADialogTableDefinesPage(m_InspectionID, m_TaskObjectID));
			AddPage(pConditionalDlg);
			break;

		case SvPb::SVObjectSubTypeEnum::SVTableAnalyzerToolObjectType:
			AddPage(new SvOg::TATableSourcePage(m_InspectionID, m_TaskObjectID));
			AddPage(new SvOg::TaTableAnalyzerPage(m_InspectionID, m_TaskObjectID));
			AddPage(new SvOg::SVToolAdjustmentDialogPassFailPageClass(m_InspectionID, m_TaskObjectID, IDS_TABNAME_TABLE_ROWRANGE));
			AddPage(pConditionalDlg);
			break;

		case SvPb::SVObjectSubTypeEnum::LoopToolObjectType:
			AddPage(new SvOg::TADialogLoopToolParameterPage(m_InspectionID, m_TaskObjectID));
			AddPage(pConditionalDlg);
			break;

		case SvPb::SVObjectSubTypeEnum::SVNotSetSubObjectType:
			//Special case for the Tool Set parameters
			if(SvPb::SVObjectTypeEnum::SVToolSetObjectType == GetTaskObject()->GetObjectType())
			{
				AddPage(new SvOg::ToolAdjustToolSetPage(m_InspectionID, m_TaskObjectID));
				AddPage(pConditionalDlg);
			}
			else
			{
				delete pConditionalDlg;
				pConditionalDlg = nullptr;
			}
			break;

		default:
			SvStl::MessageMgrStd Msg(SvStl::MsgType::Log | SvStl::MsgType::Display);
			Msg.setMessage(SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_Error_CannotOpenTADlg, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_10218);
			delete pConditionalDlg;
			pConditionalDlg = nullptr;
			break;
	}

	if (SvPb::SVObjectTypeEnum::SVToolObjectType == GetTaskObject()->GetObjectType())
	{
		AddPage(new SvOg::SVToolAdjustmentDialogCommentPage(m_InspectionID, m_TaskObjectID));
	}
}

BOOL SVToolAdjustmentDialogSheetClass::OnInitDialog()
{
	BOOL bResult = CPropertySheet::OnInitDialog();

	// Disable and Hide Cancel Button
	HWND hWnd = ::GetDlgItem(m_hWnd, IDCANCEL);
	if (nullptr != hWnd)
	{
		::EnableWindow(hWnd, FALSE);
		::ShowWindow(hWnd, SW_HIDE);
	}
	// Remove Close Button
	ModifyStyle(WS_SYSMENU, 0, SWP_FRAMECHANGED);

	// Send Errors to the display
	SVObjectClass* pTaskObject = dynamic_cast<SVObjectClass*> (GetTaskObject());

	if(nullptr != pTaskObject)
	{
		CString l_Temp = _T("Tool Adjustment: ");
		l_Temp += pTaskObject->GetObjectName();
		l_Temp += _T(" - ");
		l_Temp += pTaskObject->GetName();
		SetWindowText(l_Temp);
	}
	return bResult;
}

void SVToolAdjustmentDialogSheetClass::OnDestroy()
{
	// reset Flag so errors do not go to the display.


	CPropertySheet::OnDestroy();

	// Don´t manipulate currentOperatorList at this point 
	// ( e.g. currentOperatorList.RemoveAll(); ) !!!
	// The right currentOperatorList destructor will be called in
	// ~SVToolAdjustmentDialogSheetClass() !!!
}

void SVToolAdjustmentDialogSheetClass::OnOK()
{
	CWaitCursor l_cwcMouse;

	// Try to validate the Equations
	int cnt = GetPageCount();

	for (int i = 0; i < cnt; i++)
	{
		CPropertyPage* pPage = GetPage(i);
		if (pPage && pPage->GetSafeHwnd())
		{
			if (SvOg::SVFormulaEditorPageClass* l_pFormulaEditor = dynamic_cast <SvOg::SVFormulaEditorPageClass*> (pPage))
			{
				if (!l_pFormulaEditor->validateAndSetEquation())
				{
					// Equation must be valid or disabled
					SvStl::MessageMgrStd Msg(SvStl::MsgType::Log | SvStl::MsgType::Display);
					Msg.setMessage(SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_Error_InvalidFormula, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_10219);
					return;
				}
			}
			else
			{
				if (SvOg::ISVPropertyPageDialog* pIDlg = dynamic_cast <SvOg::ISVPropertyPageDialog*> (pPage))
				{
					if (pIDlg->QueryAllowExit() == false)	// exit not allowed
					{
						return;
					}
				}
				if (CPropertyPage* l_pPropertyPage = dynamic_cast <CPropertyPage*> (pPage))
				{
					l_pPropertyPage->OnOK();
				}
			}
		}
	}

	SvOi::IObjectClass* pObject = GetTaskObject();
	bool resetResult = false;
	if (nullptr != pObject)
	{
		resetResult = pObject->resetAllObjects();
		//Reset all tools dependent on this tool
		SVGuidSet ToolSet;
		ToolSet.insert(pObject->GetUniqueObjectID());
		SVGuidSet ToolDependencySet;
		SvOi::getToolDependency(std::inserter(ToolDependencySet, ToolDependencySet.end()), ToolSet);
		for (auto const& rEntry : ToolDependencySet)
		{
			SvOi::IObjectClass* pTool = SvOi::getObject(rEntry);
			pTool->resetAllObjects();
		}
	}

	if (resetResult)
	{
		SVIPDoc* pDocument = GetIPDoc();
		if (pDocument)
		{
			pDocument->SetModifiedFlag();
			pDocument->RunOnce();
		}

		EndDialog(IDOK);
	}
	else
	{
		SvOi::ITaskObject* pTaskObject = dynamic_cast<SvOi::ITaskObject*> (pObject);
		if(nullptr != pTaskObject)
		{
			SvStl::MessageContainer message = pTaskObject->getFirstTaskMessage();
			SvStl::MessageTextEnum textEnum = SvStl::Tid_Empty;
			SvDef::StringVector textList;
			if (0 != message.getMessage().m_MessageCode)
			{
				textEnum = message.getMessage().m_AdditionalTextId;
				textList = message.getMessage().m_AdditionalTextList;
			}
			SvStl::MessageMgrStd messageMgr(SvStl::MsgType::Log | SvStl::MsgType::Display);
			INT_PTR result = messageMgr.setMessage(SVMSG_SVO_104_TA_DIALOG_CLOSING_ERROR, textEnum, textList, SvStl::SourceFileParams(StdMessageParams), 0, GetTaskObjectID(), MB_YESNO);
			if (IDYES == result)
			{
				EndDialog(IDOK);
			}
		}
	}
}

void SVToolAdjustmentDialogSheetClass::OnCancel()
{
	EndDialog(IDCANCEL);
}

void SVToolAdjustmentDialogSheetClass::OnSysCommand(UINT nID, LPARAM lParam)
{
	switch (nID & 0xFFF0)
	{
		// Ignore the close command
		case SC_CLOSE:
			return;
	}

	CPropertySheet::OnSysCommand(nID, lParam);
}

SVIPDoc* SVToolAdjustmentDialogSheetClass::GetIPDoc() const
{
	return m_pIPDoc;
}

SvOi::IObjectClass* SVToolAdjustmentDialogSheetClass::GetTaskObject() const
{
	return (SvOi::getObject(m_TaskObjectID));
}
