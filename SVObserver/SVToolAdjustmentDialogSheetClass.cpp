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
#include "InspectionEngine/SVTool.h"
#include "SVExternalToolDlg.h"
#include "SVOGui/SVPerspectiveWarpPage.h"
#include "SVTADlgAcquisitionSourcePage.h"
#include "SVTADlgPolarTransformPage.h"
#include "SVTADlgRemoteInputToolPage.h"
#include "SVTADlgRotationPage.h"
#include "SVTADlgStatisticsPage.h"
#include "SVTADlgThresholdPage.h"
#include "SVTADlgTransformationLearnPage.h"
#include "SVTADlgTranslationPage.h"
#include "SVOGui/SVToolAdjustmentDialogLinearSpecialPage.h"
#include "SVToolAdjustmentDialogMaskPageClass.h"
#include "SVTADlgArchiveResultsPage.h"
#include "SVTADlgArchiveImagePage.h"
#include "SVOGui/SVToolAdjustmentDialogCommentPage.h"
#include "SVTADlgTranslationShiftPage.h"
#include "SVTADlgTranslationResizePage.h"
#include "SVToolAdjustmentDialogSizePage.h"
#include "SVToolAdjustmentDialogAnalyzerPageClass.h"
#include "SVStatusLibrary/MessageManager.h"
#include "SVStatusLibrary/ErrorNumbers.h"
#include "SVStatusLibrary/MessageManager.h"
#include "SVOGui/ISVPropertyPageDialog.h"
#include "SVOGui/SVCylindricalWarpDlg.h"
#include "SvOGui/SVTADlgGeneralPage.h"
#include "SVOGui/SVTADlgPassFailPage.h"
#include "SvOGui/SVFormulaEditorPage.h"
#include "SVOGui/SVToolAdjustmentDialogFilterPageClass.h"
#include "SVOGui/TADialogRingBufferParameterPage.h"
#include "SVOGui/SVToolAdjustmentDialogImagePageClass.h"
#include "SVOGui/SVToolAdjustmentDialogTwoImagePage.h"
#include "SVOGui/SVTADlgTransformImagePage.h"
#include "SVOGui/SVTADlgFileImageSourcePage.h"
#include "SVOGui/SVTADlgColorToolPage.h"
#include "SVOGui/SVToolAdjustmentLUTPage.h"
#include "SVOGui/TextDefinesSvOg.h"
#include "SVOGui/TADialogTableParameterPage.h"
#include "SVOGui/TADialogTableDefinesPage.h"
#include "SVOGui/TATableSourcePage.h"
#include "SVOGui/TATableAnalyzerPage.h"
#include "Definitions/StringTypeDef.h"
#include "TextDefinesSvO.h"
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
	ON_COMMAND(IDOK,OnOK)
	ON_WM_SYSCOMMAND()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

SVToolAdjustmentDialogSheetClass::SVToolAdjustmentDialogSheetClass( SVIPDoc* p_pIPDoc, const GUID& rInspectionID, const GUID& rTaskObjectID, UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage )
: CPropertySheet(nIDCaption, pParentWnd, iSelectPage)
, m_pIPDoc( p_pIPDoc )
, m_InspectionID( rInspectionID )
, m_TaskObjectID( rTaskObjectID )
{
	init();
}

SVToolAdjustmentDialogSheetClass::SVToolAdjustmentDialogSheetClass( SVIPDoc* p_pIPDoc, const GUID& rInspectionID, const GUID& rTaskObjectID, LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage )
: CPropertySheet( pszCaption, pParentWnd, iSelectPage )
, m_pIPDoc( p_pIPDoc )
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
	for( int i = GetPageCount() - 1; i >= 0; -- i )
	{
		CPropertyPage* pPage = GetPage( i );
		RemovePage( i );
		if( pPage )
		{
			delete pPage;
		}
	}
}

