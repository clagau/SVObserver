//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVIODoc
//* .File Name       : $Workfile:   sviodoc.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.3  $
//* .Check In Date   : $Date:   01 Oct 2013 14:31:02  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
//Moved to precompiled header: #include <comdef.h>

#include "SVIODoc.h"
#include "SVIOLibrary/SVIOConfigurationInterfaceClass.h"
#include "SVUtilityLibrary\SVGUID.h"
#include "SVObserver.h"
#include "SVObjectLibrary\SVObjectManagerClass.h"
#include "SVInfoStructs.h"
#include "SVValueObjectLibrary/SVValueObject.h"
#include "SVConfigurationObject.h"
#include "SVOutputObjectList.h"
#include "SVInputObjectList.h"
#include "SVMFCControls\SVRemoteInputDialog.h"
#include "SVUtilities.h"
#include "SVRemoteInputObject.h"
#include "SVMessage\SVMessage.h"
#include "SVSystemLibrary/SVFileNameManagerClass.h"
#include "SVStatusLibrary/SVSVIMStateClass.h"
#include "SVXMLLibrary\SVNavigateTree.h"
#include "SVXMLLibrary\SVConfigurationTags.h"
#include "GlobalConstantView.h"
#include "SVMainFrm.h"
#include "ObjectInterfaces\SVUserMessage.h"
#include "SVIOController.h"
#include "SVStatusLibrary/ErrorNumbers.h"
#include "TextDefinesSvO.h"
#include "SVStatusLibrary\MessageManager.h"
#include "SVUtilityLibrary/SVString.h"
#pragma endregion Includes

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// SVIODoc IOFunc Access Macros
// X - isOutput
// Y - isModuleIO
#define SV_GET_IODOC_INPUTFUNC( X, Y )		( ( X ) ? nullptr : ( ( Y ) ? pGetModuleInput : pGetResultInput ) )
#define SV_GET_IODOC_OUTPUTFUNC( X, Y )		( ( X ) ? ( ( Y ) ? pSetModuleOutput : pSetResultOutput ) : nullptr )
#define SV_GET_IODOC_FORCEFUNC( X, Y )		( ( X ) ? ( ( Y ) ? pForceModuleOutput : pForceResultOutput ) : ( ( Y ) ? pForceModuleInput : pForceResultInput ) )
#define SV_GET_IODOC_INVERTFUNC( X, Y )		( ( X ) ? ( ( Y ) ? pInvertModuleOutput : pInvertResultOutput ) : ( ( Y ) ? pInvertModuleInput : pInvertResultInput ) )

IMPLEMENT_DYNCREATE( SVIODoc, CDocument );

BEGIN_MESSAGE_MAP(SVIODoc, CDocument)
	//{{AFX_MSG_MAP(SVIODoc)
	ON_COMMAND(ID_EXTRAS_TESTOUTPUTS, &SVIODoc::OnExtrasTestoutputs)
	ON_COMMAND(ID_EDIT_EDITREMOTEINPUTS, &SVIODoc::OnExtrasEditRemoteInputs)
	ON_COMMAND(ID_EDIT_GLOBAL_CONSTANT_ADD, &SVIODoc::OnEditGlobalConstantAdd)
	ON_UPDATE_COMMAND_UI(ID_EDIT_GLOBAL_CONSTANT_ADD,  &GlobalConstantView::OnUpdateAddItem) //Note that this calls the Global View method directly
	ON_UPDATE_COMMAND_UI(ID_APP_EXIT, &SVIODoc::OnUpdateFileExit)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// SVIODoc Konstruktion/Destruktion

SVIODoc::SVIODoc()
: CDocument()
{
	m_pIOController = nullptr;

	SVFileNameManagerClass::Instance().AddItem( &msvFileName );
}

SVIODoc::~SVIODoc()
{
	SVFileNameManagerClass::Instance().RemoveItem( &msvFileName );

	m_pIOController = nullptr;
}

BOOL SVIODoc::OnNewDocument()
{
	BOOL bOk = CDocument::OnNewDocument();

	if ( bOk )
	{
		SetPathName( "unknown.iod" );
	}

	return bOk;
}

