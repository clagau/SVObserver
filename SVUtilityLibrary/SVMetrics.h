//******************************************************************************
//* COPYRIGHT (c) 2011 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVMetrics
//* .File Name       : $Workfile:   SVMetrics.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   17 Jun 2014 10:26:14  $
//******************************************************************************

#pragma once

// This does not use the guideline namespace format because it is legacy code from the SVObserverWeb project.
namespace SeidenaderVision
{
	struct SVMetrics
	{
		double minTime;
		double maxTime;
		double avgTime;
		double lastTime;

		SVMetrics();
		void Clear();
		void Update(double elapsed);
	};
}

