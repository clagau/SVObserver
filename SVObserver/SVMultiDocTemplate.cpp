//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVMultiDocTemplate
//* .File Name       : $Workfile:   SVMultiDocTemplate.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 12:37:08  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVObserver.h"
#include "SVMultiDocTemplate.h"
#pragma endregion Includes

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

SVMultiDocTemplate::SVMultiDocTemplate( UINT NIDResource, CRuntimeClass* PDocClass, CRuntimeClass* PFrameClass, CRuntimeClass* PViewClass )
						:CMultiDocTemplate( NIDResource, PDocClass,	PFrameClass, PViewClass )
{
}

SVMultiDocTemplate::~SVMultiDocTemplate()
{
//	CMultiDocTemplate::~CMultiDocTemplate();
	// delete shared components
	if( nullptr != m_hMenuShared )
	{
		::DestroyMenu( m_hMenuShared );
	}
	if( nullptr != m_hAccelTable )
	{
		::FreeResource( ( HGLOBAL ) m_hAccelTable );
	}
}

/////////////////////////////////////////////////////////////////////////////
// SVMultiDocTemplate commands

void SVMultiDocTemplate::SetDefaultTitle( CDocument* PDocument )
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
// SVMultiDocTemplate diagnostics

#ifdef _DEBUG
void SVMultiDocTemplate::Dump( CDumpContext& dc ) const
{
	CMultiDocTemplate::Dump( dc );
}

void SVMultiDocTemplate::AssertValid() const
{
	CMultiDocTemplate::AssertValid();
}
#endif //_DEBUG

#ifdef AFX_INIT_SEG
#pragma code_seg(AFX_INIT_SEG)
#endif

IMPLEMENT_DYNAMIC( SVMultiDocTemplate, CMultiDocTemplate )

/////////////////////////////////////////////////////////////////////////////
