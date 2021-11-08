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
#include "GuiHelpers.h"
#include "SVDiscreteInputsView.h"
#include "SVMultiDocTemplate.h"
#include "SVSecurity/SVSecurityManager.h"
#include "SVInfoStructs.h"
#include "SVIOController.h"
#include "SVIOTabbedView.h"
#include "SVConfigurationObject.h"
#include "SVUtilities.h"
#include "TextDefinesSvO.h"
#include "SVFileSystemLibrary/SVFileNameManagerClass.h"
#include "SVIOLibrary/SVIOConfigurationInterfaceClass.h"
#include "SVIOLibrary/SVInputObjectList.h"
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
		
	if( TheSecurityManager().SVIsDisplayable( SECURITY_POINT_MODE_MENU_EDIT_TOOLSET ) )
	{
		if( SVSVIMStateClass::CheckState( SV_STATE_CANCELING ) || SVSVIMStateClass::CheckState( SV_STATE_CLOSING ) )
		{
			bCanClose = CDocument::CanCloseFrame(pFrame);
		}
	}
	
	if (!bCanClose)
	{
		SvStl::MessageManager Msg(SvStl::MsgType::Log | SvStl::MsgType::Display );
		Msg.setMessage( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_SVIODoc_ClosingImpossible, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_10054 );
	}

	return bCanClose;
}

void SVIODoc::OnExtrasTestoutputs()
{
	SVSVIMStateClass::SetResetState stateEditing {SV_STATE_EDITING};
	if( S_OK == TheSecurityManager().SVValidate(SECURITY_POINT_EXTRAS_MENU_TEST_OUTPUTS) )
	{
		if ( ! SVSVIMStateClass::CheckState( SV_STATE_RUNNING | SV_STATE_TEST ) )
		{
			SVIOConfigurationInterfaceClass::Instance().TestDigitalOutputs();
		}
	}
}

