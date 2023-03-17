//******************************************************************************
//* COPYRIGHT (c) 2005 by Körber Pharma Inspection GmbH. All Rights Reserved
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
#include "SVTADlgResizePage.h"
#include "SVTADlgTranslationShiftPage.h"
#include "SVToolAdjustmentDialogAnalyzerPageClass.h"
#include "SVToolAdjustmentDialogMaskPageClass.h"
#include "Definitions/StringTypeDef.h"
#include "SVOGui/SVToolAdjustmentDialogCommentPage.h"
#include "SVOGui/SVToolAdjustmentDialogSizePage.h"
#include "SVOGui/ISVPropertyPageDialog.h"
#include "SVOGui/SVCylindricalWarpDlg.h"
#include "SVOGuiUtility/SVFormulaEditorPage.h"
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
#include "SVOGui/TADialogGroupToolInputPage.h"
#include "SVOGui/TADialogLoopToolParameterPage.h"
#include "SVOGui/TADialogRingBufferParameterPage.h"
#include "SVOGui/TADialogTableParameterPage.h"
#include "SVOGui/TADialogTableDefinesPage.h"
#include "SVOGui/TATableAnalyzerPage.h"
#include "SVOGui/TADialogDrawPage.h"
#include "SVOGui/TATableSourcePage.h"
#include "SVOGui/ToolAdjustToolSetPage.h"
#include "SVStatusLibrary/MessageManager.h"
#include "Tools/SVTool.h"
#include "SVOGui/TADialogGroupToolResultPage.h"
#include "TADialogLinearResultRangesPage.h"
#include "SVUtilityLibrary/VectorHelper.h"
#include "Tools/ModuleTool.h"
#include "ObjectInterfaces/IConfigurationObject.h"

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

