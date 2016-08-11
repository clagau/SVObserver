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
#include "SVXMLLibrary\SVObjectXMLWriter.h"
#include "SVObjectLibrary/SVObjectManagerClass.h"
#include "SVObjectScriptParser.h"
#include "SVObserver.h"
#include "SVToolAdjustmentDialogSheetClass.h"
#include "SVToolSet.h"
#include "SVToolSetAdjustmentDialogSheet.h"
#include "ToolSetView.h"
#include "SVResultView.h"
//#include "SVResultsWrapperClass.h"
#include "SVTool.h"
#include "SVOGui\SVSaveToolSetImageDialog.h"
#include "SVOGui\SVShowDependentsDialog.h"
#include "SVUtilities.h"
#include "SVInspectionProcess.h"
#include "SVPPQObject.h"
#include "SVXMLLibrary/SVConfigurationTags.h"
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
#include "SVContainerLibrary/ObjectSelectorItem.h"
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
#include "TableTool.h"
#include "TableAnalyzerTool.h"
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
	ON_COMMAND(ID_ADD_TABLETOOL, OnAddTableTool)
	ON_COMMAND(ID_ADD_TABLEANALYZERTOOL, OnAddTableAnalyzerTool)
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

	if( S_OK == m_NewProductData.PopHead( l_ProductData ) )
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
	m_InspectionID = p_InspectionID;
	m_oDisplay.SetInspectionID( m_InspectionID, this );
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

	SVFileNameManagerClass svFileManager;
	svFileManager.RemoveItem( &msvFileName );

	ClearRegressionTestStructures();

	TheSVObserverApp.setCurrentDocument( nullptr );
}