void SVIODoc::OnExtrasEditRemoteInputs()
{
	SVInputObjectList* pInputList( nullptr );
	SVIOEntryHostStructPtr pIOEntry;

	SVConfigurationObject* pConfig = nullptr;
	SVObjectManagerClass::Instance().GetConfigurationObject( pConfig );

	assert( nullptr != pConfig );
	if( nullptr != pConfig )
	{
		pInputList = pConfig->GetInputObjectList( );
	}
	if( nullptr != pInputList )
	{
		SVIOEntryHostStructPtrVector inputEntryVector = pInputList->getInputList();

		long lPPQSize = pConfig->GetPPQCount( );
		long lSize = static_cast<long>(inputEntryVector.size());

		long count = pInputList->getRemoteInputCount();
		SvMc::SVRemoteInputDialog oDlg;
		oDlg.m_lRemoteInputCount = count;

		SVSVIMStateClass::SetResetState stateEditing {SV_STATE_EDITING};
		if( IDOK == oDlg.DoModal() )
		{
			SVSVIMStateClass::AddState( SV_STATE_MODIFIED );
			if(oDlg.m_lRemoteInputCount >= count)
			{
				// Add new ones until we have enough
				for(int i = count; i < oDlg.m_lRemoteInputCount; ++i )
				{
					std::string RemoteInputName = SvUl::Format(SvO::cRemoteInputNumberLabel, i + 1);

					SVRemoteInputObject* pRemoteInput = dynamic_cast<SVRemoteInputObject*> (pInputList->GetInputFlyweight( RemoteInputName, SvPb::SVRemoteInputObjectType, i).get());
					if( nullptr != pRemoteInput )
					{
						pRemoteInput->SetChannel(i + 1);

						for(int j = 0; j < lPPQSize; ++j )
						{
							SVPPQObject* pPPQ = pConfig->GetPPQ(j);
							if( nullptr != pPPQ )
							{
								pIOEntry = std::make_shared<SVIOEntryHostStruct>();
								std::shared_ptr<SvOi::IValueObject> pInputValueObject = std::make_shared<SvVol::SVVariantValueObjectClass>();

								if(nullptr != pIOEntry && nullptr != pInputValueObject)
								{
									pInputValueObject->setResetOptions(false, SvOi::SVResetItemNone);
									SVObjectClass* pObject = dynamic_cast<SVObjectClass*> (pInputValueObject.get());
									if(nullptr != pObject)
									{
										pObject->SetName(RemoteInputName.c_str());
										pObject->SetObjectOwner(GetIOController());
									}
									pIOEntry->m_IOId = pRemoteInput->getObjectId();
									pIOEntry->m_Enabled = FALSE;
									pIOEntry->m_ObjectType = IO_REMOTE_INPUT;
									pIOEntry->setValueObject(pInputValueObject);

									pPPQ->AddInput( pIOEntry );
								}// end if
							}// end if
						}// end for
					}// end if
				}// end for
			}// end if
			else
			{
				int remoteIndex = oDlg.m_lRemoteInputCount;
				// Remove all the extra ones
				for(int i = 0; i < lSize; ++i)
				{
					std::string RemoteInputName = SvUl::Format( SvO::cRemoteInputNumberLabel, remoteIndex + 1 );

					bool bFound = false;

					for ( int iRI = 0; (iRI < lSize && !bFound); iRI++ )
					{
						pIOEntry = inputEntryVector[iRI];

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

							for(int j = 0; j < lPPQSize; ++j )
							{
								SVPPQObject* pPPQ = pConfig->GetPPQ(j);
								if( nullptr != pPPQ ) { pPPQ->RemoveInput( pIOEntry ); }
							}// end for

							SVRemoteInputObject* pRemoteInput = dynamic_cast<SVRemoteInputObject*> (pObject);

							if (S_OK != pInputList->DetachInput(pRemoteInput->getObjectId()))
							{
								SvStl::MessageManager e(SvStl::MsgType::Log);
								e.setMessage(SVMSG_SVO_55_DEBUG_BREAK_ERROR, SvStl::Tid_ErrorDetachingInput, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_17033_ErrorDetachingInput);
								DebugBreak();
							}

							pIOEntry.reset();
							++remoteIndex;
						}// end if
					}// for
				}// end for
			}// end else

			pConfig->RebuildInputOutputLists();

			UpdateAllViews( nullptr );
		}// end if

		count = pInputList->getRemoteInputCount();
		if( count > 0 )
		{
			ShowIOTabIfPossible(SVIORemoteInputsViewID);
		}
		else
		{
			HideIOTabIfPossible(SVIORemoteInputsViewID);
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
	SVUtilities util;

	CWnd* pWindow = AfxGetMainWnd();
	CMenu* pMenu = (nullptr != pWindow) ? pWindow->GetMenu() : nullptr;
	pMenu = util.FindSubMenuByName(pMenu, _T("&Utilities"));
	if(nullptr != pMenu)
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

void SVIODoc::HideIOTab(DWORD dwID)
{
	POSITION lViewPos = GetFirstViewPosition();
	do
	{
		CView* pView = GetNextView(lViewPos);

		SVIOTabbedView* pIOView = dynamic_cast<SVIOTabbedView*>(pView->GetParentFrame());
		if (nullptr != pIOView)
		{
			TVisualObject* pCurrentTab = pIOView->m_Framework.GetActivePane();
			TVisualObject* pHideTab = pIOView->m_Framework.Get(dwID);
			//If tab to be hidden is active tab then select first visible tab
			if (pCurrentTab == pHideTab)
			{
				for (const auto tabID : IOTabViews)
				{
					TVisualObject* pTab = pIOView->m_Framework.Get(tabID);
					BOOL isVisible(false);
					if (nullptr != pTab && pHideTab != pTab && pTab->IsTabVisible(isVisible) && isVisible)
					{
						pIOView->m_Framework.SetActiveTab(pTab);
						pTab->SetActivePane();
						break;
					}
				}
			}
			pHideTab->ShowTab(false);
			break;
		}
	} while (lViewPos);
}


void SVIODoc::ShowIOTab(DWORD dwID)
{
	POSITION lViewPos = GetFirstViewPosition();
	do
	{
		CView* pView = GetNextView(lViewPos);

		SVIOTabbedView* pIOView = dynamic_cast<SVIOTabbedView*>(pView->GetParentFrame());
		if (nullptr != pIOView)
		{
			TVisualObject* pShowTab = pIOView->m_Framework.Get(dwID);
			if (nullptr != pShowTab)
			{
				pShowTab->ShowTab(true);
				pIOView->m_Framework.SetActiveTab(pShowTab);
				pShowTab->SetActivePane();
			}
			break;
		}
	} while (lViewPos);
}



SVIODoc* NewSVIODoc(LPCTSTR DocName, SVIOController& IOController)
{
	SVIODoc* pDoc = nullptr;
	CDocTemplate* pDocTemplate = nullptr;
	POSITION pos = AfxGetApp()->GetFirstDocTemplatePosition();
	if (pos)
	{
		pDocTemplate = AfxGetApp()->GetNextDocTemplate(pos);
		if (pDocTemplate)
		{
			pDocTemplate = AfxGetApp()->GetNextDocTemplate(pos);
			if (pDocTemplate)
			{
				// Create a new empty visible document
				pDoc = dynamic_cast<SVIODoc*>(pDocTemplate->OpenDocumentFile(nullptr, true));   // Make visible

				if (nullptr != pDoc)
				{
					pDoc->SetIOController(&IOController);
					pDoc->SetTitle(DocName);

					HideTabsAfterNewSVIODoc(GetTheIODoc());
				}
			}
		}
	}

	return pDoc;
}


SVIODoc* GetTheIODoc()
{
	SVIODoc* pIODoc(nullptr);

	POSITION pos = AfxGetApp()->GetFirstDocTemplatePosition();
	while (pos && !pIODoc)
	{
		CDocTemplate* pDocTemplate = AfxGetApp()->GetNextDocTemplate(pos);
		if (pDocTemplate)
		{
			POSITION posDoc = pDocTemplate->GetFirstDocPosition();
			while (posDoc && !pIODoc)
			{
				CDocument* newDoc = pDocTemplate->GetNextDoc(posDoc);
				if (newDoc)
				{
					SVIODoc* pTmpDoc = dynamic_cast <SVIODoc*>(newDoc);
					if (nullptr != pTmpDoc)
					{
						pIODoc = pTmpDoc;
					}
				}
			}
		}
	}
	return pIODoc;
}


void ShowIOTabIfPossible(DWORD dwID)
{
	auto pIODoc = GetTheIODoc();

	if (nullptr != pIODoc)
	{
		pIODoc->ShowIOTab(dwID);
	}
}

void HideIOTabIfPossible(DWORD dwID)
{
	auto pIODoc = GetTheIODoc();

	if (nullptr != pIODoc)
	{
		pIODoc->HideIOTab(dwID);
	}
}

CDocTemplate* CreateIoDocMultiDocTemplate()
{
	// IODoc
	return new SVMultiDocTemplate(IDR_SVOBSERVER_IODOCTYPE,
		RUNTIME_CLASS(SVIODoc),
		RUNTIME_CLASS(SVIOTabbedView),
		RUNTIME_CLASS(SVDiscreteInputsView));
}
