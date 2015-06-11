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
#include <comdef.h>
#include "SVIODoc.h"
#include "SVIOLibrary/SVIOConfigurationInterfaceClass.h"
#include "SVUtilityLibrary/SVGUID.h"
#include "SVObserver.h"
#include "SVObjectLibrary/SVObjectManagerClass.h"
#include "SVInfoStructs.h"
#include "SVValueObject.h"
#include "SVConfigurationObject.h"
#include "SVOutputObjectList.h"
#include "SVInputObjectList.h"
#include "SVMFCControls\SVRemoteInputDialog.h"
#include "SVUtilities.h"
#include "SVRemoteInputObject.h"
#include "SVMessage/SVMessage.h"
#include "SVFileNameManagerClass.h"
#include "SVSVIMStateClass.h"
#include "SVXMLLibrary/SVNavigateTreeClass.h"
#include "SVConfigurationLibrary/SVConfigurationTags.h"
#include "SVIOTabbedView.h"
#include "SVMainFrm.h"
#include "ObjectInterfaces/SVUserMessage.h"
#include "SVIOController.h"
#include "ObjectInterfaces\ErrorNumbers.h"
#include "SVStatusLibrary/ExceptionManager.h"
#include "TextDefinesSvO.h"
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
#define SV_GET_IODOC_INPUTFUNC( X, Y )		( ( X ) ? NULL : ( ( Y ) ? pGetModuleInput : pGetResultInput ) )
#define SV_GET_IODOC_OUTPUTFUNC( X, Y )		( ( X ) ? ( ( Y ) ? pSetModuleOutput : pSetResultOutput ) : NULL )
#define SV_GET_IODOC_FORCEFUNC( X, Y )		( ( X ) ? ( ( Y ) ? pForceModuleOutput : pForceResultOutput ) : ( ( Y ) ? pForceModuleInput : pForceResultInput ) )
#define SV_GET_IODOC_INVERTFUNC( X, Y )		( ( X ) ? ( ( Y ) ? pInvertModuleOutput : pInvertResultOutput ) : ( ( Y ) ? pInvertModuleInput : pInvertResultInput ) )



//******************************************************************************
//* FUNCTION IMPLEMENTATION(S):
//******************************************************************************

//******************************************************************************
//* CLASS METHOD IMPLEMENTATION(S):
//******************************************************************************


//*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/
//* Class Name : SVIODoc
//* Note(s)    : Document
//*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/

/////////////////////////////////////////////////////////////////////////////
// SVIODoc

IMPLEMENT_DYNCREATE( SVIODoc, CDocument );


BEGIN_MESSAGE_MAP(SVIODoc, CDocument)
	//{{AFX_MSG_MAP(SVIODoc)
	ON_COMMAND(ID_EXTRAS_TESTOUTPUTS, OnExtrasTestoutputs)
	ON_COMMAND(ID_EDIT_EDITREMOTEINPUTS, OnExtrasEditRemoteInputs)
	ON_UPDATE_COMMAND_UI(ID_APP_EXIT, OnUpdateFileExit)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// SVIODoc Konstruktion/Destruktion

SVIODoc::SVIODoc()
: CDocument()
{
	m_pIOController = NULL;

	SVFileNameManagerClass svFileManager;
	svFileManager.AddItem( &msvFileName );
}

SVIODoc::~SVIODoc()
{
	SVFileNameManagerClass svFileManager;
	svFileManager.RemoveItem( &msvFileName );

	m_pIOController = NULL;
}

void SVIODoc::InitIO()
{
	InitMenu ();

	UpdateAllViews( NULL );
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

	SVFileNameManagerClass svFileManager;
	svFileManager.LoadItem( &msvFileName );

	// Never add to MRU file list! 
	CDocument::SetPathName( msvFileName.GetFullFileName(), FALSE );
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
		AfxMessageBox(_T("Invalid Operation!\n")
						  _T("The IO Dialog can not be closed without\n")
						  _T("closing the whole configuration."), MB_OK|MB_ICONSTOP, 0);
		
	return bCanClose;
}