void SVIODoc::OnCloseDocument()
{
	if ( ! SVSVIMStateClass::CheckState( SV_STATE_CLOSING | SV_STATE_CANCELING ) )
	{
		return;
	}

	CDocument::OnCloseDocument();
}

void SVIODoc::CloseDocument()
{
	OnFileClose();
}

void SVIODoc::updateGlobalConstantsView() const
{
	POSITION Pos( GetFirstViewPosition() );

	while( nullptr != Pos )
	{
		GlobalConstantView* pView = dynamic_cast<GlobalConstantView*> (GetNextView( Pos ));
		if( nullptr != pView )
		{
			pView->updateView();
			break;
		}
	}

}

/////////////////////////////////////////////////////////////////////////////
// SVIODoc Diagnose

#ifdef _DEBUG
void SVIODoc::AssertValid() const
{
	CDocument::AssertValid();
}

void SVIODoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// SVIODoc Befehle

void SVIODoc::SetPathName( LPCTSTR lpszPathName, BOOL bAddToMRU )
{
	msvFileName.SetFullFileName( lpszPathName );

	SVFileNameManagerClass::Instance().LoadItem( &msvFileName );

	// Never add to MRU file list! 
	CDocument::SetPathName( msvFileName.GetFullFileName().c_str(), FALSE );
}

void SVIODoc::SetTitle(LPCTSTR lpszTitle)
{
	CDocument::SetTitle(lpszTitle);
}

BOOL SVIODoc::SaveModified()
{
	if ( SVSVIMStateClass::CheckState( SV_STATE_CANCELING ) )
	{
		return TRUE;
	}

	int index = 0;
	if( ( index = m_strTitle.Find( _TCHAR( '.' ) ) ) >= 0 )
		m_strTitle = m_strTitle.Left( index );
	
	return CDocument::SaveModified();
}

BOOL SVIODoc::CanCloseFrame(CFrameWnd* pFrame)
{
	BOOL bCanClose = FALSE;
		
	if( TheSVObserverApp.m_svSecurityMgr.SVIsDisplayable( SECURITY_POINT_MODE_MENU_EDIT_TOOLSET ) )
	{
		if( SVSVIMStateClass::CheckState( SV_STATE_CANCELING ) || SVSVIMStateClass::CheckState( SV_STATE_CLOSING ) )
		{
			bCanClose = CDocument::CanCloseFrame(pFrame);
		}
	}
	
	if (!bCanClose)
	{
		SvStl::MessageMgrStd Msg( SvStl::LogAndDisplay );
		Msg.setMessage( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_SVIODoc_ClosingImpossible, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_10054 );
	}

	return bCanClose;
}

void SVIODoc::OnExtrasTestoutputs()
{
	SVSVIMStateClass::AddState( SV_STATE_EDITING );
	if( S_OK == TheSVObserverApp.m_svSecurityMgr.SVValidate(SECURITY_POINT_EXTRAS_MENU_TEST_OUTPUTS) )
	{
		if ( ! SVSVIMStateClass::CheckState( SV_STATE_RUNNING | SV_STATE_TEST ) )
		{
			SVIOConfigurationInterfaceClass::Instance().TestDigitalOutputs();
		}
	}
	SVSVIMStateClass::RemoveState( SV_STATE_EDITING );
}