SVToolAdjustmentDialogSheetClass::SVToolAdjustmentDialogSheetClass(SVIPDoc* p_pIPDoc, uint32_t inspectionID, uint32_t taskObjectID, LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
	: CPropertySheet(pszCaption, pParentWnd, iSelectPage)
	, m_pIPDoc(p_pIPDoc)
	, m_InspectionID(inspectionID)
	, m_TaskObjectID(taskObjectID)
{}

void SVToolAdjustmentDialogSheetClass::init()
{
	//This will remove the APPLY Button from the Tool Adjustment Dialogs.
	m_psh.dwFlags |= PSH_NOAPPLYNOW;

	addPages();


	SvOi::IObjectClass* pObject = GetTaskObject();
	Log_Assert(pObject && pObject->GetObjectType() == SvPb::SVToolObjectType);

	m_dependentTools.clear();

	std::back_insert_iterator<std::vector<uint32_t>>  InsertIt(m_dependentTools);
	SvTo::InsertDependentTools(InsertIt, pObject->getObjectId());
	SvUl::RemoveDuplicates(m_dependentTools);


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


void SVToolAdjustmentDialogSheetClass::addConditionalDialog()
{
	SvDef::SVObjectTypeInfoStruct ObjectInfo;
	ObjectInfo.m_ObjectType = SvPb::SVEquationObjectType;
	ObjectInfo.m_SubType = SvPb::SVConditionalObjectType;
	SvOi::IFormulaControllerPtr pFormulaController {new SvOgu::FormulaController(m_InspectionID, m_TaskObjectID, ObjectInfo)};
	UINT disableExtensionID = (SvPb::SVObjectTypeEnum::SVToolSetObjectType == GetTaskObject()->GetObjectType()) ? IDS_CLASSNAME_SVTOOLSET : IDS_TOOL_STRING;

	AddPage(new SvOgu::SVFormulaEditorPageClass {pFormulaController, true, IDS_CONDITIONAL_STRING, disableExtensionID});
}


CPropertyPage* SVToolAdjustmentDialogSheetClass::createToolAdjustmentDialogCommentPage()
{
	return new SvOg::SVToolAdjustmentDialogCommentPage(m_InspectionID, m_TaskObjectID);
}


void SVToolAdjustmentDialogSheetClass::AddAdditionalPagesForExternalTool()
{
	addConditionalDialog();
	AddPage(new SvOg::SVTADlgGeneralPage(m_InspectionID, m_TaskObjectID, SvPb::SVObjectSubTypeEnum::SVExternalToolObjectType));
	AddPage(createToolAdjustmentDialogCommentPage());
}


void SVToolAdjustmentDialogSheetClass::addPages()
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
			AddPage(new SVTADlgAcquisitionSourcePage(m_InspectionID, m_TaskObjectID, this));
			// NOTE: No Conditional Execution for Acquisition Tool!
			// addConditionalDialog();
			// NOTE: No General page for acquisition tool!
			// AddPage( new SvOg::SVTADlgGeneralPage( m_InspectionID, m_TaskObjectID, subType) ) );
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
				AddPage(new SVTADlgThresholdPage(m_InspectionID, m_TaskObjectID, this));
				if (nullptr != pMaskOperator)
				{
					AddPage(new SVToolAdjustmentDialogMaskPageClass(m_InspectionID, m_TaskObjectID, pMaskOperator->getObjectId()));
				}
				AddPage(new SvOg::SVToolAdjustmentLUTPage(m_InspectionID, pLUTOperator->getObjectId(), pLUTEquation->getObjectId()));
			}
			addConditionalDialog();
			AddPage(new SvOg::SVTADlgGeneralPage(m_InspectionID, m_TaskObjectID, subType));
			break;

		case SvPb::SVObjectSubTypeEnum::SVWindowToolObjectType:
			AddPage(new SvOg::SVToolAdjustmentDialogImagePageClass(m_InspectionID, m_TaskObjectID));
			if (bHasSize)
			{
				AddPage(new SvOg::SVToolAdjustmentDialogSizePage(m_InspectionID, m_TaskObjectID));
			}
			AddPage(new SvOg::SVToolAdjustmentDialogFilterPageClass(m_InspectionID, m_TaskObjectID));
			AddPage(new SVTADlgThresholdPage(m_InspectionID, m_TaskObjectID, this));
			if (nullptr != pMaskOperator)
			{
				AddPage(new SVToolAdjustmentDialogMaskPageClass(m_InspectionID, m_TaskObjectID, pMaskOperator->getObjectId()));
			}
			if (nullptr != pLUTOperator && nullptr != pLUTEquation)
			{
				// To be compatible to old Window Tool...RO_22Mar2000
				AddPage(new SvOg::SVToolAdjustmentLUTPage(m_InspectionID, pLUTOperator->getObjectId(), pLUTEquation->getObjectId()));
			}
			AddPage(new SVToolAdjustmentDialogAnalyzerPageClass(m_InspectionID, m_TaskObjectID, this));
			addConditionalDialog();
			AddPage(new SvOg::SVTADlgGeneralPage(m_InspectionID, m_TaskObjectID, subType));
			break;

		case SvPb::SVObjectSubTypeEnum::SVLinearToolObjectType:
			AddPage(new SvOg::SVToolAdjustmentDialogImagePageClass(m_InspectionID, m_TaskObjectID));
			if (bHasSize)
			{
				AddPage(new SvOg::SVToolAdjustmentDialogSizePage(m_InspectionID, m_TaskObjectID));
			}
			AddPage(new SvOg::SVToolAdjustmentDialogFilterPageClass(m_InspectionID, m_TaskObjectID));
			AddPage(new SVTADlgThresholdPage(m_InspectionID, m_TaskObjectID, this));
			AddPage(new SVToolAdjustmentDialogAnalyzerPageClass(m_InspectionID, m_TaskObjectID, this));
			//add the new page.
			AddPage(new SvOg::SVToolAdjustmentDialogLinearSpecialPage(m_InspectionID, m_TaskObjectID));
			AddPage(new TADialogLinearResultRangesPage(m_InspectionID, m_TaskObjectID));
			addConditionalDialog();
			AddPage(new SvOg::SVTADlgGeneralPage(m_InspectionID, m_TaskObjectID, subType));
			break;

		case SvPb::SVObjectSubTypeEnum::SVToolLoadImageObjectType:
			AddPage(new SvOg::SVTADlgFileImageSourcePage(m_InspectionID, m_TaskObjectID));
			if (bHasSize)
			{
				AddPage(new SvOg::SVToolAdjustmentDialogSizePage(m_InspectionID, m_TaskObjectID));
			}
			addConditionalDialog();
			AddPage(new SvOg::SVTADlgGeneralPage(m_InspectionID, m_TaskObjectID, subType));
			break;

		case SvPb::SVObjectSubTypeEnum::SVToolArchiveObjectType:   // Archive tool
		{
			AddPage(new SVTADlgArchiveResultsPage(m_InspectionID, m_TaskObjectID, this));
			AddPage(new SVTADlgArchiveImagePage(m_InspectionID, m_TaskObjectID, this));
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
			AddPage(new SvOg::SVTADlgPassFailPage(m_InspectionID, m_TaskObjectID));
			addConditionalDialog();
		}
		break;

		case SvPb::SVObjectSubTypeEnum::SVStatisticsToolObjectType:
			AddPage(new SVTADlgStatisticsPage(m_InspectionID, m_TaskObjectID, this));
			addConditionalDialog();
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
			AddPage(new SVTADlgTranslationShiftPage(m_InspectionID, m_TaskObjectID, this));
			addConditionalDialog();
			AddPage(new SvOg::SVTADlgGeneralPage(m_InspectionID, m_TaskObjectID, subType));
			break;

		case SvPb::SVObjectSubTypeEnum::SVResizeToolObjectType:
			AddPage(new SvOg::SVToolAdjustmentDialogImagePageClass(m_InspectionID, m_TaskObjectID));
			if (bHasSize)
			{
				AddPage(new SvOg::SVToolAdjustmentDialogSizePage(m_InspectionID, m_TaskObjectID));
			}
			AddPage(new SVTADlgResizePage(m_InspectionID, m_TaskObjectID, this));
			addConditionalDialog();
			AddPage(new SvOg::SVTADlgGeneralPage(m_InspectionID, m_TaskObjectID, subType));
			break;

		case SvPb::SVObjectSubTypeEnum::SVTransformationToolObjectType:
			if (nullptr != pImageTransform)
			{
				AddPage(new SvOg::SVTADlgTransformImagePage(m_InspectionID, m_TaskObjectID, pImageTransform->getObjectId()));
			}
			AddPage(new SVTADlgTranslationPage(m_InspectionID, m_TaskObjectID));
			AddPage(new SVTADlgRotationPage(m_InspectionID, m_TaskObjectID));
			AddPage(new SVTADlgTransformationLearnPage(m_InspectionID, m_TaskObjectID, this));
			addConditionalDialog();
			AddPage(new SvOg::SVTADlgGeneralPage(m_InspectionID, m_TaskObjectID, subType));
			break;

		case SvPb::SVObjectSubTypeEnum::SVPolarTransformationToolObjectType:
			AddPage(new SvOg::SVToolAdjustmentDialogImagePageClass(m_InspectionID, m_TaskObjectID));
			AddPage(new SVTADlgPolarTransformPage(m_InspectionID, m_TaskObjectID, this));
			addConditionalDialog();
			AddPage(new SvOg::SVTADlgGeneralPage(m_InspectionID, m_TaskObjectID, subType));
			break;

		case SvPb::SVObjectSubTypeEnum::SVColorToolObjectType:
			AddPage(new SvOg::SVTADlgColorToolPage(m_InspectionID, m_TaskObjectID));
			if (bHasSize)
			{
				AddPage(new SvOg::SVToolAdjustmentDialogSizePage(m_InspectionID, m_TaskObjectID));
			}
			addConditionalDialog();
			AddPage(new SvOg::SVTADlgGeneralPage(m_InspectionID, m_TaskObjectID, subType));
			break;

		case SvPb::SVObjectSubTypeEnum::SVToolCylindricalObjectType:
			AddPage(new SvOg::SVToolAdjustmentDialogImagePageClass(m_InspectionID, m_TaskObjectID));
			AddPage(new SvOg::SVCylindricalWarpDlg(m_InspectionID, m_TaskObjectID));
			addConditionalDialog();
			AddPage(new SvOg::SVTADlgGeneralPage(m_InspectionID, m_TaskObjectID, subType));
			break;

		case SvPb::SVObjectSubTypeEnum::SVPerspectiveToolObjectType:
			AddPage(new SvOg::SVToolAdjustmentDialogImagePageClass(m_InspectionID, m_TaskObjectID));
			AddPage(new SvOg::SVPerspectiveWarpPage(m_InspectionID, m_TaskObjectID));
			addConditionalDialog();
			AddPage(new SvOg::SVTADlgGeneralPage(m_InspectionID, m_TaskObjectID, subType));
			break;

			//		case SvPb::SVObjectSubTypeEnum::SVExternalToolObjectType: now handled in it own class SheetForExternalToolAdjustment

		case SvPb::SVObjectSubTypeEnum::SVRingBufferToolObjectType:
			// When the Object sub type is not set, then both mono and color images are placed into the selection list
			AddPage(new SvOg::SVToolAdjustmentDialogImagePageClass(m_InspectionID, m_TaskObjectID, SvPb::SVNotSetSubObjectType));
			AddPage(new SvOg::TADialogRingBufferParameterPage(m_InspectionID, m_TaskObjectID));
			addConditionalDialog();
			break;

		case SvPb::SVObjectSubTypeEnum::SVTableToolObjectType:
			AddPage(new SvOg::TADialogTableParameterPage(m_InspectionID, m_TaskObjectID));
			AddPage(new SvOg::TADialogTableDefinesPage(m_InspectionID, m_TaskObjectID));
			addConditionalDialog();
			break;

		case SvPb::SVObjectSubTypeEnum::SVTableAnalyzerToolObjectType:
			AddPage(new SvOg::TATableSourcePage(m_InspectionID, m_TaskObjectID));
			AddPage(new SvOg::TaTableAnalyzerPage(m_InspectionID, m_TaskObjectID));
			AddPage(new SvOg::SVTADlgPassFailPage(m_InspectionID, m_TaskObjectID, IDS_TABNAME_TABLE_ROWRANGE));
			addConditionalDialog();
			break;

		case SvPb::SVObjectSubTypeEnum::LoopToolObjectType:
			AddPage(new SvOg::TADialogLoopToolParameterPage(m_InspectionID, m_TaskObjectID));
			addConditionalDialog();
			break;

		case SvPb::SVObjectSubTypeEnum::GroupToolObjectType:
		{
			ObjectInfo.m_ObjectType = SvPb::ParameterTaskObjectType;
			ObjectInfo.m_SubType = SvPb::ParameterInputObjectType;
			SvOi::IObjectClass* pObject = pTaskObject->getFirstObject(ObjectInfo);
			if (nullptr != pObject)
			{
				AddPage(new SvOg::TADialogGroupToolInputPage(m_InspectionID, m_TaskObjectID, pObject->getObjectId()));
			}
			ObjectInfo.m_SubType = SvPb::ParameterResultObjectType;
			pObject = pTaskObject->getFirstObject(ObjectInfo);
			if (nullptr != pObject)
			{
				AddPage(new SvOg::TADialogGroupToolResultPage(m_InspectionID, m_TaskObjectID, pObject->getObjectId()));
			}
			addConditionalDialog();
			AddPage(new SvOg::SVTADlgGeneralPage(m_InspectionID, m_TaskObjectID, subType));
			break;
		}

		case SvPb::SVObjectSubTypeEnum::ModuleToolObjectType:
		{
			ObjectInfo.m_ObjectType = SvPb::ParameterTaskObjectType;
			ObjectInfo.m_SubType = SvPb::ParameterInputObjectType;
			SvOi::IObjectClass* pObject = pTaskObject->getFirstObject(ObjectInfo);
			if (nullptr != pObject)
			{
				AddPage(new SvOg::TADialogGroupToolInputPage(m_InspectionID, m_TaskObjectID, pObject->getObjectId(), false));
			}
			ObjectInfo.m_SubType = SvPb::ParameterResultObjectType;
			pObject = pTaskObject->getFirstObject(ObjectInfo);
			if (nullptr != pObject)
			{
				AddPage(new SvOg::TADialogGroupToolResultPage(m_InspectionID, m_TaskObjectID, pObject->getObjectId(), false));
			}
			addConditionalDialog();
			AddPage(new SvOg::SVToolAdjustmentDialogCommentPage(m_InspectionID, m_TaskObjectID, true));
			break;
		}

		case SvPb::SVObjectSubTypeEnum::DrawToolObjectType:
			AddPage(new SvOg::TADialogDrawPage(m_InspectionID, m_TaskObjectID));
			addConditionalDialog();
			break;

		case SvPb::SVObjectSubTypeEnum::SVNotSetSubObjectType:
			//Special case for the Tool Set parameters
			if (SvPb::SVObjectTypeEnum::SVToolSetObjectType == pTaskObject->GetObjectType())
			{
				AddPage(new SvOg::ToolAdjustToolSetPage(m_InspectionID, m_TaskObjectID));
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

void SVToolAdjustmentDialogSheetClass::SetupDynamicLayout()
{
	EnableDynamicLayout(TRUE);

	CRect clientRect;
	GetClientRect(&clientRect);
	
	constexpr int gripperSize = 12;
	CRect gripperRect(clientRect.right - gripperSize, clientRect.bottom - gripperSize, clientRect.right, clientRect.bottom);

	m_resizeGripper.Create(SBS_SIZEGRIP | WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS, gripperRect, this, IDC_RESIZE_GRIPPER);

	auto pManager = GetDynamicLayout();
	if (pManager != nullptr)
	{
		pManager->Create(this);

		for (CWnd* child = GetWindow(GW_CHILD); child; child = child->GetWindow(GW_HWNDNEXT))
		{
			
			// All buttons need to be moved 100% in all directions
			if (child->SendMessage(WM_GETDLGCODE) & DLGC_BUTTON)
			{
				pManager->AddItem(child->GetSafeHwnd(),
					CMFCDynamicLayout::MoveHorizontalAndVertical(100, 100), CMFCDynamicLayout::SizeNone());
			}
			else // This will be the main tab control which needs to be stretched in both directions
			{
				pManager->AddItem(child->GetSafeHwnd(),
					CMFCDynamicLayout::MoveNone(), CMFCDynamicLayout::SizeHorizontalAndVertical(100, 100));
			}
			
		}
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

	SetupDynamicLayout();

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


bool SVToolAdjustmentDialogSheetClass::ValidateAllSheets()
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



bool SVToolAdjustmentDialogSheetClass::ResetTools()
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





void SVToolAdjustmentDialogSheetClass::markDocumentAsDirty(bool runOnce)
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

SvOi::IObjectClass* SVToolAdjustmentDialogSheetClass::GetTopTool() const
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