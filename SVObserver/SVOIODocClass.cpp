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

#include "stdafx.h"
#include "SVObserver.h"
#include "SVOIODocClass.h"
#include "SVFileNameManagerClass.h"
#include "SVStatusLibrary/SVException.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(SVOIODocClass, COleServerDoc)

#define SEJ_ErrorBase 15000
#define Err_15034 (SEJ_ErrorBase+34)

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
	SVException l_Exception;
	CString l_Message;
	l_Message.Format(_T("This was previously used for .sec files."));
	SETEXCEPTION5(l_Exception, E_NOTIMPL, Err_15034, l_Message);
	l_Exception.LogException(l_Message);
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

	// Instead of returning NULL, return a pointer to a new COleServerItem
	//  derived class that is used in conjunction with this document, then
	//  remove the ASSERT(FALSE) below.
	//  (i.e., return new CMyServerItem.)
	ASSERT(FALSE);			// remove this after completing the TODO
	return NULL;
}

void SVOIODocClass::CloseDocument()
{
	OnFileClose();
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVOIODocClass.cpp_v  $
 * 
 *    Rev 1.2   03 Sep 2014 15:59:32   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  880
 * SCR Title:  Remove .SEC
 * Checked in by:  mZiegler;  Marc Ziegler
 * Change Description:  
 *   Changed Serialize to avoid calling old .sec file code.
 * Added Err_15035.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   31 Jan 2014 17:16:36   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  884
 * SCR Title:  Update Source Code Files to Follow New Programming Standards and Guidelines
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Changed to follow guidelines more closely.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 13:06:24   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   28 Sep 2012 14:38:28   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated SVObserver application tree storage member variable name.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   18 Sep 2012 18:29:18   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to use the templated XML objects that moved from the SVLibrary to the new XML Library and to the Configuration Library.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   01 Jun 2010 10:59:34   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  693
 * SCR Title:  Fix Performance Issue with Inspection Process
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to remove redundent methodologies and fix missing or incorrect calling functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   16 Dec 2009 14:05:40   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  677
 * SCR Title:  Fix problem in camera notify thread
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fix issues with includes and comments.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   22 Jun 2007 10:42:58   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  598
 * SCR Title:  Upgrade SVObserver to compile using vc++ in VS2005
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   These changes include removal of all VC6 constraints, project files, and workspace files.  The new VC8 project and solution files will replace the VC6 files with the new tighter compilation rules.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   21 Jun 2007 14:34:02   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  598
 * SCR Title:  Upgrade SVObserver to compile using vc++ in VS2005
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   These changes include modification based on fixing compiler-based and project-based differences between VC6 and VC8.  These changes mainly include casting issues, but some include type conversion and assignment of new compiler controlling defines.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   22 Apr 2003 13:06:10   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   15 Nov 2002 13:38:12   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  226
 * SCR Title:  Monochrome SVIM configuration compatibility between ViperQUAD and ViperDUAL
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   This is the first revision of this file.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