void SVToolAdjustmentDialogSheetClass::addPages()
{
	SvOi::IFormulaControllerPtr pFormularController{ new SvOg::FormulaController(m_InspectionID, m_TaskObjectID, SvDef::SVObjectTypeInfoStruct(SvDef::SVEquationObjectType, SvDef::SVConditionalObjectType), false) };
	SvOg::SVFormulaEditorPageClass* pConditionalDlg{ new SvOg::SVFormulaEditorPageClass(pFormularController, true, IDS_CONDITIONAL_STRING, IDS_TOOL_STRING) };

	SvOi::IObjectClass* pLUTEquation{ nullptr };
	SvDef::SVObjectTypeInfoStruct ObjectInfo;
	ObjectInfo.ObjectType = SvDef::SVUnaryImageOperatorObjectType;
	ObjectInfo.SubType	 = SvDef::SVLUTOperatorObjectType;
	SvOi::IObjectClass* pLUTOperator = GetTool()->getFirstObject(ObjectInfo);
	if (nullptr != pLUTOperator)
	{
		ObjectInfo.ObjectType = SvDef::SVEquationObjectType;
		ObjectInfo.SubType = SvDef::SVLUTEquationObjectType;
		pLUTEquation = pLUTOperator->getFirstObject(ObjectInfo);
	}

	ObjectInfo.ObjectType = SvDef::SVUnaryImageOperatorObjectType;
	ObjectInfo.SubType = SvDef::SVUserMaskOperatorObjectType;
	SvOi::IObjectClass* pMaskOperator = GetTool()->getFirstObject(ObjectInfo);

	bool bHasSize = false;
	ObjectInfo.ObjectType = SvDef::SVToolSizeAdjustTaskType;
	ObjectInfo.SubType = SvDef::SVNotSetSubObjectType;

	if( nullptr != GetTool()->getFirstObject(ObjectInfo) )
	{
		bHasSize = true;
	}


	const SvDef::SVObjectTypeInfoStruct& rToolType = GetTool()->GetObjectInfo().m_ObjectTypeInfo;
	switch( rToolType.SubType )
	{
		case SvDef::SVToolAcquisitionObjectType:
			AddPage( new SVToolAdjustmentDialogAcquisitionSourcePageClass( m_InspectionID, m_TaskObjectID, this ) );
			// NOTE:
			// No Conditional Execution for Acquisition Tool !!!!
			delete pConditionalDlg;
			pConditionalDlg = nullptr;
			// NOTE:
			// No General page for acquisition tool!
			// AddPage( new SvOg::SVToolAdjustmentDialogGeneralPageClass( m_InspectionID, m_TaskObjectID ) );
			break;

		case SvDef::SVToolImageObjectType:	// Image Tool
			AddPage( new SvOg::SVToolAdjustmentDialogTwoImagePageClass( m_InspectionID, m_TaskObjectID ) );
			if(bHasSize)
			{
				AddPage(new SVToolAdjustmentDialogSizePage(m_InspectionID, m_TaskObjectID, this));
			}
			// New image tool has also this pages...RO_22Mar2000
			if (nullptr != pLUTOperator && nullptr != pLUTEquation)
			{
				AddPage(new SvOg::SVToolAdjustmentDialogFilterPageClass(m_InspectionID, m_TaskObjectID));
				AddPage( new SVToolAdjustmentDialogThresholdPageClass( m_InspectionID, m_TaskObjectID, this ) );
				if (nullptr != pMaskOperator)
				{
					AddPage(new SVToolAdjustmentDialogMaskPageClass(m_InspectionID, m_TaskObjectID, pMaskOperator->GetUniqueObjectID()));
				}
				AddPage(new SvOg::SVToolAdjustmentLUTPage(m_InspectionID, pLUTOperator->GetUniqueObjectID(), pLUTEquation->GetUniqueObjectID()));
			}
			AddPage( pConditionalDlg );
			AddPage( new SvOg::SVToolAdjustmentDialogGeneralPageClass( m_InspectionID, m_TaskObjectID ) );
			break;

		case SvDef::SVWindowToolObjectType:
			AddPage( new SvOg::SVToolAdjustmentDialogImagePageClass( m_InspectionID, m_TaskObjectID ) );
			if(bHasSize)
			{
				AddPage(new SVToolAdjustmentDialogSizePage(m_InspectionID, m_TaskObjectID, this));
			}
			AddPage( new SvOg::SVToolAdjustmentDialogFilterPageClass( m_InspectionID, m_TaskObjectID ) );
			AddPage( new SVToolAdjustmentDialogThresholdPageClass( m_InspectionID, m_TaskObjectID, this ) );
			if (nullptr != pMaskOperator)
			{
				AddPage(new SVToolAdjustmentDialogMaskPageClass(m_InspectionID, m_TaskObjectID, pMaskOperator->GetUniqueObjectID()));
			}
			if (nullptr != pLUTOperator && nullptr != pLUTEquation)
			{
				// To be compatible to old Window Tool...RO_22Mar2000
				AddPage( new SvOg::SVToolAdjustmentLUTPage( m_InspectionID, pLUTOperator->GetUniqueObjectID(), pLUTEquation->GetUniqueObjectID()) );
			}
			AddPage( new SVToolAdjustmentDialogAnalyzerPageClass( m_InspectionID, m_TaskObjectID, this ) );
			AddPage( pConditionalDlg );
			AddPage( new SvOg::SVToolAdjustmentDialogGeneralPageClass( m_InspectionID, m_TaskObjectID ) );
			break;

		case SvDef::SVLinearToolObjectType:
			AddPage( new SvOg::SVToolAdjustmentDialogImagePageClass( m_InspectionID, m_TaskObjectID ) );
			if(bHasSize)
			{
				AddPage(new SVToolAdjustmentDialogSizePage(m_InspectionID, m_TaskObjectID, this));
			}
			AddPage( new SvOg::SVToolAdjustmentDialogFilterPageClass( m_InspectionID, m_TaskObjectID ) );
			AddPage( new SVToolAdjustmentDialogThresholdPageClass( m_InspectionID, m_TaskObjectID, this ) );
			AddPage( new SVToolAdjustmentDialogAnalyzerPageClass( m_InspectionID, m_TaskObjectID, this ) );
			//add the new page.
			AddPage( new SvOg::SVToolAdjustmentDialogLinearSpecialPage(m_InspectionID, m_TaskObjectID) );
			AddPage( pConditionalDlg );
			AddPage( new SvOg::SVToolAdjustmentDialogGeneralPageClass( m_InspectionID, m_TaskObjectID ) );
			break;

		case SvDef::SVToolLoadImageObjectType:
			AddPage( new SvOg::SVToolAdjustmentDialogFileImageSourcePageClass( m_InspectionID, m_TaskObjectID ) );
			if(bHasSize)
			{
				AddPage(new SVToolAdjustmentDialogSizePage(m_InspectionID, m_TaskObjectID, this));
			}
			AddPage( pConditionalDlg );
			AddPage( new SvOg::SVToolAdjustmentDialogGeneralPageClass( m_InspectionID, m_TaskObjectID ) );
			break;

		case SvDef::SVToolArchiveObjectType:   // Archive tool
			AddPage( new SVTADlgArchiveResultsPage( m_InspectionID, m_TaskObjectID, this) );
			AddPage( new SVTADlgArchiveImagePage( m_InspectionID, m_TaskObjectID, this) );
			AddPage( pConditionalDlg );
			break;

		case SvDef::SVMathToolObjectType:
			{
				SvDef::SVObjectTypeInfoStruct info(SvDef::SVEquationObjectType, SvDef::SVMathEquationObjectType);
				SvOi::IFormulaControllerPtr pFormularController{ new SvOg::FormulaController(m_InspectionID, m_TaskObjectID, info) };
				AddPage( new SvOg::SVFormulaEditorPageClass(pFormularController, false) );
				AddPage( new SvOg::SVToolAdjustmentDialogPassFailPageClass( m_InspectionID, m_TaskObjectID) );
				AddPage( pConditionalDlg );
			}
			break;

		case SvDef::SVStatisticsToolObjectType:
			AddPage( new SVToolAdjustmentDialogStatisticsPageClass( m_InspectionID, m_TaskObjectID,this ) );
			AddPage( pConditionalDlg );
			break;

		case SvDef::SVShiftToolObjectType:
			AddPage( new SvOg::SVToolAdjustmentDialogImagePageClass( m_InspectionID, m_TaskObjectID ) );
			if(bHasSize)
			{
				AddPage(new SVToolAdjustmentDialogSizePage(m_InspectionID, m_TaskObjectID, this));
			}			
			AddPage( new SVTADlgTranslationShiftPageClass( m_InspectionID, m_TaskObjectID, this ) );
			AddPage( pConditionalDlg );
			AddPage( new SvOg::SVToolAdjustmentDialogGeneralPageClass( m_InspectionID, m_TaskObjectID ) );
			break;

		case SvDef::SVResizeToolObjectType:
			AddPage( new SvOg::SVToolAdjustmentDialogImagePageClass( m_InspectionID, m_TaskObjectID ) );
			if (bHasSize)
			{
				AddPage(new SVToolAdjustmentDialogSizePage(m_InspectionID, m_TaskObjectID, this));
			}
			AddPage( new SVTADlgTranslationResizePage( m_InspectionID, m_TaskObjectID, this ) );
			AddPage( pConditionalDlg );
			AddPage( new SvOg::SVToolAdjustmentDialogGeneralPageClass( m_InspectionID, m_TaskObjectID ) );
			break;

		case SvDef::SVTransformationToolObjectType:
			AddPage( new SvOg::SVToolAdjustmentDialogTransformImagePageClass( m_InspectionID, m_TaskObjectID ) );
			AddPage( new SVToolAdjustmentDialogTranslationPageClass( m_InspectionID, m_TaskObjectID, this ) );
			AddPage( new SVToolAdjustmentDialogRotationPageClass( m_InspectionID, m_TaskObjectID, this ) );
			AddPage( new SVToolAdjustmentDialogTransformationLearnPageClass( m_InspectionID, m_TaskObjectID, this ) );
			AddPage( pConditionalDlg );
			AddPage( new SvOg::SVToolAdjustmentDialogGeneralPageClass( m_InspectionID, m_TaskObjectID ) );
			break;

		case SvDef::SVPolarTransformationToolObjectType:
			AddPage( new SvOg::SVToolAdjustmentDialogImagePageClass( m_InspectionID, m_TaskObjectID ) );
			AddPage( new SVToolAdjustmentDialogPolarTransformPageClass( m_InspectionID, m_TaskObjectID, this ) );
			AddPage( pConditionalDlg );
			AddPage( new SvOg::SVToolAdjustmentDialogGeneralPageClass( m_InspectionID, m_TaskObjectID ) );
			break;

		case SvDef::SVColorToolObjectType:
			AddPage( new SvOg::SVTADlgColorToolPage( m_InspectionID, m_TaskObjectID ) );
			if (bHasSize)
			{
				AddPage(new SVToolAdjustmentDialogSizePage(m_InspectionID, m_TaskObjectID, this));
			}
			AddPage( pConditionalDlg );
			AddPage( new SvOg::SVToolAdjustmentDialogGeneralPageClass( m_InspectionID, m_TaskObjectID ) );
			break;

		case SvDef::SVToolCylindricalObjectType:
			AddPage( new SvOg::SVToolAdjustmentDialogImagePageClass( m_InspectionID, m_TaskObjectID ) );
			AddPage( new SvOg::SVCylindricalWarpDlg( m_InspectionID, m_TaskObjectID ) );
			AddPage( pConditionalDlg );
			AddPage( new SvOg::SVToolAdjustmentDialogGeneralPageClass( m_InspectionID, m_TaskObjectID ) );
			break;

		case SvDef::SVPerspectiveToolObjectType:
			AddPage( new SvOg::SVToolAdjustmentDialogImagePageClass( m_InspectionID, m_TaskObjectID ) );
			AddPage( new SvOg::SVPerspectiveWarpPage( m_InspectionID, m_TaskObjectID ) );
			AddPage( pConditionalDlg );
			AddPage( new SvOg::SVToolAdjustmentDialogGeneralPageClass( m_InspectionID, m_TaskObjectID ) );
			break;

		case SvDef::SVExternalToolObjectType:
			AddPage( new SVExternalToolDlg( m_InspectionID, m_TaskObjectID, this ));
			AddPage( pConditionalDlg );
			AddPage( new SvOg::SVToolAdjustmentDialogGeneralPageClass( m_InspectionID, m_TaskObjectID ) );
			break;

		case SvDef::SVRemoteInputToolObjectType:
			AddPage( new SVTADlgRemoteInputToolPage( m_InspectionID, m_TaskObjectID, this ) );
			AddPage( pConditionalDlg );
			break;

		case SvDef::SVRingBufferToolObjectType:
			AddPage( new SvOg::SVToolAdjustmentDialogImagePageClass( m_InspectionID, m_TaskObjectID ) );
			AddPage( new SvOg::TADialogRingBufferParameterPage( m_InspectionID, m_TaskObjectID ) );
			AddPage( pConditionalDlg );
			break;

		case SvDef::SVTableToolObjectType:
			AddPage( new SvOg::TADialogTableParameterPage( m_InspectionID, m_TaskObjectID ) );
			AddPage( new SvOg::TADialogTableDefinesPage( m_InspectionID, m_TaskObjectID ) );
			AddPage( pConditionalDlg );
			break;

		case SvDef::SVTableAnalyzerToolObjectType:
			AddPage( new SvOg::TATableSourcePage( m_InspectionID, m_TaskObjectID ) );
			AddPage( new SvOg::TaTableAnalyzerPage( m_InspectionID, m_TaskObjectID ) );
			AddPage( new SvOg::SVToolAdjustmentDialogPassFailPageClass( m_InspectionID, m_TaskObjectID, IDS_TABNAME_TABLE_ROWRANGE) );
			AddPage( pConditionalDlg );
			break;

		default:
			SvStl::MessageMgrStd Msg( SvStl::LogAndDisplay );
			Msg.setMessage( SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_Error_CannotOpenTADlg, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_10218 );
			delete pConditionalDlg;
			pConditionalDlg = nullptr;
			break;
	}

	AddPage( new SvOg::SVToolAdjustmentDialogCommentPage(m_InspectionID, m_TaskObjectID));
}
	
