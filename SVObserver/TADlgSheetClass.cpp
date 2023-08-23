//******************************************************************************
//* COPYRIGHT (c) 2005 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : TADlgSheetClass
//* .File Name       : $Workfile:   TADlgSheetClass.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.7  $
//* .Check In Date   : $Date:   17 Jul 2014 20:51:50  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "TADlgSheetClass.h"
#include "SVIPDoc.h"
#include "TADlgAcquisitionSourcePage.h"
#include "TADlgArchiveImagePage.h"
#include "TADlgArchiveResultsPage.h"
#include "TADlgPolarTransformPage.h"
#include "TADlgRotationPage.h"
#include "TADlgStatisticsPage.h"
#include "TADlgThresholdPage.h"
#include "TADlgTransformationLearnPage.h"
#include "TADlgTranslationPage.h"
#include "TADlgTranslationShiftPage.h"
#include "TADlgAnalyzerPageClass.h"
#include "TADlgMaskPageClass.h"
#include "TADlgLinearResultRangesPage.h"
#include "Definitions/StringTypeDef.h"
#include "SVOGui/TADlgCommentPage.h"
#include "SVOGui/TADlgResizePage.h"
#include "SVOGui/TADlgSizePage.h"
#include "SVOGui/ISVPropertyPageDialog.h"
#include "SVOGui/TADlgCylindricalWarp.h"
#include "SVOGuiUtility/SVFormulaEditorPage.h"
#include "SVOGui/TADlgFilterPageClass.h"
#include "SVOGui/TADlgColorToolPage.h"
#include "SVOGui/TADlgFileImageSourcePage.h"
#include "SvOGui/TADlgGeneralPage.h"
#include "SVOGui/TADlgPassFailPage.h"
#include "SVOGui/TADlgPerspectiveWarpPage.h"
#include "SVOGui/TADlgTransformImagePage.h"
#include "SVOGui/TADlgImagePageClass.h"
#include "SVOGui/TADlgLinearSpecialPage.h"
#include "SVOGui/TADlgTwoImagePage.h"
#include "SVOGui/TADlgLUTPage.h"
#include "SVOGui/TADlgGroupToolInputPage.h"
#include "SVOGui/TADlgLoopToolParameterPage.h"
#include "SVOGui/TADlgRingBufferParameterPage.h"
#include "SVOGui/TADlgTableParameterPage.h"
#include "SVOGui/TADlgTableDefinesPage.h"
#include "SVOGui/TADlgTableAnalyzerPage.h"
#include "SVOGui/TADlgDrawPage.h"
#include "SVOGui/TADlgTableSourcePage.h"
#include "SVOGui/TADlgGroupToolResultPage.h"
#include "SVStatusLibrary/MessageManager.h"
#include "SVStatusLibrary/SvimState.h"
#include "SVUtilityLibrary/VectorHelper.h"
#include "Tools/ModuleTool.h"


#pragma endregion Includes

#pragma region Declarations
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations


BEGIN_MESSAGE_MAP(TADlgSheetClass, SvMc::ResizablePropertySheet)
	//{{AFX_MSG_MAP(TADlgSheetClass)
	ON_WM_DESTROY()
	ON_COMMAND(IDOK, OnOK)

	ON_WM_SYSCOMMAND()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

