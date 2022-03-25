//******************************************************************************
//* COPYRIGHT (c) 2012 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVObjectScriptParserBase
//* .File Name       : $Workfile:   SVObjectScriptParserBase.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 12:40:58  $
//******************************************************************************
#pragma region Includes
#include "stdafx.h"
#include "SVObjectScriptParserBase.h"
#include "SVParserProgressStruct.h"
#include "Definitions/SVUserMessage.h"
#include "SVObjectLibrary/SVObjectClass.h"
#pragma endregion Includes

SVObjectScriptParserBase::SVObjectScriptParserBase(unsigned long parserHandle, uint32_t ownerId, SVObjectClass* pOwnerObject, CWnd* pWnd)
: m_parserHandle(parserHandle)
, m_OwnerId(ownerId)
, m_pOwnerObject(pOwnerObject)
, m_pWnd(pWnd)
{
}

SVObjectScriptParserBase::~SVObjectScriptParserBase()
{
}

uint32_t SVObjectScriptParserBase::GetOwnerId() const
{
	return m_OwnerId;
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