BOOL SVToolAdjustmentDialogSheetClass::OnInitDialog() 
{
	BOOL bResult = CPropertySheet::OnInitDialog();

	// Disable and Hide Cancel Button
	HWND hWnd = ::GetDlgItem(m_hWnd, IDCANCEL);
	if (nullptr != hWnd)
	{
		::EnableWindow( hWnd, FALSE );
		::ShowWindow(hWnd, SW_HIDE );
	}
	// Remove Close Button
	ModifyStyle( WS_SYSMENU, 0, SWP_FRAMECHANGED );

	// Send Errors to the display
	SVToolClass* pTool = GetTool();
	

	CString l_Temp = _T( "Tool Adjustment: " );
	l_Temp += pTool->GetObjectName();
	l_Temp += _T(" - ");
	l_Temp += pTool->GetName();

	SetWindowText( l_Temp );
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

	for( int i = 0;i < cnt; i++ )
	{
		CPropertyPage* pPage = GetPage(i);
		if( pPage && pPage->GetSafeHwnd() ) 
		{
			if( SvOg::SVFormulaEditorPageClass* l_pFormulaEditor = dynamic_cast <SvOg::SVFormulaEditorPageClass*> ( pPage ) )
			{
				if( !l_pFormulaEditor->validateAndSetEquation() )
				{
					// Equation must be valid or disabled
					SvStl::MessageMgrStd Msg( SvStl::LogAndDisplay );
					Msg.setMessage( SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_Error_InvalidFormula, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_10219 );
					return;
				}
			}
			else
			{
				if ( SvOg::ISVPropertyPageDialog* pIDlg = dynamic_cast <SvOg::ISVPropertyPageDialog*> ( pPage ) )
				{
					if ( pIDlg->QueryAllowExit() == false )	// exit not allowed
					{
						return;
					}
				}
				if( CPropertyPage* l_pPropertyPage = dynamic_cast <CPropertyPage*> ( pPage ) )
				{
					l_pPropertyPage->OnOK();
				}
			}
		}
	}

	SVImageExtentClass oldImageExtend;
	SVToolClass* pTool = GetTool();
	pTool->GetImageExtent(oldImageExtend);
	bool resetResult = false;
	if ( nullptr != pTool )
	{
		resetResult = pTool->resetAllObjects();
	}
		
	if( resetResult )
	{
		SVIPDoc* pDocument = GetIPDoc();
		if (pDocument)
		{
			pDocument->SetModifiedFlag();
			pDocument->RunOnce();
		}
		
		EndDialog( IDOK );
	}
	else
	{
		SvStl::MessageContainer message = pTool->getFirstTaskMessage();
		SvStl::MessageTextEnum textEnum = SvStl::Tid_Empty;
		SvDef::StringVector textList; 
		if (0 != message.getMessage().m_MessageCode)
		{
			textEnum = message.getMessage().m_AdditionalTextId;
			textList = message.getMessage().m_AdditionalTextList;
		}
		SvStl::MessageMgrStd messageMgr(  SvStl::LogAndDisplay );
		INT_PTR result = messageMgr.setMessage( SVMSG_SVO_104_TA_DIALOG_CLOSING_ERROR, textEnum, textList, SvStl::SourceFileParams(StdMessageParams), 0, pTool->GetUniqueObjectID(), MB_YESNO );
		if (IDYES == result)
		{
			EndDialog( IDOK );
		}
	}
}

void SVToolAdjustmentDialogSheetClass::OnCancel() 
{
	EndDialog( IDCANCEL );
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

// Temporary solution until all dialogs are converted
SVToolClass* SVToolAdjustmentDialogSheetClass::GetTool() const
{
	SVToolClass* pTool = static_cast<SVToolClass *>(SVObjectManagerClass::Instance().GetObject(m_TaskObjectID));
	return pTool;
}

const GUID& SVToolAdjustmentDialogSheetClass::GetInspectionID() const
{
	return m_InspectionID;
}

const GUID& SVToolAdjustmentDialogSheetClass::GetToolID() const
{
	return m_TaskObjectID;
}