CMDIChildWnd* SVIPDoc::GetMDIChild()
{
	if( nullptr == m_pMDIChildWnd )
	{
		m_pMDIChildWnd = SVSearchForMDIChildWnd( GetToolSetView() );
	}

	if( nullptr == m_pMDIChildWnd )
	{
		m_pMDIChildWnd = SVSearchForMDIChildWnd( GetResultView() );
	}

	if( nullptr == m_pMDIChildWnd )
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

			if( nullptr != pView )
			{
				SVGUID l_ImageId;

				if( nullptr != GetToolSet()->getCurrentImage() )
				{
					l_ImageId = GetToolSet()->getCurrentImage()->GetUniqueObjectID();
				}

				pView->AttachToImage( l_ImageId );

				SVImageViewScroll* pScroll( nullptr );

				pScroll = dynamic_cast< SVImageViewScroll* >( pView->GetParent() );

				if( nullptr != pScroll )
				{
					CWnd* pWnd( pScroll->GetParent() );

					SVIPSplitterFrame* pSplitterFrame( nullptr );

					do
					{
						pSplitterFrame = dynamic_cast< SVIPSplitterFrame* >( pWnd );

						if( nullptr == pSplitterFrame )
						{
							pWnd = pWnd->GetParent();
						}
					}
					while( nullptr != pWnd && nullptr == pSplitterFrame );

					if( nullptr != pSplitterFrame )
					{
						RECT IPRect = {0, 0, 0, 0};
						CMDIChildWnd* l_pMDIChild = GetMDIChild();

						if( nullptr != l_pMDIChild )
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

				if( nullptr != pView )
				{
					if (mbInitImagesByName)
					{
						pView->AttachToImage( _T( "" ) );
					}
					else
					{
						SVGUID l_ImageId;

						if( nullptr != GetToolSet()->getCurrentImage() )
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
						SvStl::MessageMgrDisplayAndNotify Msg( SvStl::LogAndDisplay );
						Msg.setMessage( SVMSG_SVO_93_GENERAL_WARNING, SvOi::Tid_ColorToolMustBeFirstMessage, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_10055 );
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

CView* SVIPDoc::getView() const
{
	CView* retVal( nullptr );
	POSITION pos = GetFirstViewPosition();
	if( nullptr != pos )
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

	if( nullptr != l_pToolSet )
	{
		l_pObject = l_pToolSet->GetToolSetConditional();
	}

	return l_pObject;
}

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
		SvStl::MessageMgrDisplayAndNotify Msg( SvStl::LogAndDisplay );
		Msg.setMessage( SVMSG_SVO_92_GENERAL_ERROR, SvOi::Tid_SVIPDoc_ClosingImpossible, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_10056 );
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
		!( S_OK == TheSVObserverApp.m_svSecurityMgr.SVValidate( SECURITY_POINT_FILE_MENU_CLOSE_CONFIGURATION ) ) )
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
	}
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
		bEnable = (S_OK == hr && cameraSet.size() > 0) ? true : false;
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
		bEnable = (S_OK == hr && cameraSet.size() > 0) ? true : false;
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
		if(  nullptr != ( *l_Iter ) )
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
		if( nullptr != *l_Iter )
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

				// Update DeviceParameters as well...
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

void SVIPDoc::OnAddTableTool()
{
	SVToolClass* pTool = new TableTool;

	if( AddTool( pTool ) ) { return; }

	if( pTool ) { delete( pTool ); }
}

void SVIPDoc::OnAddTableAnalyzerTool()
{
	SVToolClass* pTool = new TableAnalyzerTool;

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
						SvStl::MessageMgrDisplayAndNotify Msg( SvStl::LogAndDisplay );
						Msg.setMessage( SVMSG_SVO_93_GENERAL_WARNING, SvOi::Tid_ColorToolMustBeFirstMessage, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_10057 );
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

			::SVSendMessage( pInspection, SVM_CONNECT_ALL_OBJECTS, reinterpret_cast<DWORD_PTR>(&createStruct), 0 );
			::SVSendMessage( pInspection, SVM_CONNECT_ALL_INPUTS, 0, 0 );

			SVObjectLevelCreateStruct createObjStruct;

			::SVSendMessage( pInspection, SVM_CREATE_ALL_OBJECTS,reinterpret_cast<DWORD_PTR>(&createObjStruct), 0 );
			//Reset only the inserted tool
			::SVSendMessage( pTool, SVM_RESET_ALL_OBJECTS, TRUE, 0 );

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
						TheSVObserverApp.GetIODoc()->UpdateAllViews( nullptr );
					}
				}
				l_pTool->ResetObject();
				SVSVIMStateClass::RemoveState( SV_STATE_EDITING );
			}
		}
		UpdateAllViews( nullptr, RefreshView );
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

			SvOsl::SelectorOptions BuildOptions( GetInspectionID(), SV_VIEWABLE, GUID_NULL, true );
			SvOsl::ObjectTreeGenerator::Instance().BuildSelectableItems<SvOg::GlobalSelector, SvOg::PPQSelector, SvOg::ToolSetItemSelector<>>( BuildOptions );
			
			const SVObjectReferenceVector& rSelectedObjects( pResultList->GetSelectedObjects() );
			SvOsl::ObjectTreeGenerator::Instance().setCheckItems( rSelectedObjects, InspectionName );

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
					pResultList->Insert( Iter->getLocation() );
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
		SvOsl::ObjectTreeGenerator::Instance().setLocationFilter( SvOsl::ObjectTreeGenerator::FilterInput, SVString(csRootName), SVString( _T("") ) );

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

				SVSendMessage ( ObjectGuid, SVM_RESET_ALL_OBJECTS, 0, 0 );
			}

			SVConfigurationObject* pConfig( nullptr );
			SVObjectManagerClass::Instance().GetConfigurationObject( pConfig );
			if( nullptr != pConfig )
			{
				pConfig->ValidateRemoteMonitorList();
				TheSVObserverApp.GetIODoc()->UpdateAllViews( nullptr );
			}
		}

		SVSVIMStateClass::RemoveState(SV_STATE_EDITING);
	}
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

	if( S_OK == hres )
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
		SVCameraImageTemplate* pMainImage = nullptr;

		bool l_bAllowAccess = false;

		if ( l_bWasRunMode )
		{
			// Dual Security access point
			if( S_OK == TheSVObserverApp.m_svSecurityMgr.SVValidate( SECURITY_POINT_MODE_MENU_REGRESSION_TEST, 
				SECURITY_POINT_MODE_MENU_EXIT_RUN_MODE ) )
			{
				l_bAllowAccess = true;
			}
			else
			{
				return;
			}
		}
		else if( S_OK == TheSVObserverApp.m_svSecurityMgr.SVValidate( SECURITY_POINT_MODE_MENU_REGRESSION_TEST ) )
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

			m_hRegressionHandle = ::CreateThread( nullptr, 0, SVRegressionTestRunThread, (LPVOID)this, 0, &dwThreadID  );
		}
	}
	else
	{
		SvStl::MessageMgrDisplayAndNotify Msg( SvStl::LogAndDisplay );
		Msg.setMessage( SVMSG_SVO_92_GENERAL_ERROR, SvOi::Tid_SVIPDoc_GoIntoRegTestFailed, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_10058 );
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
				if( pMenu->ModifyMenu( pos, MF_BYPOSITION | MF_POPUP, reinterpret_cast<UINT_PTR> (myPopUp.GetSafeHmenu()), strMenu ) )
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
				UpdateAllViews( nullptr );
			}
		}
	}
}

