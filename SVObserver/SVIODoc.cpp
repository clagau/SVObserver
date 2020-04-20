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
#include "GlobalConstantView.h"
#include "SVMainFrm.h"
#include "SVObserver.h"
#include "SVInfoStructs.h"
#include "SVIOController.h"
#include "SVConfigurationObject.h"
#include "SVUtilities.h"
#include "TextDefinesSvO.h"
#include "Definitions/SVUserMessage.h"
#include "SVFileSystemLibrary/SVFileNameManagerClass.h"
#include "SVIOLibrary/SVIOConfigurationInterfaceClass.h"
#include "SVIOLibrary/SVInputObjectList.h"
#include "SVIOLibrary/SVOutputObjectList.h"
#include "SVIOLibrary/SVRemoteInputObject.h"
#include "SVMFCControls/SVRemoteInputDialog.h"
#include "SVMessage/SVMessage.h"
#include "SVObjectLibrary/SVObjectManagerClass.h"
#include "SVStatusLibrary/SVSVIMStateClass.h"
#include "SVValueObjectLibrary/SVVariantValueObjectClass.h"
#include "SVStatusLibrary/ErrorNumbers.h"
#include "SVStatusLibrary/MessageManager.h"
#include "SVUtilityLibrary/StringHelper.h"
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
}

SVIODoc::~SVIODoc()
{
	m_pIOController = nullptr;
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
		SvStl::MessageMgrStd Msg(SvStl::MsgType::Log | SvStl::MsgType::Display );
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
			SvStl::MessageMgrStd e(SvStl::MsgType::Log );
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

					std::string RemoteInputName = SvUl::Format(SvO::cRemoteInputNumberLabel, j + 1);

					pRemoteInput = dynamic_cast<SVRemoteInputObject*> (pInputList->GetInputFlyweight( RemoteInputName, SvPb::SVRemoteInputObjectType, j));

					if( nullptr != pRemoteInput )
					{
						pRemoteInput->m_lIndex = j + 1;
						pRemoteInput->Create();

						for( k = 0; k< lPPQSize; k++ )
						{
							pPPQ = pConfig->GetPPQ( k );
							if( nullptr != pPPQ )
							{
								pIOEntry = SVIOEntryHostStructPtr{ new SVIOEntryHostStruct };
								SvVol::SVVariantValueObjectClass* pValueObject = new SvVol::SVVariantValueObjectClass;

								if(nullptr != pIOEntry && nullptr != pValueObject )
								{
									pValueObject->SetName( RemoteInputName.c_str() );
									pValueObject->SetObjectOwner(GetIOController());
									pValueObject->setResetOptions( false, SvOi::SVResetItemNone );
									pIOEntry->m_IOId = pRemoteInput->getObjectId();
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
					std::string RemoteInputName = SvUl::Format( SvO::cRemoteInputNumberLabel, j + 1 );

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

							if( S_OK != pInputList->DetachInput( pRemoteInput->getObjectId() ) )
							{
								SvStl::MessageMgrStd e(SvStl::MsgType::Log );
								e.setMessage( SVMSG_SVO_55_DEBUG_BREAK_ERROR, SvStl::Tid_ErrorDetachingInput, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_17033_ErrorDetachingInput );
								DebugBreak();
							}

							for( k = 0; k < lPPQSize; k++ )
							{
								pPPQ = pConfig->GetPPQ( k );
								if( nullptr != pPPQ ) { pPPQ->RemoveInput( pIOEntry ); }
							}// end for

							pIOEntry.reset();

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

SVIOController* SVIODoc::GetIOController() const
{
	return m_pIOController;
}

void SVIODoc::SetIOController(SVIOController* pController)
{
	m_pIOController = pController;
	pController->SetIODoc(this);
}

