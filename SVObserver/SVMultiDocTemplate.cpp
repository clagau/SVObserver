//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVMultiDocTemplateClass
//* .File Name       : $Workfile:   SVMultiDocTemplate.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 12:37:08  $
//******************************************************************************

#include "stdafx.h"
#include "SVObserver.h"
#include "SVMultiDocTemplate.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// SVMultiDocTemplateClass


SVMultiDocTemplateClass::SVMultiDocTemplateClass( UINT NIDResource, CRuntimeClass* PDocClass, CRuntimeClass* PFrameClass, CRuntimeClass* PViewClass )
						:CMultiDocTemplate( NIDResource, PDocClass,	PFrameClass, PViewClass )
{
}

SVMultiDocTemplateClass::~SVMultiDocTemplateClass()
{
//	CMultiDocTemplate::~CMultiDocTemplate();
	// delete shared components
	if( m_hMenuShared != NULL )
		::DestroyMenu( m_hMenuShared );
	if( m_hAccelTable != NULL )
		::FreeResource( ( HGLOBAL ) m_hAccelTable );
}


/////////////////////////////////////////////////////////////////////////////
// SVMultiDocTemplateClass commands

void SVMultiDocTemplateClass::SetDefaultTitle( CDocument* PDocument )
{
	CString strDocName;
	if( GetDocString( strDocName, CDocTemplate::docName ) && ! strDocName.IsEmpty() )
	{
		TCHAR szNum[ 8 ];

		// S P E C I A L  F O R  S V O B S E R V E R
		// Check default name if it already exists!!!
		
		int i = 0;
		do
		{
			++i;
#ifndef _MAC
			wsprintf( szNum, _T( "%d" ), m_nUntitledCount + i );
#else
			wsprintf( szNum, _T( " %d" ), m_nUntitledCount + i );
#endif
		} while( TheSVObserverApp.AlreadyExistsIPDocTitle( CString( strDocName + szNum ) ) );

		strDocName += szNum;
		m_nUntitledCount += i - 1;
	}
	else
	{
		// use generic 'untitled' - ignore untitled count
		VERIFY( strDocName.LoadString( AFX_IDS_UNTITLED ) );
	}
	PDocument->SetTitle( strDocName );
}


/////////////////////////////////////////////////////////////////////////////
// SVMultiDocTemplateClass diagnostics

#ifdef _DEBUG
void SVMultiDocTemplateClass::Dump( CDumpContext& dc ) const
{
	CMultiDocTemplate::Dump( dc );
}

void SVMultiDocTemplateClass::AssertValid() const
{
	CMultiDocTemplate::AssertValid();
}
#endif //_DEBUG

#ifdef AFX_INIT_SEG
#pragma code_seg(AFX_INIT_SEG)
#endif

IMPLEMENT_DYNAMIC( SVMultiDocTemplateClass, CMultiDocTemplate )

/////////////////////////////////////////////////////////////////////////////
//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVMultiDocTemplate.cpp_v  $
 * 
 *    Rev 1.0   23 Apr 2013 12:37:08   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.3   27 Jan 2011 11:33:38   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  712
   SCR Title:  Fix issues with black images when using command interface (SIAC)
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated source code to fix issues with Matrox Buffer Management.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.2   22 Apr 2003 11:24:38   rschock
   Project:  SVObserver
   Change Request (SCR) nbr:  346
   SCR Title:  Update SVObserver to Version 4.21 Release
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
   
   /////////////////////////////////////////////////////////////////////////////////////
*/
