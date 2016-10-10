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
#include "SVTool.h"
#include "SVCylindricalWarpDlg.h"
#include "SVExternalToolDlg.h"
#include "SVPerspectiveWarpDlg.h"
#include "SVTADlgAcquisitionSourcePage.h"
#include "SVTADlgColorToolPage.h"
#include "SVTADlgLUTPage.h"
#include "SVTADlgPolarTransformPage.h"
#include "SVTADlgRemoteInputToolPage.h"
#include "SVTADlgRotationPage.h"
#include "SVTADlgStatisticsPage.h"
#include "SVTADlgThresholdPage.h"
#include "SVTADlgTransformationLearnPage.h"
#include "SVTADlgTranslationPage.h"
#include "SVToolAdjustmentDialogLinearSpecialPageClass.h"
#include "SVToolAdjustmentDialogMaskPageClass.h"
#include "SVTADlgArchiveResultsPage.h"
#include "SVTADlgArchiveImagePage.h"
#include "SVToolAdjustmentDialogCommentPageClass.h"
#include "SVTADlgTranslationShiftPage.h"
#include "SVTADlgTranslationResizePage.h"
#include "SVToolAdjustmentDialogSizePage.h"
#include "SVToolAdjustmentDialogAnalyzerPageClass.h"
#include "SVOGui\ISVPropertyPageDialog.h"
#include "SvOGui\SVTADlgGeneralPage.h"
#include "SVOGui\SVTADlgPassFailPage.h"
#include "SvOGui\SVFormulaEditorPage.h"
#include "SVOGui\SVToolAdjustmentDialogFilterPageClass.h"
#include "SVOGui\TADialogRingBufferParameterPage.h"
#include "SVOGui\SVToolAdjustmentDialogImagePageClass.h"
#include "SVOGui\SVToolAdjustmentDialogTwoImagePage.h"
#include "SVOGui\SVTADlgTransformImagePage.h"
#include "SVOGui\SVTADlgFileImageSourcePage.h"
#include "SVStatusLibrary\MessageManager.h"
#include "TextDefinesSvO.h"
#include "ObjectInterfaces\ErrorNumbers.h"
#include "SVStatusLibrary\MessageManager.h"
#include "SVOGui\TextDefinesSvOg.h"
#include "SVOGui\TADialogTableParameterPage.h"
#include "SVOGui\TADialogTableDefinesPage.h"
#include "SVOGui\TATableSourcePage.h"
#include "SVOGui\TATableAnalyzerPage.h"
#include "SVUtilityLibrary\SVString.h"
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
	SvOg::SVFormulaEditorPageClass* pConditionalDlg = new SvOg::SVFormulaEditorPageClass( m_InspectionID, m_TaskObjectID, new SvOg::FormulaController(m_InspectionID, m_TaskObjectID, SVObjectTypeInfoStruct(SVEquationObjectType, SVConditionalObjectType), false), true, IDS_CONDITIONAL_STRING, IDS_TOOL_STRING );

	// Get LUT Operator...RO_22Mar2000
	BOOL bHasLUT = FALSE;
	SVObjectTypeInfoStruct lutObjectInfo;
	lutObjectInfo.ObjectType = SVUnaryImageOperatorObjectType;
	lutObjectInfo.SubType	 = SVLUTOperatorObjectType;
	if( ::SVSendMessage( GetTool(), SVM_GETFIRST_OBJECT, 0, reinterpret_cast<DWORD_PTR>(&lutObjectInfo) ) )
	{
		bHasLUT = TRUE;
	}

	bool bHasSize = false;
	SVObjectTypeInfoStruct ToolSizeAdjustTaskInfo;
	ToolSizeAdjustTaskInfo.ObjectType = SVToolSizeAdjustTaskType;

	if( ::SVSendMessage( GetTool(), SVM_GETFIRST_OBJECT, 0, reinterpret_cast<DWORD_PTR>(&ToolSizeAdjustTaskInfo) ) )
	{
		bHasSize = true;
	}


	const SVObjectTypeInfoStruct& rToolType = GetTool()->GetObjectInfo().ObjectTypeInfo;
	switch( rToolType.SubType )
	{
		case SVToolAcquisitionObjectType:
			AddPage( new SVToolAdjustmentDialogAcquisitionSourcePageClass( m_InspectionID, m_TaskObjectID, this ) );
			// NOTE:
			// No Conditional Execution for Acquisition Tool !!!!
			delete pConditionalDlg;
			pConditionalDlg = nullptr;
			// NOTE:
			// No General page for acquisition tool!
			// AddPage( new SvOg::SVToolAdjustmentDialogGeneralPageClass( m_InspectionID, m_TaskObjectID ) );
			break;

		case SVToolImageObjectType:	// Image Tool
			AddPage( new SvOg::SVToolAdjustmentDialogTwoImagePageClass( m_InspectionID, m_TaskObjectID ) );
			if(bHasSize)
			{
				AddPage(new SVToolAdjustmentDialogSizePage(m_InspectionID, m_TaskObjectID, this));
			}
			if( bHasLUT )
			{
				// New image tool has also this pages...RO_22Mar2000
				AddPage( new SvOg::SVToolAdjustmentDialogFilterPageClass( m_InspectionID, m_TaskObjectID ) );
				AddPage( new SVToolAdjustmentDialogThresholdPageClass( m_InspectionID, m_TaskObjectID, this ) );
				AddPage( new SVToolAdjustmentDialogMaskPageClass( m_InspectionID, m_TaskObjectID ) );
				AddPage( new SVToolAdjustmentDialogLUTPageClass( m_InspectionID, m_TaskObjectID, this ) );
			}
			AddPage( pConditionalDlg );
			AddPage( new SvOg::SVToolAdjustmentDialogGeneralPageClass( m_InspectionID, m_TaskObjectID ) );
			break;

		case SVWindowToolObjectType:
			AddPage( new SvOg::SVToolAdjustmentDialogImagePageClass( m_InspectionID, m_TaskObjectID ) );
			if(bHasSize)
			{
				AddPage(new SVToolAdjustmentDialogSizePage(m_InspectionID, m_TaskObjectID, this));
			}
			AddPage( new SvOg::SVToolAdjustmentDialogFilterPageClass( m_InspectionID, m_TaskObjectID ) );
			AddPage( new SVToolAdjustmentDialogThresholdPageClass( m_InspectionID, m_TaskObjectID, this ) );
			AddPage( new SVToolAdjustmentDialogMaskPageClass( m_InspectionID, m_TaskObjectID ) );
			if( bHasLUT )
			{
				// To be compatible to old Window Tool...RO_22Mar2000
				AddPage( new SVToolAdjustmentDialogLUTPageClass( m_InspectionID, m_TaskObjectID, this ) );
			}
			AddPage( new SVToolAdjustmentDialogAnalyzerPageClass( m_InspectionID, m_TaskObjectID, this ) );
			AddPage( pConditionalDlg );
			AddPage( new SvOg::SVToolAdjustmentDialogGeneralPageClass( m_InspectionID, m_TaskObjectID ) );
			break;

		case SVLinearToolObjectType:
			AddPage( new SvOg::SVToolAdjustmentDialogImagePageClass( m_InspectionID, m_TaskObjectID ) );
			if(bHasSize)
			{
				AddPage(new SVToolAdjustmentDialogSizePage(m_InspectionID, m_TaskObjectID, this));
			}
			AddPage( new SvOg::SVToolAdjustmentDialogFilterPageClass( m_InspectionID, m_TaskObjectID ) );
			AddPage( new SVToolAdjustmentDialogThresholdPageClass( m_InspectionID, m_TaskObjectID, this ) );
			AddPage( new SVToolAdjustmentDialogAnalyzerPageClass( m_InspectionID, m_TaskObjectID, this ) );
			//add the new page.
			AddPage( new SVToolAdjustmentDialogLinearSpecialPageClass(m_InspectionID, m_TaskObjectID, this) );
			AddPage( pConditionalDlg );
			AddPage( new SvOg::SVToolAdjustmentDialogGeneralPageClass( m_InspectionID, m_TaskObjectID ) );
			break;

		case SVToolLoadImageObjectType:
			AddPage( new SvOg::SVToolAdjustmentDialogFileImageSourcePageClass( m_InspectionID, m_TaskObjectID ) );
			if(bHasSize)
			{
				AddPage(new SVToolAdjustmentDialogSizePage(m_InspectionID, m_TaskObjectID, this));
			}
			AddPage( pConditionalDlg );
			AddPage( new SvOg::SVToolAdjustmentDialogGeneralPageClass( m_InspectionID, m_TaskObjectID ) );
			break;

		case SVToolArchiveObjectType:   // Archive tool
			AddPage( new SVTADlgArchiveResultsPage( m_InspectionID, m_TaskObjectID, this) );
			AddPage( new SVTADlgArchiveImagePage( m_InspectionID, m_TaskObjectID, this) );
			AddPage( pConditionalDlg );
			break;

		case SVMathToolObjectType:
			{
				SVObjectTypeInfoStruct info(SVEquationObjectType, SVMathEquationObjectType);
				AddPage( new SvOg::SVFormulaEditorPageClass( m_InspectionID, m_TaskObjectID, new SvOg::FormulaController(m_InspectionID, m_TaskObjectID, info), false ) );
				AddPage( new SvOg::SVToolAdjustmentDialogPassFailPageClass( m_InspectionID, m_TaskObjectID) );
				AddPage( pConditionalDlg );
			}
			break;

		case SVStatisticsToolObjectType:
			AddPage( new SVToolAdjustmentDialogStatisticsPageClass( m_InspectionID, m_TaskObjectID,this ) );
			AddPage( pConditionalDlg );
			break;

		case SVShiftToolObjectType:
			AddPage( new SvOg::SVToolAdjustmentDialogImagePageClass( m_InspectionID, m_TaskObjectID ) );
			if(bHasSize)
			{
				AddPage(new SVToolAdjustmentDialogSizePage(m_InspectionID, m_TaskObjectID, this));
			}			
			AddPage( new SVTADlgTranslationShiftPageClass( m_InspectionID, m_TaskObjectID, this ) );
			AddPage( pConditionalDlg );
			AddPage( new SvOg::SVToolAdjustmentDialogGeneralPageClass( m_InspectionID, m_TaskObjectID ) );
			break;

		case SVResizeToolObjectType:
			AddPage( new SvOg::SVToolAdjustmentDialogImagePageClass( m_InspectionID, m_TaskObjectID ) );
			if (bHasSize)
			{
				AddPage(new SVToolAdjustmentDialogSizePage(m_InspectionID, m_TaskObjectID, this));
			}
			AddPage( new SVTADlgTranslationResizePage( m_InspectionID, m_TaskObjectID, this ) );
			AddPage( pConditionalDlg );
			AddPage( new SvOg::SVToolAdjustmentDialogGeneralPageClass( m_InspectionID, m_TaskObjectID ) );
			break;

		case SVTransformationToolObjectType:
			AddPage( new SvOg::SVToolAdjustmentDialogTransformImagePageClass( m_InspectionID, m_TaskObjectID ) );
			AddPage( new SVToolAdjustmentDialogTranslationPageClass( m_InspectionID, m_TaskObjectID, this ) );
			AddPage( new SVToolAdjustmentDialogRotationPageClass( m_InspectionID, m_TaskObjectID, this ) );
			AddPage( new SVToolAdjustmentDialogTransformationLearnPageClass( m_InspectionID, m_TaskObjectID, this ) );
			AddPage( pConditionalDlg );
			AddPage( new SvOg::SVToolAdjustmentDialogGeneralPageClass( m_InspectionID, m_TaskObjectID ) );
			break;

		case SVPolarTransformationToolObjectType:
			AddPage( new SvOg::SVToolAdjustmentDialogImagePageClass( m_InspectionID, m_TaskObjectID ) );
			AddPage( new SVToolAdjustmentDialogPolarTransformPageClass( m_InspectionID, m_TaskObjectID, this ) );
			AddPage( pConditionalDlg );
			AddPage( new SvOg::SVToolAdjustmentDialogGeneralPageClass( m_InspectionID, m_TaskObjectID ) );
			break;

		case SVColorToolObjectType:
			AddPage( new SVTADlgColorToolPageClass( m_InspectionID, m_TaskObjectID ) );
			AddPage( pConditionalDlg );
			AddPage( new SvOg::SVToolAdjustmentDialogGeneralPageClass( m_InspectionID, m_TaskObjectID ) );
			break;

		case SVToolCylindricalObjectType:
			AddPage( new SvOg::SVToolAdjustmentDialogImagePageClass( m_InspectionID, m_TaskObjectID ) );
			AddPage( new SVCylindricalWarpDlg( m_InspectionID, m_TaskObjectID, this ) );
			AddPage( pConditionalDlg );
			AddPage( new SvOg::SVToolAdjustmentDialogGeneralPageClass( m_InspectionID, m_TaskObjectID ) );
			break;

		case SVPerspectiveToolObjectType:
			AddPage( new SvOg::SVToolAdjustmentDialogImagePageClass( m_InspectionID, m_TaskObjectID ) );
			AddPage( new SVPerspectiveWarpDlg( m_InspectionID, m_TaskObjectID, this ) );
			AddPage( pConditionalDlg );
			AddPage( new SvOg::SVToolAdjustmentDialogGeneralPageClass( m_InspectionID, m_TaskObjectID ) );
			break;

		case SVExternalToolObjectType:
			AddPage( new SVExternalToolDlg( m_InspectionID, m_TaskObjectID, this ));
			AddPage( pConditionalDlg );
			AddPage( new SvOg::SVToolAdjustmentDialogGeneralPageClass( m_InspectionID, m_TaskObjectID ) );
			break;

		case SVRemoteInputToolObjectType:
			AddPage( new SVTADlgRemoteInputToolPage( m_InspectionID, m_TaskObjectID, this ) );
			AddPage( pConditionalDlg );
			break;

		case SVRingBufferToolObjectType:
			AddPage( new SvOg::SVToolAdjustmentDialogImagePageClass( m_InspectionID, m_TaskObjectID ) );
			AddPage( new SvOg::TADialogRingBufferParameterPage( m_InspectionID, m_TaskObjectID ) );
			AddPage( pConditionalDlg );
			break;

		case SVTableToolObjectType:
			AddPage( new SvOg::TADialogTableParameterPage( m_InspectionID, m_TaskObjectID ) );
			AddPage( new SvOg::TADialogTableDefinesPage( m_InspectionID, m_TaskObjectID ) );
			AddPage( pConditionalDlg );
			break;

		case SVTableAnalyzerToolObjectType:
			AddPage( new SvOg::TATableSourcePage( m_InspectionID, m_TaskObjectID ) );
			AddPage( new SvOg::TaTableAnalyzerPage( m_InspectionID, m_TaskObjectID ) );
			AddPage( new SvOg::SVToolAdjustmentDialogPassFailPageClass( m_InspectionID, m_TaskObjectID, IDS_TABNAME_TABLE_ROWRANGE) );
			AddPage( pConditionalDlg );
			break;

		default:
			SvStl::MessageMgrStd Msg( SvStl::LogAndDisplay );
			Msg.setMessage( SVMSG_SVO_93_GENERAL_WARNING, SvOi::Tid_Error_CannotOpenTADlg, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_10218 );
			delete pConditionalDlg;
			pConditionalDlg = nullptr;
			break;
		}

		AddPage( new SVToolAdjustmentDialogCommentPageClass(this));
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
						Msg.setMessage( SVMSG_SVO_93_GENERAL_WARNING, SvOi::Tid_Error_InvalidFormula, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_10219 );
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
		DWORD_PTR dwRet =  ::SVSendMessage( pTool, SVM_RESET_ALL_OBJECTS, 0, 0 );
		
		if( SVMR_SUCCESS == dwRet )
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
			SvOi::MessageTextEnum textEnum = SvOi::Tid_Empty;
			SVStringArray textList; 
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

