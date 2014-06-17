//******************************************************************************
//* COPYRIGHT (c) 2011 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVMetrics
//* .File Name       : $Workfile:   SVMetrics.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   17 Jun 2014 10:26:14  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#undef max
#include <limits>
#include "SVMetrics.h"
#pragma endregion Includes

#pragma region Declarations
// This does not use the guideline namespace format because it is legacy code from the SVObserverWeb project.
using namespace SeidenaderVision;
#pragma endregion Declarations

SVMetrics::SVMetrics()
{
	Clear();
}

void SVMetrics::Clear()
{
	minTime = std::numeric_limits<double>::max();
	maxTime = 0.0;
	avgTime = 0.0;
	lastTime = 0.0;
}

void SVMetrics::Update(double elapsed)
{
	bool bAvg = true;

	lastTime = elapsed;

	if (elapsed > maxTime) { maxTime = elapsed; }

	if (minTime == std::numeric_limits<double>::max())
	{
		minTime = elapsed;
		bAvg = false;
	}
	else if (elapsed < minTime)
	{
		minTime = elapsed;
	}

	avgTime += elapsed;

	if (bAvg) { avgTime /= 2.0; }
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVUtilityLibrary\SVMetrics.cpp_v  $
 * 
 *    Rev 1.0   17 Jun 2014 10:26:14   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  886
 * SCR Title:  Add RunReject Server Support to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Initial check in.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   19 Dec 2011 13:39:28   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  744
 * SCR Title:  Add Shared Memory and Socket Functionality for Run Page Web Server
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Initial check-in.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
