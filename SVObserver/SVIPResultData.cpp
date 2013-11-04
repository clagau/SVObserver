//******************************************************************************
//* COPYRIGHT (c) 2012 by Seidenader Vision, Inc.
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVIPResultData
//* .File Name       : $Workfile:   SVIPResultData.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 11:17:44  $
//******************************************************************************

#include "stdafx.h"
#include "SVIPResultData.h"

SVIPResultData::SVIPResultData()
: m_ResultData(),
  m_ToolSetEndTime( 0.0 ),
  m_ToolSetAvgTime( 0.0 ),
  m_TriggerDistance( 0.0 ),
  m_ToolSetTime( 0.0 )
{
}

SVIPResultData::SVIPResultData( const SVResultDataMap& p_rResultData, double p_ToolSetEndTime, double p_ToolSetAvgTime, double p_TriggerDistance, double p_ToolSetTime )
: m_ResultData( p_rResultData ),
  m_ToolSetEndTime( p_ToolSetEndTime ),
  m_ToolSetAvgTime( p_ToolSetAvgTime ),
  m_TriggerDistance( p_TriggerDistance ),
  m_ToolSetTime( p_ToolSetTime )
{
}

SVIPResultData::SVIPResultData( const SVIPResultData& p_rObject )
: m_ResultData( p_rObject.m_ResultData ),
  m_ToolSetEndTime( p_rObject.m_ToolSetEndTime ),
  m_ToolSetAvgTime( p_rObject.m_ToolSetAvgTime ),
  m_TriggerDistance( p_rObject.m_TriggerDistance ),
  m_ToolSetTime( p_rObject.m_ToolSetTime )
{
}

void SVIPResultData::clear()
{
	m_ResultData.clear();

	m_ToolSetEndTime = 0.0;
	m_ToolSetAvgTime = 0.0;

	m_TriggerDistance = 0.0;
	m_ToolSetTime = 0.0;
}


//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVIPResultData.cpp_v  $
 * 
 *    Rev 1.0   23 Apr 2013 11:17:44   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   02 Jul 2012 16:58:46   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to promote new display functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/