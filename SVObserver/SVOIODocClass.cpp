//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVOIODocClass
//* .File Name       : $Workfile:   SVOIODocClass.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   03 Sep 2014 15:59:32  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVObserver.h"
#include "SVOIODocClass.h"
#include "SVFileNameManagerClass.h"
#include "SVStatusLibrary\MessageManager.h"
#include "ObjectInterfaces\ErrorNumbers.h"
#include "TextDefinesSvO.h"
#pragma endregion Includes

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(SVOIODocClass, COleServerDoc)

SVOIODocClass::SVOIODocClass()
: COleServerDoc()
{
	EnableCompoundFile();

	SVFileNameManagerClass svFileManager;
	svFileManager.AddItem( &msvFileName );
}

BOOL SVOIODocClass::OnNewDocument()
{
	if (!COleServerDoc::OnNewDocument())
	{
		return FALSE;
	}

	return TRUE;
}

SVOIODocClass::~SVOIODocClass()
{
	SVFileNameManagerClass svFileManager;
	svFileManager.RemoveItem( &msvFileName );
}

BEGIN_MESSAGE_MAP(SVOIODocClass, COleServerDoc)
	//{{AFX_MSG_MAP(SVOIODocClass)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// SVOIODocClass diagnostics

#ifdef _DEBUG
void SVOIODocClass::AssertValid() const
{
	COleServerDoc::AssertValid();
}

void SVOIODocClass::Dump(CDumpContext& dc) const
{
	COleServerDoc::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// SVOIODocClass serialization

void SVOIODocClass::Serialize(CArchive& ar)
{
	SvStl::MessageMgrNoDisplay Exception( SvStl::LogOnly );
	Exception.setMessage( E_NOTIMPL, SvOi::Tid_ErrorUsedPreviouslyForSec, StdMessageParams, SvOi::Err_15034 );
}

/////////////////////////////////////////////////////////////////////////////
// SVOIODocClass commands

BOOL SVOIODocClass::OnOpenDocument(LPCTSTR lpszPathName) 
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
		bOk = COleServerDoc::OnOpenDocument( msvFileName.GetFullFileName() );
	}

	return bOk;
}

void SVOIODocClass::SetPathName( LPCTSTR lpszPathName, BOOL bAddToMRU ) 
{
	msvFileName.SetFullFileName( lpszPathName );

	SVFileNameManagerClass svFileManager;
	svFileManager.LoadItem( &msvFileName );

	// Never add to MRU file list! 
	COleServerDoc::SetPathName( msvFileName.GetFullFileName(), FALSE );
}

COleServerItem* SVOIODocClass::OnGetEmbeddedItem()
{
	// OnGetEmbeddedItem is called by the framework to get the COleServerItem
	//  that is associated with the document.  It is only called when necessary.

	// Instead of returning nullptr, return a pointer to a new COleServerItem
	//  derived class that is used in conjunction with this document, then
	//  remove the ASSERT(FALSE) below.
	//  (i.e., return new CMyServerItem.)
	ASSERT(FALSE);			// remove this after completing the TODO
	return nullptr;
}

void SVOIODocClass::CloseDocument()
{
	OnFileClose();
}

