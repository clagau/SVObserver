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

namespace SvUl 
{
	//************************************
	/// SVMetrics is usually not used but should stay in the utility library to be able to use when needed
	//************************************
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
} // namespace SvUl

