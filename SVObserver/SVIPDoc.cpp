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
#include "stdafx.h"		// SVClids.h
#include <comdef.h>
#include <algorithm>
#include "SVIPDoc.h"

#include "SVObjectLibrary/SVObjectSynchronousCommandTemplate.h"
#include "SVObjectLibrary/SVObjectWriter.h"
#include "SVTimerLibrary/SVClock.h"
#include "SVUtilityLibrary/SVGUID.h"

#include "SVConditional.h"
#include "SVFileNameManagerClass.h"
#include "SVGlobal.h"
#include "SVImageArchive.h"
#include "SVImageViewScroll.h"
#include "SVImageView.h"
#include "SVImageProcessingClass.h"
#include "SVIPChildFrm.h"
#include "SVLightReferenceDialog.h"
#include "SVLutDlg.h"
#include "SVMainFrm.h"
#include "SVObjectLibrary/SVObjectXMLWriter.h"
#include "SVObjectLibrary/SVObjectManagerClass.h"
#include "SVObjectScriptParser.h"
#include "SVObserver.h"
#include "SVToolAdjustmentDialogSheetClass.h"
#include "SVToolSet.h"
#include "SVToolSetAdjustmentDialogSheet.h"
#include "ToolSetView.h"
#include "SVResultView.h"
#include "SVResultsWrapperClass.h"
#include "SVTool.h"
#include "SVOGui\SVSaveToolSetImageDialog.h"
#include "SVOGui\SVShowDependentsDialog.h"
#include "SVUtilities.h"
#include "SVInspectionProcess.h"
#include "SVPPQObject.h"
#include "SVConfigurationLibrary/SVConfigurationTags.h"
#include "SVSVIMStateClass.h"
#include "SVAcquisitionClass.h"
#include "SVConfigurationObject.h"
#include "SVIODoc.h"
#include "SVXMLLibrary/SVNavigateTree.h"
#include "SVMessage/SVMessage.h"
#include "ObjectInterfaces/SVUserMessage.h"
#include "SVImageLibrary/SVImagingDeviceParams.h"
#include "SVObjectLibrary/SVAnalyzerLevelCreateStruct.h"
#include "SVObjectLibrary/SVInspectionLevelCreateStruct.h"
#include "SVLinearToolClass.h"
#include "SVAdjustToolSizePositionDlg.h"
#include "SVConditionalHistorySheet.h"
#include "SVDataDefinitionSheet.h"
#include "SVRegressionRunDlg.h"
#include "SVHBitmapUtilitiesLibrary\SVHBitmapUtilities.h"
#include "SVDirectX.h"
#include "SVCommandInspectionCollectImageData.h"
#include "GuiCommands/InspectionRunOnce.h"
#include "SVGuiExtentUpdater.h"
#include "SVArchiveTool.h"
#include "SVColorTool.h"
#include "SVMathTool.h"
#include "SVPolarTransformationTool.h"
#include "SVTransformationTool.h"
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
#include "SVObjectLibrary/GlobalConst.h"
#include "SVTreeLibrary/ObjectSelectorItem.h"
#include "RootObject.h"
#include "ToolClipboard.h"
#include "ExtrasEngine.h"
#include "TextDefinesSvO.h"
#include "SVShiftTool.h"
#include "SVShiftToolUtility.h"
#include "RingBufferTool.h"
#include "SVOGui/NoSelector.h"
#include "SVOGui/GlobalSelector.h"
#include "SVOGui/PPQSelector.h"
#include "SVOGui/ToolSetItemSelector.h"
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
	SVResultViewClass *pResultView;
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
	ON_COMMAND(ID_EDIT_DELETE, OnEditDelete)
	ON_COMMAND(ID_EDIT_ADJUSTLIGHTREFERENCE, OnAdjustLightReference)
	ON_COMMAND(ID_EDIT_ADJUSTLUT, OnAdjustLut)
	ON_COMMAND(ID_FILE_SAVE_IMAGE, OnFileSaveImage)
	ON_COMMAND(ID_ADD_IMAGETOOL, OnAddImageTool)
	ON_COMMAND(ID_ADD_ARCHIVETOOL,OnAddArchiveTool)
	ON_COMMAND(ID_ADD_MATHTOOL,OnAddMathTool)
	ON_COMMAND(ID_ADD_STATISTICSTOOL,OnAddStatisticsTool)
	ON_COMMAND(ID_EDIT_EDITTOOLSETCONDITION, OnEditToolSetCondition)
	ON_COMMAND(ID_RESULTS_PICKER, OnResultsPicker)
	ON_COMMAND(ID_PUBLISHED_RESULTS_PICKER, OnPublishedResultsPicker)
	ON_COMMAND(ID_PUBLISHED_RESULT_IMAGES_PICKER, OnPublishedResultImagesPicker)
	ON_COMMAND(ID_EDIT_CONDITIONAL_HISTORY, OnConditionalHistory)
	ON_UPDATE_COMMAND_UI(ID_EDIT_CONDITIONAL_HISTORY, OnUpdateConditionalHistory)
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
	ON_COMMAND_RANGE( ID_VIEW_TOOLSETDRAW_POP_BASE, ID_VIEW_TOOLSETDRAW_POP_MAX, OnChangeToolSetDrawFlag )
	ON_UPDATE_COMMAND_UI_RANGE( ID_VIEW_TOOLSETDRAW_POP_BASE + 1, ID_VIEW_TOOLSETDRAW_POP_MAX, OnUpdateViewToolSetDrawSubMenus )
	ON_UPDATE_COMMAND_UI(ID_EDIT_DATA_DEFINITION_LISTS, &SVIPDoc::OnUpdateEditDataDefinitionLists)
	ON_COMMAND(ID_ADD_REMOTEINPUTTOOL, OnAddRemoteInputTool)
	ON_COMMAND(ID_ADD_RESIZETOOL, &SVIPDoc::OnAddResizetool)
	ON_COMMAND(ID_ADD_RINGBUFFERTOOL, OnAddRingBufferTool)
	ON_UPDATE_COMMAND_UI(ID_EDIT_ADJUSTLIGHTREFERENCE, OnAllowAdjustLightReference)
	ON_UPDATE_COMMAND_UI(ID_EDIT_ADJUSTLUT, OnAllowAdjustLut)
	ON_UPDATE_COMMAND_UI(ID_ADD_STARTTOOLGROUPING, OnUpdateAddStartToolGrouping)
	ON_UPDATE_COMMAND_UI(ID_ADD_ENDTOOLGROUPING, OnUpdateAddEndToolGrouping)
	ON_COMMAND(ID_ADD_STARTTOOLGROUPING, OnAddStartToolGrouping)
	ON_COMMAND(ID_ADD_ENDTOOLGROUPING, OnAddEndToolGrouping)
END_MESSAGE_MAP()
#pragma endregion Declarations

#pragma region Constructor
SVIPDoc::SVIPDoc()
: CDocument()
, m_NewProductData( 1 )
, m_RegisteredImages()
, m_Images()
, m_AllViewsUpdated( 0 )
, m_pMDIChildWnd( nullptr )
, m_ResultDefinitionsTimestamp( 0 )
, m_ToolSetListTimestamp( 0 )
, m_PPQListTimestamp( 0 )
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

	if( m_NewProductData.PopHead( l_ProductData ) == S_OK )
	{
		SVIPImageDataElementMap::const_iterator l_Iter = l_ProductData.m_ImageData.begin();

		while( l_Iter != l_ProductData.m_ImageData.end() )
		{
			SetImageData( l_Iter->first, l_Iter->second.m_ImageDIB, l_Iter->second.m_OverlayData );

			++l_Iter;
		}

		m_Results = l_ProductData.m_ResultData;

		if( !( l_ProductData.m_ImageData.empty() ) )
		{
			::InterlockedExchange( &m_AllViewsUpdated, 0 );
		}
		else
		{
			::InterlockedExchange( &m_AllViewsUpdated, 1 );

			m_oDisplay.SetIPDocDisplayComplete();
		}

		UpdateAllViews( nullptr );
	}
}

void SVIPDoc::SetInspectionID( const SVGUID& p_InspectionID )
{
	SVObjectManagerClass::Instance().UnregisterIPDoc( m_InspectionID );

	m_InspectionID = p_InspectionID;

	SVObjectManagerClass::Instance().RegisterIPDoc( m_InspectionID, this );

	m_oDisplay.SetInspectionID( m_InspectionID );
}

void SVIPDoc::init()
{
	SVFileNameManagerClass svFileManager;
	svFileManager.AddItem( &msvFileName );

	mbRegressionSingleStep = FALSE;
	mbInitImagesByName = FALSE;

	IsNew = true;

	TheSVObserverApp.setCurrentDocument( this );

	//
	// Default result height and tool set view width.
	//
	m_nHeightResultView = SV_DEFAULT_RESULT_VIEW_HEIGHT;
	m_nWidthToolSetView = SV_DEFAULT_TOOL_SET_VIEW_WIDTH;

	m_bAllowRefresh = false;
	m_bRegressionTestRunning = false;

	m_oDisplay.Create();
}

//******************************************************************************
// Destructor:
//******************************************************************************
SVIPDoc::~SVIPDoc()
{
	::InterlockedExchange( &m_AllViewsUpdated, 1 );
	m_oDisplay.SetIPDocDisplayComplete();

	m_oDisplay.Destroy();

	SVObjectManagerClass::Instance().UnregisterIPDoc( m_InspectionID );

	SVFileNameManagerClass svFileManager;
	svFileManager.RemoveItem( &msvFileName );

	ClearRegressionTestStructures();

	TheSVObserverApp.setCurrentDocument( nullptr );
}

//******************************************************************************
// Operator(s):
//******************************************************************************

CMDIChildWnd* SVIPDoc::GetMDIChild()
{
	if( m_pMDIChildWnd == NULL )
	{
		m_pMDIChildWnd = SVSearchForMDIChildWnd( GetToolSetView() );
	}

	if( m_pMDIChildWnd == NULL )
	{
		m_pMDIChildWnd = SVSearchForMDIChildWnd( GetResultView() );
	}

	if( m_pMDIChildWnd == NULL )
	{
		SVImageViewClass* l_pView( GetImageView( 0 ) );

		m_pMDIChildWnd = SVSearchForMDIChildWnd( l_pView );
	}

	return m_pMDIChildWnd;
}

void SVIPDoc::SetMDIChild( CMDIChildWnd* p_pMDIChildWnd )
{
	m_pMDIChildWnd = p_pMDIChildWnd;
}

////////////////////////////////////////////////////////////////////////////////
// .Description : "Creates this IPDoc" and his embedded tool and freeze set.
//              : Shall become some kind of CreateObject(...) in future time.
////////////////////////////////////////////////////////////////////////////////
BOOL SVIPDoc::InitAfterSystemIsDocked()
{
	// Init Utility and Tool Set Draw menus...
	InitMenu();

	SVInspectionProcess* pInspection( GetInspectionProcess() );

	BOOL l_bOk = ( nullptr != pInspection );

	if( l_bOk )
	{
		SVImageViewClass* pView = nullptr;
		int i;

		if (IsNew)
		{
			pView = GetImageView();

			if( pView != nullptr )
			{
				SVGUID l_ImageId;

				if( GetToolSet()->getCurrentImage() != nullptr )
				{
					l_ImageId = GetToolSet()->getCurrentImage()->GetUniqueObjectID();
				}

				pView->AttachToImage( l_ImageId );

				SVImageViewScroll* pScroll( nullptr );

				pScroll = dynamic_cast< SVImageViewScroll* >( pView->GetParent() );

				if( pScroll != nullptr )
				{
					CWnd* pWnd( pScroll->GetParent() );

					SVIPSplitterFrame* pSplitterFrame( nullptr );

					do
					{
						pSplitterFrame = dynamic_cast< SVIPSplitterFrame* >( pWnd );

						if( pSplitterFrame == nullptr )
						{
							pWnd = pWnd->GetParent();
						}
					}
					while( pWnd != nullptr && nullptr == pSplitterFrame );

					if( pSplitterFrame != nullptr )
					{
						RECT IPRect = {0, 0, 0, 0};
						CMDIChildWnd* l_pMDIChild = GetMDIChild();

						if( l_pMDIChild != nullptr )
						{
							l_pMDIChild->GetWindowRect( &IPRect );
						}

						pSplitterFrame->SetDefaultPaneSizes( IPRect );
					}
				}
			}
		}
		else
		{
			for (i = 0; i < MaxImageViews; ++i)
			{
				pView = GetImageView( i );

				if( pView != nullptr )
				{
					if (mbInitImagesByName)
					{
						pView->AttachToImage( _T( "" ) );
					}
					else
					{
						SVGUID l_ImageId;

						if( GetToolSet()->getCurrentImage() != nullptr )
						{
							l_ImageId = GetToolSet()->getCurrentImage()->GetUniqueObjectID();
						}

						pView->AttachToImage( l_ImageId );
					}
				}
			}
		}
	}

	m_bAllowRefresh = true;

	return l_bOk;
}

SVString SVIPDoc::CheckName( const SVString& rToolName ) const
{
	SVString NewName( rToolName );
	if (!m_toolGroupings.IsNameUnique( rToolName.c_str() ) )
	{
		NewName = m_toolGroupings.MakeNumericUniqueName( rToolName.c_str() ).c_str();
	}

	return NewName;
}

//******************************************************************************
// Operation(s) Of Writing Access:
//******************************************************************************

BOOL SVIPDoc::AddTool(SVToolClass* pTool)
{
	if (nullptr != pTool)
	{
		ToolSetView* pView = GetToolSetView();
		if (pView)
		{
			SVToolSetClass* pToolSet = GetToolSet();

			const ToolListSelectionInfo& info = pView->GetToolListSelectionInfo();
			int toolListIndex = GetToolToInsertBefore(info.m_selection, info.m_listIndex);

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
						AfxMessageBox( SvO::ColorToolMustBeFirstMessage, MB_OK );
						return false;
					}
				}
			}

			SVString ToolName( pTool->GetName() );
			ToolName = CheckName( ToolName );
			pTool->SetName( ToolName.c_str() );

			SVAddTool l_AddTool(pTool, toolListIndex);

			if (S_OK == SVObjectManagerClass::Instance().UpdateObserver(GetInspectionID(), l_AddTool))
			{
				m_toolGroupings.AddTool(pTool->GetName(), static_cast<LPCTSTR>(info.m_selection));

				// Refresh all views...
				UpdateAllViews(nullptr);

				const SVGUID& rGuid = pTool->GetUniqueObjectID();
				SetSelectedToolID(rGuid);

				SetModifiedFlag();

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
	if (pView )
	{
		const ToolListSelectionInfo& rInfo = pView->GetToolListSelectionInfo();
		if (bStartGroup)
		{
			m_toolGroupings.AddGroup(m_toolGroupings.GetDefaultName(), static_cast<LPCTSTR>(rInfo.m_selection));
		}
		else
		{
			// Get Group Name
			CString group = m_toolGroupings.FindGroup(static_cast<LPCTSTR>(rInfo.m_selection)).c_str();
			if (!group.IsEmpty())
			{
				// check if has end already ?
				m_toolGroupings.AddEndGroup(static_cast<LPCTSTR>(group), static_cast<LPCTSTR>(rInfo.m_selection));
			}
		}

		// Refresh all views...
		UpdateAllViews(nullptr, SVIPDoc::RefreshView);

		SetModifiedFlag();

		bRetVal = true;
	}
	return bRetVal;
}

void SVIPDoc::SetTitle( LPCTSTR LPSZTitle )
{
	CDocument::SetTitle( LPSZTitle );
}

void SVIPDoc::SetPathName( LPCTSTR LPSZPathName, BOOL bAddToMRU )
{
	BOOL bMod = GetPathName().CollateNoCase( LPSZPathName ) != 0;

	if ( bMod )
	{
		msvFileName.SetFullFileName( LPSZPathName );

		SVFileNameManagerClass svFileManager;
		svFileManager.LoadItem( &msvFileName );

		SVSVIMStateClass::AddState( SV_STATE_MODIFIED );
	}

	// Never Add To MRU !!!
	CDocument::SetPathName( msvFileName.GetFullFileName(), FALSE );
}

//******************************************************************************
// Operation(s) Of Reading Access:
//******************************************************************************

CView* SVIPDoc::getView() const
{
	CView* retVal( nullptr );
	POSITION pos = GetFirstViewPosition();
	if( pos != nullptr )
	{
		retVal = GetNextView(pos);
	}

	return retVal;
}

ToolSetView* SVIPDoc::GetToolSetView() const
{
	ToolSetView* pView( nullptr );
	POSITION pos( GetFirstViewPosition() );

	while( nullptr == pView && nullptr != pos )
	{
		pView = dynamic_cast< ToolSetView* >( GetNextView( pos ) );
	}

	return pView;
}

