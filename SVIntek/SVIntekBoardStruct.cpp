// ******************************************************************************
// * COPYRIGHT (c) 2004 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVIntekBoardStruct
// * .File Name       : $Workfile:   SVIntekBoardStruct.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   26 Apr 2013 14:50:28  $
// ******************************************************************************

#include "stdafx.h"
#include "SVIntekBoardStruct.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

SVIntekBoardStruct::SVIntekBoardStruct()
{
	m_csName.Empty();
	m_csFireStackName.Empty();

	m_ulCardId.AsUINT32 = 0;

	m_hThread = NULL;

	m_bFirstBusResetStarted = false;
	m_bFirstBusResetComplete = false;

	m_pFireBus = NULL;
}

SVIntekBoardStruct::~SVIntekBoardStruct()
{
	if ( m_hThread != NULL )
	{
		::CloseHandle( m_hThread );

		m_hThread = NULL;
	}
}