TADlgSheetClass::TADlgSheetClass(SVIPDoc* p_pIPDoc, uint32_t inspectionID, uint32_t taskObjectID, LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
	: SvMc::ResizablePropertySheet(pszCaption, pParentWnd, iSelectPage)
	, m_pIPDoc(p_pIPDoc)
	, m_InspectionID(inspectionID)
	, m_TaskObjectID(taskObjectID)
{}

void TADlgSheetClass::init()
{
	//This will remove the APPLY Button from the Tool Adjustment Dialogs.
	m_psh.dwFlags |= PSH_NOAPPLYNOW;

	addPages();

	SvOi::IObjectClass* pObject = GetTaskObject();
	auto objectType = (nullptr != pObject) ? pObject->GetObjectType() : SvPb::SVNotSetObjectType;
	Log_Assert(objectType == SvPb::SVToolObjectType || objectType == SvPb::SVToolSetObjectType);

	m_dependentTools.clear();

	std::back_insert_iterator<std::vector<uint32_t>>  InsertIt(m_dependentTools);
	SvTo::InsertDependentTools(InsertIt, pObject->getObjectId());
	SvUl::RemoveDuplicates(m_dependentTools);
}

TADlgSheetClass::~TADlgSheetClass()
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


void TADlgSheetClass::addConditionalDialog()
{
	SvDef::SVObjectTypeInfoStruct ObjectInfo;
	ObjectInfo.m_ObjectType = SvPb::SVEquationObjectType;
	ObjectInfo.m_SubType = SvPb::SVConditionalObjectType;
	SvOi::IFormulaControllerPtr pFormulaController {new SvOgu::FormulaController(m_InspectionID, m_TaskObjectID, ObjectInfo)};
	UINT disableExtensionID = (SvPb::SVObjectTypeEnum::SVToolSetObjectType == GetTaskObject()->GetObjectType()) ? IDS_CLASSNAME_SVTOOLSET : IDS_TOOL_STRING;

	AddPage(new SvOgu::SVFormulaEditorPageClass {pFormulaController, true, IDS_CONDITIONAL_STRING, disableExtensionID});
}


CPropertyPage* TADlgSheetClass::createToolAdjustmentDialogCommentPage()
{
	return new SvOg::TADlgCommentPage(m_InspectionID, m_TaskObjectID);
}


void TADlgSheetClass::AddAdditionalPagesForExternalTool()
{
	addConditionalDialog();
	AddPage(new SvOg::TADlgGeneralPage(m_InspectionID, m_TaskObjectID, SvPb::SVObjectSubTypeEnum::SVExternalToolObjectType));
	AddPage(createToolAdjustmentDialogCommentPage());
}


void TADlgSheetClass::addPages()
{
	SvDef::SVObjectTypeInfoStruct ObjectInfo;

	SvOi::IObjectClass* pLUTEquation {nullptr};
	ObjectInfo.m_ObjectType = SvPb::SVUnaryImageOperatorObjectType;
	ObjectInfo.m_SubType = SvPb::SVLUTOperatorObjectType;
	SvOi::IObjectClass* pTaskObject = GetTaskObject();
	Log_Assert(pTaskObject);
	if (nullptr == pTaskObject)
	{
		return;
	}
	SvOi::IObjectClass* pLUTOperator = pTaskObject->getFirstObject(ObjectInfo);
	if (nullptr != pLUTOperator)
	{
		ObjectInfo.m_ObjectType = SvPb::SVEquationObjectType;
		ObjectInfo.m_SubType = SvPb::SVLUTEquationObjectType;
		pLUTEquation = pLUTOperator->getFirstObject(ObjectInfo);
	}

	ObjectInfo.m_ObjectType = SvPb::SVUnaryImageOperatorObjectType;
	ObjectInfo.m_SubType = SvPb::SVUserMaskOperatorObjectType;
	SvOi::IObjectClass* pMaskOperator = pTaskObject->getFirstObject(ObjectInfo);

	ObjectInfo.m_ObjectType = SvPb::SVEquationObjectType;
	ObjectInfo.m_SubType = SvPb::SVMathEquationObjectType;
	SvOi::IObjectClass* pMathEquation = pTaskObject->getFirstObject(ObjectInfo);

	ObjectInfo.m_ObjectType = SvPb::SVTransformObjectType;
	ObjectInfo.m_SubType = SvPb::SVImageTransformObjectType;
	SvOi::IObjectClass* pImageTransform = pTaskObject->getFirstObject(ObjectInfo);

	bool bHasSize = false;
	ObjectInfo.m_ObjectType = SvPb::SVToolSizeAdjustTaskType;
	ObjectInfo.m_SubType = SvPb::SVNotSetSubObjectType;

	if (nullptr != pTaskObject->getFirstObject(ObjectInfo))
	{
		bHasSize = true;
	}

	const auto subType = pTaskObject->GetObjectSubType();
	switch (subType)
	{
		case SvPb::SVObjectSubTypeEnum::SVToolAcquisitionObjectType:
			AddPage(new TADlgAcquisitionSourcePage(m_InspectionID, m_TaskObjectID, this));
			// NOTE: No Conditional Execution for Acquisition Tool!
			// addConditionalDialog();
			// NOTE: No General page for acquisition tool!
			// AddPage( new SvOg::TADlgGeneralPage( m_InspectionID, m_TaskObjectID, subType) ) );
			break;

		case SvPb::SVObjectSubTypeEnum::SVToolImageObjectType:	// Image Tool
			AddPage(new SvOg::TADlgTwoImagePageClass(m_InspectionID, m_TaskObjectID));
			if (bHasSize)
			{
				AddPage(new SvOg::TADlgSizePage(m_InspectionID, m_TaskObjectID));
			}
			// New image tool has also this pages...RO_22Mar2000
			if (nullptr != pLUTOperator && nullptr != pLUTEquation)
			{
				AddPage(new SvOg::TADlgFilterPageClass(m_InspectionID, m_TaskObjectID));
				AddPage(new TADlgThresholdPage(m_InspectionID, m_TaskObjectID, this));
				if (nullptr != pMaskOperator)
				{
					AddPage(new TADlgMaskPageClass(m_InspectionID, m_TaskObjectID, pMaskOperator->getObjectId()));
				}
				AddPage(new SvOg::TADlgLUTPage(m_InspectionID, pLUTOperator->getObjectId(), pLUTEquation->getObjectId()));
			}
			addConditionalDialog();
			AddPage(new SvOg::TADlgGeneralPage(m_InspectionID, m_TaskObjectID, subType));
			break;

		case SvPb::SVObjectSubTypeEnum::SVWindowToolObjectType:
			AddPage(new SvOg::TADlgImagePageClass(m_InspectionID, m_TaskObjectID));
			if (bHasSize)
			{
				AddPage(new SvOg::TADlgSizePage(m_InspectionID, m_TaskObjectID));
			}
			AddPage(new SvOg::TADlgFilterPageClass(m_InspectionID, m_TaskObjectID));
			AddPage(new TADlgThresholdPage(m_InspectionID, m_TaskObjectID, this));
			if (nullptr != pMaskOperator)
			{
				AddPage(new TADlgMaskPageClass(m_InspectionID, m_TaskObjectID, pMaskOperator->getObjectId()));
			}
			if (nullptr != pLUTOperator && nullptr != pLUTEquation)
			{
				// To be compatible to old Window Tool...RO_22Mar2000
				AddPage(new SvOg::TADlgLUTPage(m_InspectionID, pLUTOperator->getObjectId(), pLUTEquation->getObjectId()));
			}
			AddPage(new TADlgAnalyzerPageClass(m_InspectionID, m_TaskObjectID, this));
			addConditionalDialog();
			AddPage(new SvOg::TADlgGeneralPage(m_InspectionID, m_TaskObjectID, subType));
			break;

		case SvPb::SVObjectSubTypeEnum::SVLinearToolObjectType:
			AddPage(new SvOg::TADlgImagePageClass(m_InspectionID, m_TaskObjectID));
			if (bHasSize)
			{
				AddPage(new SvOg::TADlgSizePage(m_InspectionID, m_TaskObjectID));
			}
			AddPage(new SvOg::TADlgFilterPageClass(m_InspectionID, m_TaskObjectID));
			AddPage(new TADlgThresholdPage(m_InspectionID, m_TaskObjectID, this));
			AddPage(new TADlgAnalyzerPageClass(m_InspectionID, m_TaskObjectID, this));
			//add the new page.
			AddPage(new SvOg::TADlgLinearSpecialPage(m_InspectionID, m_TaskObjectID));
			AddPage(new TADlgLinearResultRangesPage(m_InspectionID, m_TaskObjectID));
			addConditionalDialog();
			AddPage(new SvOg::TADlgGeneralPage(m_InspectionID, m_TaskObjectID, subType));
			break;

		case SvPb::SVObjectSubTypeEnum::SVToolLoadImageObjectType:
			AddPage(new SvOg::TADlgFileImageSourcePage(m_InspectionID, m_TaskObjectID));
			if (bHasSize)
			{
				AddPage(new SvOg::TADlgSizePage(m_InspectionID, m_TaskObjectID));
			}
			addConditionalDialog();
			AddPage(new SvOg::TADlgGeneralPage(m_InspectionID, m_TaskObjectID, subType));
			break;

		case SvPb::SVObjectSubTypeEnum::SVToolArchiveObjectType:   // Archive tool
		{
			AddPage(new TADlgArchiveResultsPage(m_InspectionID, m_TaskObjectID, this));
			AddPage(new TADlgArchiveImagePage(m_InspectionID, m_TaskObjectID, this));
			addConditionalDialog();
		}
		break;
		case SvPb::SVObjectSubTypeEnum::SVMathToolObjectType:
		{
			if (nullptr != pMathEquation)
			{
				SvOi::IFormulaControllerPtr pFormulaController2 {new SvOgu::FormulaController(m_InspectionID, m_TaskObjectID, pMathEquation->getObjectId())};
				AddPage(new SvOgu::SVFormulaEditorPageClass(pFormulaController2, false));
			}
			AddPage(new SvOg::TADlgPassFailPage(m_InspectionID, m_TaskObjectID));
			addConditionalDialog();
		}
		break;

		case SvPb::SVObjectSubTypeEnum::SVStatisticsToolObjectType:
			AddPage(new TADlgStatisticsPage(m_InspectionID, m_TaskObjectID));
			addConditionalDialog();
			break;

		case SvPb::SVObjectSubTypeEnum::SVShiftToolObjectType:
			AddPage(new SvOg::TADlgImagePageClass(m_InspectionID, m_TaskObjectID, SvPb::SVNotSetSubObjectType));
			//@WARNING [Arvid][12.12.2019]: for enabling colour images for this tool, SVNotSetSubObjectType is used here instead of
			// SVImageMonoType to indicate that color _or_ monochrome images are acceptable here.
			// However, it would be better if there was a way to specify "any type of image" instead of "any type of objects",
			// i.e. something like 'SVImageMonoOrColorType'. Maybe a new type of enum is needed for this, since SVObjectSubTypeEnum
			// is meant to describe properties of individual images (which of course are always either 'mono' or 'color')

			if (bHasSize)
			{
				AddPage(new SvOg::TADlgSizePage(m_InspectionID, m_TaskObjectID));
			}
			AddPage(new TADlgTranslationShiftPage(m_InspectionID, m_TaskObjectID, this));
			addConditionalDialog();
			AddPage(new SvOg::TADlgGeneralPage(m_InspectionID, m_TaskObjectID, subType));
			break;

		case SvPb::SVObjectSubTypeEnum::SVResizeToolObjectType:
			AddPage(new SvOg::TADlgImagePageClass(m_InspectionID, m_TaskObjectID));
			if (bHasSize)
			{
				AddPage(new SvOg::TADlgSizePage(m_InspectionID, m_TaskObjectID));
			}
			AddPage(new TADlgResizePage(m_InspectionID, m_TaskObjectID, this));
			addConditionalDialog();
			AddPage(new SvOg::TADlgGeneralPage(m_InspectionID, m_TaskObjectID, subType));
			break;

		case SvPb::SVObjectSubTypeEnum::SVTransformationToolObjectType:
			if (nullptr != pImageTransform)
			{
				AddPage(new SvOg::TADlgTransformImagePage(m_InspectionID, m_TaskObjectID, pImageTransform->getObjectId()));
			}
			AddPage(new TADlgTranslationPage(m_InspectionID, m_TaskObjectID));
			AddPage(new TADlgRotationPage(m_InspectionID, m_TaskObjectID));
			AddPage(new TADlgTransformationLearnPage(m_InspectionID, m_TaskObjectID, this));
			addConditionalDialog();
			AddPage(new SvOg::TADlgGeneralPage(m_InspectionID, m_TaskObjectID, subType));
			break;

		case SvPb::SVObjectSubTypeEnum::SVPolarTransformationToolObjectType:
			AddPage(new SvOg::TADlgImagePageClass(m_InspectionID, m_TaskObjectID));
			AddPage(new TADlgPolarTransformPage(m_InspectionID, m_TaskObjectID, this));
			addConditionalDialog();
			AddPage(new SvOg::TADlgGeneralPage(m_InspectionID, m_TaskObjectID, subType));
			break;

		case SvPb::SVObjectSubTypeEnum::SVColorToolObjectType:
			AddPage(new SvOg::TADlgColorToolPage(m_InspectionID, m_TaskObjectID));
			if (bHasSize)
			{
				AddPage(new SvOg::TADlgSizePage(m_InspectionID, m_TaskObjectID));
			}
			addConditionalDialog();
			AddPage(new SvOg::TADlgGeneralPage(m_InspectionID, m_TaskObjectID, subType));
			break;

		case SvPb::SVObjectSubTypeEnum::SVToolCylindricalObjectType:
			AddPage(new SvOg::TADlgImagePageClass(m_InspectionID, m_TaskObjectID));
			AddPage(new SvOg::TADlgCylindricalWarp(m_InspectionID, m_TaskObjectID));
			addConditionalDialog();
			AddPage(new SvOg::TADlgGeneralPage(m_InspectionID, m_TaskObjectID, subType));
			break;

		case SvPb::SVObjectSubTypeEnum::SVPerspectiveToolObjectType:
			AddPage(new SvOg::TADlgImagePageClass(m_InspectionID, m_TaskObjectID));
			AddPage(new SvOg::TADlgPerspectiveWarpPage(m_InspectionID, m_TaskObjectID));
			addConditionalDialog();
			AddPage(new SvOg::TADlgGeneralPage(m_InspectionID, m_TaskObjectID, subType));
			break;

			//		case SvPb::SVObjectSubTypeEnum::SVExternalToolObjectType: now handled in it own class SheetForExternalToolAdjustment

		case SvPb::SVObjectSubTypeEnum::SVRingBufferToolObjectType:
			// When the Object sub type is not set, then both mono and color images are placed into the selection list
			AddPage(new SvOg::TADlgImagePageClass(m_InspectionID, m_TaskObjectID, SvPb::SVNotSetSubObjectType));
			AddPage(new SvOg::TADlgRingBufferParameterPage(m_InspectionID, m_TaskObjectID));
			addConditionalDialog();
			break;

		case SvPb::SVObjectSubTypeEnum::SVTableToolObjectType:
			AddPage(new SvOg::TADlgTableParameterPage(m_InspectionID, m_TaskObjectID));
			AddPage(new SvOg::TADlgTableDefinesPage(m_InspectionID, m_TaskObjectID));
			addConditionalDialog();
			break;

		case SvPb::SVObjectSubTypeEnum::SVTableAnalyzerToolObjectType:
			AddPage(new SvOg::TADlgTableSourcePage(m_InspectionID, m_TaskObjectID));
			AddPage(new SvOg::TADlgTableAnalyzerPage(m_InspectionID, m_TaskObjectID));
			AddPage(new SvOg::TADlgPassFailPage(m_InspectionID, m_TaskObjectID, IDS_TABNAME_TABLE_ROWRANGE));
			addConditionalDialog();
			break;

		case SvPb::SVObjectSubTypeEnum::LoopToolObjectType:
			AddPage(new SvOg::TADlgLoopToolParameterPage(m_InspectionID, m_TaskObjectID));
			addConditionalDialog();
			break;

		case SvPb::SVObjectSubTypeEnum::GroupToolObjectType:
		{
			ObjectInfo.m_ObjectType = SvPb::ParameterTaskObjectType;
			ObjectInfo.m_SubType = SvPb::ParameterInputObjectType;
			SvOi::IObjectClass* pObject = pTaskObject->getFirstObject(ObjectInfo);
			if (nullptr != pObject)
			{
				AddPage(new SvOg::TADlgGroupToolInputPage(m_InspectionID, m_TaskObjectID, pObject->getObjectId()));
			}
			ObjectInfo.m_SubType = SvPb::ParameterResultObjectType;
			pObject = pTaskObject->getFirstObject(ObjectInfo);
			if (nullptr != pObject)
			{
				AddPage(new SvOg::TADlgGroupToolResultPage(m_InspectionID, m_TaskObjectID, pObject->getObjectId()));
			}
			addConditionalDialog();
			AddPage(new SvOg::TADlgGeneralPage(m_InspectionID, m_TaskObjectID, subType));
			break;
		}

		case SvPb::SVObjectSubTypeEnum::ModuleToolObjectType:
		{
			ObjectInfo.m_ObjectType = SvPb::ParameterTaskObjectType;
			ObjectInfo.m_SubType = SvPb::ParameterInputObjectType;
			SvOi::IObjectClass* pObject = pTaskObject->getFirstObject(ObjectInfo);
			if (nullptr != pObject)
			{
				AddPage(new SvOg::TADlgGroupToolInputPage(m_InspectionID, m_TaskObjectID, pObject->getObjectId(), true));
			}
			ObjectInfo.m_SubType = SvPb::ParameterResultObjectType;
			pObject = pTaskObject->getFirstObject(ObjectInfo);
			if (nullptr != pObject)
			{
				AddPage(new SvOg::TADlgGroupToolResultPage(m_InspectionID, m_TaskObjectID, pObject->getObjectId(), pTaskObject->getObjectId() == SvimState::getModuleEditingId() ? SvOg::TADlgGroupToolResultPage::ToolStateEnum::ModuleToolEditing : SvOg::TADlgGroupToolResultPage::ToolStateEnum::ModuleTool));
			}
			addConditionalDialog();
			AddPage(new SvOg::TADlgCommentPage(m_InspectionID, m_TaskObjectID, true, pTaskObject->getObjectId() != SvimState::getModuleEditingId()));
			break;
		}

		case SvPb::SVObjectSubTypeEnum::DrawToolObjectType:
			AddPage(new SvOg::TADlgDrawPage(m_InspectionID, m_TaskObjectID));
			addConditionalDialog();
			break;

		case SvPb::SVObjectSubTypeEnum::SVNotSetSubObjectType:
			//Special case for the Tool Set parameters
			if (SvPb::SVObjectTypeEnum::SVToolSetObjectType == pTaskObject->GetObjectType())
			{
				addConditionalDialog();
			}
			else
			{
				return;
			}
			break;

		default:
			SvStl::MessageManager Msg(SvStl::MsgType::Log | SvStl::MsgType::Display);
			Msg.setMessage(SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_Error_CannotOpenTADlg, SvStl::SourceFileParams(StdMessageParams));
			return;
	}

	AddPage(createToolAdjustmentDialogCommentPage());
}

BOOL TADlgSheetClass::OnInitDialog()
{
	BOOL bResult = __super::OnInitDialog();

	// Disable and Hide Cancel Button
	HWND hWnd = ::GetDlgItem(m_hWnd, IDCANCEL);
	if (nullptr != hWnd)
	{
		::EnableWindow(hWnd, FALSE);
		::ShowWindow(hWnd, SW_HIDE);
	}
	// Remove Close Button
	ModifyStyle(WS_SYSMENU, WS_THICKFRAME, SWP_FRAMECHANGED);

	// Send Errors to the display
	SVObjectClass* pTaskObject = dynamic_cast<SVObjectClass*> (GetTaskObject());

	if (nullptr != pTaskObject)
	{
		CString Temp = _T("Tool Adjustment: ");
		Temp += pTaskObject->GetObjectName();
		if (auto* pModule = dynamic_cast<SvTo::ModuleTool*>(pTaskObject); nullptr != pModule)
		{
			Temp += " (";
			Temp += SvOi::getModuleName(pModule->getModuleGuid()).c_str();
			Temp += ") ";
		}
		Temp += _T(" - ");
		Temp += pTaskObject->GetName();
		SetWindowText(Temp);
	}

	return bResult;
}

void TADlgSheetClass::OnOK()
{
	CWaitCursor cwcMouse;

	if (false == ValidateAllSheets())
	{
		return;
	}


	if (true == ResetTools())
	{
		markDocumentAsDirty(true);
		EndDialog(IDOK);
	}
	else
	{
		SvOi::ITaskObject* pTaskObject = dynamic_cast<SvOi::ITaskObject*> (GetTaskObject());
		if (nullptr != pTaskObject)
		{
			SvStl::MessageContainer message = pTaskObject->getFirstTaskMessage();
			SvStl::MessageTextEnum textEnum = SvStl::Tid_Empty;
			SvDef::StringVector textList;
			if (0 != message.getMessage().m_MessageCode)
			{
				textEnum = message.getMessage().m_AdditionalTextId;
				textList = message.getMessage().m_AdditionalTextList;
			}
			SvStl::MessageManager messageMgr(SvStl::MsgType::Log | SvStl::MsgType::Display);
			INT_PTR result = messageMgr.setMessage(SVMSG_SVO_104_TA_DIALOG_CLOSING_ERROR, textEnum, textList, SvStl::SourceFileParams(StdMessageParams), GetTaskObjectID(), MB_YESNO);
			if (IDYES == result)
			{
				EndDialog(IDOK);
			}
		}
	}
}


bool TADlgSheetClass::ValidateAllSheets()
{
	// Try to validate the Equations
	int cnt = GetPageCount();

	for (int i = 0; i < cnt; i++)
	{
		CPropertyPage* pPage = GetPage(i);
		if (pPage && pPage->GetSafeHwnd())
		{
			if (SvOgu::SVFormulaEditorPageClass* pFormulaEditor = dynamic_cast <SvOgu::SVFormulaEditorPageClass*> (pPage))
			{
				if (!pFormulaEditor->validateAndSetEquation())
				{
					// Equation must be valid or disabled
					SvStl::MessageManager Msg(SvStl::MsgType::Log | SvStl::MsgType::Display);
					Msg.setMessage(SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_Error_InvalidFormula, SvStl::SourceFileParams(StdMessageParams));
					return false;
				}
			}
			else
			{
				if (SvOg::ISVPropertyPageDialog* pIDlg = dynamic_cast <SvOg::ISVPropertyPageDialog*> (pPage))
				{
					if (pIDlg->QueryAllowExit() == false)	// exit not allowed
					{
						return false;
					}
				}
				if (CPropertyPage* pPropertyPage = dynamic_cast <CPropertyPage*> (pPage))
				{
					pPropertyPage->OnOK();
				}
			}
		}
	}
	return true;
}



bool TADlgSheetClass::ResetTools()
{
	
	bool resetResult = false;
	//If parent is a tool then it must be reset
	SvOi::IObjectClass* pTool = GetTopTool();
	if (nullptr == pTool)
	{
		return resetResult;
	}
	
	resetResult = pTool->resetAllObjects();

	
	/// Reset Dependent
	bool isOK {true};
	for (const auto id : m_dependentTools)
	{
		SVObjectClass* pObj = nullptr;
		SVObjectManagerClass::Instance().GetObjectByIdentifier(id, pObj);
		if (pObj)
		{
			isOK = pObj->resetAllObjects(nullptr, SvTo::cResetDepth ) && isOK;
			Log_Assert(pObj->getToolPtr() != nullptr);
		}

	}

	return resetResult;
}





void TADlgSheetClass::markDocumentAsDirty(bool runOnce)
{
	SVIPDoc* pDocument = GetIPDoc();
	if (nullptr != pDocument)
	{
		pDocument->SetModifiedFlag();
		if (runOnce)
		{
			pDocument->RunOnce();
		}
	}
}


void TADlgSheetClass::OnCancel()
{
	EndDialog(IDCANCEL);
}


void TADlgSheetClass::OnSysCommand(UINT nID, LPARAM lParam)
{
	switch (nID & 0xFFF0)
	{
		// Ignore the close command
		case SC_CLOSE:
			return;
	}

	__super::OnSysCommand(nID, lParam);
}


SVIPDoc* TADlgSheetClass::GetIPDoc() const
{
	return m_pIPDoc;
}

SvOi::IObjectClass* TADlgSheetClass::GetTaskObject() const
{
	return (SvOi::getObject(m_TaskObjectID));
}

SvOi::IObjectClass* TADlgSheetClass::GetTopTool() const
{
	SVObjectClass* pTool = static_cast<SVObjectClass*>(GetTaskObject());
	if (pTool)
	{
		SVObjectClass* pObject = pTool->GetAncestor(SvPb::SVToolObjectType, true);
		if (pObject)
		{
			return pObject;
		}

	}
	return pTool;
}