void SVIODoc::OnExtrasEditRemoteInputs()
{
	SvMc::SVRemoteInputDialog oDlg;
	SVPPQObject* pPPQ( nullptr );
	SVRemoteInputObject* pRemoteInput ( nullptr );
	SVInputObjectList* pInputList( nullptr );
	SVIOEntryHostStructPtrVector ppIOEntries;
	SVIOEntryHostStructPtr pIOEntry;
	long lCount;
	int i;
	int j;
	int k;

	SVConfigurationObject* pConfig = nullptr;
	SVObjectManagerClass::Instance().GetConfigurationObject( pConfig );

	ASSERT( nullptr != pConfig );
	if( nullptr != pConfig )
	{
		pInputList = pConfig->GetInputObjectList( );
	}
	if( nullptr != pInputList )
	{
		if( !pInputList->FillInputs( ppIOEntries ) )
		{
			SvStl::MessageMgrStd e( SvStl::LogOnly );
			e.setMessage( SVMSG_SVO_55_DEBUG_BREAK_ERROR, SvStl::Tid_ErrorFillingInputs, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_17032_ErrorFillingInputs );
			DebugBreak();
		}
		long lPPQSize = pConfig->GetPPQCount( );
		long lSize = static_cast<long>(ppIOEntries.size());

		pInputList->GetRemoteInputCount( lCount );

		SVSVIMStateClass::AddState( SV_STATE_EDITING );
		// Show Dialog...
		oDlg.m_lRemoteInputCount = lCount;
		if( IDOK == oDlg.DoModal() )
		{
			SVSVIMStateClass::AddState( SV_STATE_MODIFIED );
			if( oDlg.m_lRemoteInputCount >= lCount )
			{
				// Add new ones until we have enough
				for( j = lCount; j < oDlg.m_lRemoteInputCount; j++ )
				{
					pRemoteInput = nullptr;

					SVString RemoteInputName = SvUl_SF::Format( _T("Remote Input %d"), j + 1 );

					pRemoteInput = dynamic_cast<SVRemoteInputObject*> (pInputList->GetInputFlyweight( RemoteInputName, SVRemoteInputObjectType));

					if( nullptr != pRemoteInput )
					{
						pRemoteInput->m_lIndex = j + 1;
						pRemoteInput->Create();

						for( k = 0; k< lPPQSize; k++ )
						{
							pPPQ = pConfig->GetPPQ( k );
							if( nullptr != pPPQ )
							{
								pIOEntry = new SVIOEntryHostStruct;
								SVVariantValueObjectClass* pValueObject = new SVVariantValueObjectClass;

								if( !( pIOEntry.empty() ) && pValueObject )
								{
									pValueObject->SetName( RemoteInputName.c_str() );
									pValueObject->SetObjectOwner(GetIOController());
									pValueObject->setResetOptions( false, SvOi::SVResetItemNone );
									pValueObject->setBucketized(true);
									pIOEntry->m_IOId = pRemoteInput->GetUniqueObjectID();
									pIOEntry->m_Enabled = FALSE;
									pIOEntry->m_ObjectType = IO_REMOTE_INPUT;
									pIOEntry->setObject(dynamic_cast<SVObjectClass*> (pValueObject));

									pPPQ->AddInput( pIOEntry );
								}// end if
							}// end if
						}// end for
					}// end if
				}// end for
			}// end if
			else
			{
				// Remove all the extra ones
				j = oDlg.m_lRemoteInputCount;
				for( i = 0; i < lSize; i++ )
				{
					SVString RemoteInputName = SvUl_SF::Format( _T("Remote Input %d"), j + 1 );

					bool bFound = false;

					for ( int iRI = 0; (iRI < lSize && !bFound); iRI++ )
					{
						pIOEntry = ppIOEntries[iRI];

						if ( pIOEntry->m_ObjectType != IO_REMOTE_INPUT )
							continue;

						SVObjectClass* pObject = SVObjectManagerClass::Instance().GetObject( pIOEntry->m_IOId );
						if ( !pObject )
						{
							continue; //item already removed.  move on to the next one.
						}

						if( RemoteInputName == pObject->GetName() )
						{
							bFound = true;
							pRemoteInput = dynamic_cast< SVRemoteInputObject* >( pObject );

							if( S_OK != pInputList->DetachInput( pRemoteInput->GetUniqueObjectID() ) )
							{
								SvStl::MessageMgrStd e( SvStl::LogOnly );
								e.setMessage( SVMSG_SVO_55_DEBUG_BREAK_ERROR, SvStl::Tid_ErrorDetachingInput, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_17033_ErrorDetachingInput );
								DebugBreak();
							}

							for( k = 0; k < lPPQSize; k++ )
							{
								pPPQ = pConfig->GetPPQ( k );
								if( nullptr != pPPQ ) { pPPQ->RemoveInput( pIOEntry ); }
							}// end for

							pIOEntry.clear();

							delete pRemoteInput;
							j++;
						}// end if
					}// for
				}// end for
			}// end else

			pConfig->RebuildInputOutputLists();

			UpdateAllViews( nullptr );
		}// end if
		SVSVIMStateClass::RemoveState( SV_STATE_EDITING );

		pInputList->GetRemoteInputCount( lCount );
		if( lCount > 0 )
		{
			TheSVObserverApp.ShowIOTab( SVIORemoteInputsViewID );
		}
		else
		{
			TheSVObserverApp.SetActiveIOTabView( SVIODiscreteInputsViewID );
			SVMainFrame* pWndMain = (SVMainFrame*)TheSVObserverApp.GetMainWnd();
			pWndMain->PostMessage( SV_IOVIEW_HIDE_TAB, SVIORemoteInputsViewID );
		}
	}// end if
}// end OnExtrasEditRemoteInputs