void SVIODoc::OnExtrasTestoutputs()
{
	SVSVIMStateClass::AddState( SV_STATE_EDITING );
	if( TheSVObserverApp.m_svSecurityMgr.SVValidate(SECURITY_POINT_EXTRAS_MENU_TEST_OUTPUTS)  == S_OK )
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
	SVRemoteInputObject* pRemInput ( nullptr );
	SVInputObjectList* pInputList( nullptr );
	SVIOEntryHostStructPtrList ppIOEntries;
	SVIOEntryHostStructPtr pIOEntry;
	SVValueObjectClass* pObject( nullptr );
	CString strName;
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
			SvStl::ExceptionMgr1 e; // The default constructor sets the type to LogOnly.
			e.setMessage( SVMSG_SVO_55_DEBUG_BREAK_ERROR, SvO::ErrorFillingInputs, StdExceptionParams, SvOi::Err_17032_ErrorFillingInputs );
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
					pRemInput = NULL;

					strName.Format( "Remote Input %d", j + 1 );

					pInputList->GetInputFlyweight( static_cast< LPCTSTR >( strName ), pRemInput );

					if( pRemInput != NULL )
					{
						pRemInput->m_lIndex = j + 1;
						pRemInput->Create();

						for( k = 0; k< lPPQSize; k++ )
						{
							pPPQ = pConfig->GetPPQ( k );
							if( nullptr != pPPQ )
							{
								pIOEntry = new SVIOEntryHostStruct;
								pObject = new SVVariantValueObjectClass;

								if( !( pIOEntry.empty() ) && pObject )
								{
									pObject->SetName( strName );
									pObject->SetResetOptions( false, SVResetItemNone );
									pIOEntry->m_IOId = pRemInput->GetUniqueObjectID();
									pIOEntry->m_Enabled = FALSE;
									pIOEntry->m_ObjectType = IO_REMOTE_INPUT;
									pIOEntry->m_pValueObject = pObject;
									pIOEntry->m_pValueParent = GetIOController();

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
					strName.Format( "Remote Input %d", j + 1 );

					BOOL bFound = FALSE;

					for ( int iRI = 0; (iRI < lSize && !bFound); iRI++ )
					{
						pIOEntry = ppIOEntries[iRI];

						if ( pIOEntry->m_ObjectType != IO_REMOTE_INPUT )
							continue;

						SVObjectClass* l_pObject = SVObjectManagerClass::Instance().GetObject( pIOEntry->m_IOId );
						if ( !l_pObject )
							continue; //item already removed.  move on to the next one.

						if( strName == l_pObject->GetName() )
						{
							bFound = TRUE;
							pRemInput = dynamic_cast< SVRemoteInputObject* >( l_pObject );

							if( pInputList->DetachInput( pRemInput->GetUniqueObjectID() ) != S_OK )
							{
								SvStl::ExceptionMgr1 e; // The default constructor sets the type to LogOnly.
								e.setMessage( SVMSG_SVO_55_DEBUG_BREAK_ERROR, SvO::ErrorDetachingInput, StdExceptionParams, SvOi::Err_17033_ErrorDetachingInput );
								DebugBreak();
							}

							for( k = 0; k < lPPQSize; k++ )
							{
								pPPQ = pConfig->GetPPQ( k );
								if( nullptr != pPPQ ) { pPPQ->RemoveInput( pIOEntry ); }
							}// end for

							pIOEntry.clear();

							delete pRemInput;
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

void SVIODoc::InitMenu()
{
	// Load Utilities Menu
	SVUtilitiesClass util;
	CWnd* pWindow;
	CMenu* pMenu;
	CString szMenuText;

	pWindow = AfxGetMainWnd();
	pMenu = pWindow->GetMenu();
	szMenuText = _T("&Utilities");

	if (pMenu = util.FindSubMenuByName(pMenu, szMenuText))
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
		SVFileNameManagerClass svFileManager;

		bOk = svFileManager.LoadItem( &msvFileName );
	}

	if ( bOk )
	{
		bOk = CDocument::OnOpenDocument( msvFileName.GetFullFileName() );
	}

	return bOk;
}

BOOL SVIODoc::OnSaveDocument(LPCTSTR lpszPathName)
{
	SVFileNameManagerClass svFileManager;

	BOOL bOk = msvFileName.SetFullFileName( lpszPathName );

	if ( bOk && CString( msvFileName.GetPathName() ).CompareNoCase( svFileManager.GetRunPathName() ) != 0 )
	{
		bOk = msvFileName.SetPathName( svFileManager.GetRunPathName() );
	}

	if ( bOk && CString( msvFileName.GetExtension() ).CompareNoCase( ".iod" ) != 0 )
	{
		bOk = msvFileName.SetExtension( ".iod" );
	}
	
	if ( bOk )
	{
		bOk = CDocument::OnSaveDocument( msvFileName.GetFullFileName() );
	}
	
	if ( bOk )
	{
		bOk = svFileManager.SaveItem( &msvFileName );

		if ( bOk )
		{
			CDocument::SetPathName( msvFileName.GetFullFileName(), FALSE );
		}
	}

	return bOk;
}

SVIOController* SVIODoc::GetIOController() const
{
	return m_pIOController;
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\sviodoc.cpp_v  $
 * 
 *    Rev 1.3   01 Oct 2013 14:31:02   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Add x64 platform.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   07 Aug 2013 13:29:44   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  809
 * SCR Title:  Non I/O SVIM
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Renamed SVIOEntryHostStructPtrVector to SVIOEntryHostStructPtrList.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   06 May 2013 20:06:20   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 010.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
      Rev 3.58   16 Apr 2013 14:24:16   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  822
   SCR Title:  Remove DDE Input and Output functionality from SVObserver
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Removed DDE functionality from the source code.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
 *    Rev 1.0   23 Apr 2013 11:09:08   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.57   11 Oct 2012 10:54:22   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  602
   SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated include information based on the navigate tree class moving to the XML library.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.56   18 Sep 2012 18:21:42   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  602
   SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated code to use the templated XML objects that moved from the SVLibrary to the new XML Library and to the Configuration Library.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.55   15 Aug 2012 14:37:44   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  779
   SCR Title:  Fix Problems with DDE Inputs and DDE Outputs
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated source code to use the new Singleton version of the SVIOConfigurationInterfaceClass.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.54   13 Aug 2012 14:27:04   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  779
   SCR Title:  Fix Problems with DDE Inputs and DDE Outputs
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated code to change the functionality associated with the Input Object List to use a new method to get the input from the input list.  If the input does not exist in the input list, a NULL pointer is returned.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.53   09 Jul 2012 14:03:52   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  769
   SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated source code to change the include for SVIM State.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.52   02 Jul 2012 16:58:44   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  769
   SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated source code to promote new display functionality.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.51   19 Jun 2012 12:54:34   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  744
   SCR Title:  Add Shared Memory and Socket Functionality for Run Page Web Server
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Optimized the application state functionality.  The application state class was altered to simplify its usage and elements were added to object manager to separate tracking states.
   Centralized the location of the configuration object identifier and removed synchronization problems.
   
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.50   19 Apr 2011 16:17:08   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  707
   SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated code to fix issues with IO Assignment.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.49   18 Apr 2011 10:24:16   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  707
   SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated code to move the shared data for the IO Entry data to a different class to fix issues with PPQ and Inspection shared data.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.48   16 Mar 2011 09:22:52   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  715
   SCR Title:  Change the reset functionality for Input Object in the Inspection Process
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated source to include change in reset requirements for input value objects.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.47   09 Mar 2011 13:48:54   ryoho
   Project:  SVObserver
   Change Request (SCR) nbr:  707
   SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     fixed issue with editing remote inputs did not cause the configuration to become dirty
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.46   09 Mar 2011 13:23:18   ryoho
   Project:  SVObserver
   Change Request (SCR) nbr:  707
   SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     fixed issues with remote inputs with the descriptions not matching up with the input name
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.45   27 Jan 2011 11:20:54   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  712
   SCR Title:  Fix issues with black images when using command interface (SIAC)
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated source code to fix issues with Matrox Buffer Management.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.44   15 Dec 2010 10:00:12   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  707
   SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated source code to fix connections and synchronization issues with the IO Sub-system.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.43   13 Dec 2010 11:23:18   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  707
   SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Change the usage of _variant_t to clear the variable after use to eliminate invalid variant type for assignment.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.42   08 Dec 2010 12:52:00   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  707
   SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated source code to include changes in notification functionality using the Observer Design Pattern.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.41   05 Nov 2010 11:02:24   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  703
   SCR Title:  Allow SVObserver to Change Remote Input Values
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated source code to remove redundent data objects.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.40   21 Oct 2010 11:02:20   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  703
   SCR Title:  Allow SVObserver to Change Remote Input Values
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated source code to change internal value object to variant value object from double value object.  Added persistance to configuration object.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.39   01 Jun 2010 10:43:56   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  693
   SCR Title:  Fix Performance Issue with Inspection Process
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated code to remove redundent methodologies and fix missing or incorrect calling functionality.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.38   16 Dec 2009 11:08:06   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  677
   SCR Title:  Fix problem in camera notify thread
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Fix issues with includes and comments.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.37   30 Jul 2009 11:56:26   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  665
   SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Merged branched changes into current code branch with appropriate updates.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.36   14 Nov 2008 14:19:08   tbair
   Project:  SVObserver
   Change Request (SCR) nbr:  615
   SCR Title:  Integrate PLC Classes into SVObserver Outputs
   Checked in by:  tBair;  Tom Bair
   Change Description:  
     Added PLC
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.35.1.0   14 Jul 2009 14:31:14   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  665
   SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated code with new data manager objects and image methods.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.35   09 Apr 2007 11:54:50   ryoho
   Project:  SVObserver
   Change Request (SCR) nbr:  588
   SCR Title:  Add the RAID Error Bit for the digital IO
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
     check to determine if the product has the raid bit
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.34   22 Jan 2007 10:35:44   ryoho
   Project:  SVObserver
   Change Request (SCR) nbr:  588
   SCR Title:  Add the RAID Error Bit for the digital IO
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
     Added the new RaidErrorBit 
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.33   22 Aug 2005 11:16:04   tbair
   Project:  SVObserver
   Change Request (SCR) nbr:  455
   SCR Title:  New Security for SVObserver
   Checked in by:  tBair;  Tom Bair
   Change Description:  
     Changed name of function SVIsAllowed to SVIsDisplayable.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.32   28 Jul 2005 16:11:00   tbair
   Project:  SVObserver
   Change Request (SCR) nbr:  455
   SCR Title:  New Security for SVObserver
   Checked in by:  tBair;  Tom Bair
   Change Description:  
     Updated GUI Editing Flag to block mode changes from SIAC
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.31   21 Jun 2005 13:07:36   tbair
   Project:  SVObserver
   Change Request (SCR) nbr:  455
   SCR Title:  New Security for SVObserver
   Checked in by:  tBair;  Tom Bair
   Change Description:  
     Initial changes to add SVSecurity
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.30   17 Feb 2005 14:32:34   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  456
   SCR Title:  Update Image and Tool Objects to use the new Extent Classes
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Update class with new methodologies based on new base object, extents and reset structure.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.29   17 Feb 2005 08:25:32   ryoho
   Project:  SVObserver
   Change Request (SCR) nbr:  440
   SCR Title:  Create Internal Tool Object to Managing Tool Extents and Result Extents
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
     updated header files
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.28   17 Dec 2003 14:14:18   rschock
   Project:  SVObserver
   Change Request (SCR) nbr:  381
   SCR Title:  Combine PPQ Data to Inspection Data for a Digital Output
   Checked in by:  rSchock;  Rosco Schock
   Change Description:  
     Fixed Trigger Toggle and other outputs that are written at times different than the rest of the outputs so that they no longer conflict.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.27   18 Aug 2003 15:48:02   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  322
   SCR Title:  Add Additional Digital IO Resources to SVObserver
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
     Removed SVDDEControlClass and SVDigitalIOControlClass from class.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.26   08 Jul 2003 12:03:52   ebeyeler
   Project:  SVObserver
   Change Request (SCR) nbr:  322
   SCR Title:  Add Additional Digital IO Resources to SVObserver
   Checked in by:  eBeyeler;  Eric Beyeler
   Change Description:  
     changed OnExtrasTestoutputs to use DLL IO interface
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.25   22 Apr 2003 10:31:22   rschock
   Project:  SVObserver
   Change Request (SCR) nbr:  346
   SCR Title:  Update SVObserver to Version 4.21 Release
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.24   19 Mar 2003 15:20:32   rschock
   Project:  SVObserver
   Change Request (SCR) nbr:  332
   SCR Title:  Module Ready not available after being unassigned and other small I/O bugs
   Checked in by:  eBeyeler;  Eric Beyeler
   Change Description:  
     Fixed some problems with input/outputs and the Go online/Go offline process. Also, made changes to force the PPQ to always have valid inputs and outputs even without an inspection attached.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.23   14 Mar 2003 17:03:26   rschock
   Project:  SVObserver
   Change Request (SCR) nbr:  332
   SCR Title:  Module Ready not available after being unassigned and other small I/O bugs
   Checked in by:  rSchock;  Rosco Schock
   Change Description:  
     Fixed bug with how remote inputs are created.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.22   13 Jan 2003 12:58:06   rschock
   Project:  SVObserver
   Change Request (SCR) nbr:  307
   SCR Title:  Implement the ability to remotely change Tool parameters on the SVIM
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
     Added new code to fully enable Remote Inputs in the system.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.21   19 Nov 2002 14:37:30   ryoho
   Project:  SVObserver
   Change Request (SCR) nbr:  272
   SCR Title:  Outputs do not always get set correctly.
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
     file completely changed
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.18   12 Jun 2001 15:25:56   ed
   Project:  SVObserver
   Change Request (SCR) nbr:  200
   SCR Title:  Update code to implement SVIM COM Server
   Checked in by:  Ed;  Ed Chobanoff
   Change Description:  
     Modified the following functions: OnCloseDocument, SaveModified, CanCloseFrame, OnExtrasTestoutputs.  Added the following function, GetFile.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.17   25 Jan 2001 16:17:46   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  191
   SCR Title:  Restructure File Management
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     These changes fixed the SVObserverApp file handling and associated systems.  These systems include the .SEC, .IOD, and .IPD files as well as the camera files.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.16.1.0   11 May 2001 09:12:02   rschock
   Project:  SVObserver
   Change Request (SCR) nbr:  197
   SCR Title:  Fix lock-ups in application when loading configuration from SVFocusNT
   Checked in by:  rSchock;  Rosco Schock
   Change Description:  
     Changed SVObserver state code in
   OnCloseDocument
   SaveModified
   CanCloseFrame
   OnExtrasTestoutputs
   
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.16   07 May 2000 23:29:52   Jim
   Project:  SVObserver
   Change Request (SCR) nbr:  137
   SCR Title:  Remove the need to explicitly reference specific version numbers.
   Checked in by:  JimAdmin;  James A. Brown
   Change Description:  
     Changed the Serialization () functions so that versioning does not need to be explicitly addressed unless a change actually affects file compatabillity.
   
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.15   05 May 2000 13:43:48   sjones
   Project:  SVObserver
   Change Request (SCR) nbr:  134
   SCR Title:  Restricted Operator Security level
   Checked in by:  sJones;  Steve Jones
   Change Description:  
     Added OnUpdateFileExit message handler so exit menu item
   will be disabled for Restricted Operator.
   Revised CanCloseFrame to restrict closing for the Restricted Operator.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.14   19 Apr 2000 16:35:42   Jim
   Project:  SVObserver
   Change Request (SCR) nbr:  128
   SCR Title:  Suport for 3.11 versioning
   Checked in by:  JimAdmin;  James A. Brown
   Change Description:  
     SVIODoc::Serialize () was changed to support program version number 3.11 beta 1.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.13   11 Apr 2000 18:01:18   Jim
   Project:  SVObserver
   Change Request (SCR) nbr:  121
   SCR Title:  Support for 3.06 versioning
   Checked in by:  JimAdmin;  James A. Brown
   Change Description:  
     Changed SVIODoc::Serialize () to support program version number 3.06.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.12   14 Mar 2000 14:42:52   sjones
   Project:  SVObserver
   Change Request (SCR) nbr:  101
   SCR Title:  Versioning 3.10
   Checked in by:  sJones;  Steve Jones
   Change Description:  
     Revised version number to 3.10 release (0x00030AFF)
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.11   09 Mar 2000 08:52:32   sjones
   Project:  SVObserver
   Change Request (SCR) nbr:  101
   SCR Title:  Versioning 3.10
   Checked in by:  sJones;  Steve Jones
   Change Description:  
     Revised Version number from 3.10 Beta 3 to 3.10 Beta 4
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.10   07 Mar 2000 14:06:50   sjones
   Project:  SVObserver
   Change Request (SCR) nbr:  101
   SCR Title:  Versioning 3.10
   Checked in by:  sJones;  Steve Jones
   Change Description:  
     Changed Version number to 3.1 Beta 3.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.9   Feb 21 2000 10:50:00   Nick
   Project:  SVObserver
   Change Request (SCR) nbr:  109
   SCR Title:  Freeze on Reject for v3.1
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
     Versioning to v3.10 Beta 2.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.8   04 Feb 2000 13:31:32   sjones
   Project:  SVObserver
   Change Request (SCR) nbr:  101
   SCR Title:  Versioning 3.10
   Checked in by:  sJones;  Steve Jones
   Change Description:  
     Changed version to 3.10
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.7   Feb 03 2000 16:33:18   Nick
   Project:  SVObserver
   Change Request (SCR) nbr:  102
   SCR Title:  Versioning v3.05
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
     Changed version from v3.04 to v3.05.  Corrected multiple inheritance order in System.h.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.6   Jan 19 2000 15:18:54   Nick
   Project:  SVObserver
   Change Request (SCR) nbr:  88
   SCR Title:  Version 3.04 versioning changes.
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
     Changes required to change v3.04 Beta 1 to v3.04 Released.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.5   Jan 14 2000 15:15:06   Nick
   Project:  SVObserver
   Change Request (SCR) nbr:  85
   SCR Title:  Version 3.04 Beta 1 versioning changes.
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
     Changes required for v3.03 to v3.04 Beta 1.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.4   Jan 04 2000 08:58:30   Nick
   Project:  SVObserver
   Change Request (SCR) nbr:  81
   SCR Title:  Version 3.03 versioning changes
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
     Changes required to convert v3.02 to v3.03.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.3   Dec 02 1999 08:36:34   Nick
   Project:  SVObserver
   Change Request (SCR) nbr:  72
   SCR Title:  Version 3.02 versioning changes.
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
     Change v3.01 to v3.02.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.2   Dec 02 1999 08:32:36   Nick
   Project:  SVObserver
   Change Request (SCR) nbr:  57
   SCR Title:  Version 3.00 Beta 18 Versioning
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
     Changed v3.01 to v3.02.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.1   Nov 30 1999 15:48:08   Nick
   Project:  SVObserver
   Change Request (SCR) nbr:  67
   SCR Title:  Version 3.01 versioning changes.
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
     Changes for v3.00 to v3.01 versioning.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.0   23 Nov 1999 12:55:16   mike
   Project:  SVObserver
   Change Request (SCR) nbr:  61
   SCR Title:  Update PVCS versioning to version 3.0.
   Checked in by:  Mike;  Mike McCarl
   Change Description:  
     
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.29   18 Nov 1999 12:43:56   mike
   Project:  SVObserver
   Change Request (SCR) nbr:  60
   SCR Title:  Create 'Utilities' menu option
   Checked in by:  Mike;  Mike McCarl
   Change Description:  
     Modified initialization function to load the 'Utility' menu.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.28   Nov 18 1999 09:05:42   Nick
   Project:  SVObserver
   Change Request (SCR) nbr:  59
   SCR Title:  Version 3.00 Release Versioning
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
     v3.00 Release versioning changes.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.27   Nov 10 1999 12:21:04   Nick
   Project:  SVObserver
   Change Request (SCR) nbr:  57
   SCR Title:  Version 3.00 Beta 18 Versioning
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
     Changes v3.00 Beta 17 to v3.00 Beta 18.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.26   Nov 05 1999 11:18:30   Nick
   Project:  SVObserver
   Change Request (SCR) nbr:  56
   SCR Title:  Version 3.00 Beta 17 Versioning
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
     v3.00 Beta 16 to v3.00 Beta 17.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.25   Nov 04 1999 15:11:30   Nick
   Project:  SVObserver
   Change Request (SCR) nbr:  15
   SCR Title:  Integrate archive tool
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
     Changes to eliminate a configuration restore on a configuration save operation.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.24   Nov 02 1999 08:59:04   Nick
   Project:  SVObserver
   Change Request (SCR) nbr:  55
   SCR Title:  Version 3.00 Beta 16 Versioning
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
     Changes required for v3.00 Beta 15 to v3.00 Beta 16 change.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.23   Oct 29 1999 17:43:48   Nick
   Project:  SVObserver
   Change Request (SCR) nbr:  54
   SCR Title:  Version 3.00 Beta 15 Versioning.
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
     v3.00 Beta 14 to v3.00 Beta 15.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.22   Oct 26 1999 14:57:52   Nick
   Project:  SVObserver
   Change Request (SCR) nbr:  52
   SCR Title:  Version 3.00 Beta 14 Versioning
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
     v3.00 Beta 13 to v3.00 Beta 14.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.21   Oct 22 1999 09:01:58   Nick
   Project:  SVObserver
   Change Request (SCR) nbr:  51
   SCR Title:  Version 3.00 Beta 13 Versioning
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
     v3.00 Beta 12 to v3.00 Beta 13.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.20   Oct 20 1999 16:06:46   Nick
   Project:  SVObserver
   Change Request (SCR) nbr:  50
   SCR Title:  Version 3.00 Beta 12 Versioning
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
     Changed version from v3.00 Beta 11 to v3.00 Beta 12.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.19   Oct 18 1999 12:50:02   Nick
   Project:  SVObserver
   Change Request (SCR) nbr:  47
   SCR Title:  Version 3.00 Beta 11 Versioning
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
     Changes for v3.00 Beta 10 to v3.00 Beta 11.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.18   Oct 13 1999 15:37:14   Nick
   Project:  SVObserver
   Change Request (SCR) nbr:  46
   SCR Title:  Version 3.00 Beta 10 Versioning
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
     Version v3.00 Beta 9 to v3.00 Beta 10 changes.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.17   Oct 12 1999 08:16:46   Nick
   Project:  SVObserver
   Change Request (SCR) nbr:  44
   SCR Title:  Version 3.00 Beta 9 Versioning
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
     Changes required to change v3.00 Beta 8 to v3.00 Beta 9.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.16   Oct 08 1999 10:54:46   Nick
   Project:  SVObserver
   Change Request (SCR) nbr:  43
   SCR Title:  Version 3.00 Beta 7 Versioning
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
     Versioning changes to Beta 8.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.15   Oct 04 1999 11:32:04   Nick
   Project:  SVObserver
   Change Request (SCR) nbr:  43
   SCR Title:  Version 3.00 Beta 7 Versioning
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
     V3.00 Beta 7 versioning.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.14   Sep 30 1999 14:43:10   Nick
   Project:  SVObserver
   Change Request (SCR) nbr:  42
   SCR Title:  Version 3.00 Beta 6 Versioning.
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
     Software changes required for version change from v3.00 Beta 5 to v3.00 Beta 6.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.13   Sep 29 1999 14:20:56   Nick
   Project:  SVObserver
   Change Request (SCR) nbr:  41
   SCR Title:  Version 3.00 Beta 5 Versioning.
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
     v3.00 Beta 5
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.12   Sep 28 1999 16:48:42   Nick
   Project:  SVObserver
   Change Request (SCR) nbr:  12
   SCR Title:  Port OCR functionallity from custom 2.xx version.
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
     Remove Obsolete Wit setup flag.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.11   Sep 27 1999 19:04:46   Nick
   Project:  SVObserver
   Change Request (SCR) nbr:  38
   SCR Title:  Version 3.00 Beta 4 Versioning
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
     Version from 0x00030003 to 0x00030004.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.10   Sep 23 1999 13:53:52   Nick
   Project:  SVObserver
   Change Request (SCR) nbr:  37
   SCR Title:  Version 3.00 Beta 3 Versioning
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
     New version specific changes to Serialize methods.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.9   Sep 23 1999 08:55:56   Nick
   Project:  SVObserver
   Change Request (SCR) nbr:  35
   SCR Title:  PPQ mode to wait for input data
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
     Bugfix: Multiple extern trigger and timer after reloading.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.8   Sep 21 1999 15:35:12   Nick
   Project:  SVObserver
   Change Request (SCR) nbr:  36
   SCR Title:  Version 3.00 Beta 2 Versioning
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
     Fixed the Trigger Source combo box descriptions for the Position Queue page in the SVObserver Inspection Environment Assistant property sheet.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.7   Sep 21 1999 09:29:16   Nick
   Project:  SVObserver
   Change Request (SCR) nbr:  36
   SCR Title:  Version 3.00 Beta 2 Versioning
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
     Added 0x00030002 (v3.00 Beta 2) case statement  to all serialization methods.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.6   Sep 20 1999 10:02:30   Nick
   Project:  SVObserver
   Change Request (SCR) nbr:  35
   SCR Title:  PPQ mode to wait for input data
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
     Removed old SV_FAST_IO code.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.5   Sep 16 1999 12:45:26   Nick
   Project:  SVObserver
   Change Request (SCR) nbr:  34
   SCR Title:  Add Build Reference Tool
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
     Introduced Digital Input Objects.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.4   Sep 07 1999 09:10:24   Nick
   Project:  SVObserver
   Change Request (SCR) nbr:  12
   SCR Title:  Port OCR functionallity from custom 2.xx version.
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
     Fixed memory leak.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.3   01 Sep 1999 11:47:48   sjones
   Project:  SVObserver
   Change Request (SCR) nbr:  9
   SCR Title:  Integr. a common object interface to handle and to identify obj. in-/outputs
   Checked in by:  sJones;  Steve Jones
   Change Description:  
     Revised versioning to 3.0 Beta 1
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.2   Aug 23 1999 10:35:16   sjones
   Project:  SVObserver
   Change Request (SCR) nbr:  22
   SCR Title:  Port Multiple Position Queue code from program version 2.4.2
   Checked in by:  Steve Jones
   Change Description:  
     Revised to use new Digitizer Classes.
   Revised to use new Timer Classes.
   
   /////////////////////////////////////////////////////////////////////////////////////
*/