void SVIPDoc::RefreshDocument()
{
	SVGuidSet l_ImageIds;

	for( int i = 0; i < MaxImageViews; ++i )
	{
		SVImageViewClass* l_pImageView = GetImageView( i );

		if( nullptr != l_pImageView )
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

	if( S_OK == l_Command.Execute( 120000 ) )
	{
		if( m_NewProductData.size() == m_NewProductData.capacity() )
		{
			m_NewProductData.PopHead();
		}

		if( S_OK == m_NewProductData.PushTail( l_DataPtr->GetProduct() ) )
		{
			CWnd* pWnd = GetMDIChild();
			if( nullptr != pWnd && ::IsWindow(pWnd->GetSafeHwnd() ) )
			{
				pWnd->PostMessage( SV_MDICHILDFRAME_UPDATE_ALL_DATA );
			}
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

	SaveViews(rWriter);

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

	if( nullptr == pInspection ) { return false; }

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

	if ( bOk )
	{
		WINDOWPLACEMENT wndpl;

		SVTreeType::SVBranchHandle htiWindow = nullptr;

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
				SVTreeType::SVBranchHandle htiData = nullptr;

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
				SVTreeType::SVBranchHandle htiData = nullptr;

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
				SVTreeType::SVBranchHandle htiData = nullptr;

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
		SVTreeType::SVBranchHandle htiIPViews = nullptr;
		SVTreeType::SVBranchHandle htiViews = nullptr;

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

			SVTreeType::SVBranchHandle htiBranch = nullptr;

			bOk = SVNavigateTree::GetItemBranch( rTree, CTAG_VIEWS, htiIPViews, htiViews );
			if ( bOk )
			{
				SVTreeType::SVBranchHandle htiItem( nullptr ); 

				htiItem = rTree.getFirstBranch( htiViews );

				while ( bOk && nullptr != htiItem )
				{
					Name = rTree.getBranchName( htiItem );

					// The class SVToolSetTabView was changed to ToolSetView when the Tool Set Tree View was removed.
					SvUl_SF::searchAndReplace( Name, CTAG_SVTOOLSET_TAB_VIEW_CLASS, CTAG_TOOLSET_VIEW );

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
								Name.compare(View.pView->GetRuntimeClass()->m_lpszClassName))
							{
								vPos = GetFirstViewPosition();
								while( ( View.pView = GetNextView( vPos ) ) && 
									( Name.compare( View.pView->GetRuntimeClass()->m_lpszClassName ) ) );
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
								Name.compare(View.pView->GetRuntimeClass()->m_lpszClassName))
							{
								vPos = GetFirstViewPosition();
								while( ( View.pView = GetNextView( vPos ) ) && 
									( Name.compare( View.pView->GetRuntimeClass()->m_lpszClassName ) ) );
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

SVImageClass* SVIPDoc::GetHSIMainImage()
{
	SVImageClass* l_pImage = nullptr;

	if( nullptr != GetInspectionProcess() )
	{
		l_pImage = GetInspectionProcess()->GetHSIMainImage();
	}

	return l_pImage;
}

HRESULT SVIPDoc::GetCameras( SVVirtualCameraPtrSet& p_rCameras ) const
{
	HRESULT l_Status = S_OK;

	if( nullptr != GetInspectionProcess() )
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
				TheSVObserverApp.GetIODoc()->UpdateAllViews( nullptr );
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

			if ( nullptr == posCamera ) { continue; }
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

				if( nullptr != l_IPDoc->GetInspectionProcess() )
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
				if ( nullptr != posDel )
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
	SendMessage(( ( SVMainFrame* ) pWnd )->m_pregTestDlg->GetSafeHwnd(), WM_REGRESSION_TEST_CLOSE_REGRESSION, 0, 0);

	return 0L;
}

void SVIPDoc::RegressionTestComplete()
{
	// @WARNING:  Pointers should be checked before they are dereferenced.
	( ( SVMainFrame* ) AfxGetApp()->m_pMainWnd )->m_pregTestDlg->DestroyWindow();
	delete ( ( SVMainFrame* ) AfxGetApp()->m_pMainWnd )->m_pregTestDlg;
	( ( SVMainFrame* ) AfxGetApp()->m_pMainWnd )->m_pregTestDlg = nullptr;

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

	SVSendMessage( GetInspectionProcess(), SVM_GOING_OFFLINE, 0, 0 ); // Mainly used to tell Archive Tool to shutdown
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
	if( nullptr != GetToolSet() )
	{
		l_Status = GetToolSet()->IsToolPreviousToSelected( p_rToolID );
	}

	return l_Status;
}

CString SVIPDoc::GetCompleteToolSetName() const
{
	CString l_Name;

	SVToolSetClass* l_pToolSet = GetToolSet();

	if( nullptr != l_pToolSet )
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
		SVString inspectionName = pInspection->GetName();
		const SVGUID& inspectionID = pInspection->GetUniqueObjectID();
		CString strTitle = _T("Data Definition Lists - ");
		strTitle += inspectionName.c_str();
		SVDataDefinitionSheet sheet(this, strTitle, inspectionName, inspectionID);

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

				if( ( nullptr != l_pImage ) )
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

	if( nullptr != l_pImage )
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
	if (S_OK == l_Status ) { SetModifiedFlag(); }
	return l_Status;
}

HRESULT SVIPDoc::UpdateExtentsToFit( SVTaskObjectClass* p_pTask, const SVImageExtentClass& p_rExtents )
{
	HRESULT l_Status = SVGuiExtentUpdater::SetImageExtentToFit(p_pTask, p_rExtents);
	if (S_OK == l_Status ) { SetModifiedFlag(); }
	return l_Status;
}

SVImageClass* SVIPDoc::GetImageByName( CString& p_imageName ) const
{
	SVImageClass* l_pImage = nullptr;
	SVInspectionProcess* pInspection = GetInspectionProcess();

	if( nullptr != pInspection )
	{
		if( IsColorInspectionDocument() && nullptr != pInspection->GetHSIMainImage() )
		{
			if( p_imageName == pInspection->GetHSIMainImage()->GetCompleteObjectName() )
			{
				l_pImage = pInspection->GetHSIMainImage();
			}
		}

		if( nullptr == l_pImage )
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
	BOOL l_Status = ( nullptr != pInspection );

	if( l_Status )
	{
		SVGUID l_ToolId;

		if( nullptr != p_pTool ) { l_ToolId = p_pTool->GetUniqueObjectID(); }

		GuiCmd::InspectionRunOncePtr l_CommandPtr = new GuiCmd::InspectionRunOnce( pInspection->GetUniqueObjectID(), l_ToolId );
		SVObjectSynchronousCommandTemplate< GuiCmd::InspectionRunOncePtr > l_Command( pInspection->GetUniqueObjectID(), l_CommandPtr );

		l_Status = ( S_OK == l_Command.Execute( TWO_MINUTE_CMD_TIMEOUT ) );
	}

	return l_Status;
}

int SVIPDoc::GetSelectedToolIndex(const CString& toolName) const
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
			if (nullptr != pTool && pTool->GetName() == toolName)
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