void SVIODoc::OnEditGlobalConstantAdd()
{
	POSITION Pos( GetFirstViewPosition() );

	while( nullptr != Pos )
	{
		GlobalConstantView* pView = dynamic_cast<GlobalConstantView*> (GetNextView( Pos ));
		if( nullptr != pView )
		{
			pView->OnAddItem();
			break;
		}
	}
}

void SVIODoc::InitMenu()
{
	// Load Utilities Menu
	SVUtilitiesClass util;
	CWnd* pWindow;
	CMenu* pMenu;

	pWindow = AfxGetMainWnd();
	pMenu = pWindow->GetMenu();

	if( pMenu = util.FindSubMenuByName(pMenu, _T("&Utilities") ) )
	{
		util.LoadMenu(pMenu);
	}
}

void SVIODoc::OnUpdateFileExit( CCmdUI* pCmdUI )
{
	pCmdUI->Enable( ! SVSVIMStateClass::CheckState( SV_STATE_RUNNING ) );
}

CFile* SVIODoc::GetFile( LPCTSTR lpszFileName, UINT nOpenFlags, CFileException* pError )
{
	UINT nNewFlags = nOpenFlags & ~( CFile::shareDenyRead | CFile::shareDenyWrite ) |
	                              CFile::shareDenyNone;

	return CDocument::GetFile( lpszFileName, nNewFlags, pError );
}

BOOL SVIODoc::OnOpenDocument(LPCTSTR lpszPathName)
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
		bOk = SVFileNameManagerClass::Instance().LoadItem( &msvFileName );
	}

	if ( bOk )
	{
		bOk = CDocument::OnOpenDocument( msvFileName.GetFullFileName().c_str() );
	}

	return bOk;
}

BOOL SVIODoc::OnSaveDocument(LPCTSTR lpszPathName)
{

	BOOL bOk = msvFileName.SetFullFileName( lpszPathName );

	if ( bOk && 0 != SvUl_SF::CompareNoCase( msvFileName.GetPathName(),SVFileNameManagerClass::Instance().GetRunPathName() ) )
	{
		bOk = msvFileName.SetPathName( SVFileNameManagerClass::Instance().GetRunPathName().c_str() );
	}

	if ( bOk && 0 != SvUl_SF::CompareNoCase( msvFileName.GetExtension(), SVString(_T(".iod") ) ) )
	{
		bOk = msvFileName.SetExtension( _T(".iod") );
	}
	
	if ( bOk )
	{
		bOk = CDocument::OnSaveDocument( msvFileName.GetFullFileName().c_str() );
	}
	
	if ( bOk )
	{
		bOk = SVFileNameManagerClass::Instance().SaveItem( &msvFileName );

		if ( bOk )
		{
			CDocument::SetPathName( msvFileName.GetFullFileName().c_str(), FALSE );
		}
	}

	return bOk;
}

SVIOController* SVIODoc::GetIOController() const
{
	return m_pIOController;
}

void SVIODoc::SetIOController(SVIOController* pController)
{
	m_pIOController = pController;
	pController->SetIODoc(this);
}

