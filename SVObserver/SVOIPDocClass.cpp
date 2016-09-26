//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVOIPDocClass
//* .File Name       : $Workfile:   SVOIPDocClass.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   03 Sep 2014 16:01:04  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVObserver.h"
#include "SVOIPDocClass.h"
#include "SVSystemLibrary/SVFileNameManagerClass.h"
#include "SVStatusLibrary\MessageManager.h"
#include "ObjectInterfaces\ErrorNumbers.h"
#include "TextDefinesSvO.h"
#include "SVOMFCLibrary/SVDeviceParams.h"
#pragma endregion Includes

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(SVOIPDocClass, COleServerDoc)

SVOIPDocClass::SVOIPDocClass()
: COleServerDoc()
{
	EnableCompoundFile();

	SVFileNameManagerClass::AddItem( &msvFileName );
}

BOOL SVOIPDocClass::OnNewDocument()
{
	if (!COleServerDoc::OnNewDocument())
	{
		return FALSE;
	}

	return TRUE;
}

SVOIPDocClass::~SVOIPDocClass()
{
	SVFileNameManagerClass::RemoveItem( &msvFileName );
}

BEGIN_MESSAGE_MAP(SVOIPDocClass, COleServerDoc)
	//{{AFX_MSG_MAP(SVOIPDocClass)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// SVOIPDocClass diagnostics

#ifdef _DEBUG
void SVOIPDocClass::AssertValid() const
{
	COleServerDoc::AssertValid();
}

void SVOIPDocClass::Dump(CDumpContext& dc) const
{
	COleServerDoc::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// SVOIPDocClass serialization

void SVOIPDocClass::Serialize(CArchive& ar)
{
	SvStl::MessageMgrNoDisplay Exception( SvStl::LogOnly );
	Exception.setMessage( E_NOTIMPL, SvOi::Tid_ErrorUsedPreviouslyForSec, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_15035 );
}

/////////////////////////////////////////////////////////////////////////////
// SVOIPDocClass commands

BOOL SVOIPDocClass::OnOpenDocument(LPCTSTR lpszPathName) 
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
		bOk = SVFileNameManagerClass::LoadItem( &msvFileName );
	}

	if ( bOk )
	{
		bOk = COleServerDoc::OnOpenDocument( msvFileName.GetFullFileName() );
	}

	return bOk;
}

void SVOIPDocClass::SetPathName( LPCTSTR lpszPathName, BOOL bAddToMRU ) 
{
	msvFileName.SetFullFileName( lpszPathName );

	SVFileNameManagerClass::LoadItem( &msvFileName );

	// Never add to MRU file list! 
	COleServerDoc::SetPathName( msvFileName.GetFullFileName(), FALSE );
}

COleServerItem* SVOIPDocClass::OnGetEmbeddedItem()
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

void SVOIPDocClass::CloseDocument()
{
	OnFileClose();
}

