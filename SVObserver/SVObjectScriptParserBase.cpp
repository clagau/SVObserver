//******************************************************************************
//* COPYRIGHT (c) 2012 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVObjectScriptParserBase
//* .File Name       : $Workfile:   SVObjectScriptParserBase.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 12:40:58  $
//******************************************************************************
#include "stdafx.h"
#include "SVObjectScriptParserBase.h"
#include "SVParserProgressStruct.h"
#include "ObjectInterfaces/SVUserMessage.h"

SVObjectScriptParserBase::SVObjectScriptParserBase(unsigned long parserHandle, const GUID& OwnerGuid, SVObjectClass* pOwnerObject, CWnd* pWnd)
: m_parserHandle(parserHandle)
, m_OwnerGuid(OwnerGuid)
, m_pOwnerObject(pOwnerObject)
, m_pWnd(pWnd)
{
}

SVObjectScriptParserBase::~SVObjectScriptParserBase()
{
}

const GUID& SVObjectScriptParserBase::GetOwnerGuid() const
{
	return m_OwnerGuid;
}

SVObjectClass* SVObjectScriptParserBase::GetOwnerObject()
{
	return m_pOwnerObject;
}
	
///////////////////////////////////////////////////////////////////
// Update Progress Dialog
///////////////////////////////////////////////////////////////////
int g_nProgressPercent = 0; // this can't work in a threading environment...
void SVObjectScriptParserBase::UpdateProgress( size_t amtProcessed, size_t amtTotal )
{
	SVParserProgressStruct parserProgressStruct(m_parserHandle);
	int nTempPercent = static_cast<int>(( ( static_cast<float>(amtProcessed)  / static_cast<float>(amtTotal) ) * 100.0f));
	// No need to notify if there are no changes
	if( m_pWnd && m_pWnd->GetSafeHwnd() && g_nProgressPercent != nTempPercent )
	{
		// update current state	
		g_nProgressPercent = nTempPercent;

		parserProgressStruct.amtProcessed = amtProcessed;
		parserProgressStruct.amtTotal = amtTotal;

		m_pWnd->SendMessage( SV_UPDATE_PROGRESS, 0, ( LPARAM )&parserProgressStruct );
	}
}

void SVObjectScriptParserBase::Complete()
{
	m_pWnd->PostMessage( SV_END_PROGRESS_DIALOG, 0, (LPARAM )m_parserHandle );
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVObjectScriptParserBase.cpp_v  $
 * 
 *    Rev 1.0   23 Apr 2013 12:40:58   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   04 Sep 2012 14:00:56   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Initial check-in of new files.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
