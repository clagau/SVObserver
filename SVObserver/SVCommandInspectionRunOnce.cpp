//******************************************************************************
//* COPYRIGHT (c) 2012 by Seidenader Vision, Inc.
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVCommandInspectionRunOnce
//* .File Name       : $Workfile:   SVCommandInspectionRunOnce.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 09:56:34  $
//******************************************************************************

#include "stdafx.h"
#include "SVCommandInspectionRunOnce.h"

#include "SVObjectLibrary/SVObjectManagerClass.h"

#include "SVInspectionProcess.h"
#include "SVTool.h"

SVCommandInspectionRunOnce::SVCommandInspectionRunOnce()
: m_InspectionId(), m_ToolId()
{
}

SVCommandInspectionRunOnce::SVCommandInspectionRunOnce(const SVCommandInspectionRunOnce& p_rObject)
: m_InspectionId( p_rObject.m_InspectionId ), m_ToolId( p_rObject.m_ToolId )
{
}

SVCommandInspectionRunOnce::SVCommandInspectionRunOnce(const SVGUID& p_rInspectionId, const SVGUID& p_rToolId)
: m_InspectionId( p_rInspectionId ), m_ToolId( p_rToolId )
{
}

SVCommandInspectionRunOnce::~SVCommandInspectionRunOnce()
{
}

HRESULT SVCommandInspectionRunOnce::Execute()
{
	HRESULT l_Status = S_OK;

	SVObjectClass* l_pObject = SVObjectManagerClass::Instance().GetObject( m_InspectionId );
	SVInspectionProcess* pInspection = dynamic_cast< SVInspectionProcess* >( l_pObject );

	if( nullptr != pInspection )
	{
		l_pObject = SVObjectManagerClass::Instance().GetObject( m_ToolId );
		SVToolClass* l_pTool = dynamic_cast< SVToolClass* >( l_pObject );

		l_Status = pInspection->RunOnce( l_pTool ) ? S_OK : S_FALSE;
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

bool SVCommandInspectionRunOnce::empty() const
{
	bool l_Status = true;

	l_Status = l_Status && ( m_InspectionId.empty() );
	l_Status = l_Status && ( m_ToolId.empty() );

	return l_Status;
}

void SVCommandInspectionRunOnce::clear()
{
	m_InspectionId.clear();
	m_ToolId.clear();
}

const SVGUID& SVCommandInspectionRunOnce::GetInspectionId() const
{
	return m_InspectionId;
}

const SVGUID& SVCommandInspectionRunOnce::GetToolId() const
{
	return m_ToolId;
}

HRESULT SVCommandInspectionRunOnce::SetCommandData(const SVGUID& p_rInspectionId, const SVGUID& p_rToolId)
{
	HRESULT l_Status = S_OK;

	m_InspectionId = p_rInspectionId;
	m_ToolId = p_rToolId;

	return l_Status;
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVCommandInspectionRunOnce.cpp_v  $
 * 
 *    Rev 1.0   23 Apr 2013 09:56:34   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   01 Aug 2012 11:44:46   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added new command functionality to allow for synchronous command execution through the inspection thread.  This will fix problems with race conditions with the inspection process.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/





