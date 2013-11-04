//******************************************************************************
//* COPYRIGHT (c) 2012 by Seidenader Vision, Inc.
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVIPResultData
//* .File Name       : $Workfile:   SVIPResultData.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 11:17:54  $
//******************************************************************************

#ifndef SVIPRESULTDATA_H
#define SVIPRESULTDATA_H

#include <map>

#include "SVIPResultItemData.h"
#include "SVIPResultItemDefinition.h"

struct SVIPResultData
{
	typedef std::map< SVIPResultItemDefinition, SVIPResultItemData > SVResultDataMap;

	SVIPResultData();
	SVIPResultData( const SVResultDataMap& p_rResultData, double p_ToolSetEndTime, double p_ToolSetAvgTime, double p_TriggerDistance, double p_ToolSetTime );
	SVIPResultData( const SVIPResultData& p_rObject );

	void clear();

	SVResultDataMap m_ResultData;

	double m_ToolSetEndTime;
	double m_ToolSetAvgTime;

	double m_TriggerDistance;
	double m_ToolSetTime;

};

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVIPResultData.h_v  $
 * 
 *    Rev 1.0   23 Apr 2013 11:17:54   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   02 Jul 2012 16:58:48   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to promote new display functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/