SVImageViewClass* SVIPDoc::GetImageView( int p_Index )
{
	SVImageViewClass* pReturnView( nullptr );
	POSITION pos( GetFirstViewPosition() );
	long l_Count( 0 );

	while( nullptr == pReturnView && nullptr != pos )
	{
		SVImageViewClass* pIPView = dynamic_cast<SVImageViewClass*>(GetNextView( pos ));
		if( nullptr != pIPView  )
		{
			if( l_Count == p_Index )
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

SVResultViewClass* SVIPDoc::GetResultView()
{
	SVResultViewClass* pView( nullptr );
	POSITION pos( GetFirstViewPosition() );

	while( nullptr == pView && nullptr != pos )
	{
		pView = dynamic_cast<SVResultViewClass*>(GetNextView( pos ));
	}

	return pView;
}

SVInspectionProcess* SVIPDoc::GetInspectionProcess() const
{
	SVObjectClass* l_pObject = SVObjectManagerClass::Instance().GetObject( m_InspectionID );
	SVInspectionProcess* pInspection( dynamic_cast< SVInspectionProcess* >( l_pObject ) );

	return pInspection;
}

SVToolSetClass* SVIPDoc::GetToolSet() const
{
	SVToolSetClass* l_pObject( nullptr );
	SVInspectionProcess* pInspection( GetInspectionProcess() );

	if( nullptr != pInspection )
	{
		l_pObject = pInspection->GetToolSet();
	}

	return l_pObject;
}

SVResultListClass* SVIPDoc::GetResultList() const
{
	SVResultListClass* pResultList( nullptr );
	SVToolSetClass* pToolSet = GetToolSet();
	if(pToolSet)
	{
		pResultList = pToolSet->GetResultList();
	}
	return pResultList;
}

SVConditionalClass* SVIPDoc::GetToolSetCondition()
{
	SVConditionalClass* l_pObject( nullptr );
	SVToolSetClass* l_pToolSet( GetToolSet() );

	if( l_pToolSet != nullptr )
	{
		l_pObject = l_pToolSet->GetToolSetConditional();
	}

	return l_pObject;
}

//******************************************************************************
// Operation(s) Of Process:
//******************************************************************************

BOOL SVIPDoc::GoOnline()
{
	return m_oDisplay.GoOnline();
}

BOOL SVIPDoc::GoOffline()
{
	return m_oDisplay.GoOffline();
}

BOOL SVIPDoc::CanCloseFrame(CFrameWnd* pFrame)
{
	BOOL bCanClose = FALSE;

	if( TheSVObserverApp.m_svSecurityMgr.SVIsDisplayable( SECURITY_POINT_FILE_MENU_CLOSE_CONFIGURATION ) )
	{
		if( SVSVIMStateClass::CheckState( SV_STATE_CANCELING ) || SVSVIMStateClass::CheckState( SV_STATE_CLOSING ) )
		{
			bCanClose = CDocument::CanCloseFrame(pFrame);
		}
	}

	if (!bCanClose)
	{
		AfxMessageBox(_T("Invalid Operation!\n")
			_T("If You wish to remove this inspection\n")
			_T("Go to the Edit menu and Select Edit Configuration."), MB_OK|MB_ICONSTOP, 0);
	}

	return bCanClose;
}

BOOL SVIPDoc::SaveModified()
{
	// Don't save modified
	if ( SVSVIMStateClass::CheckState( SV_STATE_CANCELING ) ) { return TRUE; }

	int index = 0;
	if( ( index = m_strTitle.Find( _TCHAR( '.' ) ) ) >= 0 )
	{
		m_strTitle = m_strTitle.Left( index );
	}

	return CDocument::SaveModified();
}

BOOL SVIPDoc::OnNewDocument()
{
	if( ! CDocument::OnNewDocument() ) { return FALSE; }

	// ZU ERLEDIGEN: Hier Code zur Reinitialisierung einfügen
	// (SDI-Dokumente verwenden dieses Dokument)

	return TRUE;
}

void SVIPDoc::OnCloseDocument()
{
	if( this )
	{
		// Do not save unsaved IPDoc without updating config
		if( CString( TheSVObserverApp.getConfigFullFileName() ).IsEmpty() && 
			!SVSVIMStateClass::CheckState( SV_STATE_CANCELING ) )
		{
			TheSVObserverApp.OnFileSaveConfig();
		}

		Sleep( 100 );
	}

	CDocument::OnCloseDocument();
}

void SVIPDoc::CloseDocument()
{
	if( !SVSVIMStateClass::CheckState( SV_STATE_CANCELING | SV_STATE_CLOSING ) &&
		!( TheSVObserverApp.m_svSecurityMgr.SVValidate( SECURITY_POINT_FILE_MENU_CLOSE_CONFIGURATION ) == S_OK ) )
	{
		return;
	}

	OnFileClose();
}

void SVIPDoc::OnUpdateStatusInfo( CCmdUI *pCmdUI )
{
	switch( pCmdUI->m_nID )
	{
		case ID_INDICATOR_INFO:
		{
			( ( SVMainFrame* ) AfxGetApp()->m_pMainWnd )->OnUpdateStatusInfo( pCmdUI );
			break;
		}
		default:
		{
			// Do nothing.
			break;
		}
	};
}

void SVIPDoc::OnUpdateRunRegressionTest( CCmdUI* PCmdUI )
{
	// @WARNING:  Pointers should be checked before they are dereferenced.
	PCmdUI->Enable(TheSVObserverApp.m_svSecurityMgr.SVIsDisplayable( SECURITY_POINT_MODE_MENU_REGRESSION_TEST ));
	PCmdUI->SetCheck( SVSVIMStateClass::CheckState( SV_STATE_REGRESSION ) );
}

void SVIPDoc::OnAllowAdjustLightReference(CCmdUI* pCmdUI)
{
	bool bEnable = ( ! SVSVIMStateClass::CheckState( SV_STATE_RUNNING | SV_STATE_REGRESSION ) &&
					SVSVIMStateClass::CheckState( SV_STATE_TEST | SV_STATE_EDIT) &&
					TheSVObserverApp.m_svSecurityMgr.SVIsDisplayable(SECURITY_POINT_MODE_MENU_EDIT_TOOLSET) );
	if (bEnable)
	{
		SVVirtualCameraPtrSet cameraSet;
		SVInspectionProcess* pInspection( GetInspectionProcess() );
		HRESULT hr = S_FALSE;
		if( nullptr != pInspection ){ hr = pInspection->GetCamerasForLightReference(cameraSet); }
		bEnable = (hr == S_OK && cameraSet.size() > 0) ? true : false;
	}
	pCmdUI->Enable(bEnable);
}

void SVIPDoc::OnAllowAdjustLut(CCmdUI* pCmdUI)
{
	bool bEnable = ( ! SVSVIMStateClass::CheckState( SV_STATE_RUNNING | SV_STATE_REGRESSION ) &&
					SVSVIMStateClass::CheckState( SV_STATE_TEST | SV_STATE_EDIT) &&
					TheSVObserverApp.m_svSecurityMgr.SVIsDisplayable(SECURITY_POINT_MODE_MENU_EDIT_TOOLSET) );
	if (bEnable)
	{
		SVVirtualCameraPtrSet cameraSet;
		SVInspectionProcess* pInspection( GetInspectionProcess() );
		HRESULT hr = S_FALSE;
		if( nullptr != pInspection ){ hr = pInspection->GetCamerasForLut(cameraSet); }
		bEnable = (hr == S_OK && cameraSet.size() > 0) ? true : false;
	}
	pCmdUI->Enable(bEnable);
}

void SVIPDoc::OnAdjustLightReference()
{
	SVInspectionProcess* pInspection( GetInspectionProcess() );

	int i( 0 );

	ASSERT( GetToolSet() );
	ASSERT( GetToolSet()->IsCreated() );

	if( nullptr == pInspection || !GetToolSet()->IsCreated() ) { return; }

	// NOTE:
	//		 At this time an IPDoc has only one camera image input!!!

	int iChannel = 0;
	// Show extreme LUT: black --> blue, white --> green...
	GetImageView()->ShowExtremeLUT();

	SVLightReferenceDialogPropertySheetClass dlg( CString( _T( "Light Reference Adjustment - [" ) + GetTitle() + _T( "]" ) ));

	// obtain copies of the arrays to set if IDOK
	SVLightReferenceArray apLRA;
	SVLightReferenceArray apLRAorig;

	SVVirtualCameraPtrSet setCameras;

	pInspection->GetCameras( setCameras );

	SVVirtualCameraPtrSet::iterator l_Iter = setCameras.begin();

	while( l_Iter != setCameras.end() )
	{
		if( ( *l_Iter ) != NULL )
		{
			SVLightReference* pLRA = new SVLightReference();
			( *l_Iter )->GetAcquisitionDevice()->GetLightReference(*pLRA);
			apLRA.Add(pLRA);

			// make another copy for cancel... this should move into the propsheet
			SVLightReference* pLRAorig = new SVLightReference();
			( *l_Iter )->GetAcquisitionDevice()->GetLightReference(*pLRAorig);
			apLRAorig.Add(pLRAorig);
		}

		++l_Iter;
	}

	//remove apply button
	dlg.m_psh.dwFlags |= PSH_NOAPPLYNOW;

	if (dlg.CreatePages(setCameras, apLRA))
	{
		SVSVIMStateClass::AddState( SV_STATE_EDITING );

		SVConfigurationObject* pConfig( nullptr );
		SVObjectManagerClass::Instance().GetConfigurationObject( pConfig );
		ASSERT(nullptr != pConfig); 

		if( nullptr != pConfig && dlg.DoModal() == IDOK )
		{
			SetModifiedFlag();

			for (i=0, l_Iter = setCameras.begin(); l_Iter != setCameras.end(); ++i, ++l_Iter )
			{
				( *l_Iter )->SetLightReference(*(apLRA.GetAt(i)));
				SVLightReference lra; // get new device lra; camera Set only modifies its band(s)
				( *l_Iter )->GetAcquisitionDevice()->GetLightReference(lra);
				pConfig->ModifyAcquisitionDevice( ( *l_Iter )->GetAcquisitionDevice()->GetRootDeviceName(), lra );
				delete apLRA.GetAt(i);
				delete apLRAorig.GetAt(i);
			}
		}
		else    // if cancel
		{
			for (i=0, l_Iter = setCameras.begin(); l_Iter != setCameras.end(); ++i, ++l_Iter )
			{
				( *l_Iter )->SetLightReference(*(apLRAorig.GetAt(i)));
				SVLightReference lra; // get new device lra; camera Set only modifies its band(s)
				( *l_Iter )->GetAcquisitionDevice()->GetLightReference(lra);
				pConfig->ModifyAcquisitionDevice( ( *l_Iter )->GetAcquisitionDevice()->GetRootDeviceName(), lra );
				delete apLRA.GetAt(i);
				delete apLRAorig.GetAt(i);
			}
		}
		SVSVIMStateClass::RemoveState( SV_STATE_EDITING );
	}
	// Show default LUT: black --> black, white --> white...
	GetImageView()->ShowExtremeLUT( FALSE );
}

void SVIPDoc::OnAdjustLut()
{
	int i( 0 );

	SVInspectionProcess* pInspection( GetInspectionProcess() );

	ASSERT( GetToolSet() );
	ASSERT( GetToolSet()->IsCreated() );

	if( nullptr == pInspection || ! GetToolSet()->IsCreated() ) // is tool set created
	{
		return;
	}

	int iChannel = 0;
	// Show extreme LUT: black --> blue, white --> green...
	GetImageView()->ShowExtremeLUT();

	SVLutDlg dlg( CString( _T( "LUT Adjustment - [" ) + GetTitle() + _T( "]" ) ));

	// obtain copies of the luts to set if IDOK / IDCANCEL
	SVLutDlg::SVLutMap aLut;
	SVLutDlg::SVLutMap aLutOrig;

	SVVirtualCameraPtrSet setCameras;

	pInspection->GetCameras( setCameras );

	SVVirtualCameraPtrSet::iterator l_Iter = setCameras.begin();

	while( l_Iter != setCameras.end() )
	{
		if( *l_Iter != NULL )
		{
			( *l_Iter )->GetAcquisitionDevice()->GetLut( aLut[ ( *l_Iter )->GetUniqueObjectID() ] );
		}

		++l_Iter;
	}

	aLutOrig = aLut;

	//remove the apply button
	dlg.m_psh.dwFlags |= PSH_NOAPPLYNOW;

	bool bSuccess = dlg.Create(setCameras, aLut);

	if ( bSuccess )
	{
		SVSVIMStateClass::AddState( SV_STATE_EDITING );


		if( dlg.DoModal() == IDOK )
		{
			SetModifiedFlag();
		}
		else    // if cancel
		{
			// restore old LUT
			for( SVVirtualCameraPtrSet::iterator l_Iter = setCameras.begin(); l_Iter != setCameras.end(); ++l_Iter )
			{
				( *l_Iter )->SetLut( aLutOrig[ ( *l_Iter )->GetUniqueObjectID() ] );
			}
		}
		SVConfigurationObject* pConfig( nullptr );
		SVObjectManagerClass::Instance().GetConfigurationObject( pConfig );
		ASSERT(nullptr != pConfig);

		if( nullptr != pConfig )
		{
			for( SVVirtualCameraPtrSet::iterator l_Iter = setCameras.begin(); l_Iter != setCameras.end(); ++l_Iter )
			{
				SVLut lut;
				SVAcquisitionClassPtr pDevice = (*l_Iter)->GetAcquisitionDevice();
				pDevice->GetLut(lut);
				pConfig->ModifyAcquisitionDevice( pDevice->GetRootDeviceName(), lut );

				// SEJ - update DeviceParameters as well...
				SVDeviceParamCollection deviceParams;
				pDevice->GetDeviceParameters(deviceParams);
				pConfig->ModifyAcquisitionDevice( pDevice->GetRootDeviceName(), &deviceParams );
			}
		}
		SVSVIMStateClass::RemoveState( SV_STATE_EDITING );
	}

	// Show default LUT: black --> black, white --> white...
	GetImageView()->ShowExtremeLUT( FALSE );
}

void SVIPDoc::OnAddShiftTool()
{
	SVToolClass* pTool = new SVShiftTool( TRUE );

	if( AddTool( pTool ) ) { return; }

	if( pTool ) { delete( pTool ); }
}

void SVIPDoc::OnAddWindowTool()
{
	SVToolClass* pTool = new SVWindowToolClass( TRUE );

	if( AddTool( pTool ) ) { return; }

	if( pTool ) { delete( pTool ); }
}

void SVIPDoc::OnAddCylindricalWarpTool()
{
	SVToolClass* pTool = new SVCylindricalWarpToolClass( TRUE );

	if( AddTool( pTool ) ) { return; }

	if( pTool ) { delete( pTool ); }
}

void SVIPDoc::OnAddPerspectiveTool()
{
	SVToolClass* pTool = new SVPerspectiveToolClass( TRUE );

	if( AddTool( pTool ) ) { return; }

	if( pTool ) { delete( pTool ); }
}

void SVIPDoc::OnAddImageTool()
{
	SVToolClass* pTool = new SVImageToolClass( TRUE );

	if( AddTool( pTool ) ) { return; }

	if( pTool ) { delete( pTool ); }
}

void SVIPDoc::OnAddAcquisitionTool()
{
	SVToolClass* pTool = new SVAcquisitionToolClass( TRUE );

	if( AddTool( pTool ) ) { return; }

	if( pTool ) { delete( pTool ); }
}

void SVIPDoc::OnAddArchiveTool()
{
	SVToolClass* pTool = new SVArchiveTool;

	if( AddTool( pTool ) ) { return; }

	if( pTool ) { delete( pTool ); }
}

void SVIPDoc::OnAddLinearTool()
{
	SVToolClass* pTool = new SVLinearToolClass( TRUE );

	if( AddTool( pTool ) ) { return; }

	if( pTool ) { delete( pTool ); }
}

void SVIPDoc::OnAddLoadImageTool()
{
	SVToolClass* pTool = new SVLoadImageToolClass( TRUE );

	if( AddTool( pTool ) ) { return; }

	if( pTool ) { delete( pTool ); }
}

void SVIPDoc::OnAddMathTool()
{
	SVToolClass* pTool = new SVMathToolClass;

	if( AddTool( pTool ) ) { return; }

	if( pTool ) { delete( pTool ); }
}

void SVIPDoc::OnAddStatisticsTool()
{
	SVToolClass* pTool = new SVStatisticsToolClass( TRUE );

	if( AddTool( pTool ) ) { return; }

	if( pTool ) { delete( pTool ); }
}

void SVIPDoc::OnAddTransformationTool()
{
	SVToolClass* pTool = new SVTransformationToolClass;

	if( AddTool( pTool ) ) { return; }

	if( pTool ) { delete( pTool ); }
}

void SVIPDoc::OnAddRemoteInputTool()
{
	SVToolClass* pTool = new SVRemoteInputTool;

	if( AddTool( pTool ) ) { return; }

	if( pTool ) { delete( pTool ); }
}

void SVIPDoc::OnAddResizetool()
{
	SVToolClass* pTool = new ResizeTool ( TRUE );

	if( AddTool( pTool ) ) { return; }

	if( pTool ) { delete( pTool ); }
}

void SVIPDoc::OnAddRingBufferTool()
{
	SVToolClass* pTool = new RingBufferTool;

	if( AddTool( pTool ) ) { return; }

	if( pTool ) { delete( pTool ); }
}

void SVIPDoc::OnAddPolarUnwrapTool()
{
	SVPolarTransformationToolClass* pTool = new SVPolarTransformationToolClass;

	if( AddTool( pTool ) )
	{
		// Set the Use Anglular Method2 to the new type.
		pTool->m_svAngularMethod.SetDefaultValue( _T("Angular Method2"), TRUE );

		return;
	}

	if( pTool ) { delete( pTool ); }
}

void SVIPDoc::OnAddColorTool()
{
	SVToolClass* pTool = new SVColorToolClass( TRUE );

	if( AddTool( pTool ) ) { return; }

	if( pTool ) { delete( pTool ); }
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

					if( pFirstTaskObject && SV_IS_KIND_OF(pFirstTaskObject, SVColorToolClass) &&
						pNextTool && !SV_IS_KIND_OF(pNextTool, SVColorToolClass))
					{
						AfxMessageBox( SvO::ColorToolMustBeFirstMessage, MB_OK );
						return;
					}
				}
			}
			SVTaskObjectClass* pTaskObject = dynamic_cast<SVTaskObjectClass *>(SVObjectManagerClass::Instance().GetObject(rGuid));
			if (pTaskObject)
			{
				int index = GetSelectedToolIndex(rInfo.m_selection);

				HRESULT hr = DeleteTool(pTaskObject);

				if (S_OK == hr)
				{
					m_toolGroupings.RemoveTool(static_cast<LPCTSTR>(rInfo.m_selection));

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
			m_toolGroupings.RemoveGroup(static_cast<LPCTSTR>(rInfo.m_selection));

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
	if ( !SVSVIMStateClass::CheckState( SV_STATE_READY ) || !SVSVIMStateClass::CheckState( SV_STATE_EDIT ) )
	{
		return;
	}

	SVInspectionProcess* pInspection( GetInspectionProcess() );

	if (nullptr != pInspection)
	{
		ToolSetView* pToolSetView = GetToolSetView();
		if (nullptr != pToolSetView && !pToolSetView->IsLabelEditing())
		{
			const SVGUID& rGuid = pToolSetView->GetSelectedTool();
			if( !rGuid.empty() )
			{
				ToolClipboard Clipboard( *pInspection );

				Clipboard.writeToClipboard( rGuid );
			}
		}
	}
}

void SVIPDoc::OnUpdateEditCopy( CCmdUI* pCmdUI )
{
	BOOL Enabled = SVSVIMStateClass::CheckState( SV_STATE_READY ) && SVSVIMStateClass::CheckState( SV_STATE_EDIT );
	// Check current user access...
	Enabled = Enabled && TheSVObserverApp.OkToEdit();

	if( Enabled )
	{
		Enabled = FALSE;
		ToolSetView* pToolSetView = GetToolSetView();
		SVToolSetClass* pToolSet = GetToolSet();
		if( nullptr != pToolSet && nullptr != pToolSetView && !pToolSetView->IsLabelEditing())
		{
			ToolListSelectionInfo ToolListInfo = pToolSetView->GetToolListSelectionInfo();

			const SVGUID& rGuid = pToolSetView->GetSelectedTool();
			//Tool list active and valid tool
			if( !rGuid.empty() && -1 != ToolListInfo.m_listIndex )
			{
				Enabled = TRUE;
			}
		}
	}

	pCmdUI->Enable( Enabled );
}

void SVIPDoc::OnEditPaste()
{
	if ( !SVSVIMStateClass::CheckState( SV_STATE_READY ) || !SVSVIMStateClass::CheckState( SV_STATE_EDIT ) )
	{
		return;
	}

	SVInspectionProcess* pInspection( GetInspectionProcess() );

	if (nullptr != pInspection)
	{
		ToolClipboard Clipboard( *pInspection );
		SVGUID ToolGuid( GUID_NULL );
		ToolSetView* pView = GetToolSetView();

		const ToolListSelectionInfo& info = pView->GetToolListSelectionInfo();
		int ToolListIndex = GetToolToInsertBefore(info.m_selection, info.m_listIndex);
		Clipboard.readFromClipboard( ToolListIndex, ToolGuid );

		SVObjectClass* pObject( SVObjectManagerClass::Instance().GetObject( ToolGuid ) );

		SVToolClass* pTool = dynamic_cast<SVToolClass*> ( pObject );
		SVToolSetClass* pToolSet( pInspection->GetToolSet() );

		if( nullptr != pTool && nullptr != pToolSet )
		{
			//Set the time stamp to 0 to force an update of the tool list
			SetSelectedToolID( ToolGuid );
			m_ToolSetListTimestamp = 0;
			pToolSet->moveTool( ToolListIndex, pTool );
			//If this is the first tool then it has already been inserted otherwise the name is unique
			if( m_toolGroupings.IsNameUnique( pTool->GetName() ))
			{
				m_toolGroupings.AddTool( pTool->GetName(), static_cast<LPCTSTR>(info.m_selection) );
			}

			pTool->SetObjectDepthWithIndex( pInspection->GetObjectDepth(), 1 );
			pTool->SetImageDepth( pInspection->GetImageDepth() );

			SVInspectionLevelCreateStruct createStruct;
			createStruct.OwnerObjectInfo = pInspection;
			createStruct.InspectionObjectInfo = pInspection;

			::SVSendMessage( pInspection, SVM_CONNECT_ALL_OBJECTS, reinterpret_cast<DWORD_PTR>(&createStruct), NULL );
			::SVSendMessage( pInspection, SVM_CONNECT_ALL_INPUTS, NULL, NULL );

			SVObjectLevelCreateStruct createObjStruct;

			::SVSendMessage( pInspection, SVM_CREATE_ALL_OBJECTS,reinterpret_cast<DWORD_PTR>(&createObjStruct), NULL );
			//Reset only the inserted tool
			::SVSendMessage( pTool, SVM_RESET_ALL_OBJECTS, TRUE, NULL );

			RunOnce();
			UpdateAllViews(nullptr);
			SetModifiedFlag();
		}
	}
}

void SVIPDoc::OnUpdateEditPaste( CCmdUI* pCmdUI )
{
	BOOL Enabled = SVSVIMStateClass::CheckState( SV_STATE_READY ) && SVSVIMStateClass::CheckState( SV_STATE_EDIT );
	// Check current user access...
	Enabled = Enabled && TheSVObserverApp.OkToEdit();

	if( Enabled )
	{
		Enabled = FALSE;
		ToolSetView* pToolSetView = GetToolSetView();
		SVToolSetClass* pToolSet = GetToolSet();
		if( nullptr != pToolSet && nullptr != pToolSetView && !pToolSetView->IsLabelEditing() )
		{
			const ToolListSelectionInfo& info = pToolSetView->GetToolListSelectionInfo();
			//Only if tool list active and a selected index is valid
			if ( -1 != info.m_listIndex && ToolClipboard::isClipboardDataValid() )
			{
				Enabled = TRUE;
			}
		}
	}

	pCmdUI->Enable( Enabled );
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
	if( TheSVObserverApp.OkToEdit() )
	{
		SVToolClass* l_pTool = dynamic_cast< SVToolClass* >( SVObjectManagerClass::Instance().GetObject( GetSelectedToolID() ) );
		if( nullptr != l_pTool )
		{
			const SVObjectTypeInfoStruct& rToolType = l_pTool->GetObjectInfo().ObjectTypeInfo;

			if ( (rToolType.SubType == SVGageToolObjectType) || (rToolType.SubType == SVToolProfileObjectType) )
			{
				SVSVIMStateClass::AddState( SV_STATE_EDITING );
				AfxMessageBox(_T("This tool is now obsolete.  Please replace with a Linear Tool."));
				SVSVIMStateClass::RemoveState( SV_STATE_EDITING );
				return;
			}
			if (rToolType.SubType == SVToolBuildReferenceObjectType)
			{
				SVSVIMStateClass::AddState( SV_STATE_EDITING );
				AfxMessageBox(_T("This tool is now obsolete. Please replace with either a Shift Tool or Transformation Tool"));
				SVSVIMStateClass::RemoveState( SV_STATE_EDITING );
				return;
			}
			// Check if can Edit this Tool
			// Primarily for the Build Reference Tool
			// to check if anyone is using the output image
			if( l_pTool->IsOkToEdit() )
			{
				SVSVIMStateClass::AddState( SV_STATE_EDITING );
				SVToolAdjustmentDialogSheetClass toolAdjustmentDialog( this, GetInspectionID(), GetSelectedToolID(), _T("Tool Adjustment"), nullptr, tab );
				INT_PTR dlgResult = toolAdjustmentDialog.DoModal();
				if ( IDOK == dlgResult )
				{
					ExtrasEngine::Instance().ExecuteAutoSaveIfAppropriate(false);//Arvid: after tool was edited: update the autosave timestamp
					SVConfigurationObject* pConfig = nullptr;
					SVObjectManagerClass::Instance().GetConfigurationObject( pConfig );
					if( nullptr != pConfig)
					{
						pConfig->ValidateRemoteMonitorList();
						TheSVObserverApp.GetIODoc()->UpdateAllViews( NULL );
					}
				}
				l_pTool->ResetObject();
				SVSVIMStateClass::RemoveState( SV_STATE_EDITING );
			}
		}
		UpdateAllViews( NULL, RefreshView );
	}
}

void SVIPDoc::OnEditToolSetCondition()
{
	// Check current user access...
	if( TheSVObserverApp.OkToEdit() )
	{
		if( GetToolSet() )
		{
			SVSVIMStateClass::AddState( SV_STATE_EDITING );
			EditToolSetCondition();
			SVSVIMStateClass::RemoveState( SV_STATE_EDITING );
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
	SVSVIMStateClass::AddState( SV_STATE_EDITING ); /// do this before calling validate for security as it may display a logon dialog!
	if( S_OK == TheSVObserverApp.m_svSecurityMgr.SVValidate(SECURITY_POINT_EDIT_MENU_RESULT_PICKER) )
	{
		SVInspectionProcess* pInspection( GetInspectionProcess() );
		SVResultListClass* pResultList( GetResultList() );
		if( nullptr != pInspection && nullptr != pResultList ) 
		{
			SVString InspectionName( pInspection->GetName() );

			SvOsl::ObjectTreeGenerator::Instance().setSelectorType( SvOsl::ObjectTreeGenerator::SelectorTypeEnum::TypeMultipleObject);
			SvOsl::ObjectTreeGenerator::Instance().setLocationFilter( SvOsl::ObjectTreeGenerator::FilterInput, InspectionName, SVString( _T("") ) );

			SvOsl::SelectorOptions BuildOptions( GetInspectionID(), SV_VIEWABLE );
			SvOsl::ObjectTreeGenerator::Instance().BuildSelectableItems<SvOg::GlobalSelector, SvOg::PPQSelector, SvOg::ToolSetItemSelector<>>( BuildOptions );
			
			SVStringSet SelectedNames;
			SVStringSet SelectedNamesRaw;
			pResultList->GetNameSet(SelectedNamesRaw);
			//Need to replace the PPQ Variables name with the inspection name
			typedef std::insert_iterator<SVStringSet> Insertor;
			std::transform(SelectedNamesRaw.begin(), SelectedNamesRaw.end(), Insertor(SelectedNames, SelectedNames.end()), [&InspectionName](const SVString& name)->SVString
			{
				SVString St(name);
				// check for .DIO or .Remote Input
				if (SVString::npos != St.find(".DIO") || SVString::npos != St.find(".Remote Input"))
				{
					St.replace( InspectionName.c_str(), SvOl::FqnPPQVariables );
				}
				return St;
			});

			SvOsl::ObjectTreeGenerator::Instance().setCheckItems(SelectedNames);

			CString Title;
			CString ResultPicker;
			CString Filter;
			ResultPicker.LoadString( IDS_RESULT_PICKER ); 
			Title.Format( _T("%s - %s"), ResultPicker, InspectionName.c_str() );
			Filter.LoadString( IDS_FILTER );
			INT_PTR Result = SvOsl::ObjectTreeGenerator::Instance().showDialog( Title, ResultPicker, Filter );

			if( IDOK == Result )
			{
				const SvOsl::SelectorItemVector& SelectedItems = SvOsl::ObjectTreeGenerator::Instance().getSelectedObjects();
				SvOsl::SelectorItemVector::const_iterator Iter;
				pResultList->Clear();

				for( Iter = SelectedItems.begin(); Iter != SelectedItems.end(); ++Iter )
				{
					if(string::npos  != Iter->getLocation().find(SvOl::FqnPPQVariables))
					{
						SVString string = Iter->getLocation();
						string.replace(SvOl::FqnPPQVariables,InspectionName.c_str());
						pResultList->Insert(string);
					}
					else
					{
						pResultList->Insert(Iter->getLocation());
					}
				}
				// Set the Document as modified
				SetModifiedFlag();
				RebuildResultsList();
				UpdateWithLastProduct();
			}
			UpdateAllViews( nullptr );
		}
	}

	SVSVIMStateClass::RemoveState(SV_STATE_EDITING);
}

void SVIPDoc::OnPublishedResultsPicker()
{
	SVInspectionProcess* pInspection( GetInspectionProcess() );
	if( nullptr != pInspection ) 
	{ 
		SVSVIMStateClass::AddState(SV_STATE_EDITING);
		SVString InspectionName( pInspection->GetName() );

		SvOsl::ObjectTreeGenerator::Instance().setSelectorType( SvOsl::ObjectTreeGenerator::SelectorTypeEnum::TypeSetAttributes );
		SvOsl::ObjectTreeGenerator::Instance().setLocationFilter( SvOsl::ObjectTreeGenerator::FilterInput, InspectionName, SVString( _T("") ) );

		SvOsl::SelectorOptions BuildOptions( GetInspectionID(), SV_PUBLISHABLE );
		SvOsl::ObjectTreeGenerator::Instance().BuildSelectableItems<SvOg::NoSelector, SvOg::NoSelector, SvOg::ToolSetItemSelector<>>( BuildOptions );

		CString Title;
		CString PublishableResults;
		CString Filter;
		PublishableResults.LoadString( IDS_PUBLISHABLE_RESULTS );
		Title.Format( _T("%s - %s"), PublishableResults, InspectionName.c_str() );
		Filter.LoadString( IDS_FILTER );
		INT_PTR Result = SvOsl::ObjectTreeGenerator::Instance().showDialog( Title,PublishableResults, Filter );

		if( IDOK == Result )
		{
			pInspection->GetPublishList().Refresh( GetToolSet() );

			// Set the Document as modified
			SetModifiedFlag();
			SVPPQObject* pPPQ( nullptr );

			SVConfigurationObject* pConfig = nullptr;
			SVObjectManagerClass::Instance().GetConfigurationObject( pConfig );
			if ( nullptr != pConfig )
			{
				pConfig->ValidateRemoteMonitorList();

				// Force the PPQs to rebuild
					long lSize = pConfig->GetPPQCount( );

				for( long l = 0; l < lSize; l++ )
				{
					pPPQ = pConfig->GetPPQ( l );
					if( nullptr != pPPQ )
					{
						pPPQ->RebuildOutputList();
					}// end if
				}
			}// end for
			TheSVObserverApp.GetIODoc()->UpdateAllViews( nullptr );
		}
		SVSVIMStateClass::RemoveState(SV_STATE_EDITING);
	}
}

void SVIPDoc::OnPublishedResultImagesPicker()
{
	SVInspectionProcess* pInspection( GetInspectionProcess() );
	if( nullptr != pInspection ) 
	{
		SVSVIMStateClass::AddState(SV_STATE_EDITING);

		SVString InspectionName( pInspection->GetName() );

		SvOsl::ObjectTreeGenerator::Instance().setSelectorType( SvOsl::ObjectTreeGenerator::SelectorTypeEnum::TypeSetAttributes );

		CString csRootName;
		csRootName.LoadString(IDS_CLASSNAME_ROOTOBJECT);
		SvOsl::ObjectTreeGenerator::Instance().setLocationFilter( SvOsl::ObjectTreeGenerator::FilterInput, csRootName, SVString( _T("") ) );

		SvOsl::SelectorOptions BuildOptions( GetInspectionID(), SV_PUBLISH_RESULT_IMAGE );
		SvOsl::ObjectTreeGenerator::Instance().BuildSelectableItems<SvOg::NoSelector, SvOg::NoSelector, SvOg::ToolSetItemSelector<>>( BuildOptions );

		CString Title;
		CString PublishedResult;
		CString Filter;
		PublishedResult.LoadString( IDS_PUBLISHABLE_RESULT_IMAGES );
		Title.Format( _T("%s - %s"), PublishedResult, InspectionName.c_str() );
		Filter.LoadString( IDS_FILTER );
		
		INT_PTR Result = SvOsl::ObjectTreeGenerator::Instance().showDialog( Title, PublishedResult, Filter );

		if( IDOK == Result )
		{
			SetModifiedFlag();

			const SvOsl::SelectorItemVector& SelectedItems = SvOsl::ObjectTreeGenerator::Instance().getSelectedObjects();
			SvOsl::SelectorItemVector::const_iterator Iter;

			for ( Iter = SelectedItems.begin(); Iter != SelectedItems.end(); ++Iter )
			{
				SVGUID ObjectGuid( Iter->getItemKey() );

				SVSendMessage ( ObjectGuid, SVM_RESET_ALL_OBJECTS, NULL, NULL );
			}

			SVConfigurationObject* pConfig( nullptr );
			SVObjectManagerClass::Instance().GetConfigurationObject( pConfig );
			if( nullptr != pConfig )
			{
				pConfig->ValidateRemoteMonitorList();
				TheSVObserverApp.GetIODoc()->UpdateAllViews( NULL );
			}
		}

		SVSVIMStateClass::RemoveState(SV_STATE_EDITING);
	}
}

void SVIPDoc::OnConditionalHistory()
{
	SVInspectionProcess* pInspection( GetInspectionProcess() );
	if( nullptr != pInspection ) 
	{
		SVSVIMStateClass::AddState(SV_STATE_EDITING);
		CString strTitle = _T("Conditional History - ");
		strTitle += pInspection->GetName();
		SVConditionalHistorySheet sheet( strTitle, *pInspection );

		//remove apply button
		sheet.m_psh.dwFlags |= PSH_NOAPPLYNOW;

		sheet.DoModal();
		SVSVIMStateClass::RemoveState(SV_STATE_EDITING);
	}
}

void SVIPDoc::OnUpdateConditionalHistory( CCmdUI* pCmdUI )
{
	pCmdUI->Enable( SVSVIMStateClass::CheckState( SV_STATE_READY ) && SVSVIMStateClass::CheckState( SV_STATE_EDIT ) );
}


void SVIPDoc::EditToolSetCondition()
{
	SVConditionalClass* pCondition = GetToolSetCondition();
	ASSERT( pCondition );
	SVToolSetAdjustmentDialogSheetClass dlg( GetInspectionID(), GetToolSet()->GetUniqueObjectID(), *pCondition, GetToolSet()->GetName() );

	dlg.m_psh.dwFlags |= PSH_NOAPPLYNOW;

	INT_PTR dlgResult = dlg.DoModal();
	if( dlgResult == IDOK ) { SetModifiedFlag(); }
}

void SVIPDoc::RebuildResultsList()
{
	SVResultListClass* pResultList = GetResultList();

	if( pResultList ) 
	{ 
		pResultList->Refresh( GetToolSet() ); 
	}
}

bool  SVIPDoc::IsResultDefinitionsOutdated() const
{
	SVResultListClass* pResultList = GetResultList();
	bool res = true;
	
	if( nullptr !=  pResultList  && pResultList->getUpdateTimeStamp() < m_ResultDefinitionsTimestamp )
	{
		res  = false;
	}

	return res ;
}

HRESULT SVIPDoc::GetResultDefinitions( SVResultDefinitionDeque& p_rDefinitions ) const
{
	HRESULT hres = E_FAIL;
	p_rDefinitions.clear();
	SVResultListClass* pResultList = GetResultList();
	if(nullptr != pResultList)
	{
		
		hres = pResultList->GetResultDefinitions(p_rDefinitions);
		
	}

	if( hres == S_OK )
	{ 
		m_ResultDefinitionsTimestamp = SVClock::GetTimeStamp(); 
	}

	return hres;
}

HRESULT SVIPDoc::GetResultData( SVIPResultData& p_rResultData ) const
{
	p_rResultData = m_Results;

	return S_OK;
}

HRESULT SVIPDoc::IsToolSetListUpdated() const
{
	HRESULT l_Status = S_OK;

	SVToolSetClass* pToolSet = GetToolSet();
	if( pToolSet )
	{
		if( pToolSet->GetLastListUpdateTimestamp() < m_ToolSetListTimestamp )
		{
			l_Status = S_FALSE;
		}
		else
		{
			m_ToolSetListTimestamp = SVClock::GetTimeStamp();
		}
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

BOOL SVIPDoc::checkOkToDelete( SVTaskObjectClass* pTaskObject )
{
	BOOL bRetVal = false;

	SVSVIMStateClass::AddState(SV_STATE_EDITING);
	// show dependents dialog

	if (pTaskObject)
	{
		SVGUID inspectionID = GetInspectionID();
		SVGUID taskObjectID = pTaskObject->GetUniqueObjectID();
	
		INT_PTR dlgResult = SvOg::SVShowDependentsDialog::StandardDialog( pTaskObject->GetName(), inspectionID, taskObjectID );

		bRetVal = ( IDCANCEL == dlgResult ) ? false : true;
	}
	SVSVIMStateClass::RemoveState(SV_STATE_EDITING);
	return bRetVal;
}

void SVIPDoc::RunRegressionTest()
{
	bool l_bWasRunMode = false;
	bool l_bWasTestMode = false;

	if ( SVSVIMStateClass::CheckState(SV_STATE_REGRESSION) )
	{  // already in regression mode, do nothing...
		return;
	}

	//check to see if in Run Mode, if so stop
	if ( SVSVIMStateClass::CheckState( SV_STATE_RUNNING ) ) { l_bWasRunMode = true; }

	if ( SVSVIMStateClass::CheckState( SV_STATE_TEST ) ) { l_bWasTestMode = true; }

	if ( l_bWasRunMode || l_bWasTestMode ) { TheSVObserverApp.OnStop(); }

	SVInspectionProcess* pInspection( GetInspectionProcess() );

	if( nullptr == pInspection ) { return; }

	if ( pInspection->CanRegressionGoOnline() )
	{
		SVImageArchiveClass svImageArchive;
		SVCameraImageTemplate* pMainImage = NULL;

		bool l_bAllowAccess = false;

		if ( l_bWasRunMode )
		{
			// Dual Security access point
			if( TheSVObserverApp.m_svSecurityMgr.SVValidate( SECURITY_POINT_MODE_MENU_REGRESSION_TEST, 
				SECURITY_POINT_MODE_MENU_EXIT_RUN_MODE ) == S_OK )
			{
				l_bAllowAccess = true;
			}
			else
			{
				return;
			}
		}
		else if( TheSVObserverApp.m_svSecurityMgr.SVValidate( SECURITY_POINT_MODE_MENU_REGRESSION_TEST ) == S_OK )
		{
			l_bAllowAccess = true;
		}

		if( l_bAllowAccess )
		{
			if ( SVSVIMStateClass::CheckState( SV_STATE_TEST ) )
			{
				SVSVIMStateClass::RemoveState( SV_STATE_TEST );
			}

			if ( SVSVIMStateClass::CheckState( SV_STATE_EDIT ) )
			{
				SVSVIMStateClass::RemoveState( SV_STATE_EDIT );
			}

			TheSVObserverApp.DeselectTool();

			SVSVIMStateClass::AddState(SV_STATE_REGRESSION);

			m_oDisplay.CanGoOnline();
			// check to see if it is able to go into Regression mode

			// check to see if the list of files are the same...
			m_bRegressionTestRunning = true;
			int iNumCameras = static_cast< int >( m_listRegCameras.GetCount() );

			( ( SVMainFrame* ) AfxGetApp()->m_pMainWnd )->m_pregTestDlg = new CSVRegressionRunDlg;

			( ( SVMainFrame* ) AfxGetApp()->m_pMainWnd )->m_pregTestDlg->SetIPDocParent(this);

			( ( SVMainFrame* ) AfxGetApp()->m_pMainWnd )->m_pregTestDlg->Create(IDD_DIALOG_REGRESSIONTEST_RUN);

			( ( SVMainFrame* ) AfxGetApp()->m_pMainWnd )->m_pregTestDlg->ShowWindow(SW_SHOW);

			DWORD dwThreadID;

			m_hRegressionHandle = ::CreateThread( NULL, 0, SVRegressionTestRunThread, (LPVOID)this, 0, &dwThreadID  );
		}
	}
	else
	{
		AfxMessageBox("Inspection is unable to go into Regression Test Mode.  There is an unknown error with the inspection.");
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
	CWnd *pWindow;
	CMenu *pMenu;
	CString szMenuText;

	pWindow = AfxGetMainWnd();
	if( pWindow )
	{
		// Load and init Utility Menu
		pMenu = pWindow->GetMenu();
		szMenuText = _T("&Utilities");
		if (pMenu = util.FindSubMenuByName(pMenu, szMenuText)) { util.LoadMenu(pMenu); }

		// Load and init Tool Set Draw Menu
		int pos = 0;
		pMenu = ::SVFindMenuByCommand( pWindow->GetMenu(), ID_VIEW_TOOLSETDRAW, TRUE, pos );
		SVToolSetClass* pSet = GetToolSet();
		if( pMenu && pSet )
		{
			SVEnumerateValueObjectClass* pEnum = pSet->GetDrawFlagObject();
			CMenu myPopUp;
			if( pEnum && myPopUp.CreatePopupMenu() )
			{
				// Populate pop up with enumerations of tool set draw flag...
				UINT uiCommand = ID_VIEW_TOOLSETDRAW_POP_BASE;
				int it = pEnum->GetFirstEnumTypePos();
				CString strEnum;
				long lEnum = 0L;
				while( pEnum->GetNextEnumType( it, strEnum, lEnum ) )
				{
					myPopUp.AppendMenu( MF_STRING, uiCommand++, strEnum );
				}

				// Get current name of ID_VIEW_TOOLSETDRAW menu...
				CString strMenu =_T( "Display" );
				pMenu->GetMenuString( pos, strMenu, MF_BYPOSITION );

				// Transfer pop up...
				if( pMenu->ModifyMenu( pos, MF_BYPOSITION | MF_POPUP, ( UINT ) HMENU( myPopUp ), strMenu ) )
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
	if( pCmdUI->m_pSubMenu )
	{
		unsigned int l_uiGray = SVSVIMStateClass::CheckState( SV_STATE_EDIT ) ? 0 : MF_GRAYED;
		pCmdUI->m_pMenu->EnableMenuItem( pCmdUI->m_nIndex, MF_BYPOSITION | l_uiGray);
	}
	else
	{
		pCmdUI->Enable( SVSVIMStateClass::CheckState( SV_STATE_EDIT ) );
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
void SVIPDoc::OnUpdateViewToolSetDrawSubMenus( CCmdUI* PCmdUI )
{
	PCmdUI->Enable( SVSVIMStateClass::CheckState( SV_STATE_EDIT ) && !SVSVIMStateClass::CheckState( SV_STATE_RUNNING ) );

	SVToolSetClass* pSet = GetToolSet();

	if ( pSet && !SVSVIMStateClass::CheckState( SV_STATE_LOADING | SV_STATE_CANCELING | SV_STATE_CLOSING ) )
	{
		SVEnumerateValueObjectClass* pEnum = pSet->GetDrawFlagObject();
		if( pEnum )
		{
			// Get current value of enumeration object.
			long lValue = 0L;
			pEnum->GetValue( lValue );

			// Get Menu value...
			long lMenuValue = PCmdUI->m_nID - ID_VIEW_TOOLSETDRAW_POP_BASE;

			// Set Radio, if equal current draw flag...
			PCmdUI->SetRadio( lValue == lMenuValue );
		}
	}
}

////////////////////////////////////////////////////////////////////////////////
// .Description : Called if user 'clicks' on a tool set draw menu item.
//				: Range of nId must be 
//				: [ ID_VIEW_TOOLSETDRAW_POP_BASE...ID_VIEW_TOOLSETDRAW_POP_MAX ]
////////////////////////////////////////////////////////////////////////////////
void SVIPDoc::OnChangeToolSetDrawFlag( UINT nId )
{
	ASSERT( nId >= ID_VIEW_TOOLSETDRAW_POP_BASE && nId <= ID_VIEW_TOOLSETDRAW_POP_MAX );

	// Access denied...
	if( ! TheSVObserverApp.OkToEdit() ) { return; }

	if( nId >= ID_VIEW_TOOLSETDRAW_POP_BASE && nId <= ID_VIEW_TOOLSETDRAW_POP_MAX )
	{
		CWnd* pWindow = AfxGetMainWnd();
		if( GetToolSet() && pWindow )
		{
			// Get Menu...
			int pos = 0;
			CMenu* pMenu = ::SVFindMenuByCommand( pWindow->GetMenu(), nId, TRUE, pos );

			// Get Tool Set Draw Flag...
			SVEnumerateValueObjectClass* pEnum = GetToolSet()->GetDrawFlagObject();

			if( pEnum && pMenu )
			{
				// Get correlated enumerator...
				CString strEnum;
				pMenu->GetMenuString( pos, strEnum, MF_BYPOSITION );

				// Set Flag...
				pEnum->SetValue( 1, strEnum );

				// Update Menu...
				pMenu->CheckMenuRadioItem( 0, pMenu->GetMenuItemCount() - 1, pos, MF_BYPOSITION );

				// Update views...
				UpdateAllViews( NULL );
			}
		}
	}
}

void SVIPDoc::RefreshDocument()
{
	SVCommandInspectionCollectImageData::SVImageIdSet l_ImageIds;

	for( int i = 0; i < MaxImageViews; ++i )
	{
		SVImageViewClass* l_pImageView = GetImageView( i );

		if( l_pImageView != NULL )
		{
			SVGUID l_ImageId = l_pImageView->GetImageID();

			if( !( l_ImageId.empty() ) )
			{
				l_ImageIds.insert( l_ImageId );
			}
		}
	}
	SVCommandInspectionCollectImageDataPtr l_DataPtr = new SVCommandInspectionCollectImageData( m_InspectionID, l_ImageIds );
	SVObjectSynchronousCommandTemplate< SVCommandInspectionCollectImageDataPtr > l_Command( m_InspectionID, l_DataPtr );

	if( l_Command.Execute( 120000 ) == S_OK )
	{
		if( m_NewProductData.size() == m_NewProductData.capacity() )
		{
			m_NewProductData.PopHead();
		}

		if( m_NewProductData.PushTail( l_DataPtr->GetProduct() ) == S_OK )
		{
			CWnd* pWnd = GetMDIChild();
			pWnd->PostMessage( SV_MDICHILDFRAME_UPDATE_ALL_DATA );
		}
	}
}

void SVIPDoc::RecreateImageSurfaces()
{
	for( int i = 0; i < MaxImageViews; ++ i )
	{
		SVImageViewClass* pImageView = GetImageView( i );

		if( pImageView ) { pImageView->RecreateImageSurface(); }
	}
}

void SVIPDoc::OnUpdateFileExit( CCmdUI* pCmdUI )
{
	pCmdUI->Enable( ! SVSVIMStateClass::CheckState( SV_STATE_RUNNING ) && !SVSVIMStateClass::CheckState( SV_STATE_REGRESSION ) && !SVSVIMStateClass::CheckState( SV_STATE_TEST )
		&& TheSVObserverApp.m_svSecurityMgr.SVIsDisplayable(SECURITY_POINT_FILE_MENU_EXIT));
}

CFile* SVIPDoc::GetFile( LPCTSTR lpszFileName, UINT nOpenFlags, CFileException* pError )
{
	UINT nNewFlags = nOpenFlags & ~( CFile::shareDenyRead | CFile::shareDenyWrite ) |
		CFile::shareDenyNone;

	return CDocument::GetFile( lpszFileName, nNewFlags, pError );
}

BOOL SVIPDoc::OnOpenDocument(LPCTSTR lpszPathName)
{
	BOOL bOk = FALSE;

	if ( lpszPathName )
	{
		bOk = msvFileName.SetFullFileName( lpszPathName );
	}
	else
	{
		bOk = msvFileName.SetFullFileName( GetPathName() );
	}

	if ( bOk )
	{
		SVFileNameManagerClass svFileManager;

		bOk = svFileManager.LoadItem( &msvFileName );

		if ( bOk )
		{
			bOk = CDocument::OnOpenDocument( msvFileName.GetFullFileName() );
		}
	}

	return bOk;
}

BOOL SVIPDoc::OnSaveDocument(LPCTSTR lpszPathName)
{
	SVFileNameManagerClass svFileManager;

	BOOL bOk = msvFileName.SetFullFileName( lpszPathName );

	if ( bOk && CString( msvFileName.GetPathName() ).CompareNoCase( svFileManager.GetRunPathName() ) != 0 )
	{
		bOk = msvFileName.SetPathName( svFileManager.GetRunPathName() );
	}

	if ( bOk && CString( msvFileName.GetExtension() ).CompareNoCase( ".ipd" ) != 0 )
	{
		bOk = msvFileName.SetExtension( ".ipd" );
	}

	if ( bOk )
	{
		bOk = CDocument::OnSaveDocument( msvFileName.GetFullFileName() );

		if ( bOk )
		{
			bOk = svFileManager.SaveItem( &msvFileName );

			if ( bOk )
			{
				CDocument::SetPathName( msvFileName.GetFullFileName(), FALSE );
			}
		}
	}

	return bOk;
}

BOOL SVIPDoc::IsColorInspectionDocument() const
{
	BOOL bRetVal = FALSE;

	SVInspectionProcess* pInspection = GetInspectionProcess();

	if( nullptr != pInspection )
	{
		bRetVal = pInspection->IsColorCamera();
	}

	return bRetVal;
}

BOOL SVIPDoc::GetParameters(SVObjectWriter& rWriter)
{
	BOOL bOk = FALSE;

	CString csScript;
	CString csAlias;

	_variant_t svVariant;

	SVInspectionProcess* pInspection( GetInspectionProcess() );

	if( nullptr == pInspection ) { return false; }

#ifdef USE_OBJECT_SCRIPT
	svVariant = SVGUID( pInspection->GetUniqueObjectID() ).ToVARIANT();
	rWriter.WriteAttribute(CTAG_UNIQUE_REFERENCE_ID, svVariant);
	svVariant.Clear();

	// Get the ToolSet's Object Script - SEJ 23-06-1999
	GetToolSet()->GetObjectScript( csScript, csAlias );
	csAlias += csScript;

	svVariant.SetString( csAlias );
	rWriter.WriteAttribute(CTAG_TOOLSET_SCRIPT, svVariant);
	svVariant.Clear();
#endif

	SaveViews(rWriter);

	SaveConditionalHistory(rWriter);

	// Save the View Placements
	SaveViewPlacements(rWriter);

	SaveToolGroupings(rWriter);

	SaveViewedVariables(rWriter);

	return bOk;
}

void SVIPDoc::SaveViewedVariables(SVObjectWriter& rWriter)
{
	SVResultListClass* pResultList = GetResultList();

	
	rWriter.StartElement(CTAG_VIEWEDVARIABLES);
	if(pResultList)
	{
		pResultList->Save(rWriter);
		//pResultList->m_PPQInputReferences.Save(rWriter);
	}
	rWriter.EndElement();
}

bool SVIPDoc::LoadViewedVariables(SVTreeType& rTree, SVTreeType::SVBranchHandle htiParent)
{
	SVResultListClass* pResultList = GetResultList();
	if(nullptr == pResultList)
	{
		return false;
	}
	return  pResultList->LoadViewedVariables(rTree, htiParent);

}

void SVIPDoc::SaveViews(SVObjectWriter& rWriter)
{
	SVViewUnion View;
	_variant_t svVariant;
	rWriter.StartElement(CTAG_IPDOC_VIEWS);
	rWriter.StartElement(CTAG_VIEWS);

	POSITION vPos = GetFirstViewPosition();

	if (vPos)
	{
		long ViewNumber = 0;

		while (View.pView = GetNextView(vPos))
		{
			rWriter.StartElement(View.pView->GetRuntimeClass()->m_lpszClassName);

			svVariant = ++ViewNumber;
			rWriter.WriteAttribute(CTAG_VIEW_NUMBER, svVariant);
			svVariant.Clear();

			if (View.pView->IsKindOf(RUNTIME_CLASS(SVImageViewScroll)))
			{
				View.pImageScroll->GetParameters( rWriter );
			}
			else if (View.pView->IsKindOf(RUNTIME_CLASS(SVImageViewClass)))
			{
				View.pImageView->GetParameters( rWriter );
			}
			else if ( View.pView->IsKindOf( RUNTIME_CLASS( ToolSetView ) ) )
			{
				View.pToolSetView->GetParameters( rWriter );
			}
			else if (View.pView->IsKindOf(RUNTIME_CLASS(SVResultViewClass)))
			{
				View.pResultView->GetParameters( rWriter );
			}
			rWriter.EndElement();
		}
	}
	rWriter.EndElement();
	rWriter.EndElement();

	// save height of the Result View
	svVariant = m_nHeightResultView;
	rWriter.WriteAttribute(CTAG_HEIGHT_RESULT_VIEW, svVariant);
	svVariant.Clear();

	// save width of the Toolset View
	svVariant = m_nWidthToolSetView;
	rWriter.WriteAttribute(CTAG_WIDTH_TOOLSET_VIEW, svVariant);
	svVariant.Clear();
}

void SVIPDoc::SaveConditionalHistory(SVObjectWriter& rWriter)
{
	SVInspectionProcess* pInspection( GetInspectionProcess() );

	if( nullptr != pInspection )
	{
		_variant_t svVariant;

		// Conditional History
		rWriter.StartElement(CTAG_CONDITIONAL_HISTORY);

		SVScalarValueVector vecProperties;
		pInspection->GetConditionalHistoryProperties(vecProperties);
		for (size_t i = 0;i < vecProperties.size();++i)
		{
			SVScalarValue& rValue = vecProperties[i];
			svVariant.SetString(rValue.strValue);
			rWriter.WriteAttribute(rValue.strName, svVariant);
			svVariant.Clear();
		}
		rWriter.EndElement();
	}
}

void SVIPDoc::SaveViewPlacements(SVObjectWriter& rWriter)
{
	_variant_t svVariant;
	SVViewUnion View;

	WINDOWPLACEMENT wndpl;
	ZeroMemory(&wndpl,sizeof(WINDOWPLACEMENT));
	wndpl.length = sizeof(WINDOWPLACEMENT);

	View.pToolSetView = dynamic_cast< ToolSetView* >( getView() );
	if (View.pToolSetView && View.pToolSetView->GetSafeHwnd())
	{
		CSplitterWnd* pWndSplitter = dynamic_cast< CSplitterWnd* >( View.pToolSetView->GetParent() );
		if (pWndSplitter && pWndSplitter->GetSafeHwnd())
		{
			CSplitterWnd* pWndSplitter2 = dynamic_cast< CSplitterWnd* >( pWndSplitter->GetParent() );
			ASSERT(pWndSplitter2 && pWndSplitter2->GetSafeHwnd());

			SVIPSplitterFrame* pFrame = dynamic_cast< SVIPSplitterFrame* >( pWndSplitter2->GetParent() );

			CRuntimeClass* pClass = pFrame->GetRuntimeClass();
			CString csClass = pClass->m_lpszClassName;
			ASSERT(csClass == _T("SVIPSplitterFrame"));

			if (pFrame &&pFrame->GetSafeHwnd())
			{
				BOOL bResult = pFrame->GetWindowPlacement( &wndpl );
			}
		}
	}

	// Save Window Placement
	rWriter.StartElement(CTAG_WINDOW_PLACEMENT);

	svVariant = wndpl.length;
	rWriter.WriteAttribute(CTAG_LENGTH, svVariant);
	svVariant.Clear();

	svVariant = wndpl.flags;
	rWriter.WriteAttribute(CTAG_FLAGS, svVariant);
	svVariant.Clear();

	svVariant = wndpl.showCmd;
	rWriter.WriteAttribute(CTAG_SHOW_COMMAND, svVariant);
	svVariant.Clear();

	// Save Minimum Position
	rWriter.StartElement(CTAG_MINIMUM_POSITION);
	svVariant = wndpl.ptMinPosition.x;
	rWriter.WriteAttribute(CTAG_X, svVariant);
	svVariant.Clear();

	svVariant = wndpl.ptMinPosition.y;
	rWriter.WriteAttribute(CTAG_Y, svVariant);
	svVariant.Clear();
	rWriter.EndElement();

	// Save Maximum Position
	rWriter.StartElement(CTAG_MAXIMUM_POSITION);
	svVariant = wndpl.ptMaxPosition.x;
	rWriter.WriteAttribute(CTAG_X, svVariant);
	svVariant.Clear();

	svVariant = wndpl.ptMaxPosition.y;
	rWriter.WriteAttribute(CTAG_Y, svVariant);
	svVariant.Clear();
	rWriter.EndElement();

	// Save Normal Position
	rWriter.StartElement(CTAG_NORMAL_POSITION);
	svVariant = wndpl.rcNormalPosition.left;
	rWriter.WriteAttribute(CTAG_LEFT, svVariant);
	svVariant.Clear();

	svVariant = wndpl.rcNormalPosition.top;
	rWriter.WriteAttribute(CTAG_TOP, svVariant);
	svVariant.Clear();

	svVariant = wndpl.rcNormalPosition.right;
	rWriter.WriteAttribute(CTAG_RIGHT, svVariant);
	svVariant.Clear();

	svVariant = wndpl.rcNormalPosition.bottom;
	rWriter.WriteAttribute(CTAG_BOTTOM, svVariant);
	svVariant.Clear();
	rWriter.EndElement();

	rWriter.EndElement();
}

void SVIPDoc::SaveToolGroupings(SVObjectWriter& rWriter)
{
	m_toolGroupings.GetParameters(rWriter);
}

BOOL SVIPDoc::SetParameters( SVTreeType& rTree, SVTreeType::SVBranchHandle htiParent )
{
	BOOL bOk = FALSE;

	SVViewUnion View;
	_variant_t svVariant;

	SVInspectionProcess* pInspection( GetInspectionProcess() );

	if( pInspection == nullptr ) { return false; }

	bOk = SVNavigateTree::GetItem( rTree, CTAG_HEIGHT_RESULT_VIEW, htiParent, svVariant );
	if ( bOk )
	{
		m_nHeightResultView = svVariant;

		bOk = SVNavigateTree::GetItem( rTree, CTAG_WIDTH_TOOLSET_VIEW, htiParent, svVariant );
		if ( bOk )
		{
			m_nWidthToolSetView = svVariant;
		}
	}

	// Conditional History
	if ( bOk )
	{
		SVTreeType::SVBranchHandle htiCH = NULL;
		BOOL bTemp = SVNavigateTree::GetItemBranch( rTree, CTAG_CONDITIONAL_HISTORY, htiParent, htiCH );
		if ( bTemp )
		{
			// iterate through all sub items
			VARIANT* pVariant = NULL;
			SVScalarValueVector vecProperties;
			SVTreeType::SVLeafHandle htiChild;

			htiChild = rTree.getFirstLeaf( htiCH );

			while( rTree.isValidLeaf( htiCH, htiChild ) )
			{
				CString Name;
				_variant_t Value;

				Name = rTree.getLeafName( htiChild ).c_str();
				Value = rTree.getLeafData( htiChild );

				if ( Value.vt == VT_BSTR )
				{
					CString strValue = static_cast< LPCTSTR >( _bstr_t( Value ) );

					SVScalarValue l_Value = SVScalarValue( Name, strValue );
					vecProperties.push_back( l_Value );
				}

				htiChild = rTree.getNextLeaf( htiCH, htiChild );
			}

			pInspection->SetConditionalHistoryProperties( vecProperties );
		}
	}

	if ( bOk )
	{
		WINDOWPLACEMENT wndpl;

		SVTreeType::SVBranchHandle htiWindow = NULL;

		bOk = SVNavigateTree::GetItemBranch( rTree, CTAG_WINDOW_PLACEMENT, htiParent, htiWindow );
		if ( bOk )
		{
			bOk = SVNavigateTree::GetItem( rTree, CTAG_LENGTH, htiWindow, svVariant );
			if ( bOk ) { wndpl.length = svVariant; }

			if ( bOk )
			{
				bOk = SVNavigateTree::GetItem( rTree, CTAG_FLAGS, htiWindow, svVariant );
				if ( bOk ) { wndpl.flags = svVariant; }
			}

			if ( bOk )
			{
				bOk = SVNavigateTree::GetItem( rTree, CTAG_SHOW_COMMAND, htiWindow, svVariant );
				if ( bOk ) { wndpl.showCmd = svVariant; }
			}

			if ( bOk )
			{
				SVTreeType::SVBranchHandle htiData = NULL;

				bOk = SVNavigateTree::GetItemBranch( rTree, CTAG_MINIMUM_POSITION, htiWindow, htiData );
				if ( bOk )
				{
					bOk = SVNavigateTree::GetItem( rTree, CTAG_X, htiData, svVariant );
					if ( bOk ) { wndpl.ptMinPosition.x = svVariant; }

					if ( bOk )
					{
						bOk = SVNavigateTree::GetItem( rTree, CTAG_Y, htiData, svVariant );
						if ( bOk ) { wndpl.ptMinPosition.y = svVariant; }
					}
				}
			}

			if ( bOk )
			{
				SVTreeType::SVBranchHandle htiData = NULL;

				bOk = SVNavigateTree::GetItemBranch( rTree, CTAG_MAXIMUM_POSITION, htiWindow, htiData );
				if ( bOk )
				{
					bOk = SVNavigateTree::GetItem( rTree, CTAG_X, htiData, svVariant );
					if ( bOk ) { wndpl.ptMaxPosition.x = svVariant; }

					if ( bOk )
					{
						bOk = SVNavigateTree::GetItem( rTree, CTAG_Y, htiData, svVariant );
						if ( bOk ) { wndpl.ptMaxPosition.y = svVariant; }
					}
				}
			}

			if ( bOk )
			{
				SVTreeType::SVBranchHandle htiData = NULL;

				bOk = SVNavigateTree::GetItemBranch( rTree, CTAG_NORMAL_POSITION, htiWindow, htiData );
				if ( bOk )
				{
					bOk = SVNavigateTree::GetItem( rTree, CTAG_LEFT, htiData, svVariant );
					if ( bOk ) { wndpl.rcNormalPosition.left = svVariant; }

					if ( bOk )
					{
						bOk = SVNavigateTree::GetItem( rTree, CTAG_TOP, htiData, svVariant );
						if ( bOk ) { wndpl.rcNormalPosition.top = svVariant; }
					}

					if ( bOk )
					{
						bOk = SVNavigateTree::GetItem( rTree, CTAG_RIGHT, htiData, svVariant );
						if ( bOk ) { wndpl.rcNormalPosition.right = svVariant; }
					}

					if ( bOk )
					{
						bOk = SVNavigateTree::GetItem( rTree, CTAG_BOTTOM, htiData, svVariant );
						if ( bOk ) { wndpl.rcNormalPosition.bottom = svVariant; }
					}
				}
			}
		}

		if ( bOk )
		{
			//
			// The first view is the Tool Set view.
			//
			View.pToolSetView = dynamic_cast< ToolSetView* >( getView() );
			if( View.pToolSetView && View.pToolSetView->GetSafeHwnd() )
			{
				CSplitterWnd* pWndSplitter = dynamic_cast< CSplitterWnd* >( View.pToolSetView->GetParent() );
				if( pWndSplitter && pWndSplitter->GetSafeHwnd() )
				{
					//
					// This the one we want to retrieve the size and position.
					//
					CSplitterWnd* pWndSplitter2 = dynamic_cast< CSplitterWnd* >( pWndSplitter->GetParent() );
					ASSERT( pWndSplitter2 && pWndSplitter2->GetSafeHwnd() );

					SVIPSplitterFrame* pFrame = dynamic_cast< SVIPSplitterFrame* >( pWndSplitter2->GetParent() );

					CRuntimeClass* pClass = pFrame->GetRuntimeClass();
					CString csClass = pClass->m_lpszClassName;
					ASSERT( csClass == _T( "SVIPSplitterFrame" ) );

					if( pFrame && pFrame->GetSafeHwnd() )
					{
						bOk = pFrame->SetWindowPlacement( &wndpl ); // WINDOWPLACEMENT* lpwndpl
					}// end if
				} // if(pFrame && pFrame->GetSafeHwnd())
			} // if(pWndSplitter && pWndSplitter->GetSafeHwnd())
		} // if( View.pToolSetView && View.pToolSetView->GetSafeHwnd() )
	}

	if ( bOk )
	{
		SVTreeType::SVBranchHandle htiIPViews = NULL;
		SVTreeType::SVBranchHandle htiViews = NULL;

		bOk = SVNavigateTree::GetItemBranch( rTree, CTAG_IPDOC_VIEWS, htiParent, htiIPViews );
		if ( bOk )
		{
			// Serialze View Data...
			mbInitImagesByName = TRUE;
			POSITION vPos;
			SVString Name;
			_variant_t svVariant;

			long lViewNumber = 0;
			long lNumberOfViews = 0;

			SVTreeType::SVBranchHandle htiBranch = NULL;

			bOk = SVNavigateTree::GetItemBranch( rTree, CTAG_VIEWS, htiIPViews, htiViews );
			if ( bOk )
			{
				SVTreeType::SVBranchHandle htiItem( nullptr ); 

				htiItem = rTree.getFirstBranch( htiViews );

				while ( bOk && nullptr != htiItem )
				{
					Name = rTree.getBranchName( htiItem );

					// The class SVToolSetTabView was changed to ToolSetView when the Tool Set Tree View was removed.
					Name.replace( CTAG_SVTOOLSET_TAB_VIEW_CLASS, CTAG_TOOLSET_VIEW );

					bOk = SVNavigateTree::GetItem( rTree, CTAG_VIEW_NUMBER, htiItem, svVariant );
					if ( bOk ) { lViewNumber = svVariant; }

					if ( bOk )
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
							if ( !View.pView || 
								Name.Compare(View.pView->GetRuntimeClass()->m_lpszClassName))
							{
								vPos = GetFirstViewPosition();
								while( ( View.pView = GetNextView( vPos ) ) && 
									( Name.Compare( View.pView->GetRuntimeClass()->m_lpszClassName ) ) );
							}

							if (View.pView)  // this should never fail, but if it does, we'll try to continue
							{
								if (View.pView->IsKindOf(RUNTIME_CLASS(SVImageViewScroll)))
								{
									bOk = View.pImageScroll->SetParameters( rTree, htiItem );
								}
								else if (View.pView->IsKindOf(RUNTIME_CLASS(SVImageViewClass)))
								{
									bOk = View.pImageView->SetParameters( rTree, htiItem );
								}
								else if (View.pView->IsKindOf(RUNTIME_CLASS( ToolSetView )))
								{
									bOk = View.pToolSetView->SetParameters( rTree, htiItem );
								}
								else if (View.pView->IsKindOf(RUNTIME_CLASS(SVResultViewClass)))
								{
									bOk = View.pResultView->SetParameters( rTree, htiItem );
								}
							}
						}
					}
					htiItem = rTree.getNextBranch( htiViews, htiItem );
				}
			}

			bOk = SVNavigateTree::GetItemBranch( rTree, CTAG_VIEWS, htiIPViews, htiViews );
			if ( bOk )
			{
				SVTreeType::SVBranchHandle htiItem( nullptr );

				htiItem = rTree.getFirstBranch( htiViews );

				while ( bOk && nullptr != htiItem )
				{
					Name = rTree.getBranchName( htiItem );

					bOk = SVNavigateTree::GetItem( rTree, CTAG_VIEW_NUMBER, htiItem, svVariant );
					if ( bOk ) { lViewNumber = svVariant; }

					if ( bOk )
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
							if ( !View.pView || 
								Name.Compare(View.pView->GetRuntimeClass()->m_lpszClassName))
							{
								vPos = GetFirstViewPosition();
								while( ( View.pView = GetNextView( vPos ) ) && 
									( Name.Compare( View.pView->GetRuntimeClass()->m_lpszClassName ) ) );
							}

							if (View.pView)  // this should never fail, but if it does, we'll try to continue
							{
								if (View.pView->IsKindOf(RUNTIME_CLASS(SVImageViewScroll)))
								{
									bOk = View.pImageScroll->CheckParameters( rTree, htiItem );
								}
								else if (View.pView->IsKindOf(RUNTIME_CLASS(SVImageViewClass)))
								{
									bOk = View.pImageView->CheckParameters( rTree, htiItem );
								}
								else if (View.pView->IsKindOf(RUNTIME_CLASS( ToolSetView )))
								{
									bOk = View.pToolSetView->CheckParameters( rTree, htiItem );
								}
								else if (View.pView->IsKindOf(RUNTIME_CLASS(SVResultViewClass)))
								{
									bOk = View.pResultView->CheckParameters( rTree, htiItem );
								}
							}
						}
					}
					htiItem = rTree.getNextBranch( htiViews, htiItem );
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

	}
	return bOk;
}

void SVIPDoc::SetModifiedFlag(BOOL bModified /*= TRUE*/)
{
	CDocument::SetModifiedFlag(bModified);
	if ( bModified )
	{
		SVSVIMStateClass::AddState( SV_STATE_MODIFIED );
	}
}

void SVIPDoc::OnAddExternalTool()
{
	SVToolClass* pTool = new SVExternalTool( TRUE );

	if( AddTool( pTool ) ) { return; }

	if( pTool ) { delete( pTool ); }
}

HRESULT SVIPDoc::RemoveImage(SVImageClass* pImage)
{
	GUID sourceID = pImage->GetUniqueObjectID();

	return RemoveImage( sourceID );
}

HRESULT SVIPDoc::RemoveImage(const SVGUID& p_rImageId)
{
	HRESULT l_Status = S_FALSE;

	for (int i = 0; i < MaxImageViews; ++i)
	{
		SVImageViewClass *l_pView = GetImageView( i );

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
	publishList.Refresh( ( SVTaskObjectClass* )GetToolSet() );

	SetModifiedFlag();
}

HRESULT SVIPDoc::GetResults( SVResultsWrapperClass* p_pResults )
{
	///MEC_SVO_475 this is used by remote interface !!!!
	HRESULT Status = E_FAIL;
	int item(0);
	SVResultListClass* pResultList = GetResultList();
	if( nullptr != pResultList)
	{
		item += pResultList->AddResults( p_pResults, GetTitle() );
		
		Status = S_OK;
	}

	

	return Status;
}

SVImageClass* SVIPDoc::GetHSIMainImage()
{
	SVImageClass* l_pImage = NULL;

	if( GetInspectionProcess() != NULL )
	{
		l_pImage = GetInspectionProcess()->GetHSIMainImage();
	}

	return l_pImage;
}

HRESULT SVIPDoc::GetCameras( SVVirtualCameraPtrSet& p_rCameras ) const
{
	HRESULT l_Status = S_OK;

	if( GetInspectionProcess() != nullptr )
	{
		l_Status = GetInspectionProcess()->GetCameras( p_rCameras );
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

HRESULT SVIPDoc::DeleteTool(SVTaskObjectClass* pTaskObject)
{
	ASSERT( pTaskObject );
	HRESULT l_hr = S_OK;

	SVInspectionProcess* pInspection( GetInspectionProcess() );

	if( nullptr == pInspection )
	{
		return E_FAIL;
	}

	if( pTaskObject )
	{
		// Check for Dependents
		int rc = checkOkToDelete( pTaskObject );

		if( rc )
		{
			CWaitCursor l_cwcMouse;

			// remove image from affected displays
			int i, j;
			SVImageViewClass *pImageView;
			SVOutObjectInfoStruct *pOutputObjectInfo;
			GUID ViewImageUniqueId;

			SVOutputInfoListClass l_OutputList;

			pTaskObject->GetOutputList( l_OutputList );

			for (i = 0; i < MaxImageViews; i++)
			{
				pImageView = GetImageView( i );
				if (pImageView->GetImage())
				{
					ViewImageUniqueId = pImageView->GetImage()->GetUniqueObjectID();

					for (j = 0; j < l_OutputList.GetSize(); j++)
					{
						pOutputObjectInfo = l_OutputList.GetAt(j);
						if (ViewImageUniqueId == pOutputObjectInfo->UniqueObjectID)
						{
							// Close Display resources...
							pImageView->DetachFromImage();
							break;
						}
					}
				}
			}

			SetSelectedToolID( SVGUID() );

			SVDeleteTool l_DeleteTool( dynamic_cast< SVToolClass* >( pTaskObject ) );

			SVObjectManagerClass::Instance().UpdateObserver( GetInspectionID(), l_DeleteTool );

			SVConfigurationObject* pConfig( nullptr );
			SVObjectManagerClass::Instance().GetConfigurationObject( pConfig );
			if ( nullptr != pConfig )
			{
				pConfig->ValidateRemoteMonitorList();
				TheSVObserverApp.GetIODoc()->UpdateAllViews( NULL );
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
	SVToolClass* l_pTool = dynamic_cast< SVToolClass* >( SVObjectManagerClass::Instance().GetObject( GetSelectedToolID() ) );

	if( l_pTool )
	{
		SVObjectInfoStruct info = l_pTool->GetObjectInfo();
		SVObjectTypeInfoStruct typeInfo = info.ObjectTypeInfo;
		//------ Warp tool hands back a SVPolarTransformObjectType. Sub type 1792.
		//------ Window tool, Luminance hands back a SVImageObjectType. Sub type 0.
		//------ Gage tool (edge counting analyzer) hands back a SVROIObjectType. Sub type 1280.
		if( SVImageViewClass* pImageView = GetImageView() )
		{
			SVSVIMStateClass::AddState( SV_STATE_EDITING );
			CString DlgName;
			DlgName.Format("Adjust Tool Size and Position - %s",l_pTool->GetName());
			SVAdjustToolSizePositionDlg dlg(DlgName, dynamic_cast< CWnd* >( this->GetMDIChild() ), l_pTool );
			dlg.DoModal();
			SVSVIMStateClass::RemoveState( SV_STATE_EDITING );
		}
	}
}

void SVIPDoc::OnUpdateEditAdjustToolPosition(CCmdUI* pCmdUI)
{
	BOOL Enabled = SVSVIMStateClass::CheckState( SV_STATE_READY ) && SVSVIMStateClass::CheckState( SV_STATE_EDIT );
	// Check current user access...
	Enabled = Enabled && TheSVObserverApp.OkToEdit();

	if( Enabled )
	{
		Enabled = FALSE;
		ToolSetView* pToolSetView = GetToolSetView();
		SVToolSetClass* pToolSet = GetToolSet();
		if( nullptr != pToolSet && nullptr != pToolSetView && !pToolSetView->IsLabelEditing())
		{
			ToolListSelectionInfo ToolListInfo = pToolSetView->GetToolListSelectionInfo();

			const SVGUID& rGuid = GetSelectedToolID();
			//Tool list active and valid tool
			if( !rGuid.empty() && -1 != ToolListInfo.m_listIndex )
			{
				SVToolClass* Tool = dynamic_cast< SVToolClass* >( SVObjectManagerClass::Instance().GetObject(rGuid));
				if (Tool)
				{
					//check to see if the tool has extents
					if ( Tool->DoesObjectHaveExtents() )
					{
						Enabled = TRUE;
					}
				}
			}
		}
	}
	pCmdUI->Enable( Enabled );
}

void SVIPDoc::OnShowToolRelations()
{
	SVGUID selectedToolID = GetSelectedToolID();
	if (GUID_NULL != selectedToolID)
	{
		SVGUID inspectionID = GetInspectionID();
		
		SvOg::SVShowDependentsDialog Dlg(inspectionID, selectedToolID, false, SVToolObjectType, nullptr, SvOg::SVShowDependentsDialog::Show);
		Dlg.DoModal();
	}
}

void SVIPDoc::OnUpdateShowToolRelations(CCmdUI* pCmdUI)
{
	BOOL Enabled = SVSVIMStateClass::CheckState( SV_STATE_READY ) && SVSVIMStateClass::CheckState( SV_STATE_EDIT );
	// Check current user access...
	Enabled = Enabled && TheSVObserverApp.OkToEdit();

	pCmdUI->Enable( Enabled );
}

void SVIPDoc::OnViewResetAllCounts()
{
	if( S_OK == TheSVObserverApp.m_svSecurityMgr.SVValidate( SECURITY_POINT_VIEW_MENU_RESET_COUNTS_ALL ) )
	{
		TheSVObserverApp.ResetAllCounts();
	}
}

void SVIPDoc::OnViewResetCountsCurrentIP()
{
	if( S_OK == TheSVObserverApp.m_svSecurityMgr.SVValidate( SECURITY_POINT_VIEW_MENU_RESET_COUNTS_CURRENT ) )
	{
		SVInspectionProcess* pInspection = GetInspectionProcess();

		// @TODO:  What should we do if there's an error?  (Method is void.)
		if( nullptr != pInspection && nullptr != pInspection->GetToolSet() )
		{
			pInspection->GetToolSet()->ResetCounts();
		}

		if( !SVSVIMStateClass::CheckState( SV_STATE_RUNNING ) )
		{
			RunOnce();
		}
	}
}

void SVIPDoc::SetRegressionTestRunMode(RegressionRunModeEnum newMode)
{
	m_regtestRunMode = static_cast< RegressionRunModeEnum >( newMode );
}

void SVIPDoc::SetRegressionTestPlayMode(RegressionPlayModeEnum newPlayMode)
{
	m_regtestRunPlayMode = newPlayMode;
}

DWORD WINAPI SVIPDoc::SVRegressionTestRunThread( LPVOID lpParam )
{
	SVIPDoc* l_IPDoc = reinterpret_cast< SVIPDoc* >( lpParam );

	if( nullptr == l_IPDoc->GetInspectionProcess() ) { return E_FAIL; }

	bool l_bFirst = true;
	l_IPDoc->m_bRegressionTestRunning = true;

	l_IPDoc->m_regtestRunMode = RegModePause;
	l_IPDoc->m_bRegressionTestStopping = false;

	bool l_bUsingSingleFile = false;
	BOOL bDisplayFile = FALSE;
	BOOL bModeReset = FALSE;

	while ( l_IPDoc->m_regtestRunMode != RegModeStopExit)
	{
		//while in Pause mode, sleep
		bModeReset = FALSE;
		if ( l_IPDoc->m_regtestRunMode == RegModePause )
		{
			::Sleep(50);
			continue;
		}

		CList<RegressionRunFileStruct*,RegressionRunFileStruct*> l_lstFileNames;
		RegressionRunFileStruct *ptmpRunFileStruct;
		RegressionTestStruct *ptmpRegTestStruct;

		INT_PTR iListCnt = l_IPDoc->m_listRegCameras.GetCount();
		bool l_bDone = false;
		bool l_bListDone = false;
		int l_iNumSingleFile = 0;

		for ( INT_PTR i = 0; i < iListCnt; i++ )
		{
			POSITION posCamera = l_IPDoc->m_listRegCameras.FindIndex(i);

			if ( posCamera == NULL ) { continue; }
			l_bDone = false;

			ptmpRunFileStruct = new RegressionRunFileStruct;
			ptmpRegTestStruct = l_IPDoc->m_listRegCameras.GetAt(posCamera);

			if ( ptmpRegTestStruct->iFileMethod == RegSingleFile )
			{
				l_bUsingSingleFile = true;
				l_iNumSingleFile++;
			}

			ptmpRunFileStruct->csCameraName = ptmpRegTestStruct->csCamera;
			switch (l_IPDoc->m_regtestRunMode)
			{
			case RegModePlay:
				{
					bDisplayFile = TRUE;
					if ( !l_bFirst )
					{
						ptmpRegTestStruct->stdIteratorCurrent++;
					}
					if ( ptmpRegTestStruct->stdIteratorCurrent == ptmpRegTestStruct->stdVectorFile.end() )
					{
						ptmpRegTestStruct->stdIteratorCurrent = ptmpRegTestStruct->stdVectorFile.begin();
					}

					if ( ptmpRegTestStruct->iFileMethod != RegSingleFile)
					{
						if ( l_IPDoc->m_regtestRunPlayMode != Continue && !l_bFirst )
						{
							if ( ptmpRegTestStruct->stdIteratorCurrent == ptmpRegTestStruct->stdIteratorStart )
							{
								l_bListDone = true;
								bDisplayFile = FALSE;
							}
						}
					}

					if (l_IPDoc->m_regtestRunPlayMode != Continue)
					{
						if ( l_bUsingSingleFile )
						{
							if ( iListCnt == l_iNumSingleFile ) //all using single file
							{
								l_IPDoc->m_regtestRunMode = RegModePause;
								SendMessage(( ( SVMainFrame* ) AfxGetApp()->m_pMainWnd )->m_pregTestDlg->GetSafeHwnd(),WM_REGRESSION_TEST_SET_PLAYPAUSE,(LPARAM)&l_IPDoc->m_regtestRunMode,0);
								bModeReset = true;
								l_bFirst = true;
							}
							else
							{
								//not all using single file...
								//check to see if at end of list.  if so stop...
								if ( l_bListDone && ( (iListCnt-1) == i) )
								{
									l_IPDoc->m_regtestRunMode = RegModePause;
									SendMessage(( ( SVMainFrame* ) AfxGetApp()->m_pMainWnd )->m_pregTestDlg->GetSafeHwnd(),WM_REGRESSION_TEST_SET_PLAYPAUSE,(LPARAM)&l_IPDoc->m_regtestRunMode,0);
									bModeReset = true;
									l_bFirst = true;
									bDisplayFile = FALSE;
								}
							}
						}
						else
						{  //not using single files.
							if ( l_bListDone && ( (iListCnt-1) == i) )
							{
								l_IPDoc->m_regtestRunMode = RegModePause;
								SendMessage(( ( SVMainFrame* ) AfxGetApp()->m_pMainWnd )->m_pregTestDlg->GetSafeHwnd(),WM_REGRESSION_TEST_SET_PLAYPAUSE,(LPARAM)&l_IPDoc->m_regtestRunMode,0);
								bModeReset = true;
								l_bFirst = true;
								bDisplayFile = FALSE;
							}
						}
					}

					ptmpRunFileStruct->csFileName = *ptmpRegTestStruct->stdIteratorCurrent;
					break;
				}
			case RegModeSingleStepForward:
				{
					if ( !l_bFirst )
					{
						ptmpRegTestStruct->stdIteratorCurrent++;
					}

					if ( ptmpRegTestStruct->stdIteratorCurrent == ptmpRegTestStruct->stdVectorFile.end() )
					{
						ptmpRegTestStruct->stdIteratorCurrent = ptmpRegTestStruct->stdVectorFile.begin();
					}
					ptmpRunFileStruct->csFileName = *ptmpRegTestStruct->stdIteratorCurrent;
					bDisplayFile = TRUE;
					break;
				}
			case RegModeSingleStepBack:
				{
					if ( !l_bFirst )
					{
						if( ptmpRegTestStruct->stdIteratorCurrent == ptmpRegTestStruct->stdVectorFile.begin() )
						{
							ptmpRegTestStruct->stdIteratorCurrent = ptmpRegTestStruct->stdVectorFile.end() - 1;
						}
						else
						{
							ptmpRegTestStruct->stdIteratorCurrent--;
						}
					}

					if ( ptmpRegTestStruct->stdIteratorCurrent == ptmpRegTestStruct->stdVectorFile.end() )
					{
						ptmpRegTestStruct->stdIteratorCurrent = ptmpRegTestStruct->stdVectorFile.end() - 1;
					}

					ptmpRunFileStruct->csFileName = *ptmpRegTestStruct->stdIteratorCurrent;
					bDisplayFile = TRUE;

					break;
				}
			case RegModeBackToBeginningPlay:
				{
					ptmpRegTestStruct->stdIteratorCurrent = ptmpRegTestStruct->stdIteratorStart;
					ptmpRunFileStruct->csFileName = *ptmpRegTestStruct->stdIteratorCurrent;
					bDisplayFile = TRUE;
					break;
				}
			case RegModeBackToBeginningStop:
				{
					ptmpRegTestStruct->stdIteratorCurrent = ptmpRegTestStruct->stdIteratorStart;
					ptmpRunFileStruct->csFileName = *ptmpRegTestStruct->stdIteratorCurrent;
					bDisplayFile = TRUE;
					break;
				}
			case RegModeResetSettings:
				{
					l_bFirst = TRUE;
					ptmpRegTestStruct->stdIteratorCurrent = ptmpRegTestStruct->stdIteratorStart;

					if ( i == (iListCnt-1) ) //last item in list
					{
						//set back to pause
						l_IPDoc->m_regtestRunMode = RegModePause;
					}
					l_bUsingSingleFile = false; //reset this value
					l_iNumSingleFile = 0;
					bDisplayFile = FALSE;
					bModeReset = TRUE;
					break;
				}
			default:
				{
					break;
				}
			}//end switch

			if ( bDisplayFile )
			{
				l_IPDoc->m_listRegCameras.SetAt(posCamera,ptmpRegTestStruct);

				l_lstFileNames.AddTail(ptmpRunFileStruct);

				if( l_IPDoc->GetInspectionProcess() != NULL )
				{
					l_IPDoc->GetInspectionProcess()->AddInputImageRequestByCameraName(ptmpRunFileStruct->csCameraName,ptmpRunFileStruct->csFileName);
				}
				else
				{
					break;
				}
			}
		}//end for i to num cameras...
		if ( l_IPDoc->m_regtestRunMode == RegModeBackToBeginningPlay ) 
		{
			l_IPDoc->m_regtestRunMode = RegModePlay;
		}

		if ( l_IPDoc->m_regtestRunMode == RegModeBackToBeginningStop)
		{
			l_IPDoc->m_regtestRunMode = RegModePause;
			SendMessage(( ( SVMainFrame* ) AfxGetApp()->m_pMainWnd )->m_pregTestDlg->GetSafeHwnd(),WM_REGRESSION_TEST_SET_PLAYPAUSE,(LPARAM)&l_IPDoc->m_regtestRunMode,0);
		}
		l_bDone = false;
		//send image info to the dialog...
		if ( bDisplayFile )
		{
			bDisplayFile = FALSE;

			SendMessage(( ( SVMainFrame* ) AfxGetApp()->m_pMainWnd )->m_pregTestDlg->GetSafeHwnd(),WM_REGRESSION_TEST_SET_FILENAMES,(LPARAM)&l_lstFileNames,0);

			l_IPDoc->RunOnce();

			//////////////////////////////////////////////////////////////////////////////////
			//clean up
			INT_PTR iNumList = l_lstFileNames.GetCount();
			//delete l_lstFileNames
			for ( int iNumDel = static_cast<int>(iNumList) - 1; iNumDel >= 0; --iNumDel )
			{
				POSITION posDel = l_lstFileNames.FindIndex(iNumDel);
				if ( posDel != NULL )
				{
					RegressionRunFileStruct *pDeleteStruct = l_lstFileNames.GetAt(posDel);
					if ( pDeleteStruct )
					{
						delete pDeleteStruct;
					}
				}
			} //end of loop to delete temp structures...
		}

		if ( !l_IPDoc->m_bRegressionTestStopping )
		{
			if ( l_IPDoc->m_regtestRunMode == RegModePlay )
			{
				int iMS = l_IPDoc->m_iRegessionTimeoutMS;
				if( iMS < MinRegressionTime )
				{
					iMS = MinRegressionTime;
				}
				Sleep(iMS);
			}
			else 
			{
				l_IPDoc->m_regtestRunMode = RegModePause;
			}
		}

		if ( !bModeReset )
		{
			l_bFirst = false;
		}
	}//end of while loop

	//let the IP know that the regression test is done.
	//	CWnd* pWnd = AfxGetMainWnd(); // SEJ -this fails in VC8 (could be cause we are in a different thread?)
	// @WARNING:  Pointers should be checked before they are dereferenced.
	CWnd* pWnd = AfxGetApp()->m_pMainWnd;
	PostMessage (pWnd->m_hWnd, WM_COMMAND, WM_REGRESSION_TEST_COMPLETE, 0L);

	//let the RegressionRunDlg know that it is to shut down...
	SendMessage(( ( SVMainFrame* ) pWnd )->m_pregTestDlg->GetSafeHwnd(), WM_REGRESSION_TEST_CLOSE_REGRESSION, NULL, 0);

	return 0L;
}

void SVIPDoc::RegressionTestComplete()
{
	// @WARNING:  Pointers should be checked before they are dereferenced.
	( ( SVMainFrame* ) AfxGetApp()->m_pMainWnd )->m_pregTestDlg->DestroyWindow();
	delete ( ( SVMainFrame* ) AfxGetApp()->m_pMainWnd )->m_pregTestDlg;
	( ( SVMainFrame* ) AfxGetApp()->m_pMainWnd )->m_pregTestDlg = NULL;

	SVSVIMStateClass::RemoveState(SV_STATE_REGRESSION);
}

void SVIPDoc::RegressionTestModeChanged()
{
	// @WARNING:  Pointers should be checked before they are dereferenced.
	::BringWindowToTop(AfxGetMainWnd()->GetSafeHwnd() );
	m_bRegressionTestRunning = false;
	m_regtestRunPlayMode = RunToEnd;
	m_bRegressionTestStopping = true;
	m_regtestRunMode = RegModeStopExit;  //should cause dialog to go away...

	SVSVIMStateClass::RemoveState(SV_STATE_REGRESSION);

	SVSendMessage( GetInspectionProcess(), SVM_GOING_OFFLINE, NULL, NULL ); // Mainly used to tell Archive Tool to shutdown
	// @WARNING:  Pointers should be checked before they are dereferenced.
	GetToolSet()->ResetObject();
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

void SVIPDoc::SetSelectedToolID( const SVGUID& p_rToolID )
{
	ToolSetView* pView = GetToolSetView();
	if (pView)
	{
		pView->SetSelectedTool(p_rToolID);
	}
}

bool SVIPDoc::IsToolPreviousToSelected( const SVGUID& p_rToolID ) const
{
	bool l_Status = false;
	if( GetToolSet() != nullptr )
	{
		l_Status = GetToolSet()->IsToolPreviousToSelected( p_rToolID );
	}

	return l_Status;
}

CString SVIPDoc::GetCompleteToolSetName() const
{
	CString l_Name;

	SVToolSetClass* l_pToolSet = GetToolSet();

	if( l_pToolSet != nullptr )
	{
		l_Name = l_pToolSet->GetCompleteObjectNameToObjectType( nullptr, SVToolObjectType );
	}

	return l_Name;
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
	pCmdUI->Enable( TheSVObserverApp.m_svSecurityMgr.SVIsDisplayable( SECURITY_POINT_VIEW_MENU_RESET_COUNTS_ALL) );
}

void SVIPDoc::OnUpdateViewResetCountsCurrentIP(CCmdUI* pCmdUI)
{
	// @WARNING:  Pointers should be checked before they are dereferenced.
	pCmdUI->Enable( TheSVObserverApp.m_svSecurityMgr.SVIsDisplayable( SECURITY_POINT_VIEW_MENU_RESET_COUNTS_CURRENT) );
}

void SVIPDoc::ClearRegressionTestStructures()
{
	INT_PTR iCnt = m_listRegCameras.GetCount();
	if ( iCnt > 0 )
	{
		//empty list
		RegressionTestStruct* pTmpStruct = nullptr;
		for ( INT_PTR i = iCnt - 1; i >= 0; --i )
		{
			POSITION pos = m_listRegCameras.FindIndex(i);
			pTmpStruct = m_listRegCameras.GetAt(pos);
			if ( pTmpStruct )
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

	if( nullptr != pInspection )
	{
		SVSVIMStateClass::AddState(SV_STATE_EDITING);
		CString strTitle = _T("Data Definition Lists - ");
		strTitle += pInspection->GetName();
		SVDataDefinitionSheet sheet(strTitle, *pInspection );

		//remove apply button
		sheet.m_psh.dwFlags |= PSH_NOAPPLYNOW;

		sheet.DoModal();
		SVSVIMStateClass::RemoveState(SV_STATE_EDITING);
	}
}

void SVIPDoc::OnUpdateEditDataDefinitionLists(CCmdUI *pCmdUI)
{
	// @WARNING:  Pointers should be checked before they are dereferenced.
	pCmdUI->Enable( SVSVIMStateClass::CheckState( SV_STATE_READY ) && SVSVIMStateClass::CheckState( SV_STATE_EDIT ) );
}

HRESULT SVIPDoc::RebuildImages()
{
	HRESULT l_Status = S_OK;

	SVImageIdImageDataStructMap::iterator l_Iter = m_Images.begin();

	while( l_Iter != m_Images.end() )
	{
		SVMasterImageRegisterMap::iterator l_RegisteredIter = m_RegisteredImages.find( l_Iter->first );

		if( l_RegisteredIter != m_RegisteredImages.end() )
		{
			// If image from m_Images map is in m_RegisteredImages map (GUID)

			SVImageViewPtrImageViewStatusMap::iterator l_ViewIter = l_Iter->second.m_ImageViews.begin();

			// While a view is found for the m_Images image 
			while( l_ViewIter != l_Iter->second.m_ImageViews.end() )
			{
				// try to find the a view in the m_RegisteredImages map for the
				// associated image that matched the m_Images based image.
				SVImageViewPtrSet::iterator l_RegisteredViewIter = l_RegisteredIter->second.find( l_ViewIter->first );

				if( l_RegisteredViewIter != l_RegisteredIter->second.end() )
				{
					// If the view was found in the m_RegisteredImage, then 
					// move to the next m_Images view.
					++l_ViewIter;
				}
				else
				{
					// else remove the m_Images view.
					l_ViewIter = l_Iter->second.m_ImageViews.erase( l_ViewIter );
				}
			}

			++l_Iter;  // m_Images iterator
		}
		else
		{
			// If image from m_Images map is not in m_RegisteredImages map 
			// then delete from m_Images map.
			l_Iter = m_Images.erase( l_Iter );
		}
	}

	SVMasterImageRegisterMap::iterator l_RegisteredIter = m_RegisteredImages.begin();

	while( l_RegisteredIter != m_RegisteredImages.end() )
	{
		SVImageViewPtrSet::iterator l_RegisteredViewIter = l_RegisteredIter->second.begin();

		while( l_RegisteredViewIter != l_RegisteredIter->second.end() )
		{
			SVImageDataStruct& l_rImage = m_Images[ l_RegisteredIter->first ];

			SVImageViewPtrImageViewStatusMap::iterator l_ViewIter = l_rImage.m_ImageViews.find( *l_RegisteredViewIter );

			if( l_ViewIter == l_rImage.m_ImageViews.end() )
			{
				l_rImage.m_ImageViews[ *l_RegisteredViewIter ] = SVImageViewStatusStruct();
			}

			if( l_rImage.m_ImageData.empty() )
			{
				SVImageClass* l_pImage = dynamic_cast< SVImageClass* >( SVObjectManagerClass::Instance().GetObject( l_RegisteredIter->first.ToGUID() ) );

				if( ( l_pImage != NULL ) )
				{
					SVBitmapInfo l_Info;

					BITMAPINFOHEADER l_Header = l_pImage->GetImageInfo().GetBitmapInfoHeader();

					l_Info.Assign( l_Header.biWidth, l_Header.biHeight, l_Header.biBitCount, SVBitmapInfo::GetDefaultColorTable( l_Header.biBitCount ) );

					l_rImage.m_ImageData.resize( l_Info.GetBitmapInfoSizeInBytes() + l_Info.GetBitmapImageSizeInBytes(), 0 );

					::memcpy( &( l_rImage.m_ImageData[ 0 ] ), l_Info.GetBitmapInfo(), l_Info.GetBitmapInfoSizeInBytes() );
				}
			}

			++l_RegisteredViewIter;
		}

		++l_RegisteredIter;
	}

	::InterlockedExchange( &m_AllViewsUpdated, 0 );

	return l_Status;
}

HRESULT SVIPDoc::CheckImages()
{
	HRESULT l_Status = S_OK;

	if( m_AllViewsUpdated == 0 )
	{
		bool l_Refresh = true;

		SVImageIdImageDataStructMap::iterator l_Iter = m_Images.begin();

		while( l_Refresh && l_Iter != m_Images.end() )
		{
			SVImageViewPtrImageViewStatusMap::iterator l_ViewIter = l_Iter->second.m_ImageViews.begin();

			while( l_Refresh && l_ViewIter != l_Iter->second.m_ImageViews.end() )
			{
				l_Refresh &= l_ViewIter->second.m_DisplayComplete;

				++l_ViewIter;
			}

			++l_Iter;
		}

		if( l_Refresh )
		{
			::InterlockedExchange( &m_AllViewsUpdated, 1 );

			m_oDisplay.SetIPDocDisplayComplete();
		}
	}

	return l_Status;
}

HRESULT SVIPDoc::RegisterImage( const SVGUID& p_rImageId, SVImageViewClass* p_pImageView )
{
	HRESULT l_Status = S_OK;

	SVMasterImageRegisterMap::iterator l_Iter = m_RegisteredImages.begin();

	while( l_Iter != m_RegisteredImages.end() )
	{
		l_Iter->second.erase( p_pImageView );

		if( l_Iter->second.empty() )
		{
			l_Iter = m_RegisteredImages.erase( l_Iter );
		}
		else
		{
			++l_Iter;
		}
	}

	if( !( p_rImageId.empty() ) )
	{
		m_RegisteredImages[ p_rImageId ].insert( p_pImageView );

		l_Status = RebuildImages();
	}
	else
	{
		l_Status = E_FAIL;
	}

	SVInspectionProcess* pInspection( GetInspectionProcess() );

	if( nullptr != pInspection  ) { pInspection->LastProductNotify(); }

	return l_Status;
}

HRESULT SVIPDoc::UnregisterImage( const SVGUID& p_rImageId, SVImageViewClass* p_pImageView )
{
	HRESULT l_Status = S_OK;

	SVMasterImageRegisterMap::iterator l_Iter = m_RegisteredImages.find( p_rImageId );

	if( l_Iter != m_RegisteredImages.end() )
	{
		l_Iter->second.erase( p_pImageView );

		if( l_Iter->second.empty() ) { m_RegisteredImages.erase( l_Iter ); }

		l_Status = RebuildImages();
	}

	return l_Status;
}

HRESULT SVIPDoc::UnregisterImageView( SVImageViewClass* p_pImageView )
{
	HRESULT l_Status = S_OK;

	SVMasterImageRegisterMap::iterator l_Iter = m_RegisteredImages.begin();

	while( l_Iter != m_RegisteredImages.end() )
	{
		l_Iter->second.erase( p_pImageView );

		if( l_Iter->second.empty() )
		{
			l_Iter = m_RegisteredImages.erase( l_Iter );
		}
		else
		{
			++l_Iter;
		}
	}

	l_Status = RebuildImages();

	return l_Status;
}

HRESULT SVIPDoc::IsImageDataSent( const SVGUID& p_rImageId, SVImageViewClass* p_pImageView ) const
{
	HRESULT l_Status = S_OK;

	if( m_AllViewsUpdated == 0 )
	{
		SVImageIdImageDataStructMap::const_iterator l_Iter = m_Images.find( p_rImageId );

		if( l_Iter != m_Images.end() )
		{
			SVImageViewPtrImageViewStatusMap::const_iterator l_ViewIter = l_Iter->second.m_ImageViews.find( p_pImageView );

			if( l_ViewIter != l_Iter->second.m_ImageViews.end() )
			{
				if( !( l_ViewIter->second.m_ViewNotified ) )
				{
					l_Status = S_FALSE;
				}
			}
		}
	}

	return l_Status;
}

HRESULT SVIPDoc::IsImageDataUpdated( const SVGUID& p_rImageId, SVImageViewClass* p_pImageView ) const
{
	HRESULT l_Status = S_OK;

	if( m_AllViewsUpdated == 0 )
	{
		SVImageIdImageDataStructMap::const_iterator l_Iter = m_Images.find( p_rImageId );

		if( l_Iter != m_Images.end() )
		{
			SVImageViewPtrImageViewStatusMap::const_iterator l_ViewIter = l_Iter->second.m_ImageViews.find( p_pImageView );

			if( l_ViewIter != l_Iter->second.m_ImageViews.end() )
			{
				if( !( l_ViewIter->second.m_ViewDataUpdated ) )
				{
					l_Status = S_FALSE;
				}
			}
		}
	}

	return l_Status;
}

HRESULT SVIPDoc::IsImageDataDisplayed( const SVGUID& p_rImageId, SVImageViewClass* p_pImageView ) const
{
	HRESULT l_Status = S_OK;

	if( m_AllViewsUpdated == 0 )
	{
		SVImageIdImageDataStructMap::const_iterator l_Iter = m_Images.find( p_rImageId );

		if( l_Iter != m_Images.end() )
		{
			SVImageViewPtrImageViewStatusMap::const_iterator l_ViewIter = l_Iter->second.m_ImageViews.find( p_pImageView );

			if( l_ViewIter != l_Iter->second.m_ImageViews.end() )
			{
				if( !( l_ViewIter->second.m_DisplayComplete ) )
				{
					l_Status = S_FALSE;
				}
			}
		}
	}

	return l_Status;
}

HRESULT SVIPDoc::GetBitmapInfo( const SVGUID& p_rImageId, SVBitmapInfo& p_rBitmapInfo ) const
{
	HRESULT l_Status = S_OK;

	SVImageClass* l_pImage = dynamic_cast< SVImageClass* >( SVObjectManagerClass::Instance().GetObject( p_rImageId.ToGUID() ) );

	if( l_pImage != nullptr )
	{
		SVImageInfoClass l_svImageInfo = l_pImage->GetImageInfo();

		BITMAPINFOHEADER l_Header = l_pImage->GetImageInfo().GetBitmapInfoHeader();

		p_rBitmapInfo.Assign( l_Header.biWidth, l_Header.biHeight, l_Header.biBitCount, SVBitmapInfo::GetDefaultColorTable( l_Header.biBitCount ) );
	}
	else
	{
		p_rBitmapInfo.clear();

		l_Status = E_FAIL;
	}

	return l_Status;
}

HRESULT SVIPDoc::GetImageData( const SVGUID& p_rImageId, SVByteVector& p_rImageData, SVExtentMultiLineStructCArray& p_rMultiLineArray ) const
{
	HRESULT l_Status = S_OK;

	SVImageIdImageDataStructMap::const_iterator l_Iter = m_Images.find( p_rImageId );

	if( l_Iter != m_Images.end() )
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

HRESULT SVIPDoc::SetImageData( const SVGUID& p_rImageId, const SVByteVector& p_rImageData, const SVExtentMultiLineStructCArray& p_rMultiLineArray )
{
	HRESULT l_Status = S_OK;

	SVImageIdImageDataStructMap::iterator l_Iter = m_Images.find( p_rImageId );

	if( l_Iter != m_Images.end() )
	{
		l_Iter->second.m_ImageData = p_rImageData;
		l_Iter->second.m_OverlayData = p_rMultiLineArray;

		SVImageViewPtrImageViewStatusMap::iterator l_ViewIter = l_Iter->second.m_ImageViews.begin();

		while( l_ViewIter != l_Iter->second.m_ImageViews.end() )
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

HRESULT SVIPDoc::MarkImageDataSent( const SVGUID& p_rImageId, SVImageViewClass* p_pImageView )
{
	HRESULT l_Status = S_OK;

	if( m_AllViewsUpdated == 0 )
	{
		SVImageIdImageDataStructMap::iterator l_Iter = m_Images.find( p_rImageId );

		if( l_Iter != m_Images.end() )
		{
			SVImageViewPtrImageViewStatusMap::iterator l_ViewIter = l_Iter->second.m_ImageViews.find( p_pImageView );

			if( l_ViewIter != l_Iter->second.m_ImageViews.end() )
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

HRESULT SVIPDoc::MarkImageDataUpdated( const SVGUID& p_rImageId, SVImageViewClass* p_pImageView )
{
	HRESULT l_Status = S_OK;

	if( m_AllViewsUpdated == 0 )
	{
		SVImageIdImageDataStructMap::iterator l_Iter = m_Images.find( p_rImageId );

		if( l_Iter != m_Images.end() )
		{
			SVImageViewPtrImageViewStatusMap::iterator l_ViewIter = l_Iter->second.m_ImageViews.find( p_pImageView );

			if( l_ViewIter != l_Iter->second.m_ImageViews.end() )
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

HRESULT SVIPDoc::MarkImageDataDisplayed( const SVGUID& p_rImageId, SVImageViewClass* p_pImageView )
{
	HRESULT l_Status = S_OK;

	if( m_AllViewsUpdated == 0 )
	{
		SVImageIdImageDataStructMap::iterator l_Iter = m_Images.find( p_rImageId );

		if( l_Iter != m_Images.end() )
		{
			SVImageViewPtrImageViewStatusMap::iterator l_ViewIter = l_Iter->second.m_ImageViews.find( p_pImageView );

			if( l_ViewIter != l_Iter->second.m_ImageViews.end() )
			{
				if( !( l_ViewIter->second.m_DisplayComplete ) )
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

HRESULT SVIPDoc::UpdateExtents( SVTaskObjectClass* p_pTask, const SVImageExtentClass& p_rExtents )
{
	HRESULT l_Status = SVGuiExtentUpdater::SetImageExtent(p_pTask, p_rExtents);
	if (l_Status == S_OK) { SetModifiedFlag(); }
	return l_Status;
}

HRESULT SVIPDoc::UpdateExtentsToFit( SVTaskObjectClass* p_pTask, const SVImageExtentClass& p_rExtents )
{
	HRESULT l_Status = SVGuiExtentUpdater::SetImageExtentToFit(p_pTask, p_rExtents);
	if (l_Status == S_OK) { SetModifiedFlag(); }
	return l_Status;
}

SVImageClass* SVIPDoc::GetImageByName( CString& p_imageName ) const
{
	SVImageClass* l_pImage = NULL;
	SVInspectionProcess* pInspection = GetInspectionProcess();

	if( nullptr != pInspection )
	{
		if( IsColorInspectionDocument() && pInspection->GetHSIMainImage() != NULL )
		{
			if( p_imageName == pInspection->GetHSIMainImage()->GetCompleteObjectName() )
			{
				l_pImage = pInspection->GetHSIMainImage();
			}
		}

		if( l_pImage == NULL )
		{
			SVObjectManagerClass::Instance().GetObjectByDottedName( static_cast< LPCTSTR >( p_imageName ), l_pImage );
		}
	}

	return l_pImage;
}

HRESULT SVIPDoc::UpdateWithLastProduct()
{
	HRESULT l_Status = S_OK;

	SVInspectionProcess* pInspection = GetInspectionProcess();

	if( nullptr != pInspection )
	{
		l_Status = pInspection->LastProductNotify();
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

BOOL SVIPDoc::RunOnce( SVToolClass* p_pTool )
{
	SVInspectionProcess* pInspection = GetInspectionProcess();
	BOOL l_Status = ( pInspection != nullptr );

	if( l_Status )
	{
		SVGUID l_ToolId;

		if( p_pTool != nullptr ) { l_ToolId = p_pTool->GetUniqueObjectID(); }

		GuiCmd::InspectionRunOncePtr l_CommandPtr = new GuiCmd::InspectionRunOnce( pInspection->GetUniqueObjectID(), l_ToolId );
		SVObjectSynchronousCommandTemplate< GuiCmd::InspectionRunOncePtr > l_Command( pInspection->GetUniqueObjectID(), l_CommandPtr );

		l_Status = ( l_Command.Execute( TWO_MINUTE_CMD_TIMEOUT ) == S_OK );
	}

	return l_Status;
}

int SVIPDoc::GetSelectedToolIndex(const CString& toolName) const
{
	int toolListIndex = 0;
	const SVToolSetClass* pToolSet = GetToolSet();
	if (pToolSet != nullptr)
	{
		toolListIndex = pToolSet->GetSize();
		bool bFound = false;
		// FindTool by Name in SVTaskObjectList
		for (int i = 0; i < pToolSet->GetSize() && !bFound; i++)
		{
			SVTaskObjectClass* pTool = pToolSet->GetAt(i);
			if (pTool != nullptr && pTool->GetName() == toolName)
			{
				toolListIndex = i;
				bFound = true;
			}
		}
	}
	return toolListIndex;
}

int SVIPDoc::GetToolToInsertBefore(const CString& name, int listIndex) const
{
	int toolListIndex = 0;

	const SVToolSetClass* pToolSet = GetToolSet();
	if (pToolSet)
	{
		toolListIndex = pToolSet->GetSize();
		if (!name.IsEmpty())
		{
			bool bFound = false;
			CString toolName = m_toolGroupings.GetToolToInsertBefore(static_cast<LPCTSTR>(name)).c_str();
			if (!toolName.IsEmpty())
			{
				// FindTool by Name in SVTaskObjectList
				for (int i = 0;i < pToolSet->GetSize() && !bFound;i++)
				{
					const SVTaskObjectClass* pTool = pToolSet->GetAt(i);
					if (pTool && pTool->GetName() == toolName)
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



//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVIPDoc.cpp_v  $
 * 
 *    Rev 1.31   09 Jan 2015 01:40:54   gramseier
 * Project:  SVObserver
 * Change Request (SCR) nbr:  978
 * SCR Title:  Copy and Paste a Tool within an Inspection or Between Different Inspections
 * Checked in by:  gRamseier;  Guido Ramseier
 * Change Description:  
 *   Fixed: copy paste commands enabled while editing name
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.30   07 Jan 2015 17:43:58   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  980
 * SCR Title:  Add Non-Inspection Objects to the Result View
 * Checked in by:  mEichengruen;  Marcus Eichengruen
 * Change Description:  
 *   Added methods GetResultList, SaveViewedVariables, and LoadViewedVariables.
 * Changed method OnResultsPicker to show environment variables.
 * Changed method OnSelectPPQVariable to rebuild results list.
 * Changed methods IsResultDefinitionsUpdated, GetResultDefinitions, GetParameters, SetParameters, and GetResults to use new result list format.
 * Removed methods addToolsetObject2ResultDefinitions, addPPQInputs2ResultDefinitions, addPPQ_XParameter2ResultDefinitions, and addEnvironmentObject2ResultDefinitions.
 * Removed useless comment lines.
 * Removed dead code.
 * Changed NULL to nullptr.  
 * Cleaned up spacing.
 * Added @WARNING notes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.29   19 Dec 2014 04:13:06   gramseier
 * Project:  SVObserver
 * Change Request (SCR) nbr:  978
 * SCR Title:  Copy and Paste a Tool within an Inspection or Between Different Inspections
 * Checked in by:  gRamseier;  Guido Ramseier
 * Change Description:  
 *   Added Copy Paste command and UI handlers
 * Changed CheckName to be part of the SVIPDoc class
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.28   16 Dec 2014 17:59:30   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  933
 * SCR Title:  Add Filter Tab to Object Selector (SVO-377)
 * Checked in by:  mZiegler;  Marc Ziegler
 * Change Description:  
 *   Changed the OnResultsPicker and OnPublishedResultsPicker methods to specify the title of the Object Selector's Filter Page.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.27   09 Dec 2014 11:41:48   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  908
 * SCR Title:  Remove option for Operator Move (SVO 101)
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   removed SetSelectedToolForOperatorMove function
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.26   20 Nov 2014 08:11:04   mziegler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  930
 * SCR Title:  Improve Regression Test Dialog (SVO-136)
 * Checked in by:  mZiegler;  Marc Ziegler
 * Change Description:  
 *   in SVRegressionTestRunThread: Because the regression timeout is now really in ms, remove the multiply by 1000.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.25   10 Nov 2014 16:48:02   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  969
 * SCR Title:  SVObserver Should Check the Name of Group Tools When Naming Other Tools
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added CheckName function.
 * Revised AddTool method to call CheckName function.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.24   13 Oct 2014 11:07:56   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  925
 * SCR Title:  Add PPQ Items and SVObserver Modes to Equation Editor Object Selector
 * Checked in by:  mZiegler;  Marc Ziegler
 * Change Description:  
 *   Changed method OnResultsPicker to omit Environment and PPQ nodes.  Keep the commented out code because these will be added in the 7.10 release instead.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.23   30 Sep 2014 15:45:32   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  925
 * SCR Title:  Add PPQ Items and SVObserver Modes to Equation Editor Object Selector
 * Checked in by:  mZiegler;  Marc Ziegler
 * Change Description:  
 *   Changed the method OnResultsPicker to show PPQ items.
 * Changed GetResultDefinitions method to handle PPQ items.  Broke into multiple new methods:
 * addEnvironmentObject2ResultDefinitions,
 * addPPQ_XParameter2ResultDefinitions,
 * addToolsetObject2ResultDefinitions,
 * and addPPQInputs2ResultDefinitions.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.22   27 Aug 2014 01:26:46   gramseier
 * Project:  SVObserver
 * Change Request (SCR) nbr:  909
 * SCR Title:  Object Selector replacing Result Picker and Output Selector SVO-72, 40, 130
 * Checked in by:  gRamseier;  Guido Ramseier
 * Change Description:  
 *   Fixed input variable sort problem
 * Changed methods: OnSelectPPQVariable
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.21   17 Jul 2014 19:21:28   gramseier
 * Project:  SVObserver
 * Change Request (SCR) nbr:  909
 * SCR Title:  Object Selector replacing Result Picker and Output Selector SVO-72, 40, 130
 * Checked in by:  gRamseier;  Guido Ramseier
 * Change Description:  
 *   Replace ResultPicker Dialog with Object Selector Dialog
 * Removed namespaces and code review changes
 * Changed methods: OnResultsPicker, OnPublishedResultsPicker
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.20   26 Jun 2014 17:45:10   mziegler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  885
 * SCR Title:  Replace image display in TA-dialogs with activeX SVPictureDisplay
 * Checked in by:  mZiegler;  Marc Ziegler
 * Change Description:  
 *   change call of the constuctor of SVSaveToolSetImageDialogClass
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.19   24 Jun 2014 09:34:14   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  906
 * SCR Title:  SVObserver Tool Grouping
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised OnEditDelete method to call SVToolSetListCtrl::Rebuild to ensure deleted item has been removed before selecting another tool..
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.18   18 Jun 2014 14:19:28   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  906
 * SCR Title:  SVObserver Tool Grouping
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised AddToolGroup to allow adding end group before selection
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.17   12 Jun 2014 16:49:28   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  906
 * SCR Title:  SVObserver Tool Grouping
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added SVToolGrouping member.
 * Revised AddTool to support Tool Groupings
 * Revised OnEditDelete  to support Tool Groupings
 * Added AddStartToolGrouping method
 * Added AddEndToolGrouping method
 * Added AddToolGrouping method
 * Revised Rename logic to support Tool Groupings
 * Added memu handlers for Add Start and End Tool Groupings
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.16   26 May 2014 10:51:14   mziegler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  894
 * SCR Title:  Enhancements to Adjust Tool Position Dialog
 * Checked in by:  mZiegler;  Marc Ziegler
 * Change Description:  
 *   changed calls of SVAdjustToolSizePositionDlg-constructor 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.15   15 May 2014 13:04:38   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Revised UpdateAllData to use const_iterator.
 * Revised SVRegressionTestRunThread to remove self assignment of pWnd.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.14   22 Apr 2014 13:13:22   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  886
 * SCR Title:  Add RunReject Server Support to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Added validation logic for RemoteMonitorList
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.13   02 Apr 2014 14:05:06   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  868
 * SCR Title:  Fix issue with switching from Run Mode to Regression Mode (eRoom 88)
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Limit the minimum time for Regression test. Was delaying 0 seconds. The new minimum is now 40ms.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.12   07 Mar 2014 18:17:48   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  884
 * SCR Title:  Update Source Code Files to Follow New Programming Standards and Guidelines
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Removed THIS_FILE.
 *   Removed empty methods.
 *   Various code changes to better follow coding guidelines.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.11   28 Feb 2014 09:15:20   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  890
 * SCR Title:  Fix SVObserver Memory Leaks
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   delete regression test dialog in RegressionTestComplete.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.10   04 Feb 2014 15:24:14   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  880
 * SCR Title:  Remove .SEC
 * Checked in by:  mZiegler;  Marc Ziegler
 * Change Description:  
 *   Changed method OnCloseDocument to call getConfigFullFileName and OnFileConfigSave instead of GetSECFullFileName and OnFileSaveSec.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   31 Jan 2014 17:16:28   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  884
 * SCR Title:  Update Source Code Files to Follow New Programming Standards and Guidelines
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Changed to follow guidelines more closely.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   14 Jan 2014 12:24:24   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  877
 * SCR Title:  Add undo-button to formula and conditional pages
 * Checked in by:  mZiegler;  Marc Ziegler
 * Change Description:  
 *   Changed method OnEditTool to avoid use of new and create object on stack.
 * Moved command out of IF statement to separate line for better debugging and to match the coding guidelines.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   04 Oct 2013 12:51:06   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  868
 * SCR Title:  Fix issue with switching from Run Mode to Regression Mode (eRoom 88)
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   fixed the logic of RunRegressionTest to "Stop" the regression if it is already in Run or Test mode
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   03 Oct 2013 13:31:06   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   64 bit platform types.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   01 Oct 2013 14:57:18   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Add x64 platform.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   12 Jun 2013 15:20:04   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 018.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
      Rev 3.250   22 May 2013 09:19:36   tbair
   Project:  SVObserver
   Change Request (SCR) nbr:  834
   SCR Title:  Modify tool set list to maintain position when tools are deleted
   Checked in by:  tBair;  Tom Bair
   Change Description:  
     Fix crash when when deleting the last tool in toolset.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
 *    Rev 1.3   15 May 2013 19:46:32   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 016.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
      Rev 3.249   14 May 2013 13:36:10   tbair
   Project:  SVObserver
   Change Request (SCR) nbr:  834
   SCR Title:  Modify tool set list to maintain position when tools are deleted
   Checked in by:  tBair;  Tom Bair
   Change Description:  
     Added logic to Update to use RefreshView hint.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
 *    Rev 1.2   13 May 2013 16:29:30   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 015.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
      Rev 3.248   08 May 2013 12:27:14   tbair
   Project:  SVObserver
   Change Request (SCR) nbr:  834
   SCR Title:  Modify tool set list to maintain position when tools are deleted
   Checked in by:  tBair;  Tom Bair
   Change Description:  
     Modified OnEditDelete to maintain selected tool and scroll bar positions.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
 *    Rev 1.1   08 May 2013 16:16:40   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 012.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
      Rev 3.247   29 Apr 2013 14:16:00   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  830
   SCR Title:  Consolidate all Time Stamp and Clock Functionality
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated source code to use the new SVClock::SVTimeStamp type for all time stamp variables.  Update to use new SVClock::GetTimeStamp() function to get the time stamp value in milliseconds.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
 *    Rev 1.0   23 Apr 2013 11:16:46   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.246   27 Mar 2013 14:27:48   ryoho
   Project:  SVObserver
   Change Request (SCR) nbr:  815
   SCR Title:  Fix issue with being able to go into Regression Mode with invalid tools
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
     when going into Regression Mode if there are invalid tools it will show a message so the user knows it is unable to enter that mode.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.245   25 Mar 2013 13:36:06   ryoho
   Project:  SVObserver
   Change Request (SCR) nbr:  810
   SCR Title:  Remove support for Legacy Tools
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
     Removed OnAddGageTool and OnAddProfileTool.  Removed the InspectionToolBar
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.244   04 Feb 2013 13:51:56   tbair
   Project:  SVObserver
   Change Request (SCR) nbr:  808
   SCR Title:  Current Toolset Draw setting does not draw tool overlay on image in Run Mode
   Checked in by:  tBair;  Tom Bair
   Change Description:  
     Added temperary variable for SetSelectedToolForOperatorMove so the member variable for selected tool will not change unless the function returns true.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.243   10 Jan 2013 16:16:34   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  801
   SCR Title:  Add new Shift Tool to SVObserver
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
     Added missing log entries.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.242   07 Dec 2012 10:36:14   ryoho
   Project:  SVObserver
   Change Request (SCR) nbr:  801
   SCR Title:  Add new Shift Tool to SVObserver
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
     added new method for OnAddShiftTool
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.241   03 Dec 2012 09:50:10   tbair
   Project:  SVObserver
   Change Request (SCR) nbr:  800
   SCR Title:  Fix Blob Array Result index string
   Checked in by:  tBair;  Tom Bair
   Change Description:  
     Changed arrayIndex in getresultdefinitions.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.240   11 Oct 2012 10:54:22   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  602
   SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated include information based on the navigate tree class moving to the XML library.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.239   28 Sep 2012 14:49:46   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  602
   SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated saving operation to use the object writer instead of the XML tree.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.238   27 Sep 2012 18:34:02   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  602
   SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated type definitions from HTREEITEM to generic branch handle type and fixed initialization problems.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.237   25 Sep 2012 15:19:22   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  602
   SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated to use new tree type.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.236   18 Sep 2012 18:21:38   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  602
   SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated code to use the templated XML objects that moved from the SVLibrary to the new XML Library and to the Configuration Library.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.235   06 Sep 2012 10:01:12   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  602
   SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated Get Parameters method to change the location of some inspection process attributes.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.234   04 Sep 2012 15:27:22   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  602
   SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated Navigate Tree class to not force the programmer to create an instance of the class to use it.  Converted all functionality to static.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.233   30 Aug 2012 09:40:12   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  786
   SCR Title:  Fix Problems with IEEE 1394 Light Reference and LUT Camera Parameters
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated On Adjust Lut method in IP Document to fix issues when exiting the Adjust Lut dialog.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.232.1.1   05 Dec 2012 11:53:54   ryoho
   Project:  SVObserver
   Change Request (SCR) nbr:  801
   SCR Title:  Add new Shift Tool to SVObserver
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
     added new method for OnAddShiftTool
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.232.1.0   30 Nov 2012 14:10:20   tbair
   Project:  SVObserver
   Change Request (SCR) nbr:  800
   SCR Title:  Fix Blob Array Result index string
   Checked in by:  tBair;  Tom Bair
   Change Description:  
     Changed arrayIndex in getresultdefinitions.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.232   09 Aug 2012 16:07:48   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  769
   SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Fixed issues when enabling and disabling menu options depending on the application state.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.231   08 Aug 2012 15:59:48   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  769
   SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated code and resources to fix issues with missing linkages for dialogs associated with resources.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.230   08 Aug 2012 15:39:02   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  761
   SCR Title:  Fix issue with Clip on Adjust LUT Dialog using GigE cameras
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
     Updated code to fix issues with LUT and Light Reference Dialogs when camera has no LUT or Light Reference.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.229   07 Aug 2012 16:02:06   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  769
   SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Fixed issues with updating tool extents when the tool is off the image.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.228   03 Aug 2012 17:30:14   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  769
   SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Update source code to remove the SV_TEST_STATE state when going into regression.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.227   01 Aug 2012 12:22:04   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  769
   SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Added new command functionality to allow for synchronous command execution through the inspection thread.  This will fix problems with race conditions with the inspection process.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.226   01 Aug 2012 11:11:50   ryoho
   Project:  SVObserver
   Change Request (SCR) nbr:  778
   SCR Title:  Fix the linkage between SVObserver and the Help System
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
     Removed the OnHelp method.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.225   25 Jul 2012 14:10:34   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  769
   SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Removed dead code and updated overlay functionality.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.224   18 Jul 2012 13:35:24   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  769
   SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Removed obsolete methods assiciated with overlay drawling and fixed problem with copying data to byte buffer have wrong format.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.223   09 Jul 2012 14:03:50   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  769
   SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated source code to change the include for SVIM State.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.222   02 Jul 2012 16:58:46   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  769
   SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated source code to promote new display functionality.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.221   19 Jun 2012 12:54:32   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  744
   SCR Title:  Add Shared Memory and Socket Functionality for Run Page Web Server
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Optimized the application state functionality.  The application state class was altered to simplify its usage and elements were added to object manager to separate tracking states.
   Centralized the location of the configuration object identifier and removed synchronization problems.
   
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.220   15 Mar 2012 08:45:00   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  757
   SCR Title:  Fix results view to sort PPQ object selected for viewing
   Checked in by:  tBair;  Tom Bair
   Change Description:  
     Removed SortPPQInputs call from OnSelectPPQVariable method.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.219   13 Mar 2012 13:38:50   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  745
   SCR Title:  Fix contention issue during load process and display
   Checked in by:  tBair;  Tom Bair
   Change Description:  
     Removed dead code, added child elements as friends, and
   protected class only methods.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.218   12 Mar 2012 10:00:44   tbair
   Project:  SVObserver
   Change Request (SCR) nbr:  757
   SCR Title:  Fix results view to sort PPQ object selected for viewing
   Checked in by:  tBair;  Tom Bair
   Change Description:  
     Added call to Inspection::SortPPQInputs from SelectPPQVariable function.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.217   01 Mar 2012 15:25:16   ryoho
   Project:  SVObserver
   Change Request (SCR) nbr:  754
   SCR Title:  Fix regression test mode flag when exiting regression mode
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
     fixed issue with Regression Test Mode flag
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.216   16 Jan 2012 15:22:32   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  747
   SCR Title:  Add New Remote Input Tool to SVObserver
   Checked in by:  jSpila;  Joseph Spila
   Change Description:  
     Updated inspection process and ip doc to allow the tool to queue a command for the inspection thread to process.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.215   07 Oct 2011 14:22:12   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  730
   SCR Title:  Adjust SVObserver to fix issues with Inspection resource handshaking
   Checked in by:  tBair;  Tom Bair
   Change Description:  
     Updated MDI Child Window and IP Document to fix threading problems by moving the data through a lockless queue and notifying the IP Document via the MDI Child Window with a windows message.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.214   06 Oct 2011 09:05:12   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  730
   SCR Title:  Adjust SVObserver to fix issues with Inspection resource handshaking
   Checked in by:  tBair;  Tom Bair
   Change Description:  
     Updated IPDoc to add locking to protect a member variable that was accessed by the display thread and the application thread.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.213   04 Oct 2011 18:53:48   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  730
   SCR Title:  Adjust SVObserver to fix issues with Inspection resource handshaking
   Checked in by:  tBair;  Tom Bair
   Change Description:  
     Updated handshaking between IPDoc and ImageView to fix freezing of the ImageView.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.212   03 Oct 2011 10:04:48   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  730
   SCR Title:  Adjust SVObserver to fix issues with Inspection resource handshaking
   Checked in by:  tBair;  Tom Bair
   Change Description:  
     Updated the handshaking between the display object thread and the image view draw functionality.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.211   28 Sep 2011 12:56:24   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  730
   SCR Title:  Adjust SVObserver to fix issues with Inspection resource handshaking
   Checked in by:  tBair;  Tom Bair
   Change Description:  
     Updated source code to fix issues with camera image interface and methodology.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.210   21 Sep 2011 09:51:48   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  730
   SCR Title:  Adjust SVObserver to fix issues with Inspection resource handshaking
   Checked in by:  tBair;  Tom Bair
   Change Description:  
     Upated functionality to allow the display to be updated with the last inspected data when the image pane gets changed.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.209   16 Sep 2011 16:00:16   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  730
   SCR Title:  Adjust SVObserver to fix issues with Inspection resource handshaking
   Checked in by:  tBair;  Tom Bair
   Change Description:  
     Updated objects to use new resource management functionality.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.208   09 Jun 2011 15:14:48   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  707
   SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Changed the regression can go online functionality to not run the initialize run method when entering regression mode.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.207   17 May 2011 09:36:00   ryoho
   Project:  SVObserver
   Change Request (SCR) nbr:  713
   SCR Title:  Fix Help button on Configuration Assistant Dialog
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
     fixed F1 on IPDoc's to go to correct topic
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.206   29 Apr 2011 08:06:54   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  707
   SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated code to allow for inspection to for the display to give up when an inspection get queued.  The display is now regulated to a maximum of 10 frames per second.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.205   27 Apr 2011 11:11:08   ryoho
   Project:  SVObserver
   Change Request (SCR) nbr:  720
   SCR Title:  Change menu options for Test and Regession Modes
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
     updated Menu Options when in Test or Regression Mode
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.204   19 Apr 2011 16:17:02   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  707
   SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated code to fix issues with IO Assignment.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.203   18 Apr 2011 10:24:12   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  707
   SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated code to move the shared data for the IO Entry data to a different class to fix issues with PPQ and Inspection shared data.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.202   13 Apr 2011 15:19:08   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  712
   SCR Title:  Fix issues with black images when using command interface (SIAC)
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated system to fix color processing issues with color configurations.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.201   30 Mar 2011 08:04:28   ryoho
   Project:  SVObserver
   Change Request (SCR) nbr:  707
   SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     fixed issues with Adjust Tool position
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.200   22 Mar 2011 07:54:32   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  707
   SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated static global classes to singletons to promote proper and defined construction and destruction.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.199   14 Feb 2011 11:08:08   ryoho
   Project:  SVObserver
   Change Request (SCR) nbr:  707
   SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     backed out changes for fixing Edit Tool Position
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.198   14 Feb 2011 10:55:20   ryoho
   Project:  SVObserver
   Change Request (SCR) nbr:  707
   SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     fixed issue with Edit Tool Position
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.197   27 Jan 2011 11:20:44   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  712
   SCR Title:  Fix issues with black images when using command interface (SIAC)
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated source code to fix issues with Matrox Buffer Management.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.196   13 Dec 2010 11:23:12   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  707
   SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Change the usage of _variant_t to clear the variable after use to eliminate invalid variant type for assignment.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.195   08 Dec 2010 12:52:02   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  707
   SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated source code to include changes in notification functionality using the Observer Design Pattern.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.194   09 Nov 2010 16:21:44   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  704
   SCR Title:  Upgrade SVObserver Version for 5.33 Release
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated source code to remove duplicate container objects.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.193   05 Nov 2010 14:54:36   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  707
   SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated source code to remove the un-necessary IP View Control from the application.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.192   05 Nov 2010 11:02:20   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  703
   SCR Title:  Allow SVObserver to Change Remote Input Values
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated source code to remove redundent data objects.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.191   04 Nov 2010 13:39:46   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  707
   SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated source code to fix issues with image processing and display image processing classes and associated includes.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.190   28 Oct 2010 13:54:22   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  704
   SCR Title:  Upgrade SVObserver Version for 5.33 Release
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated source code to remove duplicate definition of HRESULT.  The SVHRESULT definition will be removed in place of the Microsoft defined HRESULT.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.189   23 Jul 2010 09:19:14   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  693
   SCR Title:  Fix Performance Issue with Inspection Process
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated status code for missing Main Image Index from error to informational.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.188   29 Jun 2010 14:02:26   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  693
   SCR Title:  Fix Performance Issue with Inspection Process
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated source to change object validity test and casting changes.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.187   01 Jun 2010 14:24:16   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  693
   SCR Title:  Fix Performance Issue with Inspection Process
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated source code to improve ability to track performance.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.186   01 Jun 2010 10:43:58   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  693
   SCR Title:  Fix Performance Issue with Inspection Process
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated code to remove redundent methodologies and fix missing or incorrect calling functionality.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.185   05 Mar 2010 11:01:16   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  686
   SCR Title:  Update Laptop version to load all valid configurations
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Added new element to run status for critical errors.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.184   03 Mar 2010 12:03:40   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  686
   SCR Title:  Update Laptop version to load all valid configurations
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated code to remove duplicate containers.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.183   16 Dec 2009 11:07:38   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  677
   SCR Title:  Fix problem in camera notify thread
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Fix issues with includes and comments.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.182   07 Oct 2009 16:32:22   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  665
   SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated code to include branch changes into mainline code.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.181   21 Sep 2009 13:26:50   sjones
   Project:  SVObserver
   Change Request (SCR) nbr:  662
   SCR Title:  Add Display Change Support to SVImageView
   Checked in by:  sJones;  Steve Jones
   Change Description:  
     Added RecreateSurfaces method.
   Moved DisplayChangedEvent to SVMainFrm.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.180   03 Sep 2009 10:08:02   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  665
   SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated files to fix moved file includes.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.179   30 Jul 2009 11:56:28   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  665
   SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Merged branched changes into current code branch with appropriate updates.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.178   09 Apr 2009 14:58:36   ryoho
   Project:  SVObserver
   Change Request (SCR) nbr:  657
   SCR Title:  Remove Apply button from the Tool Adjust Dialogs
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
     removed the apply button
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.177   27 Jan 2009 13:50:10   bwalter
   Project:  SVObserver
   Change Request (SCR) nbr:  617
   SCR Title:  Use DirectX instead of Matrox to display images
   Checked in by:  bWalter;  Ben Walter
   Change Description:  
     Moved calls to SVDirectX to SVObserver.cpp
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.176   31 Dec 2008 11:38:28   sjones
   Project:  SVObserver
   Change Request (SCR) nbr:  644
   SCR Title:  Pattern Analyzer Enhancements and Bug Fixes
   Checked in by:  sJones;  Steve Jones
   Change Description:  
     Revised SetParameters to change Angle delta Positive to Angle Delta Positive
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.175   11 Dec 2008 10:31:26   jhanebach
   Project:  SVObserver
   Change Request (SCR) nbr:  629
   SCR Title:  Add Histogram analyzer
   Checked in by:  jHanebach;  Jack Hanebach
   Change Description:  
     
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.173   04 Dec 2008 13:40:16   bwalter
   Project:  SVObserver
   Change Request (SCR) nbr:  617
   SCR Title:  Use DirectX instead of Matrox to display images
   Checked in by:  bWalter;  Ben Walter
   Change Description:  
     Changed to use SVDirectX instead of SVObserverApp for DirectDraw variables.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.172   14 Nov 2008 12:15:46   jbrown
   Project:  SVObserver
   Change Request (SCR) nbr:  641
   SCR Title:  BoundsChecker results
   Checked in by:  JimAdmin;  James A. Brown
   Change Description:  
     For SVIPDoc::SetParameters ()
   replace delete of character string from SVOVariantClass, with 
   SVOString::StaticDestroy ()
   
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.171   04 Nov 2008 12:44:10   bwalter
   Project:  SVObserver
   Change Request (SCR) nbr:  639
   SCR Title:  Update SVObserver to better handle going offline while triggering
   Checked in by:  bWalter;  Ben Walter
   Change Description:  
     Changed Run to check run status state before running toolset and log an event if the state is invalid.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.170   16 Sep 2008 13:50:26   sjones
   Project:  SVObserver
   Change Request (SCR) nbr:  634
   SCR Title:  Implement a File Acquistion Device
   Checked in by:  sJones;  Steve Jones
   Change Description:  
     Revised to check if LightReference is available before showing LightReference Dialog
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.169   12 Sep 2008 11:56:40   bwalter
   Project:  SVObserver
   Change Request (SCR) nbr:  617
   SCR Title:  Use DirectX instead of Matrox to display images
   Checked in by:  bWalter;  Ben Walter
   Change Description:  
     Added event to track changes to display settings.
   Moved management of DirectDraw surface and clipper here from SVImageView.
   Created InitializeDirectX function.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.168   11 Jun 2008 13:57:40   tbair
   Project:  SVObserver
   Change Request (SCR) nbr:  616
   SCR Title:  Add Server-side Configuration Data Def Lists to the SVObserver program
   Checked in by:  tBair;  Tom Bair
   Change Description:  
     Added UpdateCmdUI to menu option.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.167   11 Jun 2008 13:52:36   tbair
   Project:  SVObserver
   Change Request (SCR) nbr:  598
   SCR Title:  Upgrade SVObserver to compile using vc++ in VS2005
   Checked in by:  jSpila;  Joseph Spila
   Change Description:  
     Modified regresion test back function to not use itterator.begin() - 1
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.166   30 May 2008 13:27:34   BWalter
   Project:  SVObserver
   Change Request (SCR) nbr:  617
   SCR Title:  Use DirectX instead of Matrox to display images
   Checked in by:  bWalter;  Ben Walter
   Change Description:  
     Removed all DirectX changes since 5.00 Beta 1.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.165   15 May 2008 15:09:28   BWalter
   Project:  SVObserver
   Change Request (SCR) nbr:  617
   SCR Title:  Use DirectX instead of Matrox to display images
   Checked in by:  bWalter;  Ben Walter
   Change Description:  
     Made call to InitializeDirectX conditional on m_useDirectX
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.164   30 Apr 2008 11:09:58   BWalter
   Project:  SVObserver
   Change Request (SCR) nbr:  617
   SCR Title:  Use DirectX instead of Matrox to display images
   Checked in by:  bWalter;  Ben Walter
   Change Description:  
     Removed code that sets the HWND for the DirectDraw clipper.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.163   10 Apr 2008 07:50:42   BWalter
   Project:  SVObserver
   Change Request (SCR) nbr:  617
   SCR Title:  Use DirectX instead of Matrox to display images
   Checked in by:  bWalter;  Ben Walter
   Change Description:  
     * Added event to track changes to display settings.
   * Moved management of DirectDraw surface and clipper here from SVImageView.
   * Created InitializeDirectX function.
   
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.162   09 Apr 2008 08:39:44   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  624
   SCR Title:  Fix problems with Color Tool when it gets deleted
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Replaced Set Parent operator with Set Parent Image operator when accessing the parent information of the Image Class.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.161   07 Mar 2008 11:57:24   tbair
   Project:  SVObserver
   Change Request (SCR) nbr:  616
   SCR Title:  Add Server-side Configuration Data Def Lists to the SVObserver program
   Checked in by:  tBair;  Tom Bair
   Change Description:  
     Added Property sheet for Transfer Definition Lists
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.160   18 Dec 2007 12:26:10   tbair
   Project:  SVObserver
   Change Request (SCR) nbr:  614
   SCR Title:  Correct Spelling in Blob Analyzer Results
   Checked in by:  tBair;  Tom Bair
   Change Description:  
     Added code in setParameters to replace the misspelled words for the Blob result before parsing takes place.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.159   24 Jul 2007 13:31:30   tbair
   Project:  SVObserver
   Change Request (SCR) nbr:  597
   SCR Title:  Upgrade Matrox Imaging Library to version 8.0
   Checked in by:  tBair;  Tom Bair
   Change Description:  
     Update to MIL 8.0
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.158   21 Jun 2007 13:05:14   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  598
   SCR Title:  Upgrade SVObserver to compile using vc++ in VS2005
   Checked in by:  jSpila;  Joseph Spila
   Change Description:  
     These changes include modification based on fixing compiler-based and project-based differences between VC6 and VC8.  These changes mainly include casting issues, but some include type conversion and assignment of new compiler controlling defines.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.157.3.1   05 Oct 2009 16:19:10   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  665
   SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Fixed issue with Edit Tool Position when extents change.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.157.3.0   14 Jul 2009 14:31:14   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  665
   SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated code with new data manager objects and image methods.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.157.2.0   07 Apr 2008 08:23:36   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  621
   SCR Title:  Fix Bug in the SVImageClass
   Checked in by:  tBair;  Tom Bair
   Change Description:  
     Updated methods to use the new GetParentImage method instead of the GetParent method to remove confusion with CWnd GetParent method.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.157   25 Jan 2006 15:11:36   ebeyeler
   Project:  SVObserver
   Change Request (SCR) nbr:  529
   SCR Title:  Add Conditional Product History
   Checked in by:  eBeyeler;  Eric Beyeler
   Change Description:  
     Modify Conditional History Sheet title to include inspection name
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.156   25 Jan 2006 12:37:58   ebeyeler
   Project:  SVObserver
   Change Request (SCR) nbr:  529
   SCR Title:  Add Conditional Product History
   Checked in by:  eBeyeler;  Eric Beyeler
   Change Description:  
     call SVInspectionProcess BuildConditionalHistoryListAfterLoad
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.155   24 Jan 2006 14:17:36   ebeyeler
   Project:  SVObserver
   Change Request (SCR) nbr:  529
   SCR Title:  Add Conditional Product History
   Checked in by:  eBeyeler;  Eric Beyeler
   Change Description:  
     Fixed Conditional History bugs
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.154   19 Jan 2006 10:32:18   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  534
   SCR Title:  Add Image Select for Auxiliary Extent Source
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated inspection process to not process Aux Extents if a gage tool is present in the inspection.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.153   17 Jan 2006 14:55:32   ebeyeler
   Project:  SVObserver
   Change Request (SCR) nbr:  529
   SCR Title:  Add Conditional Product History
   Checked in by:  eBeyeler;  Eric Beyeler
   Change Description:  
     save Conditional History properties in the config
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.152   17 Jan 2006 09:59:14   ebeyeler
   Project:  SVObserver
   Change Request (SCR) nbr:  529
   SCR Title:  Add Conditional Product History
   Checked in by:  eBeyeler;  Eric Beyeler
   Change Description:  
     added Conditional History menu handler
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.151   16 Jan 2006 17:07:36   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  534
   SCR Title:  Add Image Select for Auxiliary Extent Source
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Moved removed embedded object to create object method for load of configuration to happen properly.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.150   06 Dec 2005 15:28:16   ryoho
   Project:  SVObserver
   Change Request (SCR) nbr:  527
   SCR Title:  Fix problems with Regression Test
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
     changed code to bring the IPDoc to the front when the mode changes out of regression
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.149   08 Nov 2005 15:56:02   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  516
   SCR Title:  Allow SVObserver to use the Intek 1394 driver for camera acquisition
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated 1394 code to allow for the use of the Intek driver.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.148   02 Nov 2005 09:07:48   ebeyeler
   Project:  SVObserver
   Change Request (SCR) nbr:  512
   SCR Title:  Add Published Result Images functionality
   Checked in by:  eBeyeler;  Eric Beyeler
   Change Description:  
     implemented published result images
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.147   20 Oct 2005 15:47:00   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  462
   SCR Title:  Redo the fuctionality of the Regression Test
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
     Updated RegressionChangeMode method to send the go-offline command to the currently running IPDoc.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.146   19 Oct 2005 12:11:16   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  504
   SCR Title:  Fix lock-up problem when pending product count is larger than PPQ size
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated code to include changes that do not require locking and reduce the workload for setting up the main images.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.145   06 Oct 2005 14:46:06   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  500
   SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Added new method to set main image indexes by product.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.144   05 Oct 2005 11:10:54   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  492
   SCR Title:  Upgrade SVObserver to version 4.60
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
     Removed compilier warning.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.143   01 Sep 2005 15:06:58   ryoho
   Project:  SVObserver
   Change Request (SCR) nbr:  462
   SCR Title:  Redo the fuctionality of the Regression Test
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
     moved the CanGoOnline before the creation of the RegressionTest Dialogs.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.142   01 Sep 2005 15:00:24   ryoho
   Project:  SVObserver
   Change Request (SCR) nbr:  462
   SCR Title:  Redo the fuctionality of the Regression Test
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
     fixed RegressionTest to allow the ArchiveTool run during this mode
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.141   01 Sep 2005 12:15:00   tbair
   Project:  SVObserver
   Change Request (SCR) nbr:  510
   SCR Title:  Add source image extents to all image using tools
   Checked in by:  tBair;  Tom Bair
   Change Description:  
     Modified RebuildResultList
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.140   31 Aug 2005 08:40:46   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  500
   SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Update source code call to handle new refresh methodology.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.139   30 Aug 2005 11:20:14   ryoho
   Project:  SVObserver
   Change Request (SCR) nbr:  462
   SCR Title:  Redo the fuctionality of the Regression Test
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
     added new method for clearing the regression structures.  instead of clearing when regression test is stopped, it will clear when the ipdoc goes away
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.138   29 Aug 2005 16:52:38   tbair
   Project:  SVObserver
   Change Request (SCR) nbr:  455
   SCR Title:  New Security for SVObserver
   Checked in by:  tBair;  Tom Bair
   Change Description:  
     Added state check for mode change to prevent validation if already in the requested mode.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.137   26 Aug 2005 15:01:28   tbair
   Project:  SVObserver
   Change Request (SCR) nbr:  500
   SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Added m_bLoadFiles flag to reset structure so ResetObject in Pattern analyzer will reload files when going online.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.136   24 Aug 2005 11:07:34   tbair
   Project:  SVObserver
   Change Request (SCR) nbr:  455
   SCR Title:  New Security for SVObserver
   Checked in by:  tBair;  Tom Bair
   Change Description:  
     Disabled Code in copy and paste because it is not functional.  Remove the unpredictable!
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.135   24 Aug 2005 09:41:10   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  504
   SCR Title:  Fix lock-up problem when pending product count is larger than PPQ size
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Removed the use of SVIPDoc::SingleModeRunLoop and replaced it with SVInspectionProcess::RunOnce.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.134   23 Aug 2005 10:11:54   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  504
   SCR Title:  Fix lock-up problem when pending product count is larger than PPQ size
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated methods to handle copy forward correctly.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.133   22 Aug 2005 13:44:56   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  504
   SCR Title:  Fix lock-up problem when pending product count is larger than PPQ size
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated method to fix result image locking problem.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.132   22 Aug 2005 13:31:40   tbair
   Project:  SVObserver
   Change Request (SCR) nbr:  455
   SCR Title:  New Security for SVObserver
   Checked in by:  tBair;  Tom Bair
   Change Description:  
     Added Update_Command_UI handlers for view reset counts options.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.131   22 Aug 2005 11:14:44   tbair
   Project:  SVObserver
   Change Request (SCR) nbr:  455
   SCR Title:  New Security for SVObserver
   Checked in by:  tBair;  Tom Bair
   Change Description:  
     Implement Dual Access Point Validate function.
   Change name of SVIsValid to SVIsDisplayable
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.130   22 Aug 2005 10:18:20   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  504
   SCR Title:  Fix lock-up problem when pending product count is larger than PPQ size
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated methods to include changes to SVProductInfoStruct and supporting classes to handle proper assignment and copy construction.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.129   17 Aug 2005 12:07:42   ryoho
   Project:  SVObserver
   Change Request (SCR) nbr:  462
   SCR Title:  Redo the fuctionality of the Regression Test
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
     Fixed problem with Regression Test.  Going between Single File and List of files for Multi Cameras
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.128   17 Aug 2005 11:49:28   tbair
   Project:  SVObserver
   Change Request (SCR) nbr:  455
   SCR Title:  New Security for SVObserver
   Checked in by:  tBair;  Tom Bair
   Change Description:  
     Added Accesspoint for exiting run
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.127   16 Aug 2005 15:15:32   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  506
   SCR Title:  Update Tool Set List Tab to include an indicator when a tool is invalid
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Added functionality to update tool set list status icons.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.126   15 Aug 2005 14:17:18   ryoho
   Project:  SVObserver
   Change Request (SCR) nbr:  462
   SCR Title:  Redo the fuctionality of the Regression Test
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
     fixed problem with regression test using a single file.  When play and in run to end mode the file will only come up once.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.125   15 Aug 2005 13:40:06   ryoho
   Project:  SVObserver
   Change Request (SCR) nbr:  462
   SCR Title:  Redo the fuctionality of the Regression Test
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
     removed unused code.  added member variable that will be set when the regression test is shutting down.  this fixes the problem of switching modes while the regression test is running without delay
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.124   10 Aug 2005 12:33:20   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  500
   SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated methods to improve performance and reduce system delay.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.123   10 Aug 2005 10:33:20   ryoho
   Project:  SVObserver
   Change Request (SCR) nbr:  462
   SCR Title:  Redo the fuctionality of the Regression Test
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
     fixed problems with regression test.  settings button will reselect files and will reset the files back to beginning of the list
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.122   09 Aug 2005 11:46:12   tbair
   Project:  SVObserver
   Change Request (SCR) nbr:  455
   SCR Title:  New Security for SVObserver
   Checked in by:  tBair;  Tom Bair
   Change Description:  
     Fixed bug in CanCloseFrame with SVIsAllowed.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.121   08 Aug 2005 13:58:12   ebeyeler
   Project:  SVObserver
   Change Request (SCR) nbr:  450
   SCR Title:  Add asynchronous functionality to the archive tool
   Checked in by:  eBeyeler;  Eric Beyeler
   Change Description:  
     SVToolArchive renamed to SVArchiveTool
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.120   03 Aug 2005 13:10:30   tbair
   Project:  SVObserver
   Change Request (SCR) nbr:  486
   SCR Title:  Add Trigger Count to Tool Set
   Checked in by:  tBair;  Tom Bair
   Change Description:  
     Fixed bug in reset counts logic and cleaned up code.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.119   02 Aug 2005 13:52:50   tbair
   Project:  SVObserver
   Change Request (SCR) nbr:  455
   SCR Title:  New Security for SVObserver
   Checked in by:  tBair;  Tom Bair
   Change Description:  
     Added Functions OkToEdit and OkToEditMove to make menus more consistant.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.118   02 Aug 2005 09:46:16   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  500
   SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Consolidated reset counts code to SVToolSetClass.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.117   29 Jul 2005 12:35:24   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  500
   SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Removed remove_if functionality when deleting owner elements from the value object and image object lists.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.116   28 Jul 2005 16:11:02   tbair
   Project:  SVObserver
   Change Request (SCR) nbr:  455
   SCR Title:  New Security for SVObserver
   Checked in by:  tBair;  Tom Bair
   Change Description:  
     Updated GUI Editing Flag to block mode changes from SIAC
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.115   27 Jul 2005 09:09:50   tbair
   Project:  SVObserver
   Change Request (SCR) nbr:  501
   SCR Title:  Expose Cylindrical Warp and Perspective Warp Tools
   Checked in by:  tBair;  Tom Bair
   Change Description:  
     Corrected ID for onAddPerspectiveTool
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.114   26 Jul 2005 14:04:36   tbair
   Project:  SVObserver
   Change Request (SCR) nbr:  485
   SCR Title:  Add a way to Reset Counts remotely
   Checked in by:  tBair;  Tom Bair
   Change Description:  
     Added Reset Flag struct to replace flag in IPDoc.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.113   26 Jul 2005 13:37:14   ebeyeler
   Project:  SVObserver
   Change Request (SCR) nbr:  450
   SCR Title:  Add asynchronous functionality to the archive tool
   Checked in by:  eBeyeler;  Eric Beyeler
   Change Description:  
     moved m_bInitializeOnReset to SVResetStruct
   added m_bArchiveToolCreateFiles to SVResetStruct
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.112   22 Jul 2005 13:56:34   ryoho
   Project:  SVObserver
   Change Request (SCR) nbr:  462
   SCR Title:  Redo the fuctionality of the Regression Test
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
     fixed code to deselect tools when going into different modes.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.111   21 Jul 2005 14:41:42   ryoho
   Project:  SVObserver
   Change Request (SCR) nbr:  462
   SCR Title:  Redo the fuctionality of the Regression Test
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
     fixed problem with regression test.  moved the run dialog to SVMainFrame, instead of having it owned by IPDoc. 
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.110   18 Jul 2005 17:02:44   tbair
   Project:  SVObserver
   Change Request (SCR) nbr:  455
   SCR Title:  New Security for SVObserver
   Checked in by:  tBair;  Tom Bair
   Change Description:  
     GUI Mode Menu Functions Bugs.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.109   15 Jul 2005 13:20:40   ryoho
   Project:  SVObserver
   Change Request (SCR) nbr:  462
   SCR Title:  Redo the fuctionality of the Regression Test
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
     in RunRegression, if SVIMState is Running, it will call OnStop before going into regression.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.108   15 Jul 2005 11:31:26   ryoho
   Project:  SVObserver
   Change Request (SCR) nbr:  462
   SCR Title:  Redo the fuctionality of the Regression Test
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
     new regression test methods added
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.107   07 Jul 2005 13:34:02   tbair
   Project:  SVObserver
   Change Request (SCR) nbr:  455
   SCR Title:  New Security for SVObserver
   Checked in by:  tBair;  Tom Bair
   Change Description:  
     Menu changes for submenu disable.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.106   27 Jun 2005 14:54:18   ebeyeler
   Project:  SVObserver
   Change Request (SCR) nbr:  464
   SCR Title:  Add array indexing for value objects
   Checked in by:  eBeyeler;  Eric Beyeler
   Change Description:  
     use SVValueObjectReference
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.105   27 Jun 2005 09:42:06   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  497
   SCR Title:  Remove Freeze Set from SVObserver
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Removed all freeze set variables and methods from the inspection document.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.104   22 Jun 2005 13:30:58   ebeyeler
   Project:  SVObserver
   Change Request (SCR) nbr:  464
   SCR Title:  Add array indexing for value objects
   Checked in by:  eBeyeler;  Eric Beyeler
   Change Description:  
     object attributes use accessor functions
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.103   21 Jun 2005 13:07:36   tbair
   Project:  SVObserver
   Change Request (SCR) nbr:  455
   SCR Title:  New Security for SVObserver
   Checked in by:  tBair;  Tom Bair
   Change Description:  
     Initial changes to add SVSecurity
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.102   19 May 2005 07:28:44   tbair
   Project:  SVObserver
   Change Request (SCR) nbr:  486
   SCR Title:  Add Trigger Count to Tool Set
   Checked in by:  tBair;  Tom Bair
   Change Description:  
     Add trigger count value object to tool set.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.101   18 May 2005 14:59:06   tbair
   Project:  SVObserver
   Change Request (SCR) nbr:  485
   SCR Title:  Add a way to Reset Counts remotely
   Checked in by:  tBair;  Tom Bair
   Change Description:  
     Added Reset Counts message handlers to view menu.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.100   16 Mar 2005 10:59:44   ebeyeler
   Project:  SVObserver
   Change Request (SCR) nbr:  456
   SCR Title:  Update Image and Tool Objects to use the new Extent Classes
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     fixed color slowness problem
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.99   14 Mar 2005 12:03:40   ryoho
   Project:  SVObserver
   Change Request (SCR) nbr:  463
   SCR Title:  Add new Linear Measurement Tool to SVObserver
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
     In GetImageView, if the tool is Linear or a Profile, SVExtentPropertyPositionPoint is not valid, so must used the BottomRight
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.98   09 Mar 2005 13:13:34   tbair
   Project:  SVObserver
   Change Request (SCR) nbr:  478
   SCR Title:  Add a new Perspective Warp Tool
   Checked in by:  tBair;  Tom Bair
   Change Description:  
     Allow the PerspectiveWarp Tool to be added from the add menu.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.97   09 Mar 2005 12:23:36   ebeyeler
   Project:  SVObserver
   Change Request (SCR) nbr:  477
   SCR Title:  Add Cylindrical warp Tool
   Checked in by:  eBeyeler;  Eric Beyeler
   Change Description:  
     added CylindricalWarp functionality
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.96   09 Mar 2005 12:08:34   ryoho
   Project:  SVObserver
   Change Request (SCR) nbr:  463
   SCR Title:  Add new Linear Measurement Tool to SVObserver
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
     Added message for adding a Gage or Profile tool to let the user know that the tools are obsolete.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.95   07 Mar 2005 12:03:38   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  262
   SCR Title:  Improve performance when loading a configuration in SVObserver
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Moved CloseObject to after Disconnect to remove undefined objects from dependents list.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.94   24 Feb 2005 10:23:36   tbair
   Project:  SVObserver
   Change Request (SCR) nbr:  449
   SCR Title:  Modify Polar Un-Wrap Tool
   Checked in by:  tBair;  Tom Bair
   Change Description:  
     Made changes to use a Angular Method combo instead of a use new method check box.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.93   22 Feb 2005 10:53:06   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  456
   SCR Title:  Update Image and Tool Objects to use the new Extent Classes
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Fixed image copy forward issue.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.92   18 Feb 2005 12:13:26   ebeyeler
   Project:  SVObserver
   Change Request (SCR) nbr:  456
   SCR Title:  Update Image and Tool Objects to use the new Extent Classes
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     changed ConnectToolsetMainImage to take a set of valid cameras.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.91   18 Feb 2005 11:34:14   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  262
   SCR Title:  Improve performance when loading a configuration in SVObserver
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated the methodology to rebuild the value object list.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.90   18 Feb 2005 11:02:48   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  463
   SCR Title:  Add new Linear Measurement Tool to SVObserver
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
     Updated IPDoc to allow the new Linear Tool to be added.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.89   18 Feb 2005 10:00:42   tbair
   Project:  SVObserver
   Change Request (SCR) nbr:  449
   SCR Title:  Modify Polar Un-Wrap Tool
   Checked in by:  tBair;  Tom Bair
   Change Description:  
     Made Change to force the Polar Unwrap tool to use new method when adding a new Polar Unwrap tool.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.88   17 Feb 2005 14:35:36   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  456
   SCR Title:  Update Image and Tool Objects to use the new Extent Classes
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Update class with new methodologies based on new base object, extents and reset structure.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.87   16 Feb 2005 14:38:08   ryoho
   Project:  SVObserver
   Change Request (SCR) nbr:  440
   SCR Title:  Create Internal Tool Object to Managing Tool Extents and Result Extents
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
     updated header files
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.86   12 Oct 2004 12:55:50   ebeyeler
   Project:  SVObserver
   Change Request (SCR) nbr:  448
   SCR Title:  Fix External Tool Change DLL functionality
   Checked in by:  eBeyeler;  Eric Beyeler
   Change Description:  
     moved some functionality of OnResultsPicker into a new function, RebuildResultsView so it could be called by other classes
   added m_ prefix to member variables
   added function DeleteTool`
   
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.85   12 Jul 2004 12:43:04   ebeyeler
   Project:  SVObserver
   Change Request (SCR) nbr:  428
   SCR Title:  Improve Load Configuration time
   Checked in by:  eBeyeler;  Eric Beyeler
   Change Description:  
     implemented change for the new parser
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.84   12 Jul 2004 10:30:14   ebeyeler
   Project:  SVObserver
   Change Request (SCR) nbr:  406
   SCR Title:  Implement External Tool
   Checked in by:  eBeyeler;  Eric Beyeler
   Change Description:  
     added OnAddExternalTool
   changed BOOL function calls to HRESULT
   
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.83   19 Nov 2003 09:46:24   ebeyeler
   Project:  SVObserver
   Change Request (SCR) nbr:  320
   SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
   Checked in by:  eBeyeler;  Eric Beyeler
   Change Description:  
     fixed crash on color system
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.82   12 Nov 2003 13:43:24   ebeyeler
   Project:  SVObserver
   Change Request (SCR) nbr:  396
   SCR Title:  Add New Product Types for 1394 SVIM
   Checked in by:  eBeyeler;  Eric Beyeler
   Change Description:  
     changed enumeration names
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.81   10 Nov 2003 12:23:56   rschock
   Project:  SVObserver
   Change Request (SCR) nbr:  395
   SCR Title:  Color ROI does not update unless you move ROI in training
   Checked in by:  rSchock;  Rosco Schock
   Change Description:  
     Made changes to the RGBMainImage and ColorTool to reduce possibilities of getting out of sync and to consolidate the color image specific code.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.80   22 Oct 2003 09:32:54   ebeyeler
   Project:  SVObserver
   Change Request (SCR) nbr:  320
   SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
   Checked in by:  eBeyeler;  Eric Beyeler
   Change Description:  
     added support for color / mixed mode 1394 images
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.79   24 Sep 2003 15:29:38   ebeyeler
   Project:  SVObserver
   Change Request (SCR) nbr:  320
   SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
   Checked in by:  eBeyeler;  Eric Beyeler
   Change Description:  
     set PRODUCT_INSPECTION_RUN status after pToolset->Run is completed
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.78   24 Sep 2003 11:15:16   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  320
   SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
   Checked in by:  eBeyeler;  Eric Beyeler
   Change Description:  
     Updated RunInspection method to check the run status object and report if the status is invalid.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.77   22 Sep 2003 14:52:30   ebeyeler
   Project:  SVObserver
   Change Request (SCR) nbr:  320
   SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
   Checked in by:  eBeyeler;  Eric Beyeler
   Change Description:  
     updated Lut Dlg code to check creation return value
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.76   22 Sep 2003 13:46:56   rschock
   Project:  SVObserver
   Change Request (SCR) nbr:  383
   SCR Title:  Scrolling problems after editing a config or selecting a new display image
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
     Fixed loading of pane and splitter sizes so that panes aren't overwritten by the frame.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.75   08 Jul 2003 13:31:36   rschock
   Project:  SVObserver
   Change Request (SCR) nbr:  377
   SCR Title:  The Color SVIM runs much slower in 4.x versions
   Checked in by:  rSchock;  Rosco Schock
   Change Description:  
     Consolidated SVStateClass into SVSVIMStateClass and fixed SVSVIMStateClass to no longer use a mutex for locking. Plus removed extra addition of the RGB MainImage to the MainImageManager.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.74   08 Jul 2003 13:24:26   ebeyeler
   Project:  SVObserver
   Change Request (SCR) nbr:  322
   SCR Title:  Add Additional Digital IO Resources to SVObserver
   Checked in by:  eBeyeler;  Eric Beyeler
   Change Description:  
     SVLightReferenceArray -> SVLightReference
   changed mpsvImaging BOOL return values to HRESULT
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.73   17 Jun 2003 15:18:14   rschock
   Project:  SVObserver
   Change Request (SCR) nbr:  374
   SCR Title:  Add check before changing tool parameters when processing input list
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Added prototype code for using a map in the inspection to lookup objects that are used in the input request queue. It will be finalized after further testing.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.72   02 Jun 2003 15:06:50   rschock
   Project:  SVObserver
   Change Request (SCR) nbr:  352
   SCR Title:  Update SVObserver to Version 4.30 Release
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Checked in some preliminary beta code for the new methode of disabling tools and toolsets.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.71   23 May 2003 12:22:14   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  347
   SCR Title:  RGB Color system has out of sync displays and improper results
   Checked in by:  rSchock;  Rosco Schock
   Change Description:  
     Updated RunRegressionTest method to fix sync problem with rgbMainImage.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.70   21 May 2003 17:49:02   rschock
   Project:  SVObserver
   Change Request (SCR) nbr:  361
   SCR Title:  SVObserver's display may become incorrect when running at very high speeds
   Checked in by:  rSchock;  Rosco Schock
   Change Description:  
     Fixed index problem with regression test so that it will run and display at the correct index.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.69   07 May 2003 13:16:46   rschock
   Project:  SVObserver
   Change Request (SCR) nbr:  361
   SCR Title:  SVObserver's display may become incorrect when running at very high speeds
   Checked in by:  rSchock;  Rosco Schock
   Change Description:  
     Modified display logic so that image view displaying a SourceImage will look into the ProductInfoStruct to find the source image index for the camera from which they are derived instead of assuming it is the first camera.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.68   30 Apr 2003 17:49:08   rschock
   Project:  SVObserver
   Change Request (SCR) nbr:  361
   SCR Title:  SVObserver's display may become incorrect when running at very high speeds
   Checked in by:  rSchock;  Rosco Schock
   Change Description:  
     Changed display logic to make a copy of the result indexes from the product before starting the SLOW display process so that the product can't be recycled before the display finishes.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.67   22 Apr 2003 10:31:32   rschock
   Project:  SVObserver
   Change Request (SCR) nbr:  346
   SCR Title:  Update SVObserver to Version 4.21 Release
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.66   17 Apr 2003 13:00:16   rschock
   Project:  SVObserver
   Change Request (SCR) nbr:  283
   SCR Title:  Adding a tool before a Color Tool causes a Dr. Watson
   Checked in by:  rSchock;  Rosco Schock
   Change Description:  
     Made changes to SVIPDoc to check that the color tool is always the first tool on a color system. This is checked before a tool is added or deleted.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.65   16 Apr 2003 17:45:26   rschock
   Project:  SVObserver
   Change Request (SCR) nbr:  343
   SCR Title:  Fix RunOnce functions so later calls to GetDataList and GetImageList work
   Checked in by:  rSchock;  Rosco Schock
   Change Description:  
     Modified SVIPDoc so that the SVInspectionProcess's last run product is erased on a RunOnce operation. Also forced index 1 of the circle buffers to always be reserved for RunOnce.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.64   16 Apr 2003 17:38:16   rschock
   Project:  SVObserver
   Change Request (SCR) nbr:  342
   SCR Title:  Configuration report printout changes after going online.
   Checked in by:  rSchock;  Rosco Schock
   Change Description:  
     Added new state SV_STATE_INTERNAL_RUN. This is used to force all objects to run during the loading process. Otherwise some objects may be conditionally not executed.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.63   11 Apr 2003 09:45:22   ryoho
   Project:  SVObserver
   Change Request (SCR) nbr:  345
   SCR Title:  Inspection Toolbar goes away when editing a configuration
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
     removed from the destructor the turning off the inspection toolbar.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.62   27 Mar 2003 10:26:16   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  301
   SCR Title:  Implement Result Image Circle Buffer
   Checked in by:  eBeyeler;  Eric Beyeler
   Change Description:  
     Updated SingleRunModeLoop method needed to update the RGBMainImage index.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.61   21 Mar 2003 12:30:36   ryoho
   Project:  SVObserver
   Change Request (SCR) nbr:  330
   SCR Title:  Add SetImageList to ActiveX Interface
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
     added call to ProcessInputImageRequest on SingleRunModeLoop
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.60   18 Mar 2003 12:10:50   ebeyeler
   Project:  SVObserver
   Change Request (SCR) nbr:  336
   SCR Title:  Change Light Reference and LUT dialogs on RGB mono system
   Checked in by:  eBeyeler;  Eric Beyeler
   Change Description:  
     Changed GetUniqueAcqDeviceCameras to get camera info from the Main Image Manager
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.59   14 Mar 2003 07:05:22   ebeyeler
   Project:  SVObserver
   Change Request (SCR) nbr:  336
   SCR Title:  Change Light Reference and LUT dialogs on RGB mono system
   Checked in by:  eBeyeler;  Eric Beyeler
   Change Description:  
     Changed GetUniqueAcqDeviceCameras
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.58   11 Mar 2003 16:15:58   ryoho
   Project:  SVObserver
   Change Request (SCR) nbr:  337
   SCR Title:  Memory leak when editing a configuration
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
     added virtual functions for DisconnectToolSetMainImage and ConnectToolSetMainImage
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.57   27 Feb 2003 16:19:42   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  314
   SCR Title:  SVIM stops inspecting when triggered very fast
   Checked in by:  rSchock;  Rosco Schock
   Change Description:  
     Updated RunInspection method to reset main images to -1 for dead products and sets RunStatus info to InValid.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.56   14 Feb 2003 16:59:06   rschock
   Project:  SVObserver
   Change Request (SCR) nbr:  301
   SCR Title:  Implement Result Image Circle Buffer
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
     Fixed problem with RunStatus resetting the wrong things. Changed SVIPDoc to do its own resetting.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.55   13 Feb 2003 12:36:00   ryoho
   Project:  SVObserver
   Change Request (SCR) nbr:  301
   SCR Title:  Implement Result Image Circle Buffer
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
     fixed problem with variables not being initialized.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.54   05 Feb 2003 17:29:56   rschock
   Project:  SVObserver
   Change Request (SCR) nbr:  301
   SCR Title:  Implement Result Image Circle Buffer
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Made modifications to the display logic to display the current source and result images indexes, respectively. This should make it work correctly regardless of whether it is online or performing RunOnce/Regression.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.53   04 Feb 2003 16:36:52   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  301
   SCR Title:  Implement Result Image Circle Buffer
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated RunRegressionTest and SingleRunModeLoop methods to fix a problem when moving a tool and the display did not refresh.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.52   30 Jan 2003 11:50:40   joe
   Project:  SVObserver
   Change Request (SCR) nbr:  301
   SCR Title:  Implement Result Image Circle Buffer
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated GetImage, RunRegressionTest, RunInspection, BuildValueObjectList, OnEditDelete, processMessage, SingleRunModeLoop, AddTool, InitAfterSystemIsDocked, destructor and init methods to use the new image circle buffer methodology.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.51   10 Jan 2003 14:02:24   rschock
   Project:  SVObserver
   Change Request (SCR) nbr:  305
   SCR Title:  Implement the ability to perform RunOnce from a SIAC client
   Checked in by:  rSchock;  Rosco Schock
   Change Description:  
     Made changes to how the images are displayed while running and while doing a RunOnce operation to use the image that is specified in the SVProductInfoStruct
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.50   10 Jan 2003 09:46:00   ebeyeler
   Project:  SVObserver
   Change Request (SCR) nbr:  306
   SCR Title:  Implement the ability for the SIAC to remotely setup the RGB LUT for a camera
   Checked in by:  eBeyeler;  Eric Beyeler
   Change Description:  
     Added OnAdjustLut and helper function GetUniqueAcqDeviceCameras
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.49   09 Dec 2002 15:25:08   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  313
   SCR Title:  Unpublishing a result that is assigned to an output causes problems
   Checked in by:  rSchock;  Rosco Schock
   Change Description:  
     Updated OnEditDelete and OnPublishedResultsPicker of SVIPDoc.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.48   09 Dec 2002 15:17:34   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  296
   SCR Title:  Fix PPQ when multiple inspections and cameras are used
   Checked in by:  rSchock;  Rosco Schock
   Change Description:  
     Updated RunInspection method of SVIPDoc class to fix a problem with the toolset IsEnabled flag condition.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.47   08 Dec 2002 22:17:12   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  311
   SCR Title:  Fix Light Reference save for RGB Mono
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated OnAdjustLightReference to correctly store changed values.
   
   Updated InitAfterSystemIsDocked and IsColorInspectionDocument methods of the SVIPDoc to reflect changes in references of SVAcquisitionClass member variables to accessor functions.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.46   19 Nov 2002 14:40:34   ryoho
   Project:  SVObserver
   Change Request (SCR) nbr:  226
   SCR Title:  Monochrome SVIM configuration compatibility between ViperQUAD and ViperDUAL
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
     major changes to the file were made
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.44   30 Jan 2002 16:08:12   rschock
   Project:  SVObserver
   Change Request (SCR) nbr:  278
   SCR Title:  Error message is displayed when removing result objects used by other tools
   Checked in by:  rSchock;  Rosco Schock
   Change Description:  
     Modified ::OnEditTool to validate the toolset before running it after the selected tool was edited.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.43   10 Oct 2001 17:21:08   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  242
   SCR Title:  Problem processing color images in regression mode.
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated RunRegressionTest method to copy image to RGB main image buffer and update HSI buffer.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.42   09 Aug 2001 07:54:38   ed
   Project:  SVObserver
   Change Request (SCR) nbr:  210
   SCR Title:  Fix of shared data, multiple IPD's, same digitizer
   Checked in by:  Ed;  Ed Chobanoff
   Change Description:  
     Removed validation code from "InitAfterSystemIsDocked()" and
   placed this code in new function "ValidateAndInitialize".  
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.41   07 Aug 2001 15:15:58   rschock
   Project:  SVObserver
   Change Request (SCR) nbr:  208
   SCR Title:  Improve configuration load time
   Checked in by:  rSchock;  Rosco Schock
   Change Description:  
     Removed unneeded call to SVM_RESET_ALL_OBJECTS
   Adjusted order of some initialization of display and results
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.40   12 Jun 2001 15:36:58   ed
   Project:  SVObserver
   Change Request (SCR) nbr:  200
   SCR Title:  Update code to implement SVIM COM Server
   Checked in by:  Ed;  Ed Chobanoff
   Change Description:  
     
   Modified the following functions: OnUpdateRunRegressionTest. 
   Modified SVObserver state code in functions, SetTitle, SetPathName, SingleRunModeLoop, CanCloseFrame, SaveModified, OnCloseDocument, CloseDocument, OnAdjustLightReference, OnEditDelete, RunRegressionTest, OnUpdateViewToolSetDrawSubMenues. Added the GetFile function.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.39   19 Apr 2001 14:17:52   Steve
   Project:  SVObserver
   Change Request (SCR) nbr:  196
   SCR Title:  Restructure Scripted Load/Save Procedures Functional Requirement
   Checked in by:  Steve;  Stephen E. Steffan
   Change Description:  
     Header file changes due to include file restructuring.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.38   08 Mar 2001 12:39:44   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  192
   SCR Title:  Restore Freeze Tool Resume and Fix Freezing Problems after Initial Load
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Removed Run Mode and Test Mode restrictions from OnResumeFreeze mthod.  Added a SVM_RESET_ALL_OBJECTS message for pFreezeSet to Validate mdthod.
   
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.37   25 Jan 2001 16:17:48   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  191
   SCR Title:  Restructure File Management
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     These changes fixed the SVObserverApp file handling and associated systems.  These systems include the .SEC, .IOD, and .IPD files as well as the camera files.
   
      Rev 3.36.1.1   11 May 2001 09:42:14   rschock
   Project:  SVObserver
   Change Request (SCR) nbr:  197
   SCR Title:  Fix lock-ups in application when loading configuration from SVFocusNT
   Checked in by:  rSchock;  Rosco Schock
   Change Description:  
     Changed SVObserver state code in
   init
   SetTitle
   SetPathName
   SingleRunModeLoop
   CanCloseFrame
   SaveModified
   OnCloseDocument
   CloseDocument
   OnAdjustLightReference
   OnEditDelete
   RunRegressionTest
   RefreshViewedPPQDataList
   
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.36.1.0   14 Feb 2001 15:08:02   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  192
   SCR Title:  Restore Freeze Tool Resume and Fix Freezing Problems after Initial Load
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Removed Run Mode and Test Mode restrictions from OnResumeFreeze mthod.
   Added a SVM_RESET_ALL_OBJECTS message for pFreezeSet to Validate mdthod.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.36   06 Sep 2000 16:30:30   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  179
   SCR Title:  Create Color SVObserver
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated files to include color SVIM changes.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.35   26 Jun 2000 14:36:56   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  167
   SCR Title:  Fix Sharing Violation Using Save Configuration As Menu Item
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     The OnSaveDocument Method was changed to call the CDocument::OnSaveDocument insted of the CDocument::OnSaveDocument method for saving the IPDoc to the archive file.  This change fixes the sharing violation problem when trying to save over the existing file.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.34   15 Jun 2000 11:47:46   sjones
   Project:  SVObserver
   Change Request (SCR) nbr:  165
   SCR Title:  Deleting a Tool in Run Mode
   Checked in by:  sJones;  Steve Jones
   Change Description:  
     Revised OnEditDelete to disallow deletion of a tool if in Run or Test mode.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.33   14 Jun 2000 16:32:50   sjones
   Project:  SVObserver
   Change Request (SCR) nbr:  161
   SCR Title:  Inspection Document size keeps increasing.
   Checked in by:  sJones;  Steve Jones
   Change Description:  
     Added Override for OnSaveDocument to bypass OLE
   compound file storgae.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.32   17 May 2000 17:54:18   sjones
   Project:  SVObserver
   Change Request (SCR) nbr:  141
   SCR Title:  View DDE Inputs/Outputs on Results View
   Checked in by:  sJones;  Steve Jones
   Change Description:  
     Added OnSelectPPQVariable menu handler.
   Added UpdateViewedPPQDataList method.
   Added RefreshViewedPQDataList method.
   Added DestroyViewedPPQDataList method.
   Added GetViewedPPQList method.
   Added SVPQDataLinkInfoPtrList variable viewedPPQDataList.
   Added include for SVPQVariableSelectionDialog.h
   Revised PreDestroy to call DestroyViewedPPQDataList.
   Revised Validate to refresh the result view.
   Revised RefreshPPQDataList to call RefreshViewedPPQDataList.
   
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.31   07 May 2000 23:29:54   Jim
   Project:  SVObserver
   Change Request (SCR) nbr:  137
   SCR Title:  Remove the need to explicitly reference specific version numbers.
   Checked in by:  JimAdmin;  James A. Brown
   Change Description:  
     Changed the Serialization () functions so that versioning does not need to be explicitly addressed unless a change actually affects file compatabillity.
   
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.30   05 May 2000 13:41:56   sjones
   Project:  SVObserver
   Change Request (SCR) nbr:  134
   SCR Title:  Restricted Operator Security level
   Checked in by:  sJones;  Steve Jones
   Change Description:  
     Added OnUpdateFileExit message handler so exit menu item
   will be disabled for restricted operator.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.29   26 Apr 2000 14:18:08   sjones
   Project:  SVObserver
   Change Request (SCR) nbr:  129
   SCR Title:  Angular Profile Tool
   Checked in by:  sJones;  Steve Jones
   Change Description:  
     Revised to include SVAngularProfileTool.h.
   Revised AddProfileTool.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.28   19 Apr 2000 16:26:00   Jim
   Project:  SVObserver
   Change Request (SCR) nbr:  128
   SCR Title:  Suport for 3.11 Beta 1 versioning
   Checked in by:  JimAdmin;  James A. Brown
   Change Description:  
     SVIPDocRequirementInfoClass::Serialize () and 
   SVIPDoc::Serialize () were both changed to support program version number 3.11 beta 1.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.27   11 Apr 2000 17:32:52   Jim
   Project:  SVObserver
   Change Request (SCR) nbr:  121
   SCR Title:  Support for 3.06 versioning
   Checked in by:  JimAdmin;  James A. Brown
   Change Description:  
     Changed SVIPDocRequirementInfoClass::Serialize () and 
   SVIPDoc::Serialize () to support program version number 3.06.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.26   23 Mar 2000 17:32:20   sjones
   Project:  SVObserver
   Change Request (SCR) nbr:  118
   SCR Title:  Integration of LUT Operator
   Checked in by:  sJones;  Steve Jones
   Change Description:  
     Revised AddTool(...) to cal standard SetDefaultFormulas().
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.25   23 Mar 2000 16:34:38   sjones
   Project:  SVObserver
   Change Request (SCR) nbr:  118
   SCR Title:  Integration of LUT Operator
   Checked in by:  sJones;  Steve Jones
   Change Description:  
     Introduced new fast refreshAlPanels(...) which can be called
   inside the Tool Adjust Dialog pages to update all image and
   result views.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.24   23 Mar 2000 13:25:42   sjones
   Project:  SVObserver
   Change Request (SCR) nbr:  116
   SCR Title:  Bug Fixes for Freeze on Reject
   Checked in by:  sJones;  Steve Jones
   Change Description:  
     Revised OnResumeFreeze() to be blocked while Run or Test mode.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.23   14 Mar 2000 14:42:54   sjones
   Project:  SVObserver
   Change Request (SCR) nbr:  101
   SCR Title:  Versioning 3.10
   Checked in by:  sJones;  Steve Jones
   Change Description:  
     Revised version number to 3.10 release (0x00030AFF)
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.22   09 Mar 2000 13:02:14   sjones
   Project:  SVObserver
   Change Request (SCR) nbr:  93
   SCR Title:  Integrate Multiple Camera Image Input per IPD
   Checked in by:  Robert;  Robert Massinger
   Change Description:  
     Revised to correct an application exception error while closing if something was published, due to a circular close
   message.
   
   
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.21   09 Mar 2000 08:52:32   sjones
   Project:  SVObserver
   Change Request (SCR) nbr:  101
   SCR Title:  Versioning 3.10
   Checked in by:  sJones;  Steve Jones
   Change Description:  
     Revised Version number from 3.10 Beta 3 to 3.10 Beta 4
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.20   07 Mar 2000 14:06:50   sjones
   Project:  SVObserver
   Change Request (SCR) nbr:  101
   SCR Title:  Versioning 3.10
   Checked in by:  sJones;  Steve Jones
   Change Description:  
     Changed Version number to 3.1 Beta 3.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.19   03 Mar 2000 14:10:12   sjones
   Project:  SVObserver
   Change Request (SCR) nbr:  113
   SCR Title:  Integrate new Polar Unwrap Tool
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
     Revised OnAddPolarUnwrapTool() to call SetDefaultFormulas() Method.
   Bug Fix in InitAfterSystemIsDocked();
   Send RESET_ALL_OBJECTS message before the first time
   SingleRunModeLoop() is called! IS needed to give equations the possibility to build their sysmbol tables first!!!
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.18   Mar 01 2000 12:29:34   Nick
   Project:  SVObserver
   Change Request (SCR) nbr:  113
   SCR Title:  Integrate new Polar Unwrap Tool
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
     Revised to handle new Message Handler for Menu: ADD->Polar Unwrap Tool.
   
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.17   Feb 24 2000 11:54:28   Nick
   Project:  SVObserver
   Change Request (SCR) nbr:  111
   SCR Title:  Integrate Conditional Tool Set/Tool Drawing
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
     Introduced new conditional tool set drawing.
   Revised InitMenu() to init tool set draw flag menu.
   Added MessageHandler to handle tool set draw flag menu messages.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.16   21 Feb 2000 15:14:46   sjones
   Project:  SVObserver
   Change Request (SCR) nbr:  104
   SCR Title:  New Build Reference Tool
   Checked in by:  sJones;  Steve Jones
   Change Description:  
     Aded OnAddTransformationTool method.
   Added include file SVTransformationTool.h.
   Revised OnEditTool() to run the toolset (all tools) instead of
   just the current tool being edited.
   
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.15   Feb 21 2000 10:36:32   Nick
   Project:  SVObserver
   Change Request (SCR) nbr:  109
   SCR Title:  Freeze on Reject for v3.1
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
     Added resume message handler.
   Introduced freeze set.
   Changed view serialization (new function).
   Revised versioning to v3.10 Beta 2.
   
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.14   Feb 16 2000 17:42:28   Nick
   Project:  SVObserver
   Change Request (SCR) nbr:  107
   SCR Title:  Add More Blob Analyzer Features
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
     Moved 'Broadcase Message' for 'Going On Line' from SVInpectionThread.cpp.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.13   16 Feb 2000 16:49:32   mike
   Project:  SVObserver
   Change Request (SCR) nbr:  96
   SCR Title:  Scroll/Zoom images displayed in the inspection document.
   Checked in by:  Mike;  Mike McCarl
   Change Description:  
     Added multi-image display to the IPD
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.12   08 Feb 2000 11:05:06   mike
   Project:  SVObserver
   Change Request (SCR) nbr:  96
   SCR Title:  Scroll/Zoom images displayed in the inspection document.
   Checked in by:  Mike;  Mike McCarl
   Change Description:  
     Changed Serialize function to also serialize the views and
   also the number of views in the IPD.
   
   
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.11   07 Feb 2000 20:08:24   robert
   Project:  SVObserver
   Change Request (SCR) nbr:  93
   SCR Title:  Integrate Multiple Camera Image Input per IPD
   Checked in by:  Robert;  Robert Massinger
   Change Description:  
     Introduced new Main Image Manager and Acquisition Tool Classes.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.10   04 Feb 2000 13:31:34   sjones
   Project:  SVObserver
   Change Request (SCR) nbr:  101
   SCR Title:  Versioning 3.10
   Checked in by:  sJones;  Steve Jones
   Change Description:  
     Changed version to 3.10
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.9   Feb 03 2000 16:33:22   Nick
   Project:  SVObserver
   Change Request (SCR) nbr:  102
   SCR Title:  Versioning v3.05
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
     Changed version from v3.04 to v3.05.  Corrected multiple inheritance order in System.h.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.8   Feb 01 2000 08:51:04   Nick
   Project:  SVObserver
   Change Request (SCR) nbr:  91
   SCR Title:  Archive Errors During Regression Test
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
     Add code to check the menu to make sure regression test is 'turned off' when system is 'online' or in 'test mode'.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.7   Jan 19 2000 15:18:50   Nick
   Project:  SVObserver
   Change Request (SCR) nbr:  88
   SCR Title:  Version 3.04 versioning changes.
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
     Changes required to change v3.04 Beta 1 to v3.04 Released.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.6   Jan 14 2000 15:15:02   Nick
   Project:  SVObserver
   Change Request (SCR) nbr:  85
   SCR Title:  Version 3.04 Beta 1 versioning changes.
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
     Changes required for v3.03 to v3.04 Beta 1.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.5   Jan 04 2000 08:58:24   Nick
   Project:  SVObserver
   Change Request (SCR) nbr:  81
   SCR Title:  Version 3.03 versioning changes
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
     Changes required to convert v3.02 to v3.03.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.4   Dec 02 1999 08:36:38   Nick
   Project:  SVObserver
   Change Request (SCR) nbr:  72
   SCR Title:  Version 3.02 versioning changes.
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
     Change v3.01 to v3.02.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.3   Dec 02 1999 08:32:32   Nick
   Project:  SVObserver
   Change Request (SCR) nbr:  57
   SCR Title:  Version 3.00 Beta 18 Versioning
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
     Changed v3.01 to v3.02.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.2   Nov 30 1999 15:54:18   Nick
   Project:  SVObserver
   Change Request (SCR) nbr:  67
   SCR Title:  Version 3.01 versioning changes.
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
     Changes required to go from v3.00 to v3.01.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.1   Nov 30 1999 13:18:22   Nick
   Project:  SVObserver
   Change Request (SCR) nbr:  65
   SCR Title:  Remove obsolute class SVToolSetViewClass
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
     Removed references to SVToolSetViewClass.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.0   23 Nov 1999 12:55:08   mike
   Project:  SVObserver
   Change Request (SCR) nbr:  61
   SCR Title:  Update PVCS versioning to version 3.0.
   Checked in by:  Mike;  Mike McCarl
   Change Description:  
     
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.59   18 Nov 1999 12:43:58   mike
   Project:  SVObserver
   Change Request (SCR) nbr:  60
   SCR Title:  Create 'Utilities' menu option
   Checked in by:  Mike;  Mike McCarl
   Change Description:  
     Modified initialization function to load the 'Utility' menu.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.58   Nov 18 1999 09:36:26   Nick
   Project:  SVObserver
   Change Request (SCR) nbr:  59
   SCR Title:  Version 3.00 Release Versioning
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
     v3.00 Release versioning changes.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.57   18 Nov 1999 09:11:36   sjones
   Project:  SVObserver
   Change Request (SCR) nbr:  9
   SCR Title:  Integr. a common object interface to handle and to identify obj. in-/outputs
   Checked in by:  sJones;  Steve Jones
   Change Description:  
     Added HandleDocumentRenamed method to update PPQDataLinkInfo variables when a document gets renamed.
   Added  RebuildResultsList method to rebuild the result list
   when a document gets renamed.
   Added InitToolSetResultDataLinkInfo method in support of
   MultiThreaded Object Script parsing.
   Correct a problem with Regression Testing and the Statistics
   Tool
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.56   Nov 10 1999 12:20:58   Nick
   Project:  SVObserver
   Change Request (SCR) nbr:  57
   SCR Title:  Version 3.00 Beta 18 Versioning
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
     Changes v3.00 Beta 17 to v3.00 Beta 18.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.55   Nov 05 1999 11:18:32   Nick
   Project:  SVObserver
   Change Request (SCR) nbr:  56
   SCR Title:  Version 3.00 Beta 17 Versioning
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
     v3.00 Beta 16 to v3.00 Beta 17.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.54   Nov 04 1999 15:11:36   Nick
   Project:  SVObserver
   Change Request (SCR) nbr:  15
   SCR Title:  Integrate archive tool
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
     Changes to eliminate a configuration restore on a configuration save operation.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.53   Nov 02 1999 08:58:56   Nick
   Project:  SVObserver
   Change Request (SCR) nbr:  55
   SCR Title:  Version 3.00 Beta 16 Versioning
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
     Changes required for v3.00 Beta 15 to v3.00 Beta 16 change.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.52   Nov 01 1999 17:00:58   Nick
   Project:  SVObserver
   Change Request (SCR) nbr:  54
   SCR Title:  Version 3.00 Beta 15 Versioning.
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
     Change IDC_RUN_REGRESSION to ID_RUN_REGRESSION.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.51   Oct 29 1999 17:43:42   Nick
   Project:  SVObserver
   Change Request (SCR) nbr:  54
   SCR Title:  Version 3.00 Beta 15 Versioning.
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
     v3.00 Beta 14 to v3.00 Beta 15.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.50   28 Oct 1999 18:37:52   mike
   Project:  SVObserver
   Change Request (SCR) nbr:  53
   SCR Title:  Implement Regression Test Capability in SVObserver
   Checked in by:  Mike;  Mike McCarl
   Change Description:  
     Implemented Regression Test
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.49   Oct 26 1999 14:57:54   Nick
   Project:  SVObserver
   Change Request (SCR) nbr:  52
   SCR Title:  Version 3.00 Beta 14 Versioning
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
     v3.00 Beta 13 to v3.00 Beta 14.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.48   25 Oct 1999 14:42:00   sjones
   Project:  SVObserver
   Change Request (SCR) nbr:  1
   SCR Title:  Integrate a common validity check for all tool base class inputs and outputs.
   Checked in by:  sJones;  Steve Jones
   Change Description:  
     Corrected problem where tool shows incorrect status after
    editing.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.47   Oct 22 1999 09:02:00   Nick
   Project:  SVObserver
   Change Request (SCR) nbr:  51
   SCR Title:  Version 3.00 Beta 13 Versioning
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
     v3.00 Beta 12 to v3.00 Beta 13.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.46   Oct 20 1999 16:06:38   Nick
   Project:  SVObserver
   Change Request (SCR) nbr:  50
   SCR Title:  Version 3.00 Beta 12 Versioning
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
     Changed version from v3.00 Beta 11 to v3.00 Beta 12.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.45   19 Oct 1999 15:38:48   sjones
   Project:  SVObserver
   Change Request (SCR) nbr:  1
   SCR Title:  Integrate a common validity check for all tool base class inputs and outputs.
   Checked in by:  sJones;  Steve Jones
   Change Description:  
     Moved Sending of message 
   SVM_CREATE_ALL_CLOSED_OBJECTS from 
   SetDefaults() to OnEditDelete.
   (caused a problem - too many creates)
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.44   19 Oct 1999 13:26:34   sjones
   Project:  SVObserver
   Change Request (SCR) nbr:  1
   SCR Title:  Integrate a common validity check for all tool base class inputs and outputs.
   Checked in by:  sJones;  Steve Jones
   Change Description:  
     Revised SetDefaults to send 
   SVM_CREATE_ALL_CLOSED_OBJECTS
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.43   18 Oct 1999 17:37:52   sjones
   Project:  SVObserver
   Change Request (SCR) nbr:  9
   SCR Title:  Integr. a common object interface to handle and to identify obj. in-/outputs
   Checked in by:  sJones;  Steve Jones
   Change Description:  
     Revised checkOkToDelete.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.42   Oct 18 1999 12:50:04   Nick
   Project:  SVObserver
   Change Request (SCR) nbr:  47
   SCR Title:  Version 3.00 Beta 11 Versioning
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
     Changes for v3.00 Beta 10 to v3.00 Beta 11.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.41   14 Oct 1999 19:29:36   sjones
   Project:  SVObserver
   Change Request (SCR) nbr:  9
   SCR Title:  Integr. a common object interface to handle and to identify obj. in-/outputs
   Checked in by:  sJones;  Steve Jones
   Change Description:  
     Added call to checkOkToDelete.
   Added checkOkToDelete method to show dependents
   dialog
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.40   Oct 13 1999 15:37:10   Nick
   Project:  SVObserver
   Change Request (SCR) nbr:  46
   SCR Title:  Version 3.00 Beta 10 Versioning
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
     Version v3.00 Beta 9 to v3.00 Beta 10 changes.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.39   Oct 13 1999 15:06:48   Nick
   Project:  SVObserver
   Change Request (SCR) nbr:  21
   SCR Title:  Port Gage Tool code from program version 2.42.
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
     Roberts bug fixes for Beta 10.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.38   13 Oct 1999 12:56:48   sjones
   Project:  SVObserver
   Change Request (SCR) nbr:  9
   SCR Title:  Integr. a common object interface to handle and to identify obj. in-/outputs
   Checked in by:  sJones;  Steve Jones
   Change Description:  
     Added message handler in processMessage for
   SVMSGID_DISCONNECT_OBJECT_INPUT for
   updating published results.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.37   Oct 12 1999 08:16:44   Nick
   Project:  SVObserver
   Change Request (SCR) nbr:  44
   SCR Title:  Version 3.00 Beta 9 Versioning
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
     Changes required to change v3.00 Beta 8 to v3.00 Beta 9.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.36   11 Oct 1999 16:26:40   sjones
   Project:  SVObserver
   Change Request (SCR) nbr:  9
   SCR Title:  Integr. a common object interface to handle and to identify obj. in-/outputs
   Checked in by:  sJones;  Steve Jones
   Change Description:  
     Moved call to SetDefaultInputs out of Serialize to
   SVSystemClass so the name of PQDataLinkInfo 
   variables are set correctly.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.35   Oct 08 1999 10:54:50   Nick
   Project:  SVObserver
   Change Request (SCR) nbr:  43
   SCR Title:  Version 3.00 Beta 7 Versioning
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
     Versioning changes to Beta 8.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.34   05 Oct 1999 11:18:26   sjones
   Project:  SVObserver
   Change Request (SCR) nbr:  39
   SCR Title:  Integrate Statistics Tool
   Checked in by:  sJones;  Steve Jones
   Change Description:  
     Added Statistics Tool
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.33   Oct 04 1999 11:32:04   Nick
   Project:  SVObserver
   Change Request (SCR) nbr:  43
   SCR Title:  Version 3.00 Beta 7 Versioning
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
     V3.00 Beta 7 versioning.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.32   Oct 04 1999 10:25:30   Nick
   Project:  SVObserver
   Change Request (SCR) nbr:  21
   SCR Title:  Port Gage Tool code from program version 2.42.
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
     Wait cursor.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.31   Sep 30 1999 14:43:08   Nick
   Project:  SVObserver
   Change Request (SCR) nbr:  42
   SCR Title:  Version 3.00 Beta 6 Versioning.
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
     Software changes required for version change from v3.00 Beta 5 to v3.00 Beta 6.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.30   Sep 29 1999 14:21:04   Nick
   Project:  SVObserver
   Change Request (SCR) nbr:  41
   SCR Title:  Version 3.00 Beta 5 Versioning.
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
     v3.00 Beta 5
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.29   29 Sep 1999 10:43:14   jim
   Project:  SVObserver
   Change Request (SCR) nbr:  40
   SCR Title:  Port Round Window Tool over from custom version 2.42.
   Checked in by:  JimAdmin;  James A. Brown
   Change Description:  
     SVIPDoc::OnAddRoundWindowTool () 
   
   was modified in order to enable adding Round Window Tools to the Tool Set.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.28   Sep 27 1999 19:35:10   Nick
   Project:  SVObserver
   Change Request (SCR) nbr:  35
   SCR Title:  PPQ mode to wait for input data
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
     BugFix: Wrong Type of PqdatalineInfoList was used.  It is now using PQdataLinkInfoPtrList.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.27   Sep 27 1999 19:04:44   Nick
   Project:  SVObserver
   Change Request (SCR) nbr:  38
   SCR Title:  Version 3.00 Beta 4 Versioning
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
     Version from 0x00030003 to 0x00030004.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.26   24 Sep 1999 19:11:12   sjones
   Project:  SVObserver
   Change Request (SCR) nbr:  9
   SCR Title:  Integr. a common object interface to handle and to identify obj. in-/outputs
   Checked in by:  sJones;  Steve Jones
   Change Description:  
     Removed call to RemoveExternalDependencies
   (Fix to restore inputs via Script)
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.25   Sep 24 1999 10:44:30   Nick
   Project:  SVObserver
   Change Request (SCR) nbr:  4
   SCR Title:  Add "Load image" option
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
     Header and Comments.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.24   Sep 24 1999 09:26:28   Nick
   Project:  SVObserver
   Change Request (SCR) nbr:  4
   SCR Title:  Add "Load image" option
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
     Added message handler for new load image tool.
   Introduced reset_all_message.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.23   23 Sep 1999 16:30:26   sjones
   Project:  SVObserver
   Change Request (SCR) nbr:  9
   SCR Title:  Integr. a common object interface to handle and to identify obj. in-/outputs
   Checked in by:  sJones;  Steve Jones
   Change Description:  
     Revised comments about destroying children
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.22   Sep 23 1999 13:53:58   Nick
   Project:  SVObserver
   Change Request (SCR) nbr:  37
   SCR Title:  Version 3.00 Beta 3 Versioning
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
     New version specific changes to Serialize methods.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.21   Sep 23 1999 09:02:46   Nick
   Project:  SVObserver
   Change Request (SCR) nbr:  35
   SCR Title:  PPQ mode to wait for input data
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
     Introduced new save tool set image dialog.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.20   Sep 21 1999 09:29:14   Nick
   Project:  SVObserver
   Change Request (SCR) nbr:  36
   SCR Title:  Version 3.00 Beta 2 Versioning
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
     Added 0x00030002 (v3.00 Beta 2) case statement  to all serialization methods.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.19   Sep 16 1999 13:40:50   Nick
   Project:  SVObserver
   Change Request (SCR) nbr:  34
   SCR Title:  Add Build Reference Tool
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
     Class name changed from SVArchiveToolClass to SVToolArchive.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.18   14 Sep 1999 11:12:50   sjones
   Project:  SVObserver
   Change Request (SCR) nbr:  16
   SCR Title:  Integrate Math Tool
   Checked in by:  sJones;  Steve Jones
   Change Description:  
     Added RefreshPPQDataList method.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.17   Sep 13 1999 13:43:18   Nick
   Project:  SVObserver
   Change Request (SCR) nbr:  33
   SCR Title:  Port Profile Tool from v2.42
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
     Added message handler for 'Add Image Tool'.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.16   11 Sep 1999 19:56:10   sjones
   Project:  SVObserver
   Change Request (SCR) nbr:  16
   SCR Title:  Integrate Math Tool
   Checked in by:  sJones;  Steve Jones
   Change Description:  
     Revised call to SVConditionalDialog.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.15   Sep 10 1999 19:18:08   Nick
   Project:  SVObserver
   Change Request (SCR) nbr:  34
   SCR Title:  Add Build Reference Tool
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
     Added Profile Tool 'OnAdd...'.
   Added Build Reference Tool 'On Add...'.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.14   10 Sep 1999 17:43:28   sjones
   Project:  SVObserver
   Change Request (SCR) nbr:  9
   SCR Title:  Integr. a common object interface to handle and to identify obj. in-/outputs
   Checked in by:  sJones;  Steve Jones
   Change Description:  
     Revised for refreshing of the published list.
   Corrected problem when deleting a tool.
   
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.13   08 Sep 1999 09:22:04   sjones
   Project:  SVObserver
   Change Request (SCR) nbr:  9
   SCR Title:  Integr. a common object interface to handle and to identify obj. in-/outputs
   Checked in by:  sJones;  Steve Jones
   Change Description:  
     Revised for new SVPublishListClass
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.12   04 Sep 1999 18:20:50   sjones
   Project:  SVObserver
   Change Request (SCR) nbr:  9
   SCR Title:  Integr. a common object interface to handle and to identify obj. in-/outputs
   Checked in by:  sJones;  Steve Jones
   Change Description:  
     Fixed bug when inserting at end of tool list.
   Revised to call SetModifiedFlag() when results pick has
   changed.
   Removed dead code.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.11   02 Sep 1999 08:03:40   sjones
   Project:  SVObserver
   Change Request (SCR) nbr:  9
   SCR Title:  Integr. a common object interface to handle and to identify obj. in-/outputs
   Checked in by:  sJones;  Steve Jones
   Change Description:  
     Revised for Published List and Results List picking.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.10   01 Sep 1999 11:47:50   sjones
   Project:  SVObserver
   Change Request (SCR) nbr:  9
   SCR Title:  Integr. a common object interface to handle and to identify obj. in-/outputs
   Checked in by:  sJones;  Steve Jones
   Change Description:  
     Revised versioning to 3.0 Beta 1
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.9   31 Aug 1999 23:40:50   sjones
   Project:  SVObserver
   Change Request (SCR) nbr:  16
   SCR Title:  Integrate Math Tool
   Checked in by:  sJones;  Steve Jones
   Change Description:  
     Revised for Editing ToolSet Conditional
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.8   30 Aug 1999 19:36:46   sjones
   Project:  SVObserver
   Change Request (SCR) nbr:  9
   SCR Title:  Integr. a common object interface to handle and to identify obj. in-/outputs
   Checked in by:  sJones;  Steve Jones
   Change Description:  
     Revised to use SVRunStatusClass.
   Revised for ToolSet Conditional (Friend Architechture).
   
   
   /////////////////////////////////////////////////////////////////////////////////////
   